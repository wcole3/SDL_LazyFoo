//
//  main.cpp
//  SDL_threads
//  Toy program to play with multithreading
//  Created by William Cole on 1/22/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"
#include <stdio.h>
#include <string>

//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//sdl constants
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
lTexture gSplash;

//forward declarations
bool init();
bool loadMedia();
void close();
//function to run on seperate thread
int threadCountdown(void* param);

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        gWindow = SDL_CreateWindow("Counting down...", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
    bool successFlag = true;
    if(!gSplash.loadFromFile("splash.png", SDL_FALSE)){
        printf("Could not load splash texture!\n");
        successFlag = false;
    }
    return successFlag;
}

void close(){
    gSplash.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;
    SDL_Quit();
    IMG_Quit();
}

//here is the method we are running on a seperate thread, the value pased should be in seconds
int threadCountdown(void* param){
    printf("Counting down...\n");
    long int startValue = (intptr_t)param;
    //want to countdown starting from the value passed in param with a 1 second delay
    for(; startValue > 0; --startValue){
        printf("You have %d seconds left...\n", (int)(startValue));
        //wait 1 second
        SDL_Delay(1000);
        if(startValue == 1){
            printf("Boom!\n");
        }
    }
    //kinda pointless since this method will always return 0 unless the thread crashes
    return (int)startValue;
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
            //start the thread with our countdown
            SDL_Thread* countDown= SDL_CreateThread(threadCountdown, "Countdown", (void*)10);
            
            while(!quit){
                while(SDL_PollEvent(&e) != 0){
                    if(e.type ==SDL_QUIT){
                        quit = true;
                    }
                }
                //render here
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                gSplash.render(0, 0);
                SDL_RenderPresent(gRenderer);
            }
            //kill the thread if it hasnt finished
            //need an int to handle the returned value from the thread
            int finalValue;
            SDL_WaitThread(countDown, &finalValue);
            printf("Countdown stopped at: %d\n", finalValue);
        }
    }
    close();
    return 0;
}
