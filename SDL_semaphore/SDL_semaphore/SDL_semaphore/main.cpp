//
//  main.cpp
//  SDL_semaphore
//  Toy mutlithreading program using semaphores to control access to a variable that each thread changes
//  Created by William Cole on 1/24/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"
#include <string>
#include <stdio.h>

//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//sdl constants
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
lTexture gSplash;
//the data lock; we can do it with semaphores or spinlocks
SDL_sem* gDataLock = NULL;
SDL_SpinLock gDataLockSpin = 0;
//and the data
int gData = -1;

//forward declarations
bool init();
bool loadMedia();
void close();
//toy method for our threads to play with
int worker(void* data);
//method for threads and spin lock
int workerSpin(void* data);

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        gWindow = SDL_CreateWindow("Busy little threads", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag = false;
        }else{
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag = false;
            }else{
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                gSplash = lTexture(gRenderer);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

bool loadMedia(){
    //create the semaphore with initial value 1; it is incremented and decremented as threads access it
    gDataLock = SDL_CreateSemaphore(1);
    //now load texture
    bool successFlag = true;
    if(!gSplash.loadFromFile("splash.png", SDL_FALSE)){
        printf("Could not load splash texture!\n");
        successFlag = false;
    }
    return successFlag;
}

void close(){
    gSplash.free();
    
    SDL_DestroySemaphore(gDataLock);
    gDataLock = NULL;
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;
    
    SDL_Quit();
    IMG_Quit();
}

//here is our method for the threads; the data will be the thread name
int worker(void* data){
    //first we need to seed the rand number
    srand(SDL_GetTicks());
    //run the loop 5 times
    for(int i = 0; i < 5; ++i){
        //we want to wait for a randomw amount of time
        SDL_Delay(50 + rand() % 50);
        //then lock the data
        SDL_SemWait(gDataLock);
        //now check the data value
        printf("%s found the value: %d\n", data, gData);
        //alter the data value
        gData = rand() % 256;
        //display what the value was changed to
        printf("%s changed the value to: %d\n", data, gData);
        //then unlock the data value
        SDL_SemPost(gDataLock);
        //wait a random amount of time
        SDL_Delay(50 + rand() % 50);
    }
    
    return 0;
}

//here is a method that does the exact same thing as thread, but used atomic locks
int workerSpin(void* data){
    //first we need to seed the rand number
    srand(SDL_GetTicks());
    //run the loop 5 times
    for(int i = 0; i < 5; ++i){
        //we want to wait for a randomw amount of time
        SDL_Delay(50 + rand() % 50);
        //then lock the data
        SDL_AtomicLock(&gDataLockSpin);
        //now check the data value
        printf("%s found the value: %d\n", data, gData);
        //alter the data value
        gData = rand() % 256;
        //display what the value was changed to
        printf("%s changed the value to: %d\n", data, gData);
        //then unlock the data value
        SDL_AtomicUnlock(&gDataLockSpin);
        //wait a random amount of time
        SDL_Delay(50 + rand() % 50);
    }
    
    return 0;
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!\n");
    }else{
        if(!loadMedia()){
            printf("Could not load media!\n");
        }else{
            //main loop start
            bool quit = false;
            SDL_Event e;
            //seed random and setup threads
            srand(SDL_GetTicks());
            //use either worker or workerSpin
            SDL_Thread* threadA = SDL_CreateThread(workerSpin, "Thread A", (void*)"Thread A");
            //wait a random amount before startign thread B
            SDL_Delay(rand() % 50);
            SDL_Thread* threadB = SDL_CreateThread(workerSpin, "Thread B", (void*)"Thread B");
            while(!quit){
                while(SDL_PollEvent(&e) != 0){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                }
                //render here
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                gSplash.render(0, 0);
                SDL_RenderPresent(gRenderer);
            }
            //wait from trheads to finish
            SDL_WaitThread(threadA, NULL);
            SDL_WaitThread(threadB, NULL);
        }
    }
    close();
    return 0;
}
