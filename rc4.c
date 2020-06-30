#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "rcHeader.h"
#include "buffer.h"

int main(int argc, char** argv){
    pthread_t readFile_ID, rc4Alg_ID, writeOutput_ID;
    
    if(argc != 4){
        perror("Please enter all command line parameters, program terminating\n");
        return -1;
    }

    dynamicData* plainTextBuffObj = (dynamicData*) malloc(sizeof(dynamicData));
    plainTextBuffObj->buff = (dynamicBuff*) malloc(sizeof(dynamicBuff));
    plainTextBuffObj->fullCounter = 0;
    plainTextBuffObj->currentCapacity = 100;
    plainTextBuffObj->increaseAmount = 100;
    plainTextBuffObj->fileName = argv[1];

    dynamicData* keyBuffObj = (dynamicData*) malloc(sizeof(dynamicData));
    keyBuffObj->buff = (dynamicBuff*) malloc(sizeof(dynamicBuff));
    keyBuffObj->fullCounter = 0;
    keyBuffObj->currentCapacity = 100;
    keyBuffObj->increaseAmount = 100;
    keyBuffObj->fileName = argv[2];
    
    pthread_create(&readFile_ID, NULL, readFileStream, (void*) plainTextBuffObj);
    pthread_create(&rc4Alg_ID, NULL, rc4Algorithm, (void*) keyBuffObj);
    //pthread_create(&writeOutput_ID, NULL, writeOutput, );
    
    pthread_join(readFile_ID, NULL);
    pthread_join(rc4Alg_ID, NULL);
    //pthread_join(writeOutput_ID, NULL);
    
    return 0;
}
