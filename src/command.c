#include "../include/command.h"

char oldpwd[1024];
char currentpwd[1024];
int valRetour = 0; //Correspond à ?

int pwd(){
    char cwd[256];
    //On utilise getcwd() pour avoir le chemin absolu vers le répertoire courant
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        print_error(Pwd);
        return 1;
    }else{
        // affichage du chemin sur la sorti standard
        write(STDOUT_FILENO,cwd, strlen(cwd));
        write(STDOUT_FILENO,"\n", 1);
        return 0;
    }
}

int cd(const char *path) {
    getcwd(oldpwd, sizeof(oldpwd));
    //On met à jour dans le cas de cd -
    if (path == NULL) {
        const char *home = getenv("HOME"); //Accès au path référencé par $HOME
        if (home == NULL || chdir(home) != 0) {
            print_error(Cd);
            return 1;
        }
    }
    //Revenir au répertoire précédent
    else if (strcmp(path, "-") == 0) {
        if (chdir(currentpwd) != 0) {
            print_error(Cd);
            return 1;
        }
    }
    //Dans le cas où on a un chemin
    else {
        if (chdir(path) != 0) {
            print_error(Cd);
            return 1;
        }
    }
    strcpy(currentpwd, oldpwd);
    return 0;
}

//Execute la commande sans fork
void extern_command_direct(char **args){
    execvp(args[0], args);
    exit(EXIT_FAILURE);
}

//Execute la commande avec fork (Cas sans redirection)
int extern_command(char *command){
    int bg2 = 0;
    if(removeBackSign(command)){
        bg2 = 1;
    }
    char **args = cut_string(command, " "); //On découpe la chaîne en tableau
    pid_t pid = fork(); //On crée un processus

    switch (pid) {
        case -1:
            print_error(Fork);
            return 1;
        case 0:
            gestion_signal_reset();
            setpgid(0,0);
            //On applique les signaux nécessaire pour s'éxectuer en arrière plan
            if(bg2 == 1){
                gestion_signal_arrière_plan();  
            }
            extern_command_direct(args);
            exit(EXIT_FAILURE);
        default:
            //On ignore les signaux SIGTTOU et SIGGTIN pour éviter des lectures ou écritures illégales
            ignore_sigttou_sigttin();
            setpgid(pid, 0);
            int status = 0;
            if(bg2 == 0){
                //Mettre le groupe de processus à l'avant plan
                tcsetpgrp(STDIN_FILENO, pid);
                waitpid(pid, &status, WUNTRACED); 
                //Remettre le shell à l'avant plan
                tcsetpgrp(STDIN_FILENO, getpgrp());
                /*
                En cas de processus stoppé,
                on ajoute le job à la liste des jobs 
                et on affiche les informations du job,
                on restaure les signaux SIGTTOU et SIGGTIN
                */
                if(status_job(status) == Stopped){
                    ajouter_job(pid, Stopped, command);
                    affichage_job_unique(getJob_bypid(pid)->id);
                    restore_sigttou_sigttin();
                    return 0;
                }
            }else{
                /*
                Processus en arrière plan,
                on ajoute le job à la liste des jobs
                et on affiche les informations du job
                */
                ajouter_job(pid, Running, command);  
                affichage_job_unique(getJob_bypid(pid)->id);
            }
            //On restaure les signaux SIGTTOU et SIGGTIN
            restore_sigttou_sigttin();
            freeTab(args);
            return WEXITSTATUS(status);
    }
}

int lastVal(){ //Correspond à ?
    char buffer[20];
    //On converti l'entier pour pouvoir l'afficher avec write
    int tailleValRetour = sprintf(buffer, "%d", valRetour); 
    //Si la conversion à réussie
    if (tailleValRetour >= 0) { 
        //On utilise write pour écrire sur la sortie standard
        write(STDOUT_FILENO, buffer, tailleValRetour);
        write(STDOUT_FILENO, "\n", 1);
        return 0;
    } else {
        print_error(Conversion);
        return 1;
    }
}

//Liste des commandes prise en charge par notre shell
void command(char * input, char * prompt){
    int nb;
    // Fin d'entrée ou ctrl + D
    if(input == NULL){
        free(prompt);
        free(input);
        exit(valRetour);
    }
    // Si on enlève ça, valeur de retour remise à 0
    else if(strcmp(input, "") == 0){
    }
    // Affiche la référence physique absolue du répertoire de travail courant.
    else if (strcmp(input, "pwd") == 0) {
        valRetour = pwd();
    }
    // Change de répertoire de travail courant
    else if(strncmp(input, "cd", 2) == 0){
        char *arg = strtok(input + 2, " ");
        valRetour = cd(arg);
    }
    // Affiche la valeur de retour de la dernière commande exécutée.
    else if (strcmp(input, "?") == 0) {
        valRetour = lastVal();
    }
    // Execution de exit
    else if(strncmp(input, "exit", 4) == 0){ //si exit
        char *arg = strtok(input + 4, " "); //on découpe l'entrée
        if(arg == NULL){ //on vérifie si argument ou pas
            if(get_nb_jobs() != 0){
                valRetour = 1;
            }else{
                free(prompt);
                free(input);
                exit(valRetour);
            }
        }else{
            if(get_nb_jobs() != 0){
                valRetour = 1;
            }else{
                //On utilise strtol pour convertir la chaîne
                long value = strtol(arg, NULL, 10); 
                free(prompt);
                free(input);
                exit((int) value);
            }
        }
    }
    // Execution de bg
    else if(strncmp(input, "bg", 2) == 0){
        char *arg = strtok(input + 2, " ");
        if(arg == NULL){
            print_error(Jobs);
        }
        else{
            char *num = arg+1;
            long value = strtol(num, NULL, 10);
            job_info *tmp = getJob(value);
            if(tmp == NULL) {
                valRetour = 1;
            }else{
                valRetour = bg(tmp->pid);
            }
        }
    }
    // Execution de fg
    else if(strncmp(input, "fg", 2) == 0){
        char *arg = strtok(input + 2, " ");
        if(arg == NULL){
            print_error(Jobs);
        }
        else{
            char *num = arg+1;
            long value = strtol(num, NULL, 10);
            job_info *tmp = getJob(value);
            if(tmp == NULL) {
                valRetour = 1;
            }else{
                valRetour = fg(tmp->pid);
            }
        }
    }
    // Execution des redirection
    else if((nb = isRedirectionAndType(input)) >= 0){
        valRetour = redirection(nb, input);
    }
    // Execution des jobs
    else if(strncmp(input, "jobs", 4) == 0){
        check_all_jobs_no_print();
        char **args = cut_string(input, " ");
        if(tailleTableau(args) == 1){
            affichage_job();
        }else{
            if(strcmp(args[1], "-t") == 0){
                if(args[2] != NULL){
                    long int value = 0;
                    value = strtol(args[2]+1, NULL, 10);
                    affichage_job_option_unique(value);
                }else{
                    affichage_job_option();
                }
            }else{
                if(args[1] != NULL){
                    long int value = 0;
                    value = strtol(args[1]+1, NULL, 10);
                    affichage_job_unique(value);
                }else{
                    affichage_job();
                }
            }
        }
        valRetour = 0;
        freeTab(args);
        remove_done();
    }
    // Execution de kill
    else if(strncmp(input, "kill", 4) == 0){
        char *arg = strtok(input, " ");
        char *num1 = strtok(NULL, " ");
        char *num2 = strtok(NULL, " ");
        if(arg == NULL){
            print_error(Jobs);
        }else{
            //Découpage et conversion des arguments en int
            long int signal_type = SIGTERM;
            long int value = -1;
            long int pid = -1;
            if(num2 != NULL){
                if(num2[0] == '%'){
                    value = strtol(num2+1, NULL, 10);
                }else{
                    pid = strtol(num2, NULL, 10);
                }
                signal_type = abs(strtol(num1, NULL, 10));
            }else{
                if(num1[0] == '%'){
                    value = strtol(num1+1, NULL, 10);
                }else{
                    pid = strtol(num1, NULL, 10);
                }
            }
            //Récupération du job et envoie du signal
            job_info *tmp = NULL;
            if(value != -1){
                tmp = getJob(value);
            }else{
                tmp = getJob_bypid(pid);
            }
            if(tmp == NULL) {
                print_error(Unknow_Job);
                valRetour = 1;
            }else{
                valRetour = kill2(signal_type, tmp->pid);
            }
        }
    }
    // Execution des commandes externe
    else{
        valRetour = extern_command(input);
    }
}

