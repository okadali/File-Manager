#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

pthread_t pipeNameT;
void *status;

pthread_mutex_t lock;

int threadIndex = 48;
const int numberOfThreads = 5;
pthread_t threads[numberOfThreads];
char fileList[10][100];
int isExit = 0;

void *pipeListener(char *pipeName) {
    char localPipeName[10];
    strcpy(localPipeName,pipeName);
    mkfifo(pipeName,0666);
    
    while (1) {
        int clientListener = open(localPipeName,O_RDONLY);
        char clientInput[400];
        int wantedTask = read(clientListener,clientInput,400);
        close(clientListener);

        //Getting user input from file_client--------------
        int commandCount = clientInput[0]-48;

        char command[10]; int commandIx = 0;
        char file_name[100]; int fileNameIx = 0;
        char data[200]; int dataIx = 0;

        int trav = 1;
        while (clientInput[trav]) {
            command[commandIx++] = clientInput[trav++];
        }
        command[commandIx] = '\0';
        trav++;
        while (clientInput[trav]) {
            file_name[fileNameIx++] = clientInput[trav++];
        }
        file_name[fileNameIx] = '\0';
        if(commandCount > 2) {
            trav++;
            while (clientInput[trav]) {
                data[dataIx++] = clientInput[trav++];
            }
            data[dataIx] = '\0';
        }
        //-------------------------------------------------

        int isExist = 0;
        for (int x = 0; x < 10; x++) {
            if (strcmp(fileList[x], file_name) == 0) {
                isExist = 1;
            }
        }

        
        if(strcmp(command, "create") == 0) {
            if(!isExist) {
                for(int x = 0; x < 10; x++) {
                    if(strcmp(fileList[x], "NULL") == 0) {
                        strcpy(fileList[x], file_name);
                        FILE *fp = fopen(file_name, "w");
                        fclose(fp);
                        break;
                    }
                }
            }
            pthread_mutex_lock(&lock);
            int clientWriter = open(localPipeName, O_WRONLY);
            char *message = isExist ? "this file is already exists" : "file is created";
            write(clientWriter, message, strlen(message));
            close(clientWriter);
            pthread_mutex_unlock(&lock);
        }
        else if (strcmp(command, "delete") == 0) {
            if (isExist) {
                for (int x = 0; x < 10; x++) {
                    if (strcmp(fileList[x], file_name) == 0) {
                        strcpy(fileList[x], "NULL");
                        remove(file_name);
                        break;
                    }
                }
            }
            pthread_mutex_lock(&lock);
            int clientWriter = open(localPipeName, O_WRONLY);
            char *message = !isExist ? "this file does not exist" : "file is deleted";
            write(clientWriter, message, strlen(message));
            close(clientWriter);
            pthread_mutex_unlock(&lock);
        }
        else if (strcmp(command, "write") == 0) {
            if (isExist) {
                FILE* fp = fopen(file_name, "w");
                fprintf(fp, "%s", data);
                fclose(fp);
            }
            pthread_mutex_lock(&lock);
            int clientWriter = open(localPipeName, O_WRONLY);
            char *message = isExist ? "data is written to file" : "this file does not exist";
            write(clientWriter, message, strlen(message));
            close(clientWriter);
            pthread_mutex_unlock(&lock);
        }
        else if (strcmp(command, "read") == 0) {
            char readedData[200];
            if (isExist) {
                FILE *fp = fopen(file_name, "r");
                fgets(readedData, 200, fp);
                fclose(fp);
            }

            pthread_mutex_lock(&lock);
            int clientWriter = open(localPipeName, O_WRONLY);
            char *message = isExist ? readedData : "this file does not exist";
            write(clientWriter, message, strlen(message));
            close(clientWriter);
            pthread_mutex_unlock(&lock);
        }
        else if (strcmp(command, "exit") == 0) {
            pthread_mutex_lock(&lock);
            int clientWriter = open(localPipeName, O_WRONLY);
            char *message = "thread closed";
            write(clientWriter, message, strlen(message));
            close(clientWriter);
            unlink(localPipeName);
            printf("connection closed\n");
            pthread_mutex_unlock(&lock);
            break;
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

    //Checks if any new file_client opened, if there is one a thread will be assing to them
    while (!isExit) {
        int ret = read(receive,str,80);
        if(ret != 0) {
            char pipeid[8] = "pipeid";
            pipeid[6] = (char) threadIndex;
            pipeid[7] = '\0';
            pthread_create(threads+((threadIndex++)-48),NULL,pipeListener,pipeid);
            write(send,pipeid,strlen(pipeid));
            printf("connection made\n");
        }
    }
    //-----------------------------------------------------

    unlink(managerSend);
    unlink(managerRecieve);
    close(receive);
    close(send);
    return NULL;
}

int main() {
    //Initilizing every name on the filelist as NULL-------
    for(int x = 0 ; x < 10 ; x++) {
        strcpy(fileList[x],"NULL");
    } 
    //-----------------------------------------------------

    if(pthread_create(&pipeNameT, NULL, namePipe, NULL) != 0) {
        perror("Error: couldn't create pipeNameT...");
        return 1;
    }

    //if the user input is exit namePipe thread will end---
    while (1) {
        char input[40];
        scanf("%s",input);
        if(strcmp(input,"exit") == 0) {
            isExit = 1;
            break;
        }
    }
    //-----------------------------------------------------
    
    if(pthread_join(pipeNameT, &status) != 0) {
        perror("phtread_join");
        return 1;
    }
    for(int x = 0 ; x < threadIndex-48 ; x++) {
        if (pthread_join(threads[x], &status) != 0) {
            perror("phtread_join");
            return 1;
        }  
    }

    return 0;
}