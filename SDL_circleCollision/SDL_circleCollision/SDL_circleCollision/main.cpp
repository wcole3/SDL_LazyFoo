//
//  main.cpp
//  SDL_circleCollision
//  Toy program to render a movable dot that can collide with objects using circular collision detection
//  Created by William Cole on 12/10/18.
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


//start with screen globals
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//SDL globals
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
//objects on screen
lRigidDot player;
lRigidDot staticEnemy;
SDL_Rect cover={200, 100, 40, 100};

//forward decs
bool init();
bool loadMedia();
void close();

void close(){
    player.free();
    staticEnemy.free();
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer=NULL;
    gWindow=NULL;
    
    SDL_Quit();
    IMG_Quit();
}

bool init(){
    bool successFlag=true;
    //start SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL eror: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Don't hit shit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                //set render conditions and dots
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                //set player at top left which is the default
                player=lRigidDot(SCREEN_WIDTH, SCREEN_HEIGHT, gRenderer);
                //put the enemy behind cover
                staticEnemy=lRigidDot(SCREEN_WIDTH , SCREEN_HEIGHT, gRenderer, 261, 150);
            }
        }
    }
    //set a hint to see if vSync was actually enabled
    if(!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1")){
        printf("VSYNC was not enabled!");
    }
    //setup IMG
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag=true;
    //load the player first
    //the player will be rendered last so we set colorkeying true for the white outline
    if(!player.loadFromFile("dot.bmp", SDL_TRUE)){
        printf("Could not load player texture!");
        successFlag=false;
    }
    if(!staticEnemy.loadFromFile("dot.bmp", SDL_FALSE)){
        printf("Could not load enemy texture!");
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
            printf("Could not load Media!");
        }else{
            //start of game lopp
            bool quit = false;
            SDL_Event e;
            //clock for frame time
            lTimer ticker;
            while(!quit){
                while(SDL_PollEvent(&e)){
                    if(e.type==SDL_QUIT){
                        //see if the user want to quit
                        quit=true;
                    }
                    //check events for the player
                    player.handleEvent(e);
                }
                //now move things and render
                //time in seconds
                float timeStep = (ticker.getTime() / 1000.f);
                player.move(timeStep, cover, staticEnemy.getCollider());
                //reset clock
                ticker.start();
                //now render
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                
                SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
                SDL_RenderDrawRect(gRenderer, &cover);
                
                staticEnemy.render(staticEnemy.getXPos(),staticEnemy.getYPos());
                player.render(player.getXPos(), player.getYPos());
                
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
