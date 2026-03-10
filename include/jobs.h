/*
 * jobs.h - Gestion des processus en arriere-plan
 *
 * On stocke ici les infos de chaque job lance avec &
 * pour pouvoir les afficher avec la commande "jobs"
 */

#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

/* nombre max de jobs qu'on peut avoir en meme temps */
#define MAX_JOBS 64

/* les differents etats possibles d'un job */
typedef enum {
    JOB_RUNNING,   /* le processus tourne encore */
    JOB_DONE,      /* il a fini normalement */
    JOB_STOPPED    /* il a ete stoppe (pas utilise pour l'instant mais c'est prevu) */
} job_status_t;

/* structure qui represente un job en arriere-plan */
typedef struct {
    int id;              /* numero du job (1, 2, 3...) */
    pid_t pid;           /* PID du processus */
    job_status_t status; /* est-ce qu'il tourne, fini, stoppe... */
    char command[256];   /* la commande qu'on a tapee, pour l'affichage */
} job_t;

/* initialise le tableau de jobs (tout a zero) */
void jobs_init(void);

/* ajoute un nouveau job dans le tableau, renvoie son id */
int jobs_add(pid_t pid, const char *command);

/* supprime un job du tableau a partir de son PID */
void jobs_remove(pid_t pid);

/* met a jour le statut d'un job quand il se termine */
void jobs_update(pid_t pid, int status);

/* affiche tous les jobs en cours (commande interne "jobs") */
void jobs_print(void);

#endif /* JOBS_H */