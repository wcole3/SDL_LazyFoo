//
//  main.cpp
//  SDL_multipleWindows
//  Toy program to play with multiple SDL windows, were not rendering anything to then though
//  Created by William Cole on 1/7/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include "lWindow.h"
using namespace std;

//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//SDL constants
const int TOTAL_WINDOWS = 3;
lWindow gWindows[TOTAL_WINDOWS];

//forward declarations
bool init();
void close();

bool init(){
    bool successFlag =true;
    //start SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        //if we load SDL we can setup the windows
        for(int i =0; i < TOTAL_WINDOWS; i++){
            if(!gWindows[i].init(SCREEN_WIDTH, SCREEN_HEIGHT)){
                printf("Could not initialize window #%s!", to_string(i).c_str());
                successFlag = false;
            }
        }
    }
    //load IMG
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

void close(){
    for(int i = 0; i < TOTAL_WINDOWS; i++){
        gWindows[i].free();
    }
    //kill SDL and IMG
    SDL_Quit();
    IMG_Quit();
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        //game loop start
        bool quit = false;
        SDL_Event e;
        
        while(!quit){
            while(SDL_PollEvent(&e) != 0){
                if(e.type == SDL_QUIT){
                    quit = true;
                }
                //else handle events for windows
                for(int i =0; i < TOTAL_WINDOWS; i++){
                    gWindows[i].handleEvent(e);
                }
                //also check if the user presses 1,2, or
                //we're using these to bring windows into focus
                if(e.type == SDL_KEYDOWN){
                    switch (e.key.keysym.sym) {
                        case SDLK_1:
                            gWindows[0].focus();
                            break;
                        case SDLK_2:
                            gWindows[1].focus();
                            break;
                        case SDLK_3:
                            gWindows[2].focus();
                            break;
                        default:
                            break;
                    }
                }
            }
            //now check to see if all windows are closed
            //if they are then quit the program
            bool allWindowsClosed = true;
            for(int i = 0; i < TOTAL_WINDOWS; i++){
                if(gWindows[i].isShown() || gWindows[i].isMinimized()){
                    allWindowsClosed = false;
                }
            }
            if(allWindowsClosed){
                quit = true;
            }
            //otherwise render
            for(int i = 0; i < TOTAL_WINDOWS; i++){
                gWindows[i].render();
            }
        }
    }
    close();
    return 0;
}
