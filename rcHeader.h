#ifndef RCHEADER_H
#define RCHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "buffer.h"

void *readFileStream(void *plainTextBuffObj){
    FILE *fStream;
    dynamicData* buffObj = (dynamicData*) plainTextBuffObj;

    fStream = fopen(buffObj->fileName, "r");
    
    if(fStream == NULL){
        perror("Error opening file, program terminating\n");
        pthread_exit(NULL);
    }
    
    while(fgets((buffObj->buff->buffer)+(buffObj->fullCounter), buffObj->increaseAmount, fStream) != NULL){
        buffObj->fullCounter += strlen((buffObj->buff->buffer)+(buffObj->fullCounter));
    
        if(((buffObj->fullCounter)+100) > buffObj->currentCapacity)
            growBuff(buffObj);        
    }
    
    fclose(fStream);
    checkBufferContents(buffObj);
    pthread_exit(NULL);
}

void *rc4_algorithm(void* keyBuffObj){
    pthread_t = readKeyFile_ID;
    pthread_create(&readKeyFile_ID, NULL, readFileStream, keyBuffObj);

    dynamicData* keyBuffObj = (dynamicData*) keyBuffObj;
    

    pthread_exit(NULL);
}

#endif
