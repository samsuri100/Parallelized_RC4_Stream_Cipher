#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 1000
#define INCREASE_AMOUNT 1000

typedef struct dynamicData{
    char* buff;
    int fullCounter;
    int currentCapacity;
    int increaseAmount;
    char* fileName;
    char finishedBool;
    char readMutex;
    char errorQuiteBool;
} dynamicData;

void growBuff(dynamicData* buffObj){
    buffObj->readMutex = 0;
    
    buffObj->currentCapacity += buffObj->increaseAmount;
    buffObj->buff = (char*) realloc(buffObj->buff, buffObj->currentCapacity);
}

void debugCheckBufferContents(dynamicData* buffObj){
    int i;

    printf("BUFFER OUTPUT:\n");

    for(i = 0; i < buffObj->fullCounter; ++i)
        printf("%c", buffObj->buff[i]);

    printf("BUFFER OUTPUT FINISHED\n");
}

void writeToDynamicBuffer(dynamicData* keyBuffObj, char keyByte){
    if(keyBuffObj->fullCounter == keyBuffObj->currentCapacity){
        keyBuffObj->readMutex = 0;
        growBuff(keyBuffObj);
    }
   
    keyBuffObj->readMutex = 2;

    keyBuffObj->buff[keyBuffObj->fullCounter] = keyByte;
    keyBuffObj->fullCounter += 1;
}

#endif
