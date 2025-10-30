#include "../inc/executing.h"
#include "../inc/communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <fcntl.h>


void exec_bloc(char *bloc){
    printf(" Execution du bloc : %s\n",bloc);
    // tableau de tableau de string (cmd)
    int n=1;
    int fd;
    int n_pipe=0;
    bool redirection=false;
    bool background_cmd=false;

    cmd* cmd_list;
    if(cut_in_cmd(bloc,&cmd_list,&n,&redirection,&n_pipe)==-1){return;}
    
    // lire cmd_list pour test

    /*
    for(int i=0;i<n;i++){
        printf(" Commande %d :\n",i);
        for(int j=0;j<cmd_list[i].arg_number;j++){
            printf("  Arg %d : %s\n",j,cmd_list[i].args[j]);
        }
    }
    */

    // execution
    char* last_arg=cmd_list[n-1].args[cmd_list[n-1].arg_number-1];
    int len = strlen(last_arg);
    
    if(last_arg[len-1]=='&'){
        printf(" commande en arriere plan\n");
        background_cmd = true;
        last_arg[len-1] = '\0';
        // /!\ peut generer une commande nulle
        // -> déjà gérer par le shell de base
    }
    if(redirection==true){
        // ligne
        fd = open(cmd_list[n-1].args[0], O_WRONLY | O_CREAT | O_TRUNC, 0644); // trouvé sur internet
        if (fd == -1) {
            perror("open failed");
            exit(1);
        }
    }

    if(n_pipe==0){
        printf("commmande simple\n");

        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        }

        if (pid == 0) {
            if(redirection==true){
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            execvp(cmd_list[0].args[0], cmd_list[0].args);
            perror("execvp failed");
            exit(1);
        } else {
            //  Parent : attend la fin du processus enfant
            int status;
            if(!background_cmd){waitpid(pid, &status, 0);printf("on attend");}
            printf("La commande s'est terminée avec le code %d\n", WEXITSTATUS(status));

            printf("Suite du programme...\n");
        }
        printf("le papa");


    }

    // si il y a des tuyaux
    else{

        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        }

        if (pid == 0) {
            // enfant
            printf("il y a %d commandes",n);
            int pipes[n-1-redirection][2]; // tableau des pipes
            // - redirection car la commande avec redirection compte pour 2 commande
            pid_t pids[n];

            for (int i = 0; i < n - 1; i++) {
                if (pipe(pipes[i]) == -1) {
                    perror("pipe");
                    exit(1);
                }
            }


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
                    execvp(cmd_list[i].args[0], cmd_list[i].args);
                    perror("execvp"); // si exec échoue
                    exit(1);
                }
            }

            for (int i = 0; i < n - 1; i++) {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            // Attend la fin de tous les fils
            for (int i = 0; i < n; i++) {
                waitpid(pids[i], NULL, 0);
            }

            perror("execvp failed");
            exit(1);
        } else {
            //  Parent : attend la fin du processus enfant
            int status;
            if(!background_cmd){waitpid(pid, &status, 0);printf("on attend");}
            printf("La commande s'est terminée avec le code %d\n", WEXITSTATUS(status));
        }


    }

}
