#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sys/wait.h"
#include "string.h"
#include <readline/readline.h>
#include <dirent.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>




int main() {
    //Global Variables------------------------
    char uniquePipeID[8];

    //Getting a unique id for named pipe------    
    char *clientRead = "MANAGERSEND";
    char *clientWrite = "MANAGERRECEIVE";

    int send = open(clientWrite,O_WRONLY);
    int receive = open(clientRead,O_RDONLY); 
    char *input = "newConnection";
    write(send,input,strlen(input)+1);
    close(send);

    while (1) {
        if(read(receive,uniquePipeID,8) != 0) {
           printf("%s\n",uniquePipeID);
           break;
        }
    }
    close(receive);
    //----------------------------------------

    

   
    
    


    while(1) {
        input = readline("FileClient>>");
        if(strlen(input) == 0) {    
            continue;
        }
        int ix = 0;
        char *inputs[80];
        char *token = strtok(input," ");
        while( token != NULL ) {
            inputs[ix++] = token;
            token = strtok(NULL," ");
        }

        if(strcmp(inputs[0],"create") == 0) {
            
        }
        else if(strcmp(inputs[0],"delete") == 0) {
            printf("delete\n");
        }
        else if(strcmp(inputs[0],"read") == 0) {
            printf("read\n");
        }
        else if(strcmp(inputs[0],"write") == 0) {
            printf("write\n");
        }
        else if(strcmp(inputs[0],"exit") == 0) {
            break;
        }
        else {
            printf("unknown command\n");
        }
        
    }

    
    return 0;
}