//
//  main.cpp
//  SDL_loopingScroll
//  DOT Moving across looping background texture
//  Created by William Cole on 12/18/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>
#include <stdio.h>
#include <sstream>
#include "lTimer.h"
#include "lTexture.h"
#include "lUtilityFunc.h"
#include "lRigidDot.h"

//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//sdl constants
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
lTexture bckTexture;
lRigidDot player;

//forward declarations
bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Don't stop moving", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            gRenderer  =SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer == NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                //initialize texture
                bckTexture = lTexture(gRenderer);
                //initialize dot in center of screen
                player = lRigidDot(SCREEN_WIDTH, SCREEN_HEIGHT, gRenderer, (SCREEN_WIDTH/2), (SCREEN_HEIGHT/2));
            }
        }
    }
    //start IMG
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag = true;
    if(!bckTexture.loadFromFile("bg.png", SDL_FALSE)){
        printf("Could not load background texture!");
        successFlag=false;
    }
    if(!player.loadFromFile("dot.bmp", SDL_TRUE)){
        printf("Could not load dot texture!");
        successFlag=false;
    }
    return successFlag;
}

void close(){
    bckTexture.free();
    player.free();
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    gRenderer=NULL;
    
    SDL_Quit();
    IMG_Quit();
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not intialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load Media!");
        }else{
            //start game loop
            bool quit = false;
            SDL_Event e;
            //need a timer
            lTimer ticker;
            //need an index for scrolling
            int scrollIndex  = 0;
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                    //handle movement events
                    player.handleEvent(e);
                }
                //now move and render
                float timeStep = (ticker.getTime() / 1000.f);
                player.move(timeStep, scrollIndex, &bckTexture);
                //reset timer
                ticker.start();
                //now render
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                //need to render the background three times since dot can move both directions
                bckTexture.render(-SCREEN_WIDTH+scrollIndex, 0);
                bckTexture.render(scrollIndex, 0);
                bckTexture.render(SCREEN_WIDTH+scrollIndex, 0);
                //render player
                player.render();
                SDL_RenderPresent(gRenderer);
                
                //only need the following for manual updating
                /*
                //update scroll index by decreasing it
                --scrollIndex;
                if(scrollIndex < -1*SCREEN_WIDTH){
                    //if the index has moved to the point that the first texture is off screen we have to reset
                    scrollIndex=0;
                }
                 */
            }
        }
    }
    close();
    return 0;
}
