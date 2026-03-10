/*
 * jobs.c - Gestion des processus en arriere-plan
 *
 * Quand l'utilisateur lance une commande avec '&', on l'ajoute
 * ici dans un tableau. Quand le processus se termine (via SIGCHLD),
 * on met a jour son statut. La commande "jobs" affiche la liste.
 */

#include <stdio.h>
#include <string.h>

#include "jobs.h"

/* tableau global des jobs */
static job_t job_table[MAX_JOBS];

/* compteur pour les IDs de jobs (1, 2, 3...) */
static int next_job_id = 1;

/*
 * Initialise la table des jobs.
 * On met tout a zero pour etre sur qu'il n'y a pas de dechets.
 */
void jobs_init(void)
{
    memset(job_table, 0, sizeof(job_table));
    next_job_id = 1;
}

/*
 * Ajoute un job dans la table.
 * On cherche la premiere case libre (pid == 0) et on la remplit.
 * Renvoie l'id du job, ou -1 si la table est pleine.
 */
int jobs_add(pid_t pid, const char *command)
{
    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_table[i].pid == 0) {
            job_table[i].id = next_job_id++;
            job_table[i].pid = pid;
            job_table[i].status = JOB_RUNNING;
            /* on copie la commande pour pouvoir l'afficher plus tard */
            strncpy(job_table[i].command, command, sizeof(job_table[i].command) - 1);
            job_table[i].command[sizeof(job_table[i].command) - 1] = '\0';

            printf("[%d] %d\n", job_table[i].id, pid);
            return job_table[i].id;
        }
    }

    fprintf(stderr, "minishell: trop de jobs en arriere-plan\n");
    return -1;
}

/*
 * Supprime un job de la table (remet la case a zero).
 * On cherche le job par son PID.
 */
void jobs_remove(pid_t pid)
{
    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_table[i].pid == pid) {
            memset(&job_table[i], 0, sizeof(job_t));
            return;
        }
    }
}

/*
 * Met a jour le statut d'un job.
 * Appele par le handler SIGCHLD quand un fils se termine.
 */
void jobs_update(pid_t pid, int status)
{
    (void)status; /* on pourrait l'utiliser pour afficher le code de retour, plus tard */

    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_table[i].pid == pid) {
            job_table[i].status = JOB_DONE;
            /* on affiche que le job est termine */
            printf("\n[%d] Termine   %s (pid=%d)\n",
                   job_table[i].id, job_table[i].command, pid);
            return;
        }
    }
}

/*
 * Affiche tous les jobs en cours.
 * Les jobs termines sont affiches puis supprimes de la table.
 */
void jobs_print(void)
{
    int found = 0;

    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_table[i].pid != 0) {
            found = 1;
            const char *status_str;

            switch (job_table[i].status) {
                case JOB_RUNNING:
                    status_str = "En cours";
                    break;
                case JOB_DONE:
                    status_str = "Termine";
                    break;
                case JOB_STOPPED:
                    status_str = "Stoppe";
                    break;
                default:
                    status_str = "Inconnu";
            }

            printf("[%d] %-10s %s (pid=%d)\n",
                   job_table[i].id, status_str,
                   job_table[i].command, job_table[i].pid);

            /* si le job est termine, on le retire de la table */
            if (job_table[i].status == JOB_DONE) {
                memset(&job_table[i], 0, sizeof(job_t));
            }
        }
    }

    if (!found) {
        printf("Pas de jobs en arriere-plan.\n");
    }
}