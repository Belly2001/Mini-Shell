/*
 * pipes.c - Gestion des pipes
 *
 * Pour faire "! ls | ! grep .c", on doit :
 *   1) Creer un pipe (tableau de 2 descripteurs)
 *   2) Fork pour le premier processus (ls) :
 *      - rediriger sa sortie (stdout) vers le cote ecriture du pipe
 *   3) Fork pour le second processus (grep) :
 *      - rediriger son entree (stdin) depuis le cote lecture du pipe
 *   4) Fermer les descripteurs du pipe dans le pere
 *   5) Attendre que les deux fils finissent
 *
 * Pour N commandes, on a besoin de N-1 pipes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "pipes.h"
#include "redirections.h"

int exec_piped_commands(pipeline_t *pipeline)
{
    int n = pipeline->count;        /* nombre de commandes */
    int pipes[MAX_PIPE_CMDS][2];    /* tableau de pipes */
    pid_t pids[MAX_PIPE_CMDS];      /* PIDs des fils */
    struct sigaction sa;

    /* on cree les N-1 pipes necessaires */
    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            return -1;
        }
    }

    /* on lance chaque commande dans un processus fils */
    for (int i = 0; i < n; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            return -1;
        }

        if (pids[i] == 0) {
            /* --- FILS : commande numero i --- */

            /* on remet SIGINT au comportement par defaut */
            sa.sa_handler = SIG_DFL;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(SIGINT, &sa, NULL);

            /*
             * Si ce n'est PAS la premiere commande,
             * on branche stdin sur le pipe precedent (cote lecture).
             * Comme ca, cette commande lit ce que la precedente a ecrit.
             */
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            /*
             * Si ce n'est PAS la derniere commande,
             * on branche stdout sur le pipe suivant (cote ecriture).
             * Comme ca, la sortie va dans le pipe vers la commande suivante.
             */
            if (i < n - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            /* on ferme TOUS les descripteurs de pipe dans le fils */
            /* c'est tres important, sinon les pipes ne se ferment jamais */
            for (int j = 0; j < n - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            /* on applique les redirections specifiques (< et >) */
            if (apply_redirections(&pipeline->commands[i]) < 0) {
                exit(1);
            }

            /* on lance la commande */
            execvp(pipeline->commands[i].args[0],
                   pipeline->commands[i].args);

            /* si on arrive la, c'est que la commande n'existe pas */
            fprintf(stderr, "minishell: commande non trouvee: %s\n",
                    pipeline->commands[i].args[0]);
            exit(127);
        }
    }

    /* --- PERE : on ferme tous les pipes --- */
    /* si on ne ferme pas, les fils resteront bloques en lecture */
    for (int i = 0; i < n - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /* on attend que tous les fils finissent */
    for (int i = 0; i < n; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}