#include "BXP/bxp.h"
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define UNUSED __attribute__((unused))

BXPService *bxps;

int checkCommand(char *command, int numArgs){
    if (strcmp(command, "OneShot") == 0){
        if(numArgs != 7){
            return 0;
        }
        return 1;
    } else if (strcmp(command, "Repeat") == 0){
        if(numArgs != 9){
            return 0;
        }
        return 1;
    } else if (strcmp(command, "Cancel") == 0){
        if(numArgs != 2){
            return 0;
        }
        return 1;
    }
    return 0;
}

void *serviceRoutine(void *args) {
    BXPEndpoint bxpep;
    char query[1024], response[1025];
    unsigned qlen;

    while ((qlen = bxp_query(bxps, &bxpep, query, 1024)) > 0) {
        char* command;
        int numArgs = 0;

        command = strtok(query, "|");
        while (strtok(NULL, "|") != NULL) {
            numArgs++;
        }
        if(checkCommand(command, numArgs)){
            sprintf(response, "1%s", query);
        } else {
            sprintf(response, "0%s", query);
        }

        unsigned rlen = strlen(response) + 1;
        assert(bxp_response(bxps, &bxpep, response, rlen));
    }
    return NULL;
}

int main(UNUSED int argc, UNUSED char *argv[]) {
    assert(bxp_init(5000, 1));

    bxps = bxp_offer("DTS");
    assert(bxps != NULL);

    pthread_t svcThread;
    if (pthread_create(&svcThread, NULL, serviceRoutine, NULL)) {
        fprintf(stderr, "Unable to create thread to receive events\n");
        exit(EXIT_FAILURE);
    }

    pthread_join(svcThread, NULL);
    return 0;
}
