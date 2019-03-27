//
//  main.cpp
//  SDL_mutex
//  Toy program using mutex to control thread access to a data buffer
//  Created by William Cole on 1/28/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"
#include "lWindow.h"
#include <stdio.h>
#include <string>
#include <SDL2/SDL_thread.h>

//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//Sdl constants
SDL_Window* gWindow = NULL;
lWindow myWindow;
lTexture gSplash;
//the mutex to protect the data buffer
SDL_mutex* gDataLock = NULL;
//sdl conditions to singnal whether the data buffer can be accessed
SDL_cond* gCanConsume = NULL;
SDL_cond* gCanProduce = NULL;
//the data buffer
int gData = -1;


// forward declarations
bool init();
bool loadMedia();
void close();

//thread functions
int produce(void* data);
int consume(void* data);
//the actual functions to consume or produce
void produce();
void consume();

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        //setup window
        if(!myWindow.init(SCREEN_WIDTH, SCREEN_HEIGHT)){
            printf("Could not create window!\n");
            successFlag = false;
        }else{
            //setup texture
            gSplash = lTexture(myWindow.getRenderer());
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag = true;
    if(!gSplash.loadFromFile("splash.png", SDL_FALSE)){
        printf("Could not load splash texture!\n");
        successFlag = false;
    }
    //setup mutex and conditions
    gDataLock = SDL_CreateMutex();
    gCanConsume = SDL_CreateCond();
    gCanProduce = SDL_CreateCond();
    return successFlag;
}

void close(){
    gSplash.free();
    myWindow.free();
    
    SDL_DestroyCond(gCanProduce);
    SDL_DestroyCond(gCanConsume);
    SDL_DestroyMutex(gDataLock);
    gCanProduce = NULL;
    gCanConsume = NULL;
    gDataLock = NULL;
    
    SDL_Quit();
    IMG_Quit();
}

//thread functions
int produce(void* data){
    //there isnt any data that will be passed
    printf("\n...Production started...\n");
    //seed random
    srand(SDL_GetTicks());
    for(int i = 0; i < 5; ++i){
        //want to produce 5 times
        produce();
        SDL_Delay(rand() % 1000);
    }
    printf("Production finished\n");
    return 0;
}

//consume is similar
int consume(void* data){
    printf("\n...Consumption started...\n");
    //seed rand
    srand(SDL_GetTicks());
    for(int i = 0; i < 5; ++i){
        consume();
        SDL_Delay(rand() % 1000);
    }
    printf("Consumption finished\n");
    return 0;
}

//method that actually access the data buffer
void produce(){
    //lock the data
    SDL_LockMutex(gDataLock);
    //then chek if the buffer is full
    if(gData != -1){
        printf("Buffer full...waiting for consumer...\n");
        SDL_CondWait(gCanProduce, gDataLock);
    }
    //if the buffer is empty we can produce
    gData = rand() % 255;
    printf("Produced %d units\n", gData);
    //signal that the buffer is full and can be consumed
    SDL_CondSignal(gCanConsume);
    //and unlcok the mutex
    SDL_UnlockMutex(gDataLock);
}

void consume(){
    //lock mutex
    SDL_LockMutex(gDataLock);
    //check if there is somethint to consume
    if(gData == -1){
        printf("Buffer empty waiting for producer...\n");
        SDL_CondWait(gCanConsume, gDataLock);
    }
    //consume all of the buffer
    printf("Consumed %d units\n", gData);
    gData = -1;
    //signal that we can produce
    SDL_CondSignal(gCanProduce);
    //unlock mutex
    SDL_UnlockMutex(gDataLock);
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!\n");
    }else{
        if(!loadMedia()){
            printf("Could not load media!\n");
        }else{
            //start main loop
            bool quit = false;
            SDL_Event e;
            srand(SDL_GetTicks());
            //setup threads
            SDL_Thread* threadA = SDL_CreateThread(consume, "Producer", NULL);
            //wait a little bit
            SDL_Delay(rand() % 500);
            SDL_Thread* threadB = SDL_CreateThread(produce, "Consumer", NULL);
            while(!quit){
                while(SDL_PollEvent(&e) != 0){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                    myWindow.handleEvent(e);
                }
                //render here
                myWindow.render();
                gSplash.render(0, 0);
                SDL_RenderPresent(myWindow.getRenderer());
            }
            //wait from threads
            SDL_WaitThread(threadA, NULL);
            SDL_WaitThread(threadB, NULL);
        }
    }
    close();
    return 0;
}
