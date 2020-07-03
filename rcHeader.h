#ifndef RCHEADER_H
#define RCHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "buffer.h"

typedef struct writeInfo{
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
    int fileSize, charRead;
    dynamicData* buffObj = (dynamicData*) plainTextBuffObj;
    int incrementVar = buffObj->currentCapacity;
    
    fStream = fopen(buffObj->fileName, "r");
    
    if(fStream == NULL){
        printf("Error opening file %s, program terminating\n", buffObj->fileName);
        buffObj->errorQuiteBool = 1;
        pthread_exit(NULL);
    }

    fseek(fStream, 0, SEEK_END);
    fileSize = ftell(fStream);
    rewind(fStream);

    while(1){
        charRead = fread((buffObj->buff)+(buffObj->fullCounter), 1, incrementVar, fStream);
        buffObj->readMutex = 1;
        
        if(switchBool == 0){
            incrementVar = buffObj->increaseAmount;
            switchBool = 1;
        }
        
        buffObj->fullCounter += charRead;
    
        if(((buffObj->fullCounter)+(buffObj->increaseAmount)) > buffObj->currentCapacity)
            growBuff(buffObj);

        if(buffObj->fullCounter == fileSize)
            break;
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

    if((keyBuffObj->errorQuiteBool == 1) || (plainTextBuffObj->errorQuiteBool == 1))
        pthread_exit(NULL);

    pthread_create(&rc4GenerateKeyStream_ID, NULL, generateKeyStream, keyBuffObj);
    pthread_join(rc4GenerateKeyStream_ID, NULL);

    pthread_exit(NULL);
}

int modulo(int x, int y){
    return ((x % y) + y) % y;
}

void *generateKeyStream(void* keyBuffer){
    int i, j;
    char s[256];
    char t[256];
    char tempSwitchVar;
    char keyMaxLengthBool = 0;

    dynamicData* keyBuffObj = (dynamicData*) keyBuffer;
    int keyLength = (int) strlen(keyBuffObj->buff);

    if((keyLength < 5) || (keyLength > 256)){
        printf("Encryption key must be between 5 and 256 Bytes, program terminating\n");
        keyBuffObj->errorQuiteBool = 1;
        pthread_exit(NULL);
    }

    if(keyLength == 256){
        keyMaxLengthBool = 1;
        strncpy(t, keyBuffObj->buff, 256);
    }

    for(i = 0; i < 256; ++i){
        s[i] = (char) i;

        if(keyMaxLengthBool == 0)
            t[i] = keyBuffObj->buff[modulo(i, keyLength)];
    }
    
    j = 0;

    for(i = 0; i < 256; ++i){
        j = modulo(j + (int)s[i] + (int)t[i], 256);
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
        i = modulo(i+1, 256);
        j = modulo(j + (int)s[i], 256);

        tempSwitchVar = s[i];
        s[i] = s[j];
        s[j] = tempSwitchVar;

        writeToDynamicBuffer(keyBuffObj, s[modulo((int)s[i] + (int)s[j], 256)]);

        if(keyBuffObj->finishedBool == 1)
            break;
    }

    pthread_exit(NULL);
}

void *writeOutput(void* multipleBuffers){
    int i = 0;
    char breakBool = 0;
    char fileBool = 0;
    FILE *fStream;

    allBuffers* mulBufs = (allBuffers*) multipleBuffers;
    dynamicData* ptBuffObj = mulBufs->plainTextBufferPtr;
    dynamicData* keyBuffObj = mulBufs->keyBufferPtr;
    writeInfo* writeDataObj = mulBufs->writeBufferPtr;

    if(strcmp(writeDataObj->writeFlagOrFile, "stdout") != 0){
        fStream = fopen(writeDataObj->writeFlagOrFile, "w");
        fileBool = 1;
    }

    while(1){
        if((ptBuffObj->errorQuiteBool == 1) || (keyBuffObj->errorQuiteBool == 1)){
            remove(writeDataObj->writeFlagOrFile);
            pthread_exit(NULL);
        }

        while((ptBuffObj->readMutex == 1) && (keyBuffObj->readMutex == 1)){
            perror("mutex");
            if(fileBool == 0){
                perror("printing");
                printf("%c", ptBuffObj->buff[i]);
                //printf("%c", (keyBuffObj->buff[i])^(ptBuffObj->buff[i]));
            }
            else
                fputc((keyBuffObj->buff[i])^(ptBuffObj->buff[i]), fStream);
            
            ++i;
            while(!((i < ptBuffObj->fullCounter) && (i < keyBuffObj->fullCounter))){
                perror("counter");
                printf("\n%d -- %d\n", i, ptBuffObj->fullCounter);
                if((i == ptBuffObj->fullCounter) && (ptBuffObj->finishedBool == 1)){
                    keyBuffObj->finishedBool = 1;
                    breakBool = 1;
                    if(fileBool == 1)
                        fclose(fStream);
                    break;
                }
            }
            if(breakBool == 1)
                break;
        }
        if(breakBool == 1)
            break;
    }
    pthread_exit(NULL);
}

#endif
