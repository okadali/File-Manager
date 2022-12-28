#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>

int threadIndex = 48;
const int numberOfThreads = 5;
pthread_t threads[numberOfThreads];

void *pipeListener(char *pipeName) {
    mkfifo(pipeName,0666);
    int clientListener = open(pipeName,O_RDONLY);
    char clientInput[400];
    while (1) {
        int wantedTask = read(clientListener,clientInput,400);
        if(wantedTask != 0) {
            int commandCount = clientInput[0]-48;
            printf("%s\n",clientInput);

            char command[10]; int commandIx= 0;
            char file_name[100]; int fileNameIx = 0;
            char data[200]; int dataIx = 0;

            int trav = 1;
            while(clientInput[trav]) {
                command[commandIx++] = clientInput[trav++]; 
            }
            trav++;
            while(clientInput[trav]) {
                file_name[fileNameIx++] = clientInput[trav++]; 
            }
            
            if(commandCount > 2) {
                trav++;
                while(clientInput[trav]) {
                    data[dataIx++] = clientInput[trav++];
                }
            }

            printf("command: %s\n",command);
            printf("file_name: %s\n",file_name);
            if (commandCount >  2) {
               printf("data: %s\n",data);
            }
            
        }
        
    }
    

    return NULL;
}

void *namePipe() {
    char *managerSend = "MANAGERSEND";
    char *managerRecieve = "MANAGERRECEIVE";
    mkfifo(managerSend,0666);
    mkfifo(managerRecieve,0666);
    int receive = open(managerRecieve,O_RDONLY);
    int send = open(managerSend,O_WRONLY);
    char str[80];

    while (1) {
        int ret = read(receive,str,80);
        if(ret != 0) {
            char pipeid[8] = "pipeid";
            pipeid[6] = (char) threadIndex++;
            pipeid[7] = '\0';
            pthread_create(threads+(threadIndex-49),NULL,pipeListener,pipeid);
            write(send,pipeid,strlen(pipeid));
            printf("connection made\n");
        }
    }

    return NULL;
}

int main() {
    pthread_t pipeNameT;
    void *status;

    if(pthread_create(&pipeNameT, NULL, namePipe, NULL) != 0) {
        perror("Error: couldn't create pipeNameT...");
        return 1;
    }
    if(pthread_join(pipeNameT, &status) != 0) {
        perror("phtread_join");
        return 1;
    }

    return 0;
}