#ifndef JOBS_H
#define JOBS_H

#include "../include/auxiliaire.h"
#include "../include/command.h"
#include "../include/jobs.h"
#include "../include/jsh.h"
#include "../include/redirection.h"
#include "../include/signaux.h"

enum etats {Running, Stopped, Detached, Killed, Done};
enum errors {Pwd, Cd, Fork, Unknow_Command, Waitpid, Conversion, Open_File, Jobs, Unknow_Job, Memoire, Argument, Pipe, Tcsetprgp, Kill, Sigaction};

typedef struct job_info{
    int id;
    pid_t pid;
    pid_t gpid;
    enum etats etat;
    char* commande;
    struct job_info *suivant;
    struct job_info *sous_job;
}job_info;

typedef struct liste{
    struct job_info *job_first;
    struct job_info *job_last;
}liste;

extern void init();
extern job_info *getJob(int num);
extern job_info *getJob_bypid(int num);
extern void ajouter_job(pid_t pid, enum etats etat, char *command);
extern void affichage_job();
extern void affichage_job_unique(int id);
extern void affichage_job_option();
extern void affichage_job_option_unique(int id);
extern void modif_job_etat(pid_t pid, enum etats etat);
extern void check_all_jobs();
extern void check_all_jobs_no_print();
extern void remove_done();
extern enum etats status_job(int status);
extern void free_all();
extern void free_jobs(job_info *liste_a_vider);
extern int get_nb_jobs();
extern void remove_job(job_info *job);
extern void print_error(enum errors errors);
extern job_info* creerJob(pid_t pid, pid_t gpid, enum etats etat, char *command);
extern void ajouter_Sous_Job(job_info* group, job_info* fils);
extern void ajouter_job_group(job_info* job);
extern void modif_sous_job_etat(pid_t gpid, pid_t pid, enum etats etat);
extern void getMoyenneJob(job_info* job);

#endif