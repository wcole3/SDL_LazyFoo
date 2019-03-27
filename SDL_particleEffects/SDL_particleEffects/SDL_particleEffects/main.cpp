//
//  main.cpp
//  SDL_particleEffects
//  Toy program to generate a movable dot that has particle effects that spawn off it
//  Created by William Cole on 1/10/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include "lUtilityFunc.h"
#include "lTimer.h"
#include "lTexture.h"
#include "lRigidDot.h"
#include "lParticle.h"
#include "lWindow.h"


//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//SDL constants
lWindow gWindow;
lRigidDot player;

//forward declarations
bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDl error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        if(!SDL_SetHint(SDL_HINT_RENDER_DRIVER, "1")){
            printf("Warning: Linear texture aliasing not enabled!");
        }
        //load window
        
        if(!gWindow.init(SCREEN_WIDTH, SCREEN_HEIGHT)){
            printf("Could not load window and renderer!");
            successFlag = false;
        }else{
             //setup the player texture
                player = lRigidDot(SCREEN_WIDTH, SCREEN_HEIGHT, gWindow.getRenderer());
            
        }
    }
    //start img
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not load IMG! IMG error: %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

//load media
bool loadMedia(){
    bool successFlag = true;
    if(!loadParticles(gWindow.getRenderer())){
        printf("Could not load particle textures!");
        successFlag = false;
    }
    SDL_Color cyanKey = {0,255,255};
    if(!player.loadFromFile("dot.bmp", SDL_TRUE, cyanKey)){
        printf("Could not load player texture!");
        successFlag = false;
    }
    return successFlag;
}

//close function to deallocate everything
void close(){
    //free player texture
    player.free();
    //free the particles textures
    freeParticleTextures();
    //now free the window and renderer
    gWindow.free();
    //close down SDl and IMG
    SDL_Quit();
    IMG_Quit();
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load Media!");
        }else{
            //start game loop
            bool quit = false;
            SDL_Event e;
            lTimer ticker = lTimer();
            
            while(!quit){
                while(SDL_PollEvent(&e) != 0){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                    player.handleEvent(e);
                }
                //here move the player and render
                float time = ticker.getTime() / 1000.f;
                player.move(time);
                ticker.start();
                
                gWindow.render();
                player.render(player.getXPos(), player.getYPos());
                SDL_RenderPresent(gWindow.getRenderer());
            }
        }
    }
    close();
    return 0;
}
