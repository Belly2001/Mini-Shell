/*
 * executor.c - Execution des commandes externes
 *
 * C'est ici qu'on fait le fameux fork + exec.
 * Comme on l'a vu en cours (exemplePedagogique.c) :
 *   1) fork() pour creer un fils
 *   2) dans le fils : on fait execvp() pour lancer la commande
 *   3) dans le pere : on attend le fils (ou pas, si c'est en arriere-plan)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "executor.h"
#include "redirections.h"
#include "pipes.h"
#include "jobs.h"

/*
 * Reconstruit la commande sous forme de chaine pour l'affichage.
 * Utilise pour stocker la commande dans la table des jobs.
 */
static void build_command_string(command_t *cmd, char *buf, size_t size)
{
    buf[0] = '\0';
    for (int i = 0; i < cmd->argc; i++) {
        if (i > 0) strncat(buf, " ", size - strlen(buf) - 1);
        strncat(buf, cmd->args[i], size - strlen(buf) - 1);
    }
}

/*
 * Execute une commande externe simple (sans pipe).
 * Gere le fork, les redirections, le background et le exec.
 */
int exec_external(command_t *cmd)
{
    pid_t pid;
    struct sigaction sa;

    pid = fork();

    if (pid < 0) {
        /* erreur de fork, c'est rare mais ca arrive */
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        /* --- On est dans le FILS --- */

        /*
         * On remet SIGINT au comportement par defaut dans le fils.
         * Comme ca, Ctrl+C tue la commande en cours mais pas le shell.
         */
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);

        /* on applique les redirections si y'en a */
        if (apply_redirections(cmd) < 0) {
            exit(1); /* si la redirection echoue, on quitte le fils */
        }

        /* on lance la commande avec execvp */
        /* execvp cherche la commande dans le PATH automatiquement */
        execvp(cmd->args[0], cmd->args);

        /* si on arrive ici, c'est que execvp a echoue */
        fprintf(stderr, "minishell: commande non trouvee: %s\n", cmd->args[0]);
        exit(127);
    }

    /* --- On est dans le PERE --- */

    if (cmd->is_background) {
        /* commande en arriere-plan : on n'attend pas */
        char cmdstr[256];
        build_command_string(cmd, cmdstr, sizeof(cmdstr));
        jobs_add(pid, cmdstr);
    } else {
        /* commande au premier plan : on attend que le fils finisse */
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}

/*
 * Execute un pipeline complet.
 * On delegue a la fonction dans pipes.c
 */
int exec_pipeline(pipeline_t *pipeline)
{
    return exec_piped_commands(pipeline);
}