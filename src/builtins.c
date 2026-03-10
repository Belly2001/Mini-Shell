/*
 * builtins.c - Implementation des commandes internes
 *
 * Les commandes internes sont executees directement par le shell,
 * sans creer de nouveau processus. C'est obligatoire pour des
 * commandes comme cd (sinon le changement de repertoire se ferait
 * dans le fils et pas dans le shell).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "builtins.h"
#include "jobs.h"

/*
 * Liste des commandes internes qu'on supporte.
 * On compare le nom de la commande avec cette liste.
 */
static const char *builtin_names[] = {
    "cd",
    "pwd",
    "mkdir",
    "exit",
    "jobs",
    NULL  /* sentinelle pour marquer la fin */
};

/*
 * Verifie si la commande donnee est une commande interne.
 * On parcourt la liste et on compare.
 */
int is_builtin(const char *cmd)
{
    if (cmd == NULL) return 0;

    for (int i = 0; builtin_names[i] != NULL; i++) {
        if (strcmp(cmd, builtin_names[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * Dispatche la commande vers la bonne fonction interne.
 * C'est un gros if/else, mais c'est clair et simple.
 */
int exec_builtin(command_t *cmd)
{
    if (strcmp(cmd->args[0], "cd") == 0)
        return builtin_cd(cmd);
    else if (strcmp(cmd->args[0], "pwd") == 0)
        return builtin_pwd();
    else if (strcmp(cmd->args[0], "mkdir") == 0)
        return builtin_mkdir(cmd);
    else if (strcmp(cmd->args[0], "exit") == 0)
        return builtin_exit();
    else if (strcmp(cmd->args[0], "jobs") == 0)
        return builtin_jobs();

    return -1;
}

/*
 * cd : change le repertoire courant
 * - sans argument : va dans le HOME
 * - avec un argument : va dans le repertoire donne
 */
int builtin_cd(command_t *cmd)
{
    const char *path;

    if (cmd->argc < 2) {
        /* pas d'argument -> on va dans HOME */
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "cd: variable HOME non definie\n");
            return -1;
        }
    } else {
        path = cmd->args[1];
    }

    if (chdir(path) != 0) {
        /* chdir a echoue, on affiche l'erreur */
        perror("cd");
        return -1;
    }

    return 0;
}

/*
 * pwd : affiche le repertoire courant
 * On utilise getcwd() pour le recuperer.
 */
int builtin_pwd(void)
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("pwd");
        return -1;
    }

    printf("%s\n", cwd);
    return 0;
}

/*
 * mkdir : cree un repertoire
 * On attend au moins un argument (le nom du repertoire).
 * Les droits par defaut : rwxr-xr-x (0755)
 */
int builtin_mkdir(command_t *cmd)
{
    if (cmd->argc < 2) {
        fprintf(stderr, "mkdir: nom de repertoire manquant\n");
        return -1;
    }

    /* on cree chaque repertoire passe en argument */
    for (int i = 1; i < cmd->argc; i++) {
        if (mkdir(cmd->args[i], 0755) != 0) {
            /* si ca rate (existe deja, pas les droits, etc.) */
            fprintf(stderr, "mkdir: impossible de creer '%s': %s\n",
                    cmd->args[i], strerror(errno));
        }
    }

    return 0;
}

/*
 * exit : quitte le shell
 * On utilise exit(0) tout simplement.
 */
int builtin_exit(void)
{
    printf("Au revoir !\n");
    exit(0);
    return 0; /* jamais atteint, mais ca evite un warning */
}

/*
 * jobs : affiche la liste des processus en arriere-plan
 * On delegue a la fonction dans jobs.c
 */
int builtin_jobs(void)
{
    jobs_print();
    return 0;
}