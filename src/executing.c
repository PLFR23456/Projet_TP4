#include "../inc/executing.h"
#include "../inc/communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <fcntl.h>

void free_cmd_list(cmd* cmd_list, int n);

void exec_bloc(char *bloc){
    if(DEBUG_PRINT){printf("\t -- Execution du bloc : %s\n",bloc);}
    // tableau de tableau de string (cmd)
    int n=1;
    int n_pipe=0;
    bool redirection=false;
    bool background_cmd=false;

    cmd* cmd_list;
    if(cut_in_cmd(bloc,&cmd_list,&n,&redirection,&n_pipe)==-1){return;}
    
    // lire cmd_list pour test

    if(DEBUG_PRINT){
        for(int i=0;i<n;i++){
            printf(" Commande %d :\n",i);
            for(int j=0;j<cmd_list[i].arg_number;j++){
                printf("  Arg %d : %s\n",j,cmd_list[i].args[j]);
            }
        }
    }

    // execution
    char* last_arg=cmd_list[n-1].args[cmd_list[n-1].arg_number-1];
    int len = strlen(last_arg);
    
    if(last_arg[len-1]=='&'){
        if(DEBUG_PRINT){printf("* commande en arriere plan\n");}
        background_cmd = true;
        last_arg[len-1] = '\0';
        // /!\ peut generer une commande nulle
        // -> déjà gérer par le shell de base
    }

    if(n_pipe==0){
        // pwd
        if(strcmp(cmd_list[0].args[0],"pwd") == 0){
            char cwd[1000];
            if(getcwd(cwd,sizeof(cwd))!=NULL){
                printf("%s\n", cwd);
            }else{
                perror("getcwd");
            }
            free_cmd_list(cmd_list,n);
            return;
        }
        // CD
        if(strcmp(cmd_list[0].args[0], "cd")==0){
            char* arg = cmd_list[0].args[1];
            if(arg == NULL){arg=getenv("HOME"); } // par défaut le home
            if(chdir(arg)==-1){
                perror("cd failed");
            }
            free_cmd_list(cmd_list,n);
            return;
        }
        

        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        }

        if (pid == 0) {
            if(redirection==true){
                FILE * f = freopen(cmd_list[0].args[0], "w", stdout);
                if (f == NULL) {
                    fprintf(stderr, "Cannot reopen streams");
                    exit(EXIT_FAILURE);
                }

            }
            execvp(cmd_list[0].args[0], cmd_list[0].args);
            perror("execvp failed");
            fclose(stdout);
            exit(1);
        } else {
            //  Parent : attend la fin du processus enfant
            int status;
            if(!background_cmd){waitpid(pid, &status, 0);}
            if(DEBUG_PRINT){printf("La commande s'est terminée avec le code %d\n", WEXITSTATUS(status));}
        }


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
            int pipes[n_pipe][2]; // tableau des pipes
            // - redirection car la commande avec redirection compte pour 2 commande
            pid_t pids[n];

            for (int i = 0; i < n_pipe; i++) {
                if (pipe(pipes[i]) == -1) {
                    perror("pipe");
                    exit(1);
                }
            }


            for (int i = 0; i < n_pipe+1 ; i++){
                pids[i] = fork();

                if(pids[i] == -1){
                    perror("fork");
                    exit(1);
                }

                if(pids[i] == 0){
                    // Processus fils

                    // Si cest pas le premier, on lit depuis le pipe precédent
                    if(i > 0){
                        dup2(pipes[i - 1][0], STDIN_FILENO);
                    }

                    // Si cest pas la derniere des commandes pipés,on écrit vers le pipe suivant
                    if(i < n_pipe){
                        dup2(pipes[i][1], STDOUT_FILENO);
                    }
                    // SI il y a redirection.
                    if(redirection){
                        // Si c'est la dernière commande pipée (vraie commande)
                        if(i==n_pipe){ 
                            if(DEBUG_PRINT){printf("redirection !");}
                            FILE * f = freopen(cmd_list[n-1].args[0], "w", stdout);
                            if (f == NULL) {
                                fprintf(stderr, "Cannot reopen streams");
                                exit(EXIT_FAILURE);
                            }
                        }
                        // Si c'est la derniere commande
                        // alors on execute rien puisque la derniere commande 
                        // est le path du fichier dans lequel écrire.
                        if(i==n_pipe+1){
                            exit(1);
                        }
                    }

                    //Ferme tous les descripteurs de pipe inutilises
                    for(int j = 0; j < n - 1; j++){
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }

                    // Execute la commande
                    execvp(cmd_list[i].args[0], cmd_list[i].args);
                    perror("execvp"); // si exec échoue
                    fclose(stdout);
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
            exit(1);
        } else {
            //  Parent : attend la fin du processus enfant
            int status;
            if(!background_cmd){waitpid(pid, &status, 0);}
            if(DEBUG_PRINT){printf("La commande s'est terminée avec le code %d\n", WEXITSTATUS(status));} // extrait du cours
        }


    }

}


void free_cmd_list(cmd* cmd_list, int n){
    for(int i=0;i<n;i++){
        for(int j=0;j<cmd_list[i].arg_number;j++){
            free(cmd_list[i].args[j]);
        }
        free(cmd_list[i].args);
    }
    free(cmd_list);
}