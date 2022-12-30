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
    
    read(receive,uniquePipeID,8);
    printf("%s\n",uniquePipeID);

    close(receive);
    //----------------------------------------

    mkfifo(uniquePipeID,0666);
   
    
    


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

        if(strcmp(inputs[0],"create") == 0 || strcmp(inputs[0],"delete") == 0 || strcmp(inputs[0],"read") == 0 || strcmp(inputs[0],"write") == 0) {
            if((ix == 2 && strcmp(inputs[0],"write") != 0 ) || (ix == 3 && strcmp(inputs[0],"write") == 0)) {
                int input0Len = strlen(inputs[0]); int input0Ix = 0;
                int input1Len = strlen(inputs[1]); int input1Ix = 0;
                int input2Len; int input2Ix = 0;
                if(ix == 3) {
                    input2Len = strlen(inputs[2]);
                }
                 
                char writtenData[400];
                int trav = 0;
                if(ix == 2) {
                    writtenData[trav++] = '2';
                }
                else {
                    writtenData[trav++] = '3';
                }
                
                while(trav < input0Len+1) {
                    writtenData[trav++] = inputs[0][input0Ix++];
                }
                writtenData[trav++] = '\0';

                while(trav < input0Len+input1Len+2) {
                    writtenData[trav++] = inputs[1][input1Ix++];
                }
                writtenData[trav++] = '\0';

                if(ix == 3) {
                    while (trav < input0Len + input1Len + input2Len + 3) {
                        writtenData[trav++] = inputs[2][input2Ix++];
                    }
                    writtenData[trav] = '\0';
                }

                int customPipeWrite = open(uniquePipeID,O_WRONLY);
                write(customPipeWrite,writtenData,400);
                close(customPipeWrite);

                int customPipeRead = open(uniquePipeID,O_RDONLY);
                char message[200];
                int lastIndex = read(customPipeRead,message,200);
                close(customPipeRead);
                message[lastIndex] = '\0';
                printf("%s\n",message);

            }
            else {
                printf("Wrong number of parameters...\n");
            }
        }
        else if(strcmp(inputs[0],"exit") == 0) {
            if(ix == 1) {
                int input0Len = strlen(inputs[0]); int input0Ix = 0;

                char writtenData[400];
                int trav = 0;
                writtenData[trav++] = '2';
                while(trav < input0Len+1) {
                    writtenData[trav++] = inputs[0][input0Ix++];
                }
                writtenData[trav++] = '\0';

                int customPipeWrite = open(uniquePipeID,O_WRONLY);
                write(customPipeWrite,writtenData,400);
                close(customPipeWrite);

                int customPipeRead = open(uniquePipeID,O_RDONLY);
                char message[200];
                int lastIndex = read(customPipeRead,message,200);
                close(customPipeRead);
                message[lastIndex] = '\0';
                printf("%s\n",message);
                unlink(uniquePipeID);
                break; 
            }
            else {
                printf("Wrong number of parameters...\n");
            }
            
            
        }
        else {
            printf("unknown command\n");
        }
        
    }

    
    return 0;
}