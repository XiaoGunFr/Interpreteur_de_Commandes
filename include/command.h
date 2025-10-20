#ifndef COMMAND_H
#define COMMAND_H

#include "../include/auxiliaire.h"
#include "../include/command.h"
#include "../include/jobs.h"
#include "../include/jsh.h"
#include "../include/redirection.h"
#include "../include/signaux.h"

extern int valRetour; //Correspond à ?

extern int pwd();
extern int cd(const char *path);
extern void extern_command_direct(char **command);
extern int extern_command(char *command);
extern int lastVal();
extern void command(char * input, char * p);

#endif