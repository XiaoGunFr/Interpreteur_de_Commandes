#include "../include/redirection.h"

/*
Verifier si la commande est un redirection 
et renvoie le type de redirection.
Si la commande est un redirection normal
renvoie 0-6, pipe renvoie 7
et substitution 8, sinon -1.
*/
int isRedirectionAndType(char * command){
    char ** args = cut_string(command, " ");
    // redirection Pipe
    for (int i = 0; i < tailleTableau(args); i++) {
        if (strcmp(args[i], "|") == 0) {
            freeTab(args);
            return 7;
        } else if (strcmp(args[i], "<(") == 0) {
            freeTab(args);
            return 8;
        }
    }
    // redirection normal
    for (int i = 0; i < tailleTableau(args); i++) {
        if (strcmp(args[i], "2>>") == 0) {
            freeTab(args);
            return 0;
        } else if (strcmp(args[i], "2>|") == 0) {
            freeTab(args);
            return 1;
        } else if (strcmp(args[i], "2>") == 0) {
            freeTab(args);
            return 2;
        } else if (strcmp(args[i], ">>") == 0) {
            freeTab(args);
            return 3;
        } else if (strcmp(args[i], ">|") == 0) {
            freeTab(args);
            return 4;
        } else if (strcmp(args[i], ">") == 0) {
            freeTab(args);
            return 5;
        } else if (strcmp(args[i], "<") == 0) {
            freeTab(args);
            return 6;
        }
    }
    freeTab(args);
    return -1;
}

//Boucle fermant les descripteurs de fichiers
void closeFile(int file[], int taille){
    for (int i = 0; i != taille ; ++i) {
        close(file[i]);
    }
}

/*
Ouverture des descripteurs de fichiers
avec leur droit correspondant.
*/
int openFile(int file[], char ** args){
    int cpt = 0;
    for (int i = 0; i < tailleTableau(args); ++i) {
            // Redirection de la commande en concaténation (>>) et (2>>)
        if ((strcmp(args[i], "2>>") == 0) || (strcmp(args[i], ">>") == 0)) {
            file[cpt] = open(args[i+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
            if (file[cpt] < 0) return 1;
            cpt++;
            // Redirection de la commande avec écrasement (>|) et (2>|)
        } else if ((strcmp(args[i], "2>|") == 0) || (strcmp(args[i], ">|") == 0)) {
            file[cpt] = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (file[cpt] < 0) return 1;
            cpt++;
            // Redirection de la commande sans écrasement, échoue si fic existe déjà (>) et (2>)
        } else if ((strcmp(args[i], "2>") == 0) || (strcmp(args[i], ">") == 0)) {
            file[cpt] = open(args[i+1], O_WRONLY | O_CREAT | O_EXCL, 0666);
            if (file[cpt] < 0) return 1;
            cpt++;
            // Redirection de la commande (<).
        } else if (strcmp(args[i], "<") == 0) {
            file[cpt] = open(args[i+1], O_RDONLY, 0666);
            if (file[cpt] < 0) return 1;
            cpt++;
        }
    }
    return 0;
}

//Renvoie le nombre de redirection dans la commande
int nb_Redir(char ** args){
    int cpt = 0;
    for (int i = 0; i < tailleTableau(args); ++i){
        if(strcmp(args[i], "2>>") == 0 ||
        strcmp(args[i], "2>|") == 0 ||
        strcmp(args[i], "2>") == 0 ||
        strcmp(args[i], ">>") == 0 ||
        strcmp(args[i], ">|") == 0 ||
        strcmp(args[i], ">") == 0 ||
        strcmp(args[i], "<") == 0){
            cpt++;
        }
    }
    return cpt;
}

//Renvoie la commande découpé par la redirection donnée
char ** getFirstCmd(int type, char * command){
    switch (type) {
        case 0: return cut_string(command, " 2>> ");
        case 1: return cut_string(command, " 2>| ");
        case 2: return cut_string(command, " 2> ");
        case 3: return cut_string(command, " >> ");
        case 4: return cut_string(command, " >| ");
        case 5: return cut_string(command, " > ");
        case 6: return cut_string(command, " < ");
        default: return NULL;
    }
}

//Duplique les descripteurs de fichiers
void dupFile(int file[], char ** args){
    int cpt = 0;
    for (int i = 0; i < tailleTableau(args); ++i) {
            // Redirection de fichier sur l'erreur standard
        if ((strcmp(args[i], "2>>") == 0) || (strcmp(args[i], "2>") == 0) || (strcmp(args[i], "2>|") == 0)) {
            dup2(file[cpt], STDERR_FILENO);
            close(file[cpt]);
            cpt++;
            // Redirection de fichier sur la sortie standard
        } else if ((strcmp(args[i], ">>") == 0) || (strcmp(args[i], ">|") == 0) || (strcmp(args[i], ">") == 0)) {
            dup2(file[cpt], STDOUT_FILENO);
            close(file[cpt]);
            cpt++;
            // Redirection de fichier sur l'entrée standard
        } else if (strcmp(args[i], "<") == 0) {
            dup2(file[cpt], STDIN_FILENO);
            close(file[cpt]);
            cpt++;
        }
    }
}

//Execute la commande avec une redirection fichier
int redirectionFile(int type, char * command, bool mode){
    char **args = cut_string(command, " ");
    int nb_cmd = nb_Redir(args);
    // Initialiser le tableau des descripteurs de fichiers
    int file[nb_cmd];
    // Ouverture des descripteurs de fichiers
    if(openFile(file, args) == 1){
        print_error(Open_File);
        freeTab(args);
        return 1;
    }
    // Avoir la premiere commande
    char **firstCmd = getFirstCmd(type, command);
    char **tmp = cut_string(firstCmd[0], " ");
    int bg2 = 0;
    if (mode){
        if(removeBackSign(args[nb_cmd-1])){
            bg2 = 1;
        }
    }
    pid_t pid = fork();

    switch (pid) {
        case -1:
            print_error(Fork);
            // Libérer la mémoire dans en cas d'erreur
            freeTab(args);
            freeTab(firstCmd);
            freeTab(tmp);
            closeFile(file, nb_cmd);
            return 1;
        case 0:
            if(mode){
                gestion_signal_reset();
                //On crée un nouveau groupe de processus avec le fils comme leader
                setpgid(0,0);
                //On exécute la commande
                if(bg2 == 1){
                    gestion_signal_arrière_plan();  
                }
            }
            // Rediriger vers le fichier
            dupFile(file, args);
            // Exécuter la commande
            extern_command_direct(tmp);
        default:
            // Attendre que le processus fils se termine
            waitpid(pid, NULL, 0);
    }
    if(mode){
        ignore_sigttou_sigttin();
        int status = 0;
        //On attend que le processus fils soit terminé
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
    }else{
        waitpid(pid, NULL, 0);
    }
    restore_sigttou_sigttin();
    freeTab(args);
    freeTab(firstCmd);
    freeTab(tmp);
    return 0;
}

//Execute la commande avec une redirection Pipe
int redirectionPipe(char * command, bool mode){
    int bg2 = 0;
    if (mode){
        if(removeBackSign(command)){
            bg2 = 1;
        }
    }
    char ** args = NULL;

    char** tmp = cut_string(command, " <( ");
    if (tailleTableau(tmp) > 1){
        args = getCommandeByRedirExter(command);
    }else{
        args = cut_string(command, " | ");
    }
    freeTab(tmp);

    int nb_cmd = tailleTableau(args);
    pid_t pids[nb_cmd];
    int pipes[nb_cmd-1][2];
    int nb = 0;

    for (int i = 0; i < nb_cmd; i++){
        char **args_bis = cut_string(args[i], " ");
        if(i < nb_cmd-1){
            pipe(pipes[i]);
        }
        switch (pids[i] = fork()){
            case -1:
                print_error(Fork);
                return 1;
            case 0:
                if(mode){
                    gestion_signal_reset();
                    //On crée un nouveau groupe de processus avec le fils comme leader
                    if(i == 0){
                        setpgid(0,0);
                    }else{
                        setpgid(0, pids[0]);
                    }
                    //On exécute la commande
                    if(bg2 == 1){
                        gestion_signal_arrière_plan();  
                    }
                }
                if(i == 0){
                    dup2(pipes[0][1], STDOUT_FILENO);
                    close(pipes[0][0]);
                    close(pipes[0][1]);
                }else if(i == nb_cmd-1){
                    dup2(pipes[i-1][0], STDIN_FILENO);
                    close(pipes[i-1][0]);
                }else{
                    dup2(pipes[i-1][0], STDIN_FILENO);
                    dup2(pipes[i][1], STDOUT_FILENO);
                    close(pipes[i-1][0]);
                    close(pipes[i][0]);
                    close(pipes[i][1]);
                }
                if (ifContain(args[i], " <( ")){
                    exit(redirectionProcess(args[i], false));
                }else if ((nb = isRedirectionAndType(args[i])) != -1){
                    exit(redirectionFile(nb, args[i], false));
                    
                }else{
                    extern_command_direct(args_bis);
                }
            default:
                if(i == 0){
                    close(pipes[0][1]);
                }else if(i == nb_cmd - 1){
                    close(pipes[i-1][0]);
                }
                else{
                    close(pipes[i-1][0]);
                    close(pipes[i][1]);
                }
                if(mode){
                    
                    setpgid(pids[i], pids[0]);
                    
                }
        }
        freeTab(args_bis);
    }

    job_info* jobTmp = creerJob(pids[0], pids[0], Running, command);

    if (mode){
        for(int i = 0; i < nb_cmd; i++){
            job_info* newJob = creerJob(pids[i], pids[0], Running, args[i]);
            ajouter_Sous_Job(jobTmp, newJob);
        }
        ignore_sigttou_sigttin();
        int status = 0;
        //On attend que le processus fils soit terminé
        if(bg2 == 0){
            for (int i = 0; i < nb_cmd; i++){                
                //Mettre le groupe de processus à l'avant plan
                tcsetpgrp(STDIN_FILENO, pids[0]);
                waitpid(pids[i], &status, WUNTRACED); 
                //Remettre le shell à l'avant plan
                tcsetpgrp(STDIN_FILENO, getpgrp());
                enum etats etat = status_job(status);
                job_info* sousJob = jobTmp->sous_job;
                while (sousJob != NULL){
                    if(sousJob->pid == pids[i]){
                        sousJob->etat = etat;
                        break;
                    }
                    sousJob = sousJob->suivant;
                }
                /*
                En cas de processus stoppé,
                on ajoute le groupe du job à la liste des jobs,
                on actualise le status global du groupe,
                et on affiche les informations du job avec l'id du groupe,
                on restaure les signaux SIGTTOU et SIGGTIN
                */
                if(etat == Stopped){
                    getMoyenneJob(jobTmp);
                    ajouter_job_group(jobTmp);
                    affichage_job_unique(getJob_bypid(pids[0])->id);
                    restore_sigttou_sigttin();
                    return 0;
                }  
            }
            free_jobs(jobTmp);
        }else{
            ajouter_job_group(jobTmp);
            affichage_job_unique(getJob_bypid(pids[0])->id);
        }
        restore_sigttou_sigttin();
    }else{
        free_jobs(jobTmp);
        for (int i = 0; i < nb_cmd; i++){
            waitpid(pids[i], NULL, 0);
        }
        
    }
    freeTab(args);
    return 0;
}

//Execute la commande avec une redirection Processus
int redirectionProcess(char * command, bool mode){
    int bg2 = 0;
    if (mode){
        if(removeBackSign(command)){
            bg2 = 1;
        }
    }

    char ** args = getCommandeImbrique(command);
    int nb_cmd = tailleTableau(args);
    pid_t pids[nb_cmd];
    int pipes[nb_cmd][2];
    int nb = 0;
    //Initialise les pipes
    for (int i = 0; i < nb_cmd; i++){
        if(pipe(pipes[i]) == -1){
            print_error(Pipe);
            return 1;
        }
    }
    /*
    Dans chaque commande, on remplace les /dev/fd/
    par le numéro du fichier descripteur correspondant
    */
    for (int i = 0; i < tailleTableau(args); i++){
        char **args_bis = cut_string(args[i], " ");
        int cpt = 0;
        for (int y = 0; y < tailleTableau(args_bis); y++){
            if(ifContain(args_bis[y], "/dev/fd/")){
                cpt++;
                int cpt2 = 0;
                int pos = 0;
                for (int z = i+1; z < tailleTableauIntBis(stock); z++)
                {
                    if(stock[z] == i){
                        cpt2++;
                        if(cpt2 == cpt){
                            pos = z;
                            break;
                        }
                    }
                }
                char * tmp = calloc(12, sizeof(char));
                sprintf(tmp, "/dev/fd/%d", pipes[pos][0]);
                for (int t = 9; t < 12; t++){
                    if(tmp[t] < '0' || tmp[t] > '9'){
                        tmp[t] = '\0';
                        break;
                    }
                }
                free(args_bis[y]);
                args_bis[y] = tmp;
                
            }
            
        }
        char* tmp2 = mergeChar(args_bis);
        freeTab(args_bis);
        free(args[i]);
        args[i] = tmp2;
    }
    
    for (int i = nb_cmd-1; i > 0; i--){
        switch (pids[i] = fork()){
            case -1:
                print_error(Fork);
                return 1;
            case 0:
                if(mode){
                    gestion_signal_reset();
                    if(bg2 == 1){
                        gestion_signal_arrière_plan();  
                    }
                }
                    dup2(pipes[i][1], STDOUT_FILENO);

                if (ifContain(args[i], " | ")){
                    exit(redirectionPipe(args[i], false));
                }else if ((nb = isRedirectionAndType(args[i])) != -1){
                    exit(redirectionFile(nb, args[i], false));
                }else{
                    char** args_bis = cut_string(args[i], " ");
                    extern_command_direct(args_bis);
                }
            default:
                close(pipes[i][1]);
        }
        
    }

    /*
    Execute la première commande dont tout les 
    substituants ont été éxécutés
    */
    if((pids[0] = fork()) == 0){
        if(mode){
            gestion_signal_reset();
            //On crée un nouveau groupe de processus avec le fils comme leader
            setpgid(0,0);
            if(bg2 == 1){
                gestion_signal_arrière_plan();  
            }
        }
        if (ifContain(args[0], " | ")){
            exit(redirectionPipe(args[0], false));
        }else if ((nb = isRedirectionAndType(args[0])) != -1){
            exit(redirectionFile(nb, args[0], false));
        }else{
            char** args_bis = cut_string(args[0], " ");
            extern_command_direct(args_bis);
        }
    }
    int status = 0;
    job_info* jobTmp = creerJob(pids[0], pids[0], Running, command);
    if(mode){
        ignore_sigttou_sigttin();
        if(bg2 == 0){
            tcsetpgrp(STDIN_FILENO, pids[0]);
            waitpid(pids[0], &status, WUNTRACED);
            tcsetpgrp(STDIN_FILENO, getpgrp());
            enum etats etat = status_job(status);
            /*
            En cas de processus stoppé,
            on ajoute le groupe du job à la liste des jobs,
            et on affiche les informations du job avec l'id du groupe,
            on restaure les signaux SIGTTOU et SIGGTIN
            */
            if(etat == Stopped){
                ajouter_job_group(jobTmp);
                affichage_job_unique(getJob_bypid(pids[0])->id);
                restore_sigttou_sigttin();
                return 0;
            }
            free_jobs(jobTmp);
        }else{
            ajouter_job_group(jobTmp);
            affichage_job_unique(getJob_bypid(pids[0])->id);
        }
        restore_sigttou_sigttin();
    }
    for (int i = 1; i < nb_cmd; i++){
        close(pipes[i][0]);
    }
    freeTab(args);
    return 0;
}

//Détermine quel type de redirection est utilisé et l'éxécute
int redirection(int type, char * command){
    if(type == 7){
        return redirectionPipe(command, true);
    } else if(type == 8){
        return redirectionProcess(command, true);
    }else{
        return redirectionFile(type, command, true);
    }
    return 0;
}