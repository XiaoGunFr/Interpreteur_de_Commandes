#include "../include/jobs.h"

// Compteur globale pour l'id des jobs.
int id_static = 1;

// Liste des jobs, global à tout le programme.
liste *job_info_list = NULL;

// Initialisation de la liste des jobs.
void init(){
    stock = NULL;
    job_info_list = malloc(sizeof(liste));
    job_info_list->job_first = NULL;
    job_info_list->job_last = NULL;
}

// Renvoie le job recherché par son id.
job_info *getJob(int num){
    job_info *job_parcours = job_info_list->job_first;
    while(job_parcours != NULL){
        if(job_parcours->id == num){
            return job_parcours;
        }
        job_parcours = job_parcours -> suivant;
    }
    print_error(Unknow_Job);
    return NULL;
}

// Renvoie le job recherché par son pid.
job_info *getJob_bypid(int num){
    job_info *job_parcours = job_info_list->job_first;
    while(job_parcours != NULL){
        if(job_parcours->pid == num){
            return job_parcours;
        }
        job_parcours = job_parcours -> suivant;
    }
    print_error(Unknow_Job);
    return NULL;
}

// Ajoute un nouveau job dans la liste.
void ajouter_job(pid_t pid, enum etats etat, char *command){
    job_info *newJob = malloc(sizeof(job_info));
    newJob->id = id_static;
    newJob->pid = pid;
    newJob->etat = etat;
    newJob->commande = allouer_attribuer_char(command);
    newJob->suivant = NULL;
    newJob->sous_job = NULL;
    if(job_info_list->job_first == NULL){
        job_info_list->job_first = newJob;
        job_info_list->job_last = newJob;
    }else{
        job_info_list->job_last->suivant = newJob;
        job_info_list->job_last = newJob;
    }
    id_static++;
}

// Affichage de tout les jobs.
void affichage_job(){
    job_info *job_parcours = job_info_list->job_first;
    while(job_parcours != NULL){
        char *etat = "";
        switch (job_parcours->etat){
            case Running:
                etat = "Running";
                break;
            case Stopped:
                etat = "Stopped";
                break;
            case Detached:
                etat = "Detached";
                break;
            case Killed:
                etat = "Killed";
                break;
            default:
                etat = "Done";
                break;
        }
        char *tmp = malloc(sizeof(int)*sizeof(job_parcours->id) + sizeof(pid_t)*sizeof(job_parcours->pid) + sizeof(char)*sizeof(etat) + sizeof(char)*sizeof(job_parcours->commande) + 17);
        sprintf(tmp, "[%d] %d %s %s\n", job_parcours->id, job_parcours->pid, etat, job_parcours->commande);
        write(STDOUT_FILENO, tmp, strlen(tmp));
        free(tmp);
        job_parcours = job_parcours->suivant;
    }
}

// Affichage d'un job spécifié par son id.
void affichage_job_unique(int id){
    job_info *job_parcours = job_info_list->job_first;
    while(job_parcours != NULL){
        if(job_parcours->id == id){
            char *etat = "";
            switch (job_parcours->etat){
                case Running:
                    etat = "Running";
                    break;
                case Stopped:
                    etat = "Stopped";
                    break;
                case Detached:
                    etat = "Detached";
                    break;
                case Killed:
                    etat = "Killed";
                    break;
                default:
                    etat = "Done";
                    break;
            }
            char *tmp = malloc(sizeof(int)*sizeof(job_parcours->id) + sizeof(pid_t)*sizeof(job_parcours->pid) + sizeof(char)*sizeof(etat) + sizeof(char)*sizeof(job_parcours->commande) + 17);
            sprintf(tmp, "[%d] %d %s %s\n", job_parcours->id, job_parcours->pid, etat, job_parcours->commande);
            write(STDERR_FILENO, tmp, strlen(tmp));
            free(tmp);
            break;
        }else{
            job_parcours = job_parcours->suivant;
        }
    }
}

// Affichage de tout les jobs avec leurs sous-jobs.
void affichage_job_option(){
    job_info *job_parcours = job_info_list->job_first;
    while(job_parcours != NULL){
        if (job_parcours->sous_job == NULL){
            affichage_job_unique(job_parcours->id);
        }else{
            job_info *tmp2 = job_parcours->sous_job;
            while(tmp2 != NULL){
                char *etat = "";
                switch (tmp2->etat){
                    case Running:
                        etat = "Running";
                        break;
                    case Stopped:
                        etat = "Stopped";
                        break;
                    case Detached:
                        etat = "Detached";
                        break;
                    case Killed:
                        etat = "Killed";
                        break;
                    default:
                        etat = "Done";
                        break;
                }
                if(tmp2->pid == job_parcours->pid){
                    char *tmp = malloc(sizeof(int)*sizeof(job_parcours->id) + sizeof(pid_t)*sizeof(tmp2->pid) + sizeof(char)*sizeof(etat) + sizeof(char)*sizeof(tmp2->commande) + 17);
                    sprintf(tmp, "\n[%d] %d %s %s\n", job_parcours->id, tmp2->pid, etat, tmp2->commande);
                    write(STDERR_FILENO, tmp, strlen(tmp));
                    free(tmp);
                }else{
                    char *tmp = malloc(sizeof(int)*sizeof(tmp2->id) + sizeof(pid_t)*sizeof(tmp2->pid) + sizeof(char)*sizeof(etat) + sizeof(char)*sizeof(tmp2->commande) + 17);
                    sprintf(tmp, "    %d %s %s\n", tmp2->pid, etat, tmp2->commande);
                    write(STDERR_FILENO, tmp, strlen(tmp));
                    free(tmp); 
                }
                tmp2 = tmp2->suivant;
            }
        }
        job_parcours = job_parcours->suivant;
    }
}

// Affichage d'un job spécifié par son id avec ses sous-jobs.
void affichage_job_option_unique(int id){
    job_info *job_parcours = job_info_list->job_first;
    while(job_parcours != NULL){
        if(job_parcours->id == id){
            if (job_parcours->sous_job == NULL){
                affichage_job_unique(job_parcours->id);
            }else{
                job_info *tmp2 = job_parcours->sous_job;
                while(tmp2 != NULL){
                    char *etat = "";
                    switch (tmp2->etat){
                        case Running:
                            etat = "Running";
                            break;
                        case Stopped:
                            etat = "Stopped";
                            break;
                        case Detached:
                            etat = "Detached";
                            break;
                        case Killed:
                            etat = "Killed";
                            break;
                        default:
                            etat = "Done";
                            break;
                    }
                    if(tmp2->pid == job_parcours->pid){
                        char *tmp = malloc(sizeof(int)*sizeof(job_parcours->id) + sizeof(pid_t)*sizeof(tmp2->pid) + sizeof(char)*sizeof(etat) + sizeof(char)*sizeof(tmp2->commande) + 17);
                        sprintf(tmp, "\n[%d] %d %s %s\n", job_parcours->id, tmp2->pid, etat, tmp2->commande);
                        write(STDERR_FILENO, tmp, strlen(tmp));
                        free(tmp);
                    }else{
                        char *tmp = malloc(sizeof(int)*sizeof(tmp2->id) + sizeof(pid_t)*sizeof(tmp2->pid) + sizeof(char)*sizeof(etat) + sizeof(char)*sizeof(tmp2->commande) + 17);
                        sprintf(tmp, "    %d %s %s\n", tmp2->pid, etat, tmp2->commande);
                        write(STDERR_FILENO, tmp, strlen(tmp));
                        free(tmp); 
                    }
                    tmp2 = tmp2->suivant;
                }
            }
            valRetour = 0;
            return;
        }
        job_parcours = job_parcours->suivant;
    }
    valRetour = 1;
}

// Modifie l'état d'un job spécifié par son pid.
void modif_job_etat(pid_t pid, enum etats etat){
    job_info *job_parcours = job_info_list->job_first;
    while (job_parcours != NULL)
    {
        if(job_parcours->pid == pid){
            job_parcours->etat = etat;
            break;
        }
        job_parcours = job_parcours->suivant;
    }
    if(job_parcours == NULL){
        print_error(Unknow_Job);
        valRetour = -1;
    }
}

// Actualise l'état des jobs surveillés, les affiches, puis les supprimes de la liste.
void check_all_jobs(){
    job_info *job_parcours = job_info_list->job_first;
    while (job_parcours != NULL){
        pid_t pid = job_parcours -> pid;
        job_info *tmp = job_parcours->suivant;
        int status;
        //WNOHANG pour ne pas bloquer
        if(job_parcours->sous_job != NULL){
            job_info *tmp2 = job_parcours->sous_job;
            while(tmp2 != NULL){
                if(waitpid(tmp2->pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0){
                    enum etats new_etat = status_job(status);
                    if(tmp2 -> etat != new_etat){
                        modif_sous_job_etat(pid, tmp2->pid, new_etat);
                    }
                }
                tmp2 = tmp2->suivant;
            }
            getMoyenneJob(job_parcours);
            if(job_parcours->etat == Done || job_parcours->etat == Killed || job_parcours->etat == Detached){
                remove_job(job_parcours);
            }
        }else{
            if(waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0){
                enum etats new_etat = status_job(status);
                if(job_parcours -> etat != new_etat){
                    modif_job_etat(pid, new_etat);
                    affichage_job_unique(job_parcours->id);
                    if(job_parcours->etat == Done || job_parcours->etat == Killed || job_parcours->etat == Detached){
                        remove_job(job_parcours);
                    }
                    
                }
            }
        }
        job_parcours = tmp;
    }
}

// Actualise l'état des jobs surveillés sans les afficher (dédié à la commande job).
void check_all_jobs_no_print(){

    job_info *job_parcours = job_info_list->job_first;
    while (job_parcours != NULL){
        pid_t pid = job_parcours -> pid;
        int status;
        //WNOHANG pour ne pas bloquer
        if(job_parcours->sous_job != NULL){
            job_info *tmp = job_parcours->sous_job;
            while(tmp != NULL){
                if(waitpid(tmp->pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0){
                    enum etats new_etat = status_job(status);
                    if(tmp -> etat != new_etat){
                        modif_sous_job_etat(pid, tmp->pid, new_etat);
                    }
                }
                tmp = tmp->suivant;
            }
            getMoyenneJob(job_parcours);
        }else{
            if(waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0){
                enum etats new_etat = status_job(status);
                if(job_parcours -> etat != new_etat){
                    modif_job_etat(pid, new_etat);
                }
            }
        }
        job_parcours = job_parcours->suivant;
    } 
 
}

// Retire les jobs dont sa terminaison a été annoncée, ou qu'il est détaché (dédié à la commande job).
void remove_done(){
    job_info *job_parcours = job_info_list->job_first;
    while(job_parcours != NULL){
        job_info *tmp = job_parcours->suivant;
        if(job_parcours->etat == Done || job_parcours->etat == Killed || job_parcours->etat == Detached){
            remove_job(job_parcours);
        }
        job_parcours = tmp;
    }
}

// Pour récupérer le statut courant du job (Fait correspondre le signal reçu et son statut interne).
enum etats status_job(int status){
    if(WIFEXITED(status)){
        return Done;
    }
    else if(WIFSIGNALED(status)){
        return Killed;
    }
    else if(WIFSTOPPED(status)){
        return Stopped;
    }
    else{
        return Running;
    }
}

// Vidage de la mémoire liée à job.
void free_all(){
    free_jobs(job_info_list->job_first);
    free(job_info_list);
}
void free_jobs(job_info *liste_a_vider){
    if(liste_a_vider->sous_job != NULL){
        free_jobs(liste_a_vider->sous_job);
    }
    if(liste_a_vider->suivant != NULL){
        free_jobs(liste_a_vider->suivant);
    }
    free(liste_a_vider->commande);
    free(liste_a_vider);
}

// Renvoie le nmobre de job surveillé.
int get_nb_jobs(){
    int cpt = 0;
    job_info *tmp = job_info_list->job_first;
    while (tmp != NULL){
        if(tmp->etat == Running || tmp->etat == Stopped){
            cpt++;
        }
        tmp = tmp->suivant;
    }
    return cpt;
}

// Retire le job concerné de la liste.
void remove_job(job_info *job){
    job_info *job_parcours = job_info_list->job_first;
    if(job == job_parcours){
        job_info *tmp = job_parcours->suivant;
        job->suivant = NULL;
        free_jobs(job);
        job_info_list->job_first = tmp;
    }else{
        while(job_parcours->suivant != NULL){
            if(job == job_parcours->suivant){
                job_info *tmp = job->suivant;
                job->suivant = NULL;
                free_jobs(job);
                job_parcours->suivant = tmp;
                break;
            }
            job_parcours = job_parcours->suivant;
        }
        
    }
}

// Regroupe tout les erreurs possibles
void print_error(enum errors errors){
    switch (errors){
        case Pwd:
            perror("pwd");
            break;
        case Cd:
            perror("cd");
            break;
        case Fork:
            perror("Erreur lors de la création du processus fils");
            break;
        case Unknow_Command:
            perror("Erreur lors de l'exécution de la commande");
            break;
        case Waitpid:
            perror("waitpid");
            break;
        case Conversion:
            perror("Erreur de conversion");
            break;
        case Open_File:
            perror("Erreur lors de l'ouverture du fichier");
            break;
        case Jobs:
            perror("Jobs requis");
            break;
        case Unknow_Job:
            perror("Job inexistant");
            break;
        case Memoire:
            perror("Erreur allocation mémoire");
            break;
        case Argument:
            perror("Argument manquant");
            break;
        case Pipe :
            perror("Erreur lors de la création du tube");
            break;
        case Tcsetprgp :
            perror("Erreur tcsetprgp");
            break;
        case Kill :
            perror("Erreur d'envoi de signal (kill)");
            break;
        case Sigaction :
            perror("Erreur sigaction");
        default:
            break;
    }
}

// Création d'un job en attente d'être ajouté à la liste.
job_info* creerJob(pid_t pid, pid_t gpid, enum etats etat, char *command){
    job_info *newJob = malloc(sizeof(job_info));
    newJob->id = -1;
    newJob->pid = pid;
    newJob->gpid = gpid;
    newJob->etat = etat;
    newJob->commande = allouer_attribuer_char(command);
    newJob->suivant = NULL;
    newJob->sous_job = NULL;
    return newJob;
}

// Ajoute un sous-job à un job.
void ajouter_Sous_Job(job_info* group, job_info* fils){
    if(group->sous_job == NULL){
        group->sous_job = fils;
    }else{
        job_info *tmp = group->sous_job;
        while(tmp->suivant != NULL){
            tmp = tmp->suivant;
        }
        tmp->suivant = fils;
    }
}

// Ajoute un groupe de job à la liste.
void ajouter_job_group(job_info* job){
    job->id = id_static;
    if(job_info_list->job_first == NULL){
        job_info_list->job_first = job;
        job_info_list->job_last = job;
    }else{
        job_info_list->job_last->suivant = job;
        job_info_list->job_last = job;
    }
    id_static++;
}

// Modifie l'état d'un sous-job.
void modif_sous_job_etat(pid_t gpid, pid_t pid, enum etats etat){
    job_info *job_parcours = job_info_list->job_first;
    if(job_parcours == NULL){
        print_error(Unknow_Job);
        valRetour = -1;
    }
    while (job_parcours != NULL){
        if(job_parcours->gpid == gpid){
            job_info *tmp = job_parcours->sous_job;
            while(tmp != NULL){
                if(tmp->pid == pid){
                    tmp->etat = etat;
                    break;
                }
                tmp = tmp->suivant;
            }
            break;
        }
        job_parcours = job_parcours->suivant;
    }
}

// Actualise l'état du job en charge du groupe de job.
void getMoyenneJob(job_info* job){
    if(job->sous_job != NULL){
        job_info *tmp = job->sous_job;
        bool stp = false;
        bool run = false;
        bool don = false;
        while(tmp != NULL){
            if(tmp->etat == Stopped){
                stp = true;
            }else if(tmp->etat == Running){
                run = true;
            }else if(tmp->etat == Done){
                don = true;
            }
            tmp = tmp->suivant;
        }
        if(!stp && !run && don){
            job->etat = Done;
        }else if(stp){
            job->etat = Stopped;
        }else if(run){
            job->etat = Running;
        }
    }
}