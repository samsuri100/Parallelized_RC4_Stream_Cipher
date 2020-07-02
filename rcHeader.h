#ifndef RCHEADER_H
#define RCHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "buffer.h"

typedef struct writeInfo{
    char* cipherText;
    int ctArrayLength;
    char* writeFlagOrFile;
} writeInfo;

typedef struct allBuffers{
    dynamicData* plainTextBufferPtr;
    dynamicData* keyBufferPtr;
    writeInfo* writeBufferPtr;
} allBuffers;

void* generateKeyStream(void* buffer);

void *readFileStream(void *plainTextBuffObj){
    FILE *fStream;
    char switchBool = 0;
    dynamicData* buffObj = (dynamicData*) plainTextBuffObj;
    int incrementVar = buffObj->currentCapacity;
    
    fStream = fopen(buffObj->fileName, "r");
    
    if(fStream == NULL){
        perror("Error opening file, program terminating\n");
        pthread_exit(NULL);
    }
     
    while(fgets((buffObj->buff)+(buffObj->fullCounter), incrementVar, fStream) != NULL){
        buffObj->readMutex = 1;
        
        if(switchBool == 0){
            incrementVar = buffObj->increaseAmount;
            switchBool = 1;
        }
        
        buffObj->fullCounter += strlen((buffObj->buff)+(buffObj->fullCounter));
    
        if(((buffObj->fullCounter)+(buffObj->increaseAmount)) > buffObj->currentCapacity)
            growBuff(buffObj);
    }

    buffObj->readMutex = 1;
    buffObj->finishedBool = 1;
    
    fclose(fStream);
    pthread_exit(NULL);
}

void *rc4Algorithm(void* bufferTuple){
    pthread_t readKeyFile_ID, rc4GenerateKeyStream_ID;
    
    allBuffers* bufferPair = (allBuffers*) bufferTuple;
    dynamicData* plainTextBuffObj = bufferPair->plainTextBufferPtr;
    dynamicData* keyBuffObj = bufferPair->keyBufferPtr;
    writeInfo* writeDataObj = bufferPair->writeBufferPtr;
    
    pthread_create(&readKeyFile_ID, NULL, readFileStream, keyBuffObj);
    pthread_join(readKeyFile_ID, NULL);

    pthread_create(&rc4GenerateKeyStream_ID, NULL, generateKeyStream, keyBuffObj);
    pthread_join(rc4GenerateKeyStream_ID, NULL);
    
    pthread_exit(NULL);
}

void *generateKeyStream(void* keyBuffer){
    int i, j;
    char s[256];
    char t[256];
    char keyMaxLengthBool = 0;

    dynamicData* keyBuffObj = (dynamicData*) keyBuffer;
    int keyLength = (int) strlen(keyBuffObj->buff);

    if(keyLength == 256)
        keyMaxLengthBool = 1;

    for(i = 0; i < 256; ++i){
        s[i] = (char) i;

        if(keyMaxLengthBool == 0)
            t[i] = keyBuffObj->buff[i % keyLength];
    }

    j = 0;
    char tempSwitchVar;

    for(i = 0; i < 256; ++i){
        j = (j + (int)s[i] + (int)t[i]) % 256;
        tempSwitchVar = s[i];
        s[i] = s[j];
        s[j] = tempSwitchVar;
    }

    free(keyBuffObj->buff);
    keyBuffObj->buff = (char*) malloc(INITIAL_CAPACITY);
    keyBuffObj->fullCounter = 0;
    keyBuffObj->currentCapacity = INITIAL_CAPACITY;
    keyBuffObj->finishedBool = 0;
    keyBuffObj->readMutex = 0;

    i = 0;
    j = 0;

    while(1){
        i = (i+1) % 256;
        j = (j + (int)s[i]) % 256;

        tempSwitchVar = s[i];
        s[i] = s[j];
        s[j] = tempSwitchVar;

        writeToDynamicBuffer(keyBuffObj, s[((int)s[i] + (int)s[j]) % 256]);

        if(keyBuffObj->finishedBool == 1)
            break;
    }

    pthread_exit(NULL);
}

void *writeOutput(void* multipleBuffers){
    int i = 0;
    char breakBool = 0;
    FILE *fStream;

    allBuffers* mulBufs = (allBuffers*) multipleBuffers;
    dynamicData* ptBuffObj = mulBufs->plainTextBufferPtr;
    dynamicData* keyBuffObj = mulBufs->keyBufferPtr;
    writeInfo* writeDataObj = mulBufs->writeBufferPtr;

    if(writeDataObj->writeFlagOrFile != "stdout")
        fStream = fopen(writeDataObj->writeFlagOrFile, "w");

    while(1){
        while((ptBuffObj->readMutex == 1) && (keyBuffObj->readMutex == 1)){
            if((i < ptBuffObj->fullCounter) && (i < keyBuffObj->fullCounter)){
                if(writeDataObj->writeFlagOrFile == "stdout") 
                    printf("%c", (keyBuffObj->buff[i])^(ptBuffObj->buff[i]));
                else
                    fputc((keyBuffObj->buff[i])^(ptBuffObj->buff[i]), fStream);
                ++i;
            }
            else if(ptBuffObj->finishedBool == 1){
                keyBuffObj->finishedBool = 1;
                breakBool = 1;
                fclose(fStream);
                break;
            }
        }
        if(breakBool == 1)
            break;
    }
    pthread_exit(NULL);
}

#endif
