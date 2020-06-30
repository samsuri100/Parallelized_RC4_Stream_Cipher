#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>

typedef struct dynamicBuff{
    char buffer[100];
} dynamicBuff;

typedef struct dynamicData{
    dynamicBuff* buff;
    int fullCounter;
    int currentCapacity;
    int increaseAmount;
    char* fileName;
} dynamicData;

void growBuff(dynamicData* buffObj){
    buffObj->currentCapacity += buffObj->increaseAmount;
    buffObj->buff = (dynamicBuff*) realloc(buffObj->buff->buffer, sizeof(dynamicBuff)+(buffObj->currentCapacity));
}

void checkBufferContents(dynamicData* buffObj){
    int i;

    printf("BUFFER OUTPUT:\n");

    for(i = 0; i < buffObj->fullCounter; ++i)
        printf("%c", buffObj->buff->buffer[i]);

    printf("BUFFER OUTPUT FINISHED\n");
}

#endif
