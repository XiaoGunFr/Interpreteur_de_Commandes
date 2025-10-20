#include "../include/signaux.h"
#include "../include/jobs.h"

int bg(int pid){
    if (kill2(SIGCONT,pid) < 0){
        print_error(Kill);
        return 1;
    }
    
    return 0;
}

int fg(int pid){
    //On ignore les signaux SIGTTOU et SIGGTIN pour éviter des lectures ou écritures illégales
    ignore_sigttou_sigttin();
    
    int status = 0;
    job_info *job = getJob_bypid(pid);
    job_info *sous_job = job->sous_job;

    //Dans le cas où le processus est stoppé
    if(job->etat == Stopped){
        if(bg(pid) == 1) return 1;
    }

    if(sous_job != NULL){
        while(sous_job != NULL){
            //Mets à l'avant plan pid à la place de STDIN (besoin d'être le terminal qui contrôle pid)
            if(tcsetpgrp(STDIN_FILENO, pid) < 0){ 
                print_error(Tcsetprgp); 
                return 1; 
            }
            waitpid(sous_job->pid, &status, WUNTRACED);
            //On remet le terminal à l'avant plan
            if(tcsetpgrp(STDIN_FILENO, getpgrp()) < 0){
                print_error(Tcsetprgp);
                return 1;
            }
            enum etats etat = status_job(status);
            if(sous_job->etat != etat){
                modif_sous_job_etat(pid, sous_job->pid, etat);
            }
            sous_job = sous_job->suivant;
        }
        getMoyenneJob(job);
        if(job->etat == Stopped){
            affichage_job_unique(job->id);
        }
    }else{
        if(tcsetpgrp(STDIN_FILENO, pid) < 0){ 
            print_error(Tcsetprgp); 
            return 1; 
        }
        waitpid(job->pid, &status, WUNTRACED);
        if(tcsetpgrp(STDIN_FILENO, getpgrp()) < 0){
            print_error(Tcsetprgp);
            return 1;
        }
        enum etats etat = status_job(status);
        if(job->etat != etat){
            modif_job_etat(pid, etat);
        }
        if(job->etat == Stopped){
            affichage_job_unique(job->id);
        }
    }
    restore_sigttou_sigttin();
    remove_done();
    return 0;
}

int kill2(int signal_type, int pid){
    if (killpg(pid, signal_type) < 0){
        print_error(Kill);
        return 1;
    }
    return 0;
}

void gestion_signaux_jsh(){
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    // Ignorer le signal
    action.sa_handler = SIG_IGN;
    if (sigaction(SIGTSTP, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTSTP.");
    }
    if (sigaction(SIGINT, &action, NULL) < 0){
        perror("Erreur de gestion de SIGINT.");
    }
    if (sigaction(SIGTTIN, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTTIN.");
    }
    if (sigaction(SIGTERM, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTERM.");
    }
    if (sigaction(SIGQUIT, &action, NULL) < 0){
        perror("Erreur de gestion de SIGQUIT.");
    }
    if (sigaction(SIGTTOU, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTTOU.");
    }
}

void gestion_signal_arrière_plan(){
    struct sigaction action = {0};
    action.sa_handler = SIG_DFL;
    if(sigaction(SIGTSTP, &action, NULL) < 0){
        print_error(Sigaction);
    }
    //Le fils envoi un signal lorque terminé
    if (sigaction(SIGCHLD, &action, NULL) < 0){
        print_error(Sigaction);
    }
}

void gestion_signal_reset(){
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    // Signal par défaut
    action.sa_handler = SIG_DFL;
    if (sigaction(SIGTSTP, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTSTP.");
    }
    if (sigaction(SIGINT, &action, NULL) < 0){
        perror("Erreur de gestion de SIGINT.");
    }
    if (sigaction(SIGTTIN, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTTIN.");
    }
    if (sigaction(SIGTERM, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTERM.");
    }
    if (sigaction(SIGQUIT, &action, NULL) < 0){
        perror("Erreur de gestion de SIGQUIT.");
    }
    if (sigaction(SIGTTOU, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTTOU.");
    }
}

void ignore_sigttou_sigttin(){
    struct sigaction action = {0};
    // Ignorer le signal
    action.sa_handler = SIG_IGN;
    if (sigaction(SIGTTOU, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTTOU.");
    }
    if (sigaction(SIGTTIN, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTTIN.");
    }
}

void restore_sigttou_sigttin(){
    struct sigaction action = {0};
    // Ignorer le signal
    action.sa_handler = SIG_DFL;
    if (sigaction(SIGTTOU, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTTOU.");
    }
    if (sigaction(SIGTTIN, &action, NULL) < 0){
        perror("Erreur de gestion de SIGTTIN.");
    }
}

