//
//  main.cpp
//  SDL_windowEvents
//
//  Created by William Cole on 1/5/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"
#include "lWindow.h"
#include <string>
#include <sstream>
#include <stdio.h>

//screen size
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//SDL constants
SDL_Renderer* gRenderer = NULL;
lTexture bckTexture;
lWindow gWindow;
//forward decs
bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDl error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        //create window
        if(!gWindow.init(SCREEN_WIDTH, SCREEN_HEIGHT)){
            printf("Could not create window!");
            successFlag = false;
        }else{
            //setup renderer
            gRenderer = gWindow.createRenderer();
            if(gRenderer == NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag = false;
            }else{
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                bckTexture = lTexture(gRenderer);
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
    if(!bckTexture.loadFromFile("window.png", SDL_FALSE)){
        printf("Could not load background texture!");
        successFlag = false;
    }
    return successFlag;
}

void close(){
    bckTexture.free();
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    gWindow.free();
    
    SDL_Quit();
    IMG_Quit();
}
int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media");
        }else{
            bool quit = false;
            SDL_Event e;
            while(!quit){
                while(SDL_PollEvent(&e) != 0 ){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                    gWindow.handleEvent(e);
                }
                //we only need to render if the window isnt minimized
                if(!gWindow.isMinimized()){
                    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                    SDL_RenderClear(gRenderer);
                    bckTexture.render((gWindow.getWidth() - bckTexture.getWidth())/2, (gWindow.getHeight() - bckTexture.getHeight())/2);
                    SDL_RenderPresent(gRenderer);
                }
            }
        }
    }
    
    close();
    return 0;
}
