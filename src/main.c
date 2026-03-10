/*
 * main.c - Point d'entree du Mini-Shell
 *
 * C'est ici que tout commence : on initialise les signaux et les jobs,
 * puis on entre dans la boucle principale qui lit les commandes
 * et les execute une par une.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "builtins.h"
#include "executor.h"
#include "jobs.h"
#include "signals.h"

int main(void)
{
    char *line;
    pipeline_t pipeline;

    /* on configure les signaux des le debut */
    setup_signals();

    /* on initialise la table des jobs */
    jobs_init();

    printf("=== Mini-Shell ===\n");
    printf("Tapez 'exit' pour quitter.\n\n");

    /* boucle principale du shell */
    while (1) {
        /* afficher le prompt */
        printf("mini-shell> ");
        fflush(stdout);

        /* lire la ligne de commande */
        line = read_line();
        if (line == NULL) {
            /* l'utilisateur a fait Ctrl+D, on quitte proprement */
            printf("\n");
            break;
        }

        /* si la ligne est vide, on passe au tour suivant */
        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        /* on parse la ligne pour savoir ce qu'il faut faire */
        if (parse_pipeline(line, &pipeline) < 0) {
            /* erreur de parsing, on affiche un message et on continue */
            fprintf(stderr, "minishell: erreur de syntaxe\n");
            free(line);
            continue;
        }

        /* cas ou il n'y a rien a executer */
        if (pipeline.count == 0) {
            free(line);
            continue;
        }

        /*
         * Si c'est une seule commande (pas de pipe),
         * on regarde si c'est une commande interne
         */
        if (pipeline.count == 1) {
            command_t *cmd = &pipeline.commands[0];

            if (!cmd->is_external && is_builtin(cmd->args[0])) {
                /* commande interne : on l'execute directement */
                exec_builtin(cmd);
            } else if (cmd->is_external) {
                /* commande externe simple : fork + exec */
                exec_external(cmd);
            } else {
                /* la commande n'est ni interne ni prefixee par '!' */
                fprintf(stderr, "minishell: commande inconnue '%s'\n", cmd->args[0]);
                fprintf(stderr, "Utilisez '!' pour les commandes externes (ex: ! ls)\n");
            }
        } else {
            /* pipeline : plusieurs commandes reliees par des pipes */
            exec_pipeline(&pipeline);
        }

        free(line);
    }

    printf("Au revoir !\n");
    return 0;
}