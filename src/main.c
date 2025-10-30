#include "../inc/communication.h"
#include "../inc/executing.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum program_state {RUNNING,WAITING,FINISHED};

int main(){
    enum program_state state = RUNNING;
    int separator, arg_number;

    while(state==RUNNING){
        printf("$");
        fflush(stdout); //vide le buffer

        char *raw_user_entry;
        read_shell(&raw_user_entry); // lis ce qu'il y a dans le terminal pour le mettre dans raw_user_entry
        
        char *saveptr_raw;
        char* token = strtok_r(raw_user_entry, ";",&saveptr_raw); 
        // si il n'y a pas de ; -> string complet
        // sinon il représente que le premier
        int i = 0;
        do{
         // rentre dans la boucle à chaque fois
            char* token_temp = strdup(token);
            printf(" string interne %d = %s\n",i,token);
            exec_bloc(token_temp);
            i++;
            token = strtok_r(NULL,";",&saveptr_raw);
            // char** args = NULL; // LISTE DE STRINGs (arguments)
            // arg_number = 0;
            // cut_in_args(token,args,arg_number);
            
            printf("\n-\n");
            //execute(args,arg_number,separator);
        }while(token != NULL);
        free(raw_user_entry);

    }

    return 0;
}
