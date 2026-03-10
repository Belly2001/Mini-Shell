/*
 * pipes.h - Gestion des pipes entre commandes
 *
 * Quand on a une commande du style "! ls | ! grep .c",
 * on cree un tube (pipe) pour connecter la sortie de ls
 * a l'entree de grep.
 */

#ifndef PIPES_H
#define PIPES_H

#include "parser.h"

/*
 * Execute un pipeline : lance chaque commande dans un fils,
 * en connectant stdout de chaque commande a stdin de la suivante
 * via des pipes.
 *
 * Renvoie 0 si tout va bien, -1 en cas d'erreur.
 */
int exec_piped_commands(pipeline_t *pipeline);

#endif /* PIPES_H */