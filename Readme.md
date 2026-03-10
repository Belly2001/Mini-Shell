# Mini-Shell

Un mini shell développé en langage C, inspiré du fonctionnement d'un shell Unix/Linux.

## Fonctionnalités

- **Commandes internes** : `cd`, `pwd`, `mkdir`, `exit`, `jobs`
- **Commandes externes** (préfixées par `!`) : `! ls -l`, `! cp fichier1 fichier2`
- **Exécution en arrière-plan** : `! sleep 10 &`
- **Gestion des jobs** : suivi des processus en arrière-plan avec `jobs`
- **Redirections** : `! cat < fichier.txt`, `! ls > resultat.txt`
- **Pipes** : `! ls | ! grep .c`

## Compilation

```bash
make
```

## Exécution

```bash
./minishell
```

## Utilisation

```
mini-shell> pwd
mini-shell> cd /tmp
mini-shell> mkdir mon_dossier
mini-shell> ! ls -l
mini-shell> ! cat < fichier.txt
mini-shell> ! ls > resultat.txt
mini-shell> ! sleep 10 &
mini-shell> jobs
mini-shell> ! ls | ! grep .c
mini-shell> exit
```

## Structure du projet

```
Mini-Shell/
├── include/          # Fichiers d'en-tête (.h)
│   ├── parser.h
│   ├── builtins.h
│   ├── executor.h
│   ├── jobs.h
│   ├── redirections.h
│   ├── pipes.h
│   └── signals.h
├── src/              # Fichiers source (.c)
│   ├── main.c
│   ├── parser.c
│   ├── builtins.c
│   ├── executor.c
│   ├── jobs.c
│   ├── redirections.c
│   ├── pipes.c
│   └── signals.c
├── Makefile
└── README.md
```

## Nettoyage

```bash
make clean    # Supprime les fichiers objets
make fclean   # Supprime les fichiers objets et l'exécutable
make re       # Recompile tout de zéro
```