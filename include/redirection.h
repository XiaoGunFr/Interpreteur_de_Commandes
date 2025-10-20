#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "../include/auxiliaire.h"
#include "../include/command.h"
#include "../include/jobs.h"
#include "../include/jsh.h"
#include "../include/redirection.h"
#include "../include/signaux.h"

extern int isRedirectionAndType(char * command);
extern void closeFile(int file[], int taille);
extern int openFile(int file[], char ** args);
extern int nb_Redir(char ** args);
extern char ** getFirstCmd(int type, char * command);
extern void dupFile(int file[], char ** args);
extern int redirectionFile(int type, char * command, bool mode);
extern int redirectionPipe(char * command, bool mode);
extern int redirectionProcess(char * command, bool mode);
extern int redirection(int type, char * command);

#endif 
