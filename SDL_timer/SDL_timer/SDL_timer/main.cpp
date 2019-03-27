//
//  main.cpp
//  SDL_timer
//  Toy program generating a timer that reset when enter is hit
//  Created by William Cole on 11/30/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include "lTexture.h"

//screen globals
int screenW=640;
int screenH=480;

//SDL globals
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
TTF_Font* gFont=NULL;
SDL_Color gTextColor={0,0,0,255};

//textures for two text lines
lTexture gPromptText;
lTexture gTimerText;

//forward decs
bool init();
bool loadMedia();
void close();


//initialize things and load wrapper textures
bool init(){
    bool successFlag=true;
    //setup SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        //create window and renderer
        gWindow=SDL_CreateWindow("Watch me count", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                //set up render and wrapper textures
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                gPromptText=lTexture(gRenderer);
                gTimerText=lTexture(gRenderer);
            }
        }
    }
    //setup the IMG
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    //setup the TTF
    if(TTF_Init()<0){
        printf("Could not initialize TTF! TTF error: %s\n", TTF_GetError());
        successFlag=false;
    }
    
    return successFlag;
}

//load all media
bool loadMedia(){
    bool successFlag=true;
    //load the
    gFont=TTF_OpenFont("lazy.ttf", 22);
    if(gFont==NULL){
        printf("Could not create font! TTF error: %s\n", TTF_GetError());
    }else{
        //set the font for both lTextures
        gPromptText.setFont(gFont);
        gTimerText.setFont(gFont);
        //now render the prompt text
        if(!gPromptText.loadFromRenderedText("Press enter to reset the time", gTextColor)){
            printf("Could not create prompt text");
            successFlag=false;
        }
    }
    
    return successFlag;
}

void close(){
    //free resources
    gPromptText.free();
    gTimerText.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer=NULL;
    gWindow=NULL;
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

int main(int argc, const char * argv[]) {
    // lets get down to buisness
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }else{
            //game loop
            bool quit=false;
            SDL_Event e;
            //need to know the start time
            Uint32 startTime=0;
            std::stringstream timerText;
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    //test if the user wants to quit
                    if(e.type==SDL_QUIT){
                        quit=true;
                    }
                    //if the user hits enter reset the start time
                    if(e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_RETURN){
                        startTime=SDL_GetTicks();
                    }
                }
                //render the timer text
                timerText.str(" ");
                timerText << "Time since enter was last pressed "<<SDL_GetTicks()-startTime;
                gTimerText.loadFromRenderedText(timerText.str().c_str(), gTextColor);
                //here we render things
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                gPromptText.render((screenW - gPromptText.getWidth())/2, 0);
                gTimerText.render((screenW - gTimerText.getWidth())/2, (screenH - gTimerText.getHeight())/2);
                SDL_RenderPresent(gRenderer);
            }
            
        }
    }
    close();
    return 0;
}
