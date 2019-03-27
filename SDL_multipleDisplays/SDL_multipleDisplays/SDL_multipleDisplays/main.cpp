//
//  main.cpp
//  SDL_multipleDisplays
//  Toy program to move a window from one display to another
//  Created by William Cole on 1/8/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include "lWindow.h"

//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//SDL constants
lWindow gWindow;

//forward declarations
bool init();
void close();

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        //setup window
        if(!gWindow.init(SCREEN_WIDTH, SCREEN_HEIGHT)){
            printf("Could not setup lWindow object!");
            successFlag = false;
        }
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
            printf("Warning: Linear texture filtering not enabled!");
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

void close(){
    gWindow.free();
    
    SDL_Quit();
    IMG_Quit();
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        //start game loop
        bool quit = false;
        SDL_Event e;
        while(!quit){
            while(SDL_PollEvent(&e) != 0){
                if(e.type == SDL_QUIT){
                    quit = true;
                }
                //otherwise handle window events
                gWindow.handleEvent(e);
            }
            //then render
            gWindow.render();
        }
    }
    close();
    return 0;
}
