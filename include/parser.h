/*
 * parser.h - Analyse de la ligne de commande
 *
 * C'est ici qu'on decoupe la ligne tapee par l'utilisateur
 * en morceaux utilisables : la commande, les arguments,
 * les redirections, les pipes, etc.
 */

#ifndef PARSER_H
#define PARSER_H

/* nombre max d'arguments pour une commande */
#define MAX_ARGS 64

/* nombre max de commandes dans un pipe (ex: cmd1 | cmd2 | cmd3 = 3) */
#define MAX_PIPE_CMDS 16

/* taille max de la ligne de commande */
#define MAX_LINE 1024

/*
 * Structure qui contient toutes les infos extraites d'une commande.
 * Apres le parsing, on sait exactement quoi faire avec.
 */
typedef struct {
    char *args[MAX_ARGS];    /* tableau des arguments (args[0] = la commande) */
    int argc;                /* nombre d'arguments */
    int is_external;         /* 1 si la commande commence par '!' */
    int is_background;       /* 1 si la commande finit par '&' */
    char *input_file;        /* fichier de redirection en entree (< fichier), NULL sinon */
    char *output_file;       /* fichier de redirection en sortie (> fichier), NULL sinon */
} command_t;

/*
 * Structure pour gerer les pipes : on peut avoir plusieurs commandes
 * chainees par des |, donc on stocke un tableau de commandes.
 */
typedef struct {
    command_t commands[MAX_PIPE_CMDS]; /* les commandes du pipeline */
    int count;                         /* combien de commandes dans le pipe */
} pipeline_t;

/* lit une ligne depuis l'entree standard, renvoie NULL si EOF (Ctrl+D) */
char *read_line(void);

/*
 * parse une ligne de commande complete
 * gere les pipes en decoupant d'abord par '|'
 * puis parse chaque sous-commande individuellement
 */
int parse_pipeline(char *line, pipeline_t *pipeline);

/*
 * parse une commande simple (sans pipe)
 * detecte le !, le &, les redirections < et >
 */
int parse_command(char *input, command_t *cmd);

#endif /* PARSER_H */