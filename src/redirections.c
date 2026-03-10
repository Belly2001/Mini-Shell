/*
 * redirections.c - Redirections d'entree et de sortie
 *
 * On utilise dup2() pour remplacer stdin ou stdout
 * par un fichier. Ca marche comme ca :
 *   1) on ouvre le fichier avec open()
 *   2) on duplique le descripteur sur stdin (0) ou stdout (1) avec dup2()
 *   3) on ferme l'ancien descripteur
 *
 * Rappel : cette fonction doit etre appelee DANS LE FILS,
 * apres le fork, pour ne pas casser les descripteurs du shell.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "redirections.h"

int apply_redirections(command_t *cmd)
{
    int fd;

    /* redirection de l'entree standard : cmd < fichier */
    if (cmd->input_file != NULL) {
        fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0) {
            perror(cmd->input_file);
            return -1;
        }
        /* on remplace stdin par notre fichier */
        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("dup2 (input)");
            close(fd);
            return -1;
        }
        close(fd); /* on n'a plus besoin du fd original */
    }

    /* redirection de la sortie standard : cmd > fichier */
    if (cmd->output_file != NULL) {
        /*
         * O_WRONLY : ouverture en ecriture
         * O_CREAT  : creer le fichier s'il n'existe pas
         * O_TRUNC  : vider le fichier s'il existe deja
         * 0644     : droits rw-r--r--
         */
        fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror(cmd->output_file);
            return -1;
        }
        /* on remplace stdout par notre fichier */
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2 (output)");
            close(fd);
            return -1;
        }
        close(fd);
    }

    return 0;
}