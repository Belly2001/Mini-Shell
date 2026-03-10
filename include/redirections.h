/*
 * redirections.h - Gestion des redirections d'entree/sortie
 *
 * On gere ici les redirections avec < et >
 * En gros, on remplace stdin ou stdout par un fichier
 * en utilisant dup2().
 */

#ifndef REDIRECTIONS_H
#define REDIRECTIONS_H

#include "parser.h"

/*
 * Applique les redirections definies dans la commande.
 * Si input_file est defini, on redirige stdin depuis ce fichier.
 * Si output_file est defini, on redirige stdout vers ce fichier.
 *
 * ATTENTION : a appeler dans le processus fils (apres fork),
 * sinon on modifie les descripteurs du shell lui-meme !
 *
 * Renvoie 0 si tout va bien, -1 en cas d'erreur.
 */
int apply_redirections(command_t *cmd);

#endif /* REDIRECTIONS_H */