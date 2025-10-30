#ifndef EXECUTING
#define EXECUTING

typedef struct{
    char **args;
    int arg_number;
}cmd;
// functions
void execute(char **args, int arg_number);
void exec_bloc(char *bloc);


#endif
