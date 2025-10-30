#include "../inc/executing.h"
#include "../inc/communication.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>


void exec_bloc(char *bloc){
    printf(" Execution du bloc : %s\n",bloc);
    // tableau de tableau de string (cmd)
    int n=1;
    int last_stop=0;
    int n_pipe=0;
    bool redirection=false;
    bool background_cmd=false;

    cmd* cmd_list;
    if(cut_in_cmd(bloc,&cmd_list,&n,&redirection,&n_pipe)==-1){return;}
    // lire cmd_list
    for(int i=0;i<n;i++){
        printf(" Commande %d :\n",i);
        for(int j=0;j<cmd_list[i].arg_number;j++){
            printf("  Arg %d : %s\n",j,cmd_list[i].args[j]);
        }
    }

    // execution
    char* last_arg=cmd_list[n-1].args[cmd_list[n-1].arg_number-1];
    int len = strlen(last_arg);
    if(last_arg[len-1]=='&'){
        printf(" commande en arriere plan\n");
        background_cmd = true;
        last_arg[len-1] = '\0';
        // /!\ peut generer une commande nulle
    }

    if(n_pipe==0){
        printf("commmande simple\n");

        pid_t pid = fork();
        
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        }

        if (pid == 0) {
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

}


void manage_executing(char **args, int arg_number){
    // char* token = strtok(args, ",");

    // while (token != NULL) { // si il y a un ";"
    //     printf("%s\n", token);
    //     token = strtok(NULL, ",");
    // }

    //strtok
    for(int i=0;i<arg_number;i++){
        if(strcmp(";",args[i])==0){
            execute(args,arg_number);
        }
    }
    // si aucun ";" n'est detecté, on enchaine sur l'execution de toute la commande.
    
    // faire le tour des arguments, si un est = ";", alors lancer execute() des arguments avant ; 
    // ensuite relancer manage_executing avec le reste des arguments.
}


void execute(char **args, int arg_number) {
    if(args == NULL || arg_number == 0)
        return;

    // BUILTIN : exit
    if(strcmp(args[0],"exit") == 0){
        printf("Bye \n");
        exit(0);
    }

    // BUILTIN : cd
    if(strcmp(args[0],"cd") == 0) {
        if (arg_number < 2) {
            fprintf(stderr, "cd: argument manquant\n");
            return;
        }
        if(chdir(args[1])!= 0) {
            perror("cd");
            return;
        }
        return;
    }

    // BUILTIN : pwd
    if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
        }
        else
        {
            perror("pwd");
        }
        return;
    }

    // Autre commande : fork + execvp

    // si args[-2] == "&"


    if(strcmp(args[arg_number-1],"&")==0){printf(" en arriere plan ! 4");}


    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Enfant
        execvp(args[0], args);
        perror("execvp");
        _exit(EXIT_FAILURE);
    } else {
        // Parent
        int status;
        waitpid(pid, &status, 0);
    }

    return;
}
