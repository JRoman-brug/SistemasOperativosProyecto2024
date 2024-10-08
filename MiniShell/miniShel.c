#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

//Minishell
/*
a) Mostrar una ayuda con los comandos posibles. -> Man
b) Crear un directorio. -> mkdir
c) Eliminar un directorio. -> rm
d) Crear un archivo. -> touch
e) Listar el contenido de un directorio. -> ls
f ) Mostrar el conenido de un archivo. -> nano o contac
g) Modificar los permisos de un archivo. Los permisos son de lectura, escritura y
ejecución. -> chmod
*/

#define max 64
#define numsArgs 10
#define longArgs 20
FILE* file;

void executeCommand(char command[max], char* args[numsArgs]){
    char* path = malloc(max*sizeof(char)); 
    strcpy(path, "./");
    strcat(path, command);
    strcat(path, "/");
    strcat(path, command);
    pid_t pid = fork();
    if(pid == 0){
        execv(path, args);
        exit(0);
    }else if(pid == -1){
        perror("Error al iniciar el proceso");
        exit(-1);
    }
    /*
    */
}

void prepareCommand(char input[max], char command[max], char* args[numsArgs]){
    for(int i = 0; i < numsArgs; i++){
        args[i] = malloc(longArgs * sizeof(char));
        if(args[i] == NULL){
            perror("Error al asignar memoria para la cadena");
            exit(0); // Salir si no se pudo asignar memoria
        }
    }
    int i=0;
    input[strcspn(input, "\n")] = 0;

    //Prepare a command
    strcpy(command,strtok(input, " "));

    char* token = malloc(max*sizeof(char));
    strcpy(token,command);
    while(i < numsArgs) {
        if(token != NULL){
            strcpy(args[i++], token);
            token = strtok(NULL, " ");
        }else args[i++] = NULL;
    }
    
    /*
    //testing
    printf("\n\n ------Testing------\n");
    printf("command: %s\n", command);

    for(int i=0;i<numsArgs;i++){
        if(args2[i] != "\0"){
            printf("Arg %i: %s\n",i, args2[i]);
        }else printf("Arg %i: NULL\n",i);
    }
    */
}

//Note: execvp, first: name of file, seconds params
int options(char input[max]){
    int option = 1;
    //Path commands
    char path[max];

    char command[max];
    char* args[numsArgs];
    prepareCommand(input, command, args);

    //Exit 
    if(strcmp(input, "quit") == 0) return 0;
    //Find in the path
    while(fgets(path, sizeof(path), file) != NULL){
        path[strcspn(path, "\n")] = 0;
        //Exacly command
        if(strcmp(path,command) == 0) {
            //here new process
            rewind(file);
            executeCommand(command, args);
            return option;
        }
        option++; 
    }
    //Command not found
    return -1;
}

int main() {
    //Open path
	file = fopen("path.txt","r");
	if(file == NULL){
        perror("Error open file");
        return 1;
	}
    char buffer[max];
    int exit = 1;
    int option = 0;
    //Main process 

    do{
        fgets(buffer, sizeof(buffer), stdin);
        option = options(buffer);
        if(option == 0){
            exit = 0;
            printf("Good bye :)\n");
        }else if(option == -1){
            printf("Command not found\n");
        }
        wait(NULL);
    } while (exit);
    
    fclose(file);
    return 0;
}
