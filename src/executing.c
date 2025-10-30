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
    //tableau de tableau de string (cmd)
    int n=1;
    int last_stop=0;
    int n_pipe=0;
    bool redirection=0;

    cmd* cmd_list;

    cut_in_cmd(bloc,&cmd_list,&n,&redirection,&n_pipe);
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
