#include "../include/auxiliaire.h"

/*
Variable globale pour stocker les liens
entre l'ordre d'éxécution des processus
et le processus qui doit être éxécuté
avec ce fichier descripteur.
*/
int * stock = NULL;

//Pour découper une chaîne de caractères en fonction d'un séparateur
char** cut_string(char* str, char* sep) {
    //On crée le tableau de chaînes
    char** res = calloc(256, sizeof(char*));
    //Nombre d'occurences du séparateur
    int occ = 0;
    //Position dans la chaîne str
    int pos = 0;
    int i = 0;
    //Nombre de mots ajoutés
    int nmbAjoute = 0;
    //Tant qu'on arrive pas à la fin de la chaîne str
    while (str[i + pos] != '\0') {
        //On vérifie si le caractère est égal au caractère 0 du séparateur
        if (str[i + pos] == sep[0]) {
            bool b = false;
            //On vérifie qu'il n'y a pas plusieurs fois le même séparateur d'affilé
            int cpt = 0;
            for (int j = 0; str[i + pos + j] != '\0'; j++){
                if(str[i + pos + j] == sep[cpt]){
                    cpt++;
                }else{
                    if(occ == 0){
                       b = true; 
                    }
                    break;
                }
                if(sep[cpt] == '\0'){
                    occ++;
                    cpt = 0;
                }
            }
            //Vérifie si les séparateurs dont on vient de vérifier sont à la fin du mot
            if(str[i + pos + (strlen(sep)*occ)] == '\0'){
                break;
            }
            //Si on est sorti alors on passe à l'itération suivante de la boucle while
            if (b) {
                ++i;
                continue;
            }
            //On alloue la mémoire pour le mot à ajouter
            res[nmbAjoute] = malloc((i + 1) * sizeof(char));
            if(res[nmbAjoute] == NULL){
                print_error(Memoire);
                return NULL;
            }
            //On copie
            memcpy(res[nmbAjoute], str + pos, i);
            //On ajoute le caractère de fin de chaîne
            res[nmbAjoute][i] = '\0';
            //On incrémente le nombre de mots ajoutés
            ++nmbAjoute;
            //On met à jour la position
            pos += i + (occ * strlen(sep));
            //On réinitialise i
            i = -1;
            occ = 0;
        }
        ++i;
    }
    //On alloue la mémoire pour le dernier mot
    res[nmbAjoute] = malloc((i + 1) * sizeof(char));
    if(res[nmbAjoute] == NULL){
        print_error(Memoire);
        return NULL;
    }
    //On copie
    memcpy(res[nmbAjoute], str + pos, i);
    res[nmbAjoute][i] = '\0';
    ++nmbAjoute;
    //On retaille le tableau
    res = realloc(res, (nmbAjoute + 1) * sizeof(char*));
    if(res == NULL){
        print_error(Memoire);
        return NULL;
    }
    return res;
}

/*
Calcul la position des "|" dit global
puis rappelle la fonction de découpage
et renvoie ce résultat
*/
char ** getCommandeByRedirExter(char * command){
    int** posOpen = nbOcc(command, " <( ");
    int** posClose = nbOcc(command, " )");
    int** link = linkTab(posOpen, posClose);
    int** nbApparation = nbOcc(command, " | ");
    int pos = 0;
    int* res = calloc(20, sizeof(int));
    for (int i = 0; i < tailleTableauInt(nbApparation); i++){
        bool status = false;
        for (int j = 0; j < tailleTableauInt(link); j++){
            if(nbApparation[i][0] > link[j][0] && nbApparation[i][0] < link[j][1]){
                status = true;
                break;
            }
        }
        if (!status){
            res[pos] = nbApparation[i][0];
            pos++;
        }
    }
    int* tmp = realloc(res, (pos+1) * sizeof(int));
    if(tmp == NULL){
        print_error(Memoire);
        return NULL;
    }
    res = tmp;
    res[pos] = -1; //On met un -1 pour savoir quand s'arrêter
    freeTabInt(posOpen);
    freeTabInt(posClose);
    freeTabInt(link);
    freeTabInt(nbApparation);
    char ** res2 = cutStringAuxRedirExter(command, res);
    free(res);
    return res2;
}

/*
Effectue la découpe de la chaine de caractère par 
"|" lorsqu'il est dit global :
Ex : "echo | echo 123 <( echo 456 | cat ) | echo 789"
Renverra :
echo
echo 123 <( echo 456 | cat )
echo 789
*/
char ** cutStringAuxRedirExter(char * command, int * pos){
    int taille = tailleTableauIntBis(pos);
    char ** res = calloc(20, sizeof(char*));
    int posRes = 0;
    int position = 0;
    while(posRes != taille){
        res[posRes] = calloc(pos[posRes]+1-position, sizeof(char*));
        memcpy(res[posRes], command+position, pos[posRes]-position);
        res[posRes][pos[posRes]-position] = '\0';
        position = pos[posRes]+3;
        posRes++;
    }
    if(strlen(command) > position){
        res[posRes] = calloc(strlen(command)-position+1, sizeof(char*));
        memcpy(res[posRes], command+position, strlen(command)-position);
        res[posRes][strlen(command)-position] = '\0';
        posRes++;
        char** tmp = realloc(res, (posRes+1) * sizeof(char*));
        if(tmp == NULL){
            print_error(Memoire);
            return NULL;
        }
        res = tmp;
        res[posRes] = NULL;
    }else{
        res[posRes] = NULL;
    }
    return res;
}

/*
Permet de faire correspondre chaque position
de chaque substitution de processus,
permet surtout de déterminer si on a des cas
imbriqués ou à la suite :
Ex : "ls -l <( cat <( ls -l ) <( ls -l ) )"
Renvoie :
5 34
12 21
23 32
On en déduit qu'il y'a une substitution global
de débute à la position 5 et se termine à la
position 34, puis deux substition qui s'enchaine
et qui sont dans la première substitution.
*/
int ** linkTab(int ** tab1, int ** tab2){
    int taille1 = tailleTableauInt(tab1);
    int taille2 = tailleTableauInt(tab2);
    int** link = calloc(20, sizeof(int*));
    int posLink = 0;
    if(taille1 > 1 && taille2 > 1){
        for (int i = 0; i < taille1; i++){
            int cpt = 1;
            bool tmp = false;
            for(int j = tab1[i][0]+1; j <= tab2[taille2-1][0]; j++){
                for (int k = i; k < taille1; k++){
                    if(tab1[k][0] == j){
                        cpt++;
                    }
                }
                for (int k = 0; k < taille2; k++){
                    if(tab2[k][0] == j){
                        cpt--;
                    }
                    if (cpt == 0)
                    {
                        link[posLink] = malloc(2 * sizeof(int*));
                        link[posLink][0] = tab1[i][0];
                        link[posLink][1] = tab2[k][0];
                        posLink++;
                        tmp = true;
                        break;
                    }
                }
                if(tmp){
                    break;
                }
            }
        }
    }
    int** tmp = realloc(link, (posLink+1) * sizeof(int*));
    if(tmp == NULL){
        print_error(Memoire);
        return NULL;
    }
    link = tmp;
    return link;
}

//Retire le & à la fin d'une chaîne de caractères
bool removeBackSign(char * command){
    if(strcmp(command, "&") == 0){
        command = NULL;
        return true;
    }else{
        bool tmp  = false;
        for (int i = strlen(command)-1; i > 0; i--){
            if(tmp && command[i] == ' '){
                command[i] = '\0';
                return true;
            }
            if(command[i] == '&'){
                tmp = true;
                command[i] = '\0';
            }else if(command[i] != ' '){
                return false;
            }
        }
        return false;
    }
}

//Fusionne un tableau de chaînes de caractères en une seule chaîne de caractères
char * mergeChar(char ** command){
    char * res = calloc(256, sizeof(char*));
    if(res == NULL){
        print_error(Memoire);
        return NULL;
    }
    for (int i = 0; i < tailleTableau(command); i++){
        memcpy(res+strlen(res), command[i], strlen(command[i]));
        res[strlen(res)] = ' ';
    }
    char* tmp = realloc(res, strlen(res)+1);
        if(tmp == NULL){
            print_error(Memoire);
            return NULL;
        }
    res = tmp;
    res[strlen(res)-1] = '\0';
    return res;
}

/*
Renvoie le découpage d'une chaîne de caractère
en les séparant par "<( ... )" sous la forme
d'une pile : "echo <( echo 123 <( echo 456 ) ) <( echo 789 )" 
renverra un char** :
echo /dev/fd/ /dev/fd/
echo 123 /dev/fd/
echo 456
echo 789
*/
char ** getCommandeImbrique(char * command){
    char ** res = calloc(20, sizeof(char*));
    if(res == NULL){
        print_error(Memoire);
        return NULL;
    }
    int cpt = 0;
    int pos = 0;
    int * value = calloc(20, sizeof(int));
    for (int i = 1; i < 20; i++){
        value[i] = -1;
    }
    while (command[pos] != '\0'){
        if(res[cpt] == NULL){
            res[cpt] = calloc(256, sizeof(char*));
            if(res[cpt] == NULL){
                print_error(Memoire);
                return NULL;
            }
        }
        if(strncmp(command+pos, " <( ", 4) == 0){
            memcpy(res[cpt]+strlen(res[cpt]), " /dev/fd/", 9);
            for (int i = 0; i < 20; i++)
            {
                if(value[i] == -1){
                    value[i] = cpt;
                    cpt = i;
                    break;
                }
            }
            pos += 4;
            continue;
        }
        if(strncmp(command+pos, " )", 2) == 0){
            cpt = value[cpt];
            if(pos+2 >= strlen(command)){
                break;
            }
            pos += 2;
            continue;
        }
        memcpy(res[cpt]+strlen(res[cpt]), command+pos, 1);
        pos++;
    }
    for (int i = 0; i < tailleTableau(res); i++){
        char* tmp = realloc(res[i], strlen(res[i])+1 * sizeof(char));
        if(tmp == NULL){
            print_error(Memoire);
            return NULL;
        }
        tmp[strlen(tmp)] = '\0';
        res[i] = tmp;
    }
    int* tmp = realloc(value, (tailleTableauIntBis(value)+1) * sizeof(int));
    if(tmp == NULL){
        print_error(Memoire);
        return NULL;
    }
    value = tmp;
    free(stock);
    stock = value;
    return res;
}

//Vérifie si une chaîne de caractères contient une autre chaîne de caractères
bool ifContain(char * command, char * str){
    int pos = 0;
    while(command[pos] != '\0'){
        if(strncmp(command+pos, str, strlen(str)) == 0){
            return true;
        }
        pos++;
    }
    return false;
}

/*
Renvoie le nombre d'occurence d'un séparateur 
dans une chaîne de caractères
et leur position correspondante
*/
int** nbOcc(char* str, char* sep){
    int** res = malloc(20 * sizeof(int*));
    int posTab = 0;
    int occ = 0;
    int pos = 0;
    int i = 0;
    while(str[i + pos] != '\0'){
        if (str[i + pos] == sep[0]){
            bool b = false;
            int cpt = 0;
            for (int j = 0; str[i + pos + j] != '\0'; j++){
                if(str[i + pos + j] == sep[cpt]){
                    cpt++;
                }else{
                    if(occ == 0){
                       b = true; 
                    }
                    break;
                }
                if(sep[cpt] == '\0'){
                    occ++;
                    cpt = 0;
                    break;
                }
            }
            if (b){
                ++i;
                continue;
            }
            pos += i;
            res[posTab] = malloc(1 * sizeof(int));
            res[posTab][0] = pos;
            pos += (occ * strlen(sep));
            i = -1;
            occ = 0;
            posTab++;
        }
        i++;
    }
    int** tmp = realloc(res, (posTab+1) * sizeof(int*));
    if(tmp == NULL){
        print_error(Memoire);
        return NULL;
    }
    res = tmp;
    res[posTab] = NULL;
    return res;
}

// Fonction auxiliaire d'allocation mémoire et soustraction de '&' dans le nom.
char *allouer_attribuer_char(char *mot){
    char *found = strrchr(mot, '&');
    if (found != NULL) {
        int cpt = 0;
        for (int i = 0; i < strlen(mot); i++){
            if((mot[i] == ' ' && mot[i+1] == '&')){
                break;
            }
            cpt++;
        }
        char *tmp = malloc(cpt + 1);
        strncpy(tmp,mot,cpt);
        tmp[cpt] = '\0';
        return tmp;
    }
    char *zone = malloc(sizeof(char) * strlen(mot)+1);
    strcpy(zone, mot);
    zone[strlen(mot)] = '\0';
    return zone;
}

//Vidage mémoire char**/int**
void freeTab(char** t) {
    int i = 0;
    while (t[i] != NULL) {
        free(t[i]);
        i++;
    }
    free(t);
}
void freeTabInt(int** t) {
    int i = 0;
    while (t[i] != NULL) {
        free(t[i]);
        i++;
    }
    free(t);
}

//Récupère la taille d'un tableau char**/int**/int*
int tailleTableau(char **tab){
    int i = 0;
    while(tab[i] != NULL){
        i++;
    }
    return i;
}
int tailleTableauInt(int **tab){
    int i = 0;
    while(tab[i] != NULL){
        i++;
    }
    return i;
}
int tailleTableauIntBis(int *tab){
    int i = 0;
    while(tab[i] != -1){
        i++;
    }
    return i;
}

