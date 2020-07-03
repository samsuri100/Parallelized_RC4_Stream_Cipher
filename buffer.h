#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>

//Defines size of initial dynamic array
#define INITIAL_CAPACITY 1000
//Defines size that dynamic array grows by, when capacity is reached
#define INCREASE_AMOUNT 1000

//Primary struct that enables dynamic arrays
typedef struct dynamicData{
    char* buff; //Variable that corresponds to dynamic char array
    int fullCounter;
    int currentCapacity;
    int increaseAmount;
    char* fileName;
    char finishedBool;
    char readMutex;
    char errorQuiteBool;
} dynamicData;

//Function takes in a dynamicData object whose buffer has reached capacity
//Reallocates memory according to INCREASE_AMOUNT macro
void growBuff(dynamicData* buffObj){
    //If another thread, other than the thread that called the function
    //tries to read from this thread while memory is being reallocated, segfault will occur
    //Setting mutex that other threads have access to, shuts down all external reading
    buffObj->readMutex = 0;
    
    //Reallocating larger amount of memory for buffer
    buffObj->currentCapacity += buffObj->increaseAmount;
    buffObj->buff = (char*) realloc(buffObj->buff, buffObj->currentCapacity);
}

//Function is used for debugging
//Will print all the contents of the buffer of a dynamicData object
void debugCheckBufferContents(dynamicData* buffObj){
    int i;

    printf("BUFFER OUTPUT:\n");

    //Iterating over and printing buffer
    for(i = 0; i < buffObj->fullCounter; ++i)
        printf("%c", buffObj->buff[i]);

    printf("BUFFER OUTPUT FINISHED\n");
}

//Function adds RC4 key stream byte to dynamicData object buffer
//If buffer reaches capacity, reallocates more memory for buffer
void writeToDynamicBuffer(dynamicData* keyBuffObj, char keyByte){
    //Reallocating more memory for buffer
    if(keyBuffObj->fullCounter == keyBuffObj->currentCapacity){
        growBuff(keyBuffObj);
  
        //Un-Setting mutex, allows external reading from other threads 
        //Number is 2, and not 1, because writeOutput thread would mistake '1' while 
        //reading in key from file and run, before the key has generated the key stream
        keyBuffObj->readMutex = 2;
    }

    //Adding key stream byte to array
    keyBuffObj->buff[keyBuffObj->fullCounter] = keyByte;
    keyBuffObj->fullCounter += 1;
}

#endif
