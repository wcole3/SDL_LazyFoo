//
//  main.cpp
//  SDL_Move
//  Toy program to move a dot around the screen
//  Created by William Cole on 12/4/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"
#include "dot.h"
#include <stdio.h>
#include "lTimer.h"

//screen globals
int screenW=640;
int screenH=480;
int MAX_TICKS_PER_FRAME = 1000/60;

SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
dot movingDot;

bool init();
bool loadMedia();
void close();

void close(){
    movingDot.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    gRenderer=NULL;
    SDL_Quit();
    IMG_Quit();
}

bool init(){
    bool successFlag=true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not create window! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Move the dot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                //setup the dot object with its renderer
                movingDot=dot(gRenderer);
                //the the screen size
                movingDot.setScreenSize(screenW, screenH);
            }
            
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    
    return successFlag;
}

bool loadMedia(){
    bool successFlag=true;
    //load the dot's texture
    if(!movingDot.loadFromFile("dot.bmp", SDL_FALSE)){
        printf("Could not load dot texture!");
        successFlag=false;
    }
    return successFlag;
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }else{
            //here is the game loop
            bool quit=false;
            SDL_Event e;
            //need a time for the frames
            lTimer ticker;
            
            while(!quit){
                
                while(SDL_PollEvent(&e)!=0){
                    if(e.type==SDL_QUIT){
                        quit=true;
                    }
                    //otherwise handle the event for the dot
                    movingDot.handleEvent(e);
                }
                float timeStep=(ticker.getTime() / 1000.f);
                //after handling the event we can move and render things
                movingDot.move(timeStep);
                //restart the timer
                ticker.start();
                //rendering
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                movingDot.render((int)movingDot.getXPos(), (int)movingDot.getYPos());
                SDL_RenderPresent(gRenderer);
                
            }
            
        }
    }
    close();
    return 0;
}
