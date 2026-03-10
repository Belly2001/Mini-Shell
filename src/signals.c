/*
 * signals.c - Gestion des signaux
 *
 * On utilise sigaction() pour installer les handlers,
 * exactement comme dans le cours (structureSigAction.c).
 *
 * Deux signaux importants :
 * - SIGCHLD : un fils en arriere-plan a termine -> on le recupere
 *   avec waitpid pour eviter les zombies (cf exempleZombie.c du cours)
 * - SIGINT  : Ctrl+C -> on l'ignore dans le shell parent
 *   (sinon Ctrl+C tuerait notre shell, pas cool)
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "signals.h"
#include "jobs.h"

/*
 * Handler pour SIGCHLD.
 * Quand un fils se termine, le noyau envoie SIGCHLD au pere.
 * On utilise waitpid avec WNOHANG pour ne pas bloquer :
 * si aucun fils n'est termine, on revient tout de suite.
 *
 * On fait une boucle parce que plusieurs fils peuvent se terminer
 * en meme temps (et un seul signal SIGCHLD est envoye).
 */
void sigchld_handler(int sig)
{
    pid_t pid;
    int status;
    int saved_errno = errno; /* on sauvegarde errno, bonne pratique dans un handler */

    (void)sig; /* on sait que c'est SIGCHLD, pas besoin de verifier */

    /* on boucle tant qu'il y a des fils a recuperer */
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        /* on met a jour la table des jobs */
        jobs_update(pid, status);
    }

    errno = saved_errno; /* on restaure errno */
}

/*
 * Met en place les handlers de signaux.
 * Appele une seule fois au demarrage du shell.
 */
void setup_signals(void)
{
    struct sigaction sa;

    /*
     * Handler pour SIGCHLD :
     * on veut recuperer les fils en arriere-plan quand ils terminent.
     * SA_RESTART : si un appel systeme (comme read) est interrompu
     * par le signal, il est relance automatiquement.
     * Sans ca, read_line() pourrait echouer apres chaque SIGCHLD.
     */
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        perror("sigaction SIGCHLD");
        exit(1);
    }

    /*
     * Pour SIGINT (Ctrl+C) : on l'ignore dans le shell parent.
     * Les fils remetront le comportement par defaut avant exec.
     */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction SIGINT");
        exit(1);
    }
}