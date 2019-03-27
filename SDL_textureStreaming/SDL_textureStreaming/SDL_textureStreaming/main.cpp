//
//  main.cpp
//  SDL_textureStreaming
//  Toy program to play with texture streaming
//  Created by William Cole on 1/20/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include "lDataStream.h"
#include "lTexture.h"

//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//sdl constants
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
lTexture gTexture;
lDataStream fooImages;

//forward decalrations
bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        gWindow = SDL_CreateWindow("Walking again", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag = false;
        }else{
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag = false;
            }else{
                //setup the texture and renderer
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                gTexture = lTexture(gRenderer);
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
    //load the data stream images
    if(!fooImages.loadImages()){
        printf("Could not setup the foo images!");
        successFlag = false;
    }
    //create blank texture
    if(!gTexture.createTexture(64, 205)){
        printf("Could not create blank texture!");
        successFlag = false;
    }
    return successFlag;
}

void close(){
    fooImages.free();
    gTexture.free();
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;
    
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
            bool quit = false;
            SDL_Event e;
            while(!quit){
                while (SDL_PollEvent(&e) != 0) {
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                }
                //here we render
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                
                //now lock, copy, and unlock
                gTexture.lockTexture();
                gTexture.copyPixels(fooImages.getBuffer());
                gTexture.unlockTexture();
                
                gTexture.render((SCREEN_WIDTH - gTexture.getWidth())/2, (SCREEN_HEIGHT - gTexture.getHeight())/2);
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
