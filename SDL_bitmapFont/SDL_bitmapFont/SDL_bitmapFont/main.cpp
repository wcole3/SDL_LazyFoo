//
//  main.cpp
//  SDL_bitmapFont
//  Program to manually load and setup a font from a png image
//  Created by William Cole on 1/18/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include "lTexture.h"
#include "lBitmapFont.h"

//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;
//SDL constants
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
lTexture gFontTexture;
lBitmapFont gFont;
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
        gWindow = SDL_CreateWindow("Write something", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag = false;
        }else{
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag = false;
            }else{
                //now setup renderer and lTexture
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                gFontTexture = lTexture(gRenderer);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag = true;
    SDL_Color colorKey={0,255,255};
    if(!gFontTexture.loadFromFileWithPixels("lazyfont.png", true, colorKey)){
        printf("Could not load font texture!");
        successFlag = false;
    }
    //now we can setup the font
    if(!gFont.buildFont(&gFontTexture)){
        printf("Could not build font!");
        successFlag = false;
    }
    return successFlag;
}

void close(){
    gFont.~lBitmapFont();
    gFontTexture.free();
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;
    
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
                while(SDL_PollEvent(&e) != 0){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                }
                //here we render
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                gFont.render(30, 30, "ABCde\n hgty");
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
