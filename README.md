# JSH - Job Control Shell

Un interpréteur de commandes (shell) en C avec gestion des tâches (job control).

## Fonctionnalités

### Commandes internes
- `pwd` - Affiche le répertoire courant
- `cd [dir|-]` - Change de répertoire
- `?` - Affiche la valeur de retour de la dernière commande
- `exit [val]` - Quitte le shell
- `jobs [-t] [%job]` - Liste les jobs en cours
- `bg %job` - Relance un job en arrière-plan
- `fg %job` - Relance un job à l'avant-plan
- `kill [-sig] %job|pid` - Envoie un signal à un job/processus

### Redirections
- `<` - Redirection d'entrée
- `>` - Redirection de sortie (sans écrasement)
- `>|` - Redirection de sortie (avec écrasement)
- `>>` - Redirection de sortie (concaténation)
- `2>`, `2>|`, `2>>` - Redirections de la sortie d'erreur
- `|` - Tube (pipeline)
- `<( cmd )` - Substitution de processus

### Autres
- Exécution en arrière-plan avec `&`
- Prompt personnalisé avec nombre de jobs et répertoire courant
- Gestion des signaux (SIGINT, SIGTSTP, etc.)
- Historique des commandes (flèches haut/bas)

## Prérequis

- GCC
- Bibliothèque `readline`
- Système Linux (testé sur Ubuntu/Debian)

## Installation

```bash
# Compiler
make

# Exécuter
./jsh
```

## Architecture

Le projet est organisé en 6 modules principaux :

- **jsh.c** - Point d'entrée et boucle principale
- **command.c** - Gestion et exécution des commandes
- **redirection.c** - Gestion des redirections et pipes
- **jobs.c** - Gestion des jobs (création, surveillance, affichage)
- **signaux.c** - Gestion des signaux
- **auxiliaire.c** - Fonctions utilitaires

Voir [ARCHITECTURE.md](ARCHITECTURE.md) pour plus de détails.

## Utilisation

```bash
# Lancer une commande simple
[0]/home/user$ ls -l

# Exécuter en arrière-plan
[0]/home/user$ sleep 100 &
[1] 12345  Running  sleep 100

# Lister les jobs
[1]/home/user$ jobs
[1] 12345  Running  sleep 100

# Pipeline avec redirections
[1]/home/user$ cat fichier.txt | grep "motif" > resultat.txt

# Substitution de processus
[1]/home/user$ diff <( ls dir1 ) <( ls dir2 )
```

## Nettoyage

```bash
# Supprimer les fichiers compilés
make clean
```

## Tests

```bash
# Vérifier les fuites mémoire
make valgrind
```

## Auteurs

- @guillaume-xue
- @XiaoGunFr
- @MatthieuRbc

## 📄 Licence

MIT License - Voir [LICENSE](LICENSE) pour plus de détails.
