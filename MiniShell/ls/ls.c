#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if(execvp("ls",argv) == -1){
        perror("errror");
    }
    return 0;
}