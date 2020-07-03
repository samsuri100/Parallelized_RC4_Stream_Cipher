/*Copyright Sam Suri, 2020, all rights reserved*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "rcHeader.h"
#include "buffer.h"

int main(int argc, char** argv){
    pthread_t readTextFileInitial_ID, rc4Alg_ID, writeOutput_ID;

    //User must pass in minimum of 3 command line arguments
    if(argc < 4){
        printf("Incorrect number of command line parameters, program terminating\n");
        return 1;
    }
    
    //Third arugment must be 'stdout' or 'file'
    if(!(strcmp(argv[3], "file") == 0 || strcmp(argv[3], "stdout") == 0)){
        printf("Argument 3 must either be 'file' or 'stdout', program terminating\n");
        return 1;
    }

    //If it's 'stdout', that's the last argument
    if(strcmp(argv[3], "stdout") == 0){
        if(argc != 4){
            printf("Incorrect number of command line parameters, program terminating\n");
            return 1;
        }
    }
    //If it's 'file', an additional argument is needed, the file name
    else{
        if(argc != 5){
            printf("Incorrect number of command line parameters, program terminating\n");
            return 1;
        }
    }
    
    //Creating struct object for plain/cipher text file
    dynamicData* plainTextBuffObj = (dynamicData*) malloc(sizeof(dynamicData));
    plainTextBuffObj->buff = (char*) malloc(INITIAL_CAPACITY);
    plainTextBuffObj->fullCounter = 0;
    plainTextBuffObj->currentCapacity = INITIAL_CAPACITY;
    plainTextBuffObj->increaseAmount = INCREASE_AMOUNT;
    plainTextBuffObj->fileName = argv[1];
    plainTextBuffObj->finishedBool = 0;
    plainTextBuffObj->readMutex = 0;
    plainTextBuffObj->errorQuiteBool = 0;
    
    //Creating struct object for key text file
    dynamicData* keyBuffObj = (dynamicData*) malloc(sizeof(dynamicData));
    keyBuffObj->buff = (char*) malloc(INITIAL_CAPACITY);
    keyBuffObj->fullCounter = 0;
    keyBuffObj->currentCapacity = INITIAL_CAPACITY;
    keyBuffObj->increaseAmount = INCREASE_AMOUNT;
    keyBuffObj->fileName = argv[2];
    keyBuffObj->finishedBool = 0;
    keyBuffObj->readMutex = 0;
    keyBuffObj->errorQuiteBool = 0;

    //Struct object that contains write info
    writeInfo* writeData = (writeInfo*) malloc(sizeof(writeInfo));
    if(strcmp(argv[3], "stdout") == 0)
        writeData->writeFlagOrFile = argv[3];
    else
        writeData->writeFlagOrFile = argv[4];
    
    //Wrapping struct objects in struct wrapper to pass into pthread_create
    allBuffers* multipleBuffers = (allBuffers*) malloc(sizeof(allBuffers));
    multipleBuffers->plainTextBufferPtr = plainTextBuffObj;
    multipleBuffers->keyBufferPtr = keyBuffObj;
    multipleBuffers->writeBufferPtr = writeData;
    
    //Launching three threads
    //rc4Algorithm will internally launch 2 more threads
    //5 threads launch in total, 4 at max will be running concurrently
    pthread_create(&readTextFileInitial_ID, NULL, readFileStream, (void*) plainTextBuffObj);
    pthread_create(&rc4Alg_ID, NULL, rc4Algorithm, (void*) multipleBuffers);
    pthread_create(&writeOutput_ID, NULL, writeOutput, (void*) multipleBuffers);
    
    //Waiting for all threads to complete
    pthread_join(readTextFileInitial_ID, NULL);
    pthread_join(rc4Alg_ID, NULL);
    pthread_join(writeOutput_ID, NULL);
    
    //Freeing memory
    free(plainTextBuffObj->buff);
    free(plainTextBuffObj);
    free(keyBuffObj->buff);
    free(keyBuffObj);
    free(writeData);
    
    return 0;
}
