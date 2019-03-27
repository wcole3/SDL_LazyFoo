//
//  main.cpp
//  SDL_textureManipulation
//  Toy program to manipulate individual pixels in a texture
//  Created by William Cole on 1/14/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"
#include <stdio.h>
#include <string>

//need screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//SDL globals
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
lTexture foo;

//forward declarations
bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        gWindow = SDL_CreateWindow("Manual Color Keying", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag = false;
        }else{
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag = false;
            }else{
                //setup texture and renderer
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                foo = lTexture(gRenderer);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

void close(){
    foo.free();
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;
    
    SDL_Quit();
    IMG_Quit();
}

//need to manually color key in the load media function
bool loadMedia(){
    bool successFlag = true;
    if(!foo.loadFromFileWithPixels("foo.png")){
        printf("Could not load foo image!");
        successFlag = false;
    }else{
        //here we do the color keying by accessing the individual pixels
        if(!foo.lockTexture()){
            printf("Could not lock texture!");
            successFlag = false;
        }else{
            Uint32 format = SDL_GetWindowPixelFormat(gWindow);
            SDL_PixelFormat* mappingFormat = SDL_AllocFormat(format);
            //need to get the pixels of the texture and the size
            Uint32* pixels = (Uint32*)foo.getPixels();
            //the pitch is in bits and there is 1 pixel per bit
            int pixelCount = (foo.getPitch()/4) * foo.getHeight();
            //need maps for the pixel to change and the color to change to
            Uint32 colorKey = SDL_MapRGB(mappingFormat, 0, 255, 255);
            Uint32 transparentColor = SDL_MapRGB(mappingFormat, 255, 255, 255);
            //loop through each pixel and set any that are the color defined by the color key to the transparent one
            for(int i = 0; i < pixelCount; i++){
                if(pixels[i] == colorKey){
                    pixels[i] = transparentColor;
                }
            }
            //then lock the texture again
            if(!foo.unlockTexture()){
                printf("Could not unlock texture!");
                successFlag = false;
            }else{
                SDL_FreeFormat(mappingFormat);
            }
        }
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
            //start main loop
            bool quit = false;
            SDL_Event e;
            while(!quit){
                while(SDL_PollEvent(&e) != 0){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                }
                //here we render
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                foo.render(0, 0);
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
