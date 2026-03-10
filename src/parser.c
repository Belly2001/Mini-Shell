/*
 * parser.c - Analyse de la ligne de commande
 *
 * Le parsing se fait en deux etapes :
 * 1) On decoupe la ligne par les pipes '|' pour avoir les sous-commandes
 * 2) On parse chaque sous-commande individuellement pour extraire
 *    les arguments, redirections, le '!' et le '&'
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

/*
 * Lit une ligne depuis stdin.
 * On utilise getline() qui alloue la memoire automatiquement.
 * On retire le '\n' a la fin si y'en a un.
 * Renvoie NULL si l'utilisateur fait Ctrl+D (EOF).
 */
char *read_line(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    nread = getline(&line, &len, stdin);
    if (nread == -1) {
        /* EOF ou erreur */
        free(line);
        return NULL;
    }

    /* on vire le retour a la ligne */
    if (nread > 0 && line[nread - 1] == '\n') {
        line[nread - 1] = '\0';
    }

    return line;
}

/*
 * Parse un pipeline complet : on decoupe par '|'
 * puis on parse chaque morceau comme une commande simple.
 *
 * Exemple : "! ls -l | ! grep .c"
 *  -> commande 1 : "! ls -l"
 *  -> commande 2 : "! grep .c"
 */
int parse_pipeline(char *line, pipeline_t *pipeline)
{
    char *copy;
    char *segment;
    char *saveptr;

    /* on reinitialise la structure */
    memset(pipeline, 0, sizeof(pipeline_t));
    pipeline->count = 0;

    /* on fait une copie parce que strtok modifie la chaine */
    copy = strdup(line);
    if (copy == NULL) {
        perror("strdup");
        return -1;
    }

    /* on decoupe par '|' */
    segment = strtok_r(copy, "|", &saveptr);
    while (segment != NULL && pipeline->count < MAX_PIPE_CMDS) {
        /* on parse chaque segment comme une commande */
        if (parse_command(segment, &pipeline->commands[pipeline->count]) == 0) {
            pipeline->count++;
        }
        segment = strtok_r(NULL, "|", &saveptr);
    }

    free(copy);
    return 0;
}

/*
 * Parse une commande simple (sans pipe).
 * On detecte :
 *   - le '!' au debut -> commande externe
 *   - le '&' a la fin -> execution en arriere-plan
 *   - le '<' -> redirection de l'entree
 *   - le '>' -> redirection de la sortie
 *
 * Le reste, ce sont les arguments de la commande.
 */
int parse_command(char *input, command_t *cmd)
{
    char *token;
    char *saveptr;
    char *trimmed;

    /* on initialise tout a zero */
    memset(cmd, 0, sizeof(command_t));
    cmd->argc = 0;
    cmd->is_external = 0;
    cmd->is_background = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;

    /* on saute les espaces au debut */
    trimmed = input;
    while (*trimmed == ' ' || *trimmed == '\t') {
        trimmed++;
    }

    /* si c'est vide, rien a faire */
    if (*trimmed == '\0') {
        return -1;
    }

    /* on verifie si ca commence par '!' -> commande externe */
    if (*trimmed == '!') {
        cmd->is_external = 1;
        trimmed++; /* on saute le '!' */
        /* on saute les espaces apres le '!' */
        while (*trimmed == ' ' || *trimmed == '\t') {
            trimmed++;
        }
    }

    /* on decoupe par espaces/tabulations */
    token = strtok_r(trimmed, " \t", &saveptr);
    while (token != NULL) {

        /* redirection de l'entree */
        if (strcmp(token, "<") == 0) {
            token = strtok_r(NULL, " \t", &saveptr);
            if (token == NULL) {
                fprintf(stderr, "minishell: nom de fichier manquant apres '<'\n");
                return -1;
            }
            cmd->input_file = strdup(token);
        }
        /* redirection de la sortie */
        else if (strcmp(token, ">") == 0) {
            token = strtok_r(NULL, " \t", &saveptr);
            if (token == NULL) {
                fprintf(stderr, "minishell: nom de fichier manquant apres '>'\n");
                return -1;
            }
            cmd->output_file = strdup(token);
        }
        /* execution en arriere-plan */
        else if (strcmp(token, "&") == 0) {
            cmd->is_background = 1;
            /* le & doit etre le dernier token normalement */
        }
        /* argument normal */
        else {
            if (cmd->argc < MAX_ARGS - 1) {
                cmd->args[cmd->argc] = strdup(token);
                cmd->argc++;
            }
        }

        token = strtok_r(NULL, " \t", &saveptr);
    }

    /* le tableau d'args doit se terminer par NULL (requis par execvp) */
    cmd->args[cmd->argc] = NULL;

    return (cmd->argc > 0) ? 0 : -1;
}