#ifndef AUXILIAIRE_H
#define AUXILIAIRE_H

#include "../include/auxiliaire.h"
#include "../include/command.h"
#include "../include/jobs.h"
#include "../include/jsh.h"
#include "../include/redirection.h"
#include "../include/signaux.h"

extern int * stock;

extern char **cut_string(char *str, char *sep);
extern char ** getCommandeByRedirExter(char * command);
extern char ** cutStringAuxRedirExter(char * command, int * pos);
extern int ** linkTab(int ** tab1, int ** tab2);
extern bool removeBackSign(char * command);
extern char * mergeChar(char ** command);
extern char ** getCommandeImbrique(char * command);
extern bool ifContain(char * command, char * str);
extern int** nbOcc(char* str, char* sep);
extern char *allouer_attribuer_char(char *mot);
extern void freeTab(char** t);
extern void freeTabInt(int** t);
extern int tailleTableau(char **tab);
extern int tailleTableauInt(int **tab);
extern int tailleTableauIntBis(int *tab);

#endif