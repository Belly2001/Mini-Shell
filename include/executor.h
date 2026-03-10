/*
 * executor.h - Execution des commandes externes
 *
 * Quand l'utilisateur tape une commande avec '!' devant,
 * c'est ici qu'on gere le fork + exec pour la lancer.
 */

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

/*
 * Execute une commande externe simple (pas de pipe).
 * Gere le fork, le exec, le background si besoin,
 * et les redirections.
 */
int exec_external(command_t *cmd);

/*
 * Execute un pipeline complet (plusieurs commandes reliees par |).
 * Cree les pipes necessaires et lance chaque commande.
 */
int exec_pipeline(pipeline_t *pipeline);

#endif /* EXECUTOR_H */