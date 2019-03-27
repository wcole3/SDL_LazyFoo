//
//  main.cpp
//  SDL_scrolling
//  Toy program to have a dot moving around a level with only a portion in the camera
//  Created by William Cole on 12/12/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include "lTimer.h"
#include "lTexture.h"
#include "lRigidDot.h"
#include "lUtilityFunc.h"


//screen constants and level constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
int LEVEL_WIDTH = 1280;
int LEVEL_HEIGHT = 960;

//sdl constants
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
lTexture bckTexture;
lRigidDot player;
lRigidDot randomDot;

bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Scroll around", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                //setup player and background
                bckTexture=lTexture(gRenderer);
                player=lRigidDot(LEVEL_WIDTH, LEVEL_HEIGHT, gRenderer);
                randomDot=lRigidDot(SCREEN_WIDTH,SCREEN_HEIGHT, gRenderer, 100, 200);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not load IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag=true;
    //load background texture
    if(!bckTexture.loadFromFile("bg.png", SDL_FALSE)){
        printf("Could not load background texture!");
        successFlag=false;
    }
    //load player texture
    if(!player.loadFromFile("dot.bmp", SDL_TRUE)){
        printf("Could not create player texture!");
        successFlag=false;
    }
    if(!randomDot.loadFromFile("dot.bmp", SDL_FALSE)){
        printf("could not load random dot!");
        successFlag=false;
    }
    return successFlag;
}

void close(){
    player.free();
    bckTexture.free();
    randomDot.free();
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer=NULL;
    gWindow=NULL;
    
    SDL_Quit();
    IMG_Quit();
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }else{
            //game loop
            bool quit = false;
            SDL_Event e;
            SDL_Rect box ={200, 100,10,10};
            //need a camera object
            SDL_Rect camera = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
            lTimer ticker;
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    if(e.type==SDL_QUIT){
                        //user wants to quit
                        quit=true;
                    }
                    //otherwise handle player events
                    player.handleEvent(e);
                }
                //now move player, relocate camera and render
                float timeStep = ticker.getTime() / 1000.f;
                player.move(timeStep, box, randomDot.getCollider());
                ticker.start();
                
                //now reloacte camera so that dot is centered
                camera.x = (int)player.getXPos() - camera.w / 2;
                camera.y = (int)player.getYPos() - camera.h / 2;
                //some checks to make sure the camera isnt off the level
                if(camera.x < 0){
                    camera.x=0;
                }
                if(camera.x > LEVEL_WIDTH-camera.w){
                    camera.x = LEVEL_WIDTH-camera.w;
                }
                if(camera.y < 0){
                    camera.y=0;
                }
                if(camera.y > LEVEL_HEIGHT - camera.h){
                    camera.y = LEVEL_HEIGHT - camera.h;
                }
                //now we can render
                SDL_RenderClear(gRenderer);
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                bckTexture.render(0, 0, &camera);
                player.render(camera.x, camera.y);
                randomDot.render(camera.x, camera.y);
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
