//
//  main.cpp
//  SDL_renderText
//  Toy program to use true text fonts to render text on screen
//  Created by William Cole on 11/14/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include "lTexture.h"

//Some global variables
int screenH=480;
int screenW=640;

SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
//need a font
TTF_Font* gFont=NULL;
//wrapper class texture
lTexture textImg;

//forward declarations
bool init();
bool loadMedia();
void close();

//define methods
bool init(){
    //startup SDL, IMG, and TTF
    //SDL
    bool successFlag=true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Look at that text", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            //create renderer
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                //set up renderer
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
            }
        }
    }
    //now IMG
    int IMG_start=IMG_INIT_PNG;
    if(!(IMG_Init(IMG_start) & IMG_start)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    //now TTF
    if(TTF_Init()==-1){
        printf("Could not start TTF! TTF error: %s\n", TTF_GetError());
        successFlag=false;
    }else{
        //we will open the font here so that we can construct the wrapper class
        gFont=TTF_OpenFont("lazy.ttf", 12);
        if(gFont==NULL){
            printf("Could not create Font! TTF error: %s\n", TTF_GetError());
            successFlag=false;
        }else{
            //create the wrapper texture
            textImg=lTexture(gRenderer);
            textImg.setFont(gFont);
        }
    }
    return successFlag;
}

void close(){
    //free wrapper class
    textImg.free();
    //kill globals
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    TTF_CloseFont(gFont);
    gWindow=NULL;
    gRenderer=NULL;
    gFont=NULL;
    //quit libs
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

bool loadMedia(){
    bool successFlag=true;
    
    //need to create the textimage
    //need a color
    SDL_Color color={0,0,0};
    if(!textImg.loadFromRenderedText("Hello World", color)){
        printf("Could not create text image!");
        successFlag=false;
    }
    
    return successFlag;
}


int main(int argc, const char * argv[]) {
    //lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }else{
            //need to set up the game loop
            bool quit=false;
            SDL_Event e;
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    if(e.type==SDL_QUIT){
                        //user wants to quit
                        quit=true;
                    }
                }
                //other wise render
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                //render the texture
                textImg.render((screenW-textImg.getWidth())/2, (screenH-textImg.getHeight())/2);
                
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
