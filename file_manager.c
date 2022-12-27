#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>

void *namePipe() {
    int threadIndex = 48;

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