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

    mkfifo(uniquePipeID,0666);
    int customPipe = open(uniquePipeID,O_WRONLY);
   
    
    


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
            if(ix == 2) {
                int input0Len = strlen(inputs[0]); int input0Ix = 0;
                int input1Len = strlen(inputs[1]); int input1Ix = 0;

                char writtenData[400];
                int trav = 0;
                writtenData[trav++] = '2';

                while(trav < input0Len+1) {
                    writtenData[trav++] = inputs[0][input0Ix++];
                }
                writtenData[trav++] = '\0';

                while(trav < input0Len+input1Len+2) {
                    writtenData[trav++] = inputs[1][input1Ix++];
                }
                writtenData[trav] = '\0';
                
                write(customPipe,writtenData,400);
            }
            else {
                printf("Wrong number of parameters...\n");
            }
        }
        else if(strcmp(inputs[0],"delete") == 0) {
            printf("delete\n");
        }
        else if(strcmp(inputs[0],"read") == 0) {
            printf("read\n");
        }
        else if(strcmp(inputs[0],"write") == 0) {
            if(ix == 3) { 
                int input0Len = strlen(inputs[0]); int input0Ix = 0;
                int input1Len = strlen(inputs[1]); int input1Ix = 0;
                int input2Len = strlen(inputs[2]); int input2Ix = 0;

                char writtenData[400];
                int trav = 0;
                writtenData[trav++] = '3';

                while(trav < input0Len + 1) {
                    writtenData[trav++] = inputs[0][input0Ix++];
                }
                writtenData[trav++] = '\0';

                while (trav < input0Len + input1Len + 2) {
                    writtenData[trav++] = inputs[1][input1Ix++];
                }
                writtenData[trav++] = '\0';

                while (trav < input0Len + input1Len + input2Len + 3) {
                    writtenData[trav++] = inputs[2][input2Ix++];
                }
                writtenData[trav] = '\0';

                write(customPipe,writtenData,400);
            }
            else {
                printf("Wrong number of parameters...\n");
            }
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