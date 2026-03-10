/*
 * builtins.h - Commandes internes du shell
 *
 * Les commandes internes sont celles qu'on execute directement
 * dans le processus du shell, sans fork. C'est le cas de cd,
 * pwd, mkdir, exit et jobs.
 */

#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h"

/*
 * Verifie si la commande est une commande interne.
 * Renvoie 1 si c'est le cas, 0 sinon.
 */
int is_builtin(const char *cmd);

/*
 * Execute la commande interne correspondante.
 * Renvoie 0 si tout s'est bien passe, -1 en cas d'erreur.
 */
int exec_builtin(command_t *cmd);

/* Les fonctions de chaque commande interne */
int builtin_cd(command_t *cmd);
int builtin_pwd(void);
int builtin_mkdir(command_t *cmd);
int builtin_exit(void);
int builtin_jobs(void);

#endif /* BUILTINS_H */