#include "../include/jsh.h"

char* prompt() {
    char cwd[256];
    char *rep_courant = getcwd(cwd, sizeof(cwd));
    int len = strlen(rep_courant);
    int tailleJobs;
    if(get_nb_jobs() > 0){
        tailleJobs = log10((double)(get_nb_jobs())) + 1; //Récupère le nombre de chiffre de l'entier
    }
    else{
        tailleJobs = 1;
    }
    char *nbrJobs = malloc(tailleJobs + 1); //On alloue en fonction du nombre de chiffres de l'entier
    sprintf(nbrJobs, "%d", get_nb_jobs());
    char *jobsColor1 = "\001\033[32m\002["; //Balise couleur 1
    char *jobsColor2 = "]\001\033[34m\002"; //Balise couleur 2
    char *jobsPrint = malloc(strlen(nbrJobs) + strlen(jobsColor1) + strlen(jobsColor2) + 1);
    strcpy(jobsPrint, jobsColor1);
    strcat(jobsPrint, nbrJobs);
    strcat(jobsPrint, jobsColor2);
    char *jobsValue = malloc(strlen(jobsPrint) + 1);
    strcpy(jobsValue, jobsPrint);
    if(len > 30){ //Si ça dépasse de la capacité
        int diff = len-30; //On regarde à partir de quel endroit on va pouvoir afficher
        rep_courant = rep_courant + diff + 6 + 2; // +6 corresponds aux caractères des jobsValue et +2 au "$ "
        jobsValue = realloc(jobsValue, strlen(jobsValue) + 3 + 1); // +3 pour les ...
        strcat(jobsValue, "...");
    }
    char *prompt = malloc(strlen(jobsValue) + 1);
    strcpy(prompt, jobsValue); //On copie la première partie de la chaîne dans le prompt
    prompt = realloc(prompt, strlen(prompt) + strlen(rep_courant) + 1); //On réalloue pour avoir asser de place
    strcat(prompt, rep_courant); //On concatène
    char *balise = "\001\033[0m\002$ ";
    prompt = realloc(prompt, strlen(prompt) + strlen(balise) + 1); //On réalloue une dernière fois pour la balise
    strcat(prompt, balise);

    //On libère la mémoire
    free(jobsPrint);
    free(jobsValue); 
    free(nbrJobs);
    return prompt;
}

int main(void) {  
    fprintf (stderr,"\033c");
    init();   
    gestion_signaux_jsh();    
    while (1) {   
        char *p = prompt();
        rl_outstream = stderr;   
        char *input = readline(p);
        command(input, p); // execute les commandes
        check_all_jobs(); // Actualise les jobs surveillés
        add_history(input); //Pour réutiliser les commandes avec la flèche du haut
        free(input);
        free(p);   
    }
    free_all();
    
    return EXIT_SUCCESS;   
}

