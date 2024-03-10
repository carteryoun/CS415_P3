#include "BXP/bxp.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define UNUSED __attribute__((unused))
#define SERVICE "DTS"

void *svcRoutine(void *args) {
    BXPService *bxps = (BXPService *)args;
    BXPEndpoint bxpep;
    char query[1024];
    unsigned qlen;

    while ((qlen = bxp_query(bxps, &bxpep, query, 1024)) > 0) {
        char response[1025];
        sprintf(response, "1%s", query);
        bxp_response(bxps, &bxpep, response, strlen(response) + 1);
    }
    return NULL;
}

int main(UNUSED int argc, UNUSED char *argv[]) {
    BXPService *bxps;
    pthread_t svcThread;
    
    assert(bxp_init(0, 1));

    /* create service to receive requests */
    if ((bxps = bxp_offer(SERVICE)) == NULL) {
        fprintf(stderr, "Unable to create BXP service to receive requests\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&svcThread, NULL, svcRoutine, (void *)bxps)) {
        fprintf(stderr, "Unable to create thread to receive requests\n");
        exit(EXIT_FAILURE);
    }
    
    /* Server loop */
    while(1) {
        sleep(1); // sleep to prevent busy waiting
    }
    
    return 0;
}

