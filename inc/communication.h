#include <stdbool.h>
#include "../inc/executing.h"
#ifndef COMMUNICATION
#define COMMUNICATION

// functions
void read_shell(char **raw_user_entry);
void cut_in_cmd(char *bloc,cmd** cmd_list,int* n,bool* redirection,int* n_pipe);
void free_args(char **args, int argc);

#endif
