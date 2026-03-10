/*
 * signals.h - Gestion des signaux
 *
 * On configure ici les handlers pour :
 * - SIGCHLD : quand un fils en arriere-plan se termine
 * - SIGINT  : quand l'utilisateur fait Ctrl+C
 *
 * C'est important pour eviter les processus zombies
 * et pour que Ctrl+C ne tue pas notre shell.
 */

#ifndef SIGNALS_H
#define SIGNALS_H

/* met en place tous les handlers de signaux */
void setup_signals(void);

/*
 * handler pour SIGCHLD : appele automatiquement quand
 * un processus fils se termine. On fait un waitpid en
 * non-bloquant pour recuperer le statut et mettre a jour
 * la liste des jobs.
 */
void sigchld_handler(int sig);

#endif /* SIGNALS_H */