
/* Multi-pipe example
 * Author: Adapté d’un code d’Alexandre Coninx
 * 2025-10-30
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    // Liste des commandes
    char *cmds[][4] = {
        {"ls", "-l", NULL},    // cmd1
        {"grep", "^d", NULL},  // cmd2
        {"wc", "-l", NULL}     // cmd3
    };
    int n = 3; // nombre de commandes

    int pipes[n-1][2]; // tableau des pipes
    pid_t pids[n];

    // Création des pipes
    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    // Boucle de création des processus
    for (int i = 0; i < n; i++) {
        pids[i] = fork();

        if (pids[i] == -1) {
            perror("fork");
            exit(1);
        }

        if (pids[i] == 0) {
            // Processus fils

            // Si ce n’est pas le premier, on lit depuis le pipe précédent
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // Si ce n’est pas le dernier, on écrit vers le pipe suivant
            if (i < n - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Ferme tous les descripteurs de pipe inutilisés
            for (int j = 0; j < n - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Exécute la commande
            execvp(cmds[i][0], cmds[i]);
            perror("execvp"); // si exec échoue
            exit(1);
        }
    }

    // Ferme tous les descripteurs dans le processus parent
    for (int i = 0; i < n - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Attend la fin de tous les fils
    for (int i = 0; i < n; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return 0;
}
