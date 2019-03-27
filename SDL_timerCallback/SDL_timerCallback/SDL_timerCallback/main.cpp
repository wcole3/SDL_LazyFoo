//
//  main.cpp
//  SDL_timerCallback
//  Toy program to play with callback functions
//  Created by William Cole on 1/22/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include "lTexture.h"

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
//our timer callback function; note it has a very specific form
Uint32 callBack(Uint32 interval, void* param);//the first arguement is the interval to wait and the param is the value to print

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        gWindow = SDL_CreateWindow("Check the console", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag =false;
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

//here is our callback function
Uint32 callBack(Uint32 interval, void* param){
    //after a set interval print to screen with message
    printf("Callback function triggered with message: %s\n", (char*)param);
    //return 0 letting the program know we are finished
    return 0;
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!\n");
    }else{
        if(!loadMedia()){
            printf("Could not load Media!\n");
        }else{
            //start main loop
            bool quit = false;
            SDL_Event e;
            //add a timer here with callback set at 30 seconds; NOTE the compiler has trouble with the last param so cast it explicitly to void pointer
            SDL_TimerID timer = SDL_AddTimer(30*1000, callBack, (void*)"30 seconds elasped!");
            while(!quit){
                while(SDL_PollEvent(&e) != 0){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                }
                //here we render
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                gSplash.render(0, 0);
                SDL_RenderPresent(gRenderer);
            }
            //need to stop the timer if the callback was not executed
            SDL_RemoveTimer(timer);
        }
    }
    close();
    return 0;
}
