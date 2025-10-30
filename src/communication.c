#define MAX_LENGTH_PROMPT 256
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../inc/executing.h"

int cut_in_cmd(char *bloc,cmd** cmd_list,int* n,bool* redirection,int* n_pipe){
    // gérer le cas si bloc nul ou vide


    if (bloc == NULL || 
        *bloc == '\0' ||
        strspn(bloc, " \t\r\n") == strlen(bloc)){
        printf("Vous avez rentré une commande vide\n"); 
        return -1;
    }

    int arg_number;
    int length_bloc = strlen(bloc);
    //boucle qui compte le nombre de | et >
    for(int i=0;i<length_bloc;i++){
        if(bloc[i]=='|'){
            (*n_pipe)++;
            (*n)++;
        }
        if(bloc[i]=='>'){
            *redirection=1;
        }
    }
    //malloc n cmd dans cmd_liste
    (*cmd_list) = malloc((*n)*sizeof(cmd));
    //sépare en token, separator = "|>"
    char* saveptr_bloc;
    char* token = strtok_r(bloc, "|>",&saveptr_bloc); 
    int nb_cmd=0;
    do{
        // compte le nb d'arguments , separator " "
        arg_number=0;
        char temp[MAX_LENGTH_PROMPT];
        strcpy(temp, token);
        //vérifier que temp n'est pas que des espaces ou vide

        char* token_arg = strtok(temp, " ");
        // if(token_arg = "\0"){return -1;} 
        do{
            arg_number++;
            token_arg = strtok(NULL, " "); 
        }while(token_arg != NULL);
        (*cmd_list)[nb_cmd].arg_number=arg_number; 
        (*cmd_list)[nb_cmd].args=malloc(sizeof(char*)*(arg_number+1));
        // malloc nb arguments chacun de taille max 256 char
        for(int y=0;y<arg_number;y++){
            (*cmd_list)[nb_cmd].args[y]= malloc(sizeof(char)*MAX_LENGTH_PROMPT);
        }
        (*cmd_list)[nb_cmd].args[arg_number]=NULL; // pas besoin d'un malloc
        // les remplit
        strcpy(temp, token);
        token_arg = strtok(temp, " ");
        strcpy((*cmd_list)[nb_cmd].args[0],token_arg); 
        for(int y=1;y<arg_number;y++){
            token_arg = strtok(NULL, " ");
            strcpy((*cmd_list)[nb_cmd].args[y],token_arg); 
        }

        nb_cmd++;
        token = strtok_r(NULL,"|>",&saveptr_bloc);
        //execute(args,arg_number,separator);
    }while(token != NULL);
    return 0;


}


void cut_in_args(char *raw_string,char **args, int arg_number){
    
    bool background_command = 0;
    if(raw_string[strlen(raw_string)-1]=='&'){
        background_command=1;
        raw_string[strlen(raw_string)-1]=' ';
    }

    // Compter les tokens
    int count = 0;
    char *token_temp_copy = malloc(strlen(raw_string)+1);
    strcpy(token_temp_copy,raw_string);
    char *token = strtok(token_temp_copy," ");
    while (token != NULL) { // tant que token_temp_copy n'est pas vidé
        count++;
        token = strtok(NULL," "); // l'argument NULL sert à dire continue là ou tu étais
    }
    free(token_temp_copy);
    if(count == 0){
        args = NULL;
        arg_number = 0;
        return;
    }

    // Allouer le tableau d’arguments

    args = malloc((count+1+background_command)*sizeof(char *));
    if(args == NULL){
        perror("malloc");
        arg_number = 0;
        return;
    }

    // Deuxième passage : découpage réel
    int i = 0;
    token = strtok(raw_string," ");
    while(token != NULL){
        (args)[i]=malloc(strlen(token)+1);
        strcpy((args)[i],token);
        if((args)[i] == NULL){
            perror("strcpy");
            for(int j = 0;j<i;++j){
                free((args)[j]);}
            free(args);
            args = NULL;
            arg_number = 0;
            return;
        }
        i++;
        token = strtok(NULL, " \t");
    }
    (args)[i] = NULL;  // pour que le dernier argument de execpv soit NULL (obligatoire)
    arg_number = count;
      

    if(background_command){ // rajoute l'argument spécifique à 
        (args)[count] = malloc(sizeof(char)*2);
        (args)[count] = "&";
    }
    return;

}

void read_shell(char **raw_user_entry) {

    char buffer[MAX_LENGTH_PROMPT];

    if(fgets(buffer,sizeof(buffer),stdin) == NULL){
        *raw_user_entry = NULL;
        return;
    }

    // Supprime le retour à la ligne
    buffer[strcspn(buffer, "\n")]='\0';
    if(buffer[0] == '\0'){
        buffer[0]=' ';
        buffer[1]='\0';
    }
    *raw_user_entry = malloc(strlen(buffer) + 1); // +1 pour le '\0'
    if (*raw_user_entry == NULL) { // ANTI ERREUR
        perror("malloc");
        return;
    }
    strcpy(*raw_user_entry,buffer);
    
}

void free_args(char **args, int arg_number){
    if(args==NULL)return;
    for(int i=0;i<arg_number;i++){
        free(args[i]);
    }
    free(args);
}
