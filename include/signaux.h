#ifndef SIGNAUX_H
#define SIGNAUX_H

#include "../include/auxiliaire.h"
#include "../include/command.h"
#include "../include/jobs.h"
#include "../include/jsh.h"
#include "../include/redirection.h"
#include "../include/signaux.h"

extern int bg(int pid);
extern int fg(int pid);
extern int kill2(int signal_type, int pid);
extern void gestion_signaux_jsh();
extern void gestion_signal_arrière_plan();
extern void gestion_signal_reset();
extern void ignore_sigttou_sigttin();
extern void restore_sigttou_sigttin();

#endif