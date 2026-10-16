// wish.c -- the shell
// it's 3am and this is due at 8. it works. dont touch it.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

// whitespace chars. tabs count too, prof said so, ugh
#define WS " \t\r\n"

// global path stuff because im not passing this around everywhere
char **paths = NULL;
int npaths = 0;

// the ONE error message. only one. dont get creative
void err() {
    char error_message[30] = "An error has occurred\n";
    if (write(STDERR_FILENO, error_message, strlen(error_message)) < 0) {
        // cant even print an error?? cool. whatever
    }
}

// chops a string up on whitespace. returns NULL terminated array
// the tokens just point into the original string so DONT free them
// (learned that the hard way. segfault city)
char **split(char *s, int *count) {
    int cap = 8;
    int n = 0;
    char **arr = malloc(cap * sizeof(char *));
    if (arr == NULL) { err(); exit(1); } // ok if malloc fails we have bigger problems
    char *tok;
    while ((tok = strsep(&s, WS)) != NULL) {
        if (*tok == '\0') continue; // strsep gives empty strings for repeated spaces. thanks
        if (n + 1 >= cap) {
            cap = cap * 2;
            arr = realloc(arr, cap * sizeof(char *));
            if (arr == NULL) { err(); exit(1); }
        }
        arr[n] = tok;
        n++;
    }
    arr[n] = NULL; // execv needs this or it dies
    *count = n;
    return arr;
}

// path builtin. nukes the old path and copies in the new one
void set_paths(char **dirs, int n) {
    int i;
    for (i = 0; i < npaths; i++) free(paths[i]);
    free(paths);
    paths = NULL;
    npaths = n;
    if (n > 0) {
        paths = malloc(n * sizeof(char *));
        if (paths == NULL) { err(); exit(1); }
        for (i = 0; i < n; i++) {
            paths[i] = strdup(dirs[i]);
            if (paths[i] == NULL) { err(); exit(1); }
        }
    }
}

// looks thru the path dirs for the command. returns full path or NULL
// caller has to free it. i will probably forget. (i didnt, check below)
char *find_cmd(char *cmd) {
    int i;
    for (i = 0; i < npaths; i++) {
        size_t len = strlen(paths[i]) + strlen(cmd) + 2; // slash + null
        char *full = malloc(len);
        if (full == NULL) { err(); exit(1); }
        snprintf(full, len, "%s/%s", paths[i], cmd);
        if (access(full, X_OK) == 0) return full;
        free(full);
    }
    return NULL; // not found, sad
}

// runs ONE command (already split on &)
// if its an external program it forks and saves the pid so we can wait later
void do_cmd(char *cmd, pid_t **pids, int *npids, int *cap) {
    char *redir = NULL;
    char *gt = strchr(cmd, '>');

    if (gt != NULL) {
        *gt = '\0'; // cut the string at the >
        redir = gt + 1;
        // second > anywhere = error. no ls > a > b allowed
        if (strchr(redir, '>') != NULL) { err(); return; }
    }

    int argc;
    char **args = split(cmd, &argc);

    // nothing before the >, or just an empty segment
    if (argc == 0) {
        if (redir != NULL) err(); // "> file" with no command is bad
        free(args);
        return; // blank line / trailing & is fine, just skip
    }

    char *outfile = NULL;
    if (redir != NULL) {
        int rc;
        char **rt = split(redir, &rc);
        if (rc != 1) { // zero files or a bunch of files. both bad
            err();
            free(rt);
            free(args);
            return;
        }
        outfile = rt[0]; // still points into the line so its ok after free(rt)
        free(rt);
    }

    // builtins. these run in the shell itself, no fork
    if (strcmp(args[0], "exit") == 0) {
        if (argc != 1) err();
        else exit(0); // bye
    } else if (strcmp(args[0], "cd") == 0) {
        if (argc != 2) err();
        else if (chdir(args[1]) != 0) err();
    } else if (strcmp(args[0], "path") == 0) {
        set_paths(args + 1, argc - 1); // skip "path" itself
    } else {
        // not a builtin so go find the actual program
        char *full = find_cmd(args[0]);
        if (full == NULL) {
            err();
        } else {
            pid_t pid = fork();
            if (pid < 0) {
                err();
            } else if (pid == 0) {
                // child. redirect output if we need to
                if (outfile != NULL) {
                    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) { err(); _exit(1); }
                    // stdout AND stderr both go to the file. the "twist"
                    if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0) {
                        err();
                        _exit(1);
                    }
                    close(fd);
                }
                execv(full, args);
                // if we get here execv failed
                err();
                _exit(1);
            } else {
                // parent. remember the pid, dont wait yet bc of the & thing
                if (*npids >= *cap) {
                    *cap = *cap * 2;
                    *pids = realloc(*pids, *cap * sizeof(pid_t));
                    if (*pids == NULL) { err(); exit(1); }
                }
                (*pids)[*npids] = pid;
                (*npids)++;
            }
            free(full); // see? i remembered
        }
    }
    free(args);
}

// handles a whole line: split on &, run everything, THEN wait
void do_line(char *line) {
    int cap = 8;
    int npids = 0;
    pid_t *pids = malloc(cap * sizeof(pid_t));
    if (pids == NULL) { err(); exit(1); }

    char *rest = line;
    char *seg;
    while ((seg = strsep(&rest, "&")) != NULL) {
        do_cmd(seg, &pids, &npids, &cap);
    }

    // ok now wait for everybody
    int i;
    for (i = 0; i < npids; i++) {
        waitpid(pids[i], NULL, 0);
    }
    free(pids);
}

int main(int argc, char *argv[]) {
    FILE *in = stdin;
    int interactive = 1;

    // more than 1 arg = nope
    if (argc > 2) { err(); exit(1); }

    // batch mode. file cant open = nope
    if (argc == 2) {
        in = fopen(argv[1], "r");
        if (in == NULL) { err(); exit(1); }
        interactive = 0; // no prompt in batch mode
    }

    char *start[] = { "/bin" }; // default path
    set_paths(start, 1);

    char *line = NULL;
    size_t len = 0;

    while (1) {
        if (interactive) {
            printf("wish> ");
            fflush(stdout); // otherwise the prompt shows up late. found out the dumb way
        }
        if (getline(&line, &len, in) == -1) break; // EOF
        do_line(line);
    }

    free(line);
    exit(0);
}
