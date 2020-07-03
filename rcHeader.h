#ifndef RCHEADER_H
#define RCHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "buffer.h"

//Struct contains information
//for writing
typedef struct writeInfo{
    char* writeFlagOrFile;
} writeInfo;

//Struct serves as a wrapper to pass
//multiple structs into threads
typedef struct allBuffers{
    dynamicData* plainTextBufferPtr;
    dynamicData* keyBufferPtr;
    writeInfo* writeBufferPtr;
} allBuffers;

void* generateKeyStream(void* buffer);

//Function reads contents of a file, binary or ascii,
//into a dynamic buffer and reallocates memory as necessary
void *readFileStream(void *plainTextBuffObj){
    FILE *fStream;
    char switchBool = 0;
    int fileSize, charRead;

    dynamicData* buffObj = (dynamicData*) plainTextBuffObj;
    int incrementVar = buffObj->currentCapacity;
    
    fStream = fopen(buffObj->fileName, "r");
    
    //Checking to see if file can be opened or not, if not, terminating program
    if(fStream == NULL){
        printf("Error opening file %s, program terminating\n", buffObj->fileName);
        //Letting other threads know to quite
        buffObj->errorQuiteBool = 1;
        pthread_exit(NULL);
    }

    //For binary or ascii files, fread will pause for '\n' characters
    //Finding total amount of bytes in file to know when to resume or stop fread
    fseek(fStream, 0, SEEK_END);
    fileSize = ftell(fStream);
    rewind(fStream);

    //If file is empty, RC4 algorithm will not work, terminating program
    if(fileSize == 0){
        printf("File %s cannot be empty, program terminating\n", buffObj->fileName);
        //Letting other threads know to quite
        buffObj->errorQuiteBool = 1;
        pthread_exit(NULL);
    }

    while(1){
        //Using fread to read from file into dynamic array, returns number of chars read
        //fgets, fgetc do not work with binary data, during decryption ciphertext is binary data
        charRead = fread((buffObj->buff)+(buffObj->fullCounter), 1, incrementVar, fStream);
        //Un-Setting mutex, allows for external reading from other threads
        buffObj->readMutex = 1;
        
        //First time in loop, freads will get amount specified by INITIAL_CAPACITY macro
        //Every time after this, freads gets amount specified by INCREASE_AMOUNT macro
        if(switchBool == 0){
            incrementVar = buffObj->increaseAmount;
            switchBool = 1;
        }
        
        buffObj->fullCounter += charRead;
    
        //If capacity is reached, reallocate more space for buffer
        if(((buffObj->fullCounter)+(buffObj->increaseAmount)) > buffObj->currentCapacity)
            growBuff(buffObj);

        //All bytes from file have been read in, break loop 
        if(buffObj->fullCounter == fileSize)
            break;
    }

    //Since reading is done and no more memory 
    //will be allocated, permanently un-setting mutex
    buffObj->readMutex = 1;
    buffObj->finishedBool = 1;
    
    //Closing file
    fclose(fStream);
    pthread_exit(NULL);
}

//Function deals with RC4 algorithm,
//both in reading in the key file and generating the keystream
//Function launches two threads, they run sequentially though
void *rc4Algorithm(void* bufferTuple){
    pthread_t readKeyFile_ID, rc4GenerateKeyStream_ID;
    
    //Un-wrapping struct objects in struct wrapper
    allBuffers* bufferPair = (allBuffers*) bufferTuple;
    dynamicData* plainTextBuffObj = bufferPair->plainTextBufferPtr;
    dynamicData* keyBuffObj = bufferPair->keyBufferPtr;
    writeInfo* writeDataObj = bufferPair->writeBufferPtr;
    
    //Launching thread to read in the key file
    pthread_create(&readKeyFile_ID, NULL, readFileStream, keyBuffObj);
    //Not proceeding to the next thread until readFileStream thread is finished
    pthread_join(readKeyFile_ID, NULL);

    //If the key file could not be opened or was empty, or, 
    //the plaintext/ciphertext text file could not be opened or was empty
    //need to inform current thread to quite
    if((keyBuffObj->errorQuiteBool == 1) || (plainTextBuffObj->errorQuiteBool == 1))
        pthread_exit(NULL);

    //Launching thread to generate keystream
    pthread_create(&rc4GenerateKeyStream_ID, NULL, generateKeyStream, keyBuffObj);
    pthread_join(rc4GenerateKeyStream_ID, NULL);

    pthread_exit(NULL);
}

//Function implements mathematical notion 
//of %, rather than default remainder version
int modulo(int x, int y){
    return ((x % y) + y) % y;
}

//Function generates keystream
//Generates keystream until notified by writeOutput thread to stop
void *generateKeyStream(void* keyBuffer){
    int i, j;
    char s[256];
    char t[256];
    char tempSwitchVar;
    char keyMaxLengthBool = 0;

    dynamicData* keyBuffObj = (dynamicData*) keyBuffer;
    int keyLength = keyBuffObj->fullCounter;

    //Key must be between 5 and 256 bytes, if not, terminating program
    if((keyLength < 5) || (keyLength > 256)){
        printf("Encryption key must be between 5 and 256 bytes, program terminating\n");
        //Letting other threads know to quite
        keyBuffObj->errorQuiteBool = 1;
        pthread_exit(NULL);
    }

    //If key is at max length of 256 bytes, t array copies it
    if(keyLength == 256){
        keyMaxLengthBool = 1;
        strncpy(t, keyBuffObj->buff, 256);
    }

    //Initializing s array
    //If key is less than 256 bytes, t array is filled with repetitions of it
    for(i = 0; i < 256; ++i){
        s[i] = (char) i;

        if(keyMaxLengthBool == 0)
            t[i] = keyBuffObj->buff[modulo(i, keyLength)];
    }
    
    j = 0;

    //Do initial permutation over both arrays, switching s[i] and s[j]
    for(i = 0; i < 256; ++i){
        j = modulo(j + (int)s[i] + (int)t[i], 256);
        tempSwitchVar = s[i];
        s[i] = s[j];
        s[j] = tempSwitchVar;
    }

    //Key is no longer needed, freeing memory
    //Using object for key stream data
    free(keyBuffObj->buff);
    
    //buff member variable holds the key stream
    keyBuffObj->buff = (char*) malloc(INITIAL_CAPACITY);
    keyBuffObj->fullCounter = 0;
    keyBuffObj->currentCapacity = INITIAL_CAPACITY;
    keyBuffObj->finishedBool = 0;
    
    i = 0;
    j = 0;

    while(1){
        //Further purmuting data
        i = modulo(i+1, 256);
        j = modulo(j + (int)s[i], 256);

        //Switching s[i] and s[j]
        tempSwitchVar = s[i];
        s[i] = s[j];
        s[j] = tempSwitchVar;

        //Writing key stream byte to buffer
        writeToDynamicBuffer(keyBuffObj, s[modulo((int)s[i] + (int)s[j], 256)]);

        //Stop generating key stream when notified by writeOutput thread to stop
        if(keyBuffObj->finishedBool == 1)
            break;
    }

    pthread_exit(NULL);
}

//Function XOR's plaintext/ciphertext and key stream to encrypt or decrypt data
//Either writes data to stdout or to a specified file
void *writeOutput(void* multipleBuffers){
    int i = 0;
    char breakBool = 0;
    char fileBool = 0;
    FILE *writeStream;

    //Un-wrapping struct objects in struct wrapper
    allBuffers* mulBufs = (allBuffers*) multipleBuffers;
    dynamicData* ptBuffObj = mulBufs->plainTextBufferPtr;
    dynamicData* keyBuffObj = mulBufs->keyBufferPtr;
    writeInfo* writeDataObj = mulBufs->writeBufferPtr;

    //Writing output to file
    if(strcmp(writeDataObj->writeFlagOrFile, "stdout") != 0){
        fflush(writeStream);
        //Will create file if it does not exist
        writeStream = fopen(writeDataObj->writeFlagOrFile, "w");
        fileBool = 1;
    }

    while(1){
        //If plaintext/ciphertext text files or key text file could not be opened or were empty
        //Or if the key was not the proper byte length, informing current thread to quite
        if((ptBuffObj->errorQuiteBool == 1) || (keyBuffObj->errorQuiteBool == 1)){
            //Removing file if created by the program
            if(fileBool == 1)
                remove(writeDataObj->writeFlagOrFile);
            pthread_exit(NULL);
        }

        //Only reading when plaintext/ciphertext and key stream are not having their
        //memory reallocated, or else memory corruption would occur
        while((ptBuffObj->readMutex == 1) && (keyBuffObj->readMutex == 2)){
            //Writing to stdout
            if(fileBool == 0)
                printf("%c", (keyBuffObj->buff[i])^(ptBuffObj->buff[i]));
            //Writing to file
            else
                fputc((keyBuffObj->buff[i])^(ptBuffObj->buff[i]), writeStream);
            
            ++i;
            //Waiting for key stream and plaintext/ciphertext to load more bytes
            while(!((i < ptBuffObj->fullCounter) && (i < keyBuffObj->fullCounter))){
                //If we have reached the end of plaintext/ciphertext buffer
                //and we are finished reading in that file, break loop
                if((i == ptBuffObj->fullCounter) && (ptBuffObj->finishedBool == 1)){
                    //Letting generateKeyStream thread know to stop
                    keyBuffObj->finishedBool = 1;
                    breakBool = 1;
                    //If writing to a file, close that file
                    if(fileBool == 1)
                        fclose(writeStream);
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
