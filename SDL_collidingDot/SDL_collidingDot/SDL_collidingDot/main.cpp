//
//  main.cpp
//  SDL_collidingDot
//  Toy program to use axis based collision detection of a dot moving on screen
//  Created by William Cole on 12/6/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include "lTexture.h"
#include "lTimer.h"
#include "lRigidDot.h"

//screen constants
int SCREEN_WIDTH=640;
int SCREEN_HEIGHT=480;

//sdl globals
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
lRigidDot player;


//forward decs
bool init();
bool loadMedia();
void close();

void close(){
    player.free();
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
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Don't hit the box", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                //setup player
                player=lRigidDot(SCREEN_WIDTH, SCREEN_HEIGHT, gRenderer);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag=true;
    if(!player.loadFromFile("dot.bmp", SDL_FALSE)){
        printf("Could not load player texture!");
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
            //game loop setup
            bool quit=false;
            SDL_Event e;
            //the collidable box
            SDL_Rect rigidBox={100, 100, 20, 50};
            //need a times for the physics based movement
            lTimer ticker;
            while(!quit){
                while(SDL_PollEvent(&e)){
                    if(e.type==SDL_QUIT){
                        quit=true;
                    }
                    //handle events for the player
                    player.handleEvent(e);
                }
                //after events have been handled we can begin rendering
                //first move the player
                float time=(ticker.getTime() / 1000.f);//put time into seconds since player velocity is in pixels per second
                player.move(time, rigidBox);
                //reset the ticker so we know how much time passes between calls
                ticker.start();
                //after the player has moved render screen, box, and player
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(gRenderer, &rigidBox);
                player.render((int)player.getXPos(), (int)player.getYPos());
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
