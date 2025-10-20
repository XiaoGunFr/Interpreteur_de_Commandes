#ifndef JSH_H
#define JSH_H

#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <math.h>

#include "../include/auxiliaire.h"
#include "../include/command.h"
#include "../include/jobs.h"
#include "../include/jsh.h"
#include "../include/redirection.h"
#include "../include/signaux.h"

extern char* prompt();
extern int main(void);

#endif