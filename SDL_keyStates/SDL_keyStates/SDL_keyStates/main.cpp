//
//  main.cpp
//  SDL_keyStates
//  Toy programs to reporduce the key press program using key states
//  Created by William Cole on 11/21/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"
#include <stdio.h>

//globals
int screenW=640;
int screenH=480;

SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;

enum screenSprite{
    startScreen=0,
    leftScreen=1,
    rightScreen=2,
    upScreen=3,
    downScreen=4,
    TOTAL_SCREENS=5
};

lTexture screenTextures[TOTAL_SCREENS];

bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag=true;
    //start up SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n",SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Press a key!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                //if youre here then we can set draw and initialize the lTextures
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                for(int i=0; i<TOTAL_SCREENS; i++){
                    screenTextures[i]=lTexture(gRenderer);
                }
            }
        }
    }
    //now need to start up the IMG lib
    int IMG_START=IMG_INIT_PNG;
    if(!(IMG_Init(IMG_START) & IMG_START)){
        printf("Could not start IMG! IMG error: %s\n",IMG_GetError());
        successFlag=false;
    }
    return successFlag;
}

//method to close and free variables
void close(){
    for(int i=0; i<TOTAL_SCREENS; i++){
        screenTextures[i].free();
    }
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    gRenderer=NULL;
    SDL_Quit();
    IMG_Quit();
}

//need to load media
bool loadMedia(){
    bool successFlag=true;
    //cannot use a for loop to load files as we need to enter the file name
    if(!screenTextures[startScreen].loadFromFile("press.png",SDL_FALSE)){
        printf("Could not load PRESS screen!");
        successFlag=false;
    }
    if(!screenTextures[leftScreen].loadFromFile("left.png",SDL_FALSE)){
        printf("Could not load PRESS screen!");
        successFlag=false;
    }
    if(!screenTextures[rightScreen].loadFromFile("right.png",SDL_FALSE)){
        printf("Could not load PRESS screen!");
        successFlag=false;
    }
    if(!screenTextures[upScreen].loadFromFile("up.png",SDL_FALSE)){
        printf("Could not load PRESS screen!");
        successFlag=false;
    }
    if(!screenTextures[downScreen].loadFromFile("down.png",SDL_FALSE)){
        printf("Could not load PRESS screen!");
        successFlag=false;
    }
    
    return successFlag;
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    //startup everything
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }else{
            //start for game loop
            bool quit=false;
            SDL_Event e;
            //start with the start screen
            screenSprite key=startScreen;
            const Uint8* keyboardState=NULL;
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    if(e.type==SDL_QUIT){
                        //user wants to quit
                        quit=true;
                    }
                    keyboardState=SDL_GetKeyboardState(NULL);
                    if(keyboardState[SDL_SCANCODE_UP]){
                        key=upScreen;
                    }
                    if(keyboardState[SDL_SCANCODE_DOWN]){
                        key=downScreen;
                    }
                    if(keyboardState[SDL_SCANCODE_LEFT]){
                        key=leftScreen;
                    }
                    if(keyboardState[SDL_SCANCODE_RIGHT]){
                        key=rightScreen;
                    }
                    if(keyboardState[SDL_SCANCODE_SPACE]){
                        key=startScreen;
                    }
                }
                //here we render the objects
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                //render the current screen
                screenTextures[key].render(0, 0);
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    
    close();
    return 0;
}
