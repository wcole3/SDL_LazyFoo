//
//  main.cpp
//  SDL_stopwatch
//  Toy program simulating a stop watch inside a window
//  Created by William Cole on 12/2/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include "lTexture.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include "lTimer.h"

//screen globals
int screenW=640;
int screenH=480;
//SDL globals
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
//three textures
lTexture gStartText;
lTexture gPauseText;
lTexture gCountText;
TTF_Font* gFont;
SDL_Color gTextColor={0,0,0,255};
//forward decs
bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag=true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Whats my time coach", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                gStartText=lTexture(gRenderer);
                gPauseText=lTexture(gRenderer);
                gCountText=lTexture(gRenderer);
            }
        }
    }
    //start IMG
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    //start TTF
    if(TTF_Init()<0){
        printf("Could not start TTF! TTF error: %s\n", TTF_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag=true;
    gFont=TTF_OpenFont("lazy.ttf", 20);
    if(gFont==NULL){
        printf("Could not open lazy font! TTF error: %s\n", TTF_GetError());
        successFlag=false;
    }
    gStartText.setFont(gFont);
    gPauseText.setFont(gFont);
    gCountText.setFont(gFont);
    
    //render out the start and pause texts
    if(!gStartText.loadFromRenderedText("Press s to start or stop the timer", gTextColor)){
        printf("Could not render start text!");
        successFlag=false;
    }
    if(!gPauseText.loadFromRenderedText("Press p to pause//unpause the timer", gTextColor)){
        printf("Could not render pause text!");
        successFlag=false;
    }
    return successFlag;
}

void close(){
    gStartText.free();
    gPauseText.free();
    gCountText.free();
    TTF_CloseFont(gFont);
    gFont=NULL;
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    gRenderer=NULL;
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
    
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }else{
            //game loop
            bool quit=false;
            SDL_Event e;
            lTimer timer;
            std::stringstream timerText;
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    if(e.type == SDL_QUIT){
                        quit=true;
                    }
                    if(e.type == SDL_KEYDOWN){
                        if(e.key.keysym.sym == SDLK_s){
                            //need to find out whether to start or stop
                            if(!timer.isStarted()){
                                timer.start();
                            }
                            else{
                                timer.stop();
                            }
                        }
                        if(e.key.keysym.sym == SDLK_p){
                            if(timer.isPaused()){
                                timer.unpause();
                            }else{
                                timer.pause();
                            }
                        }
                    }
                }
                //here we render
                timerText.str(" ");
                timerText<<"Your time is: "<<(timer.getTime()/1000.f);
                gCountText.loadFromRenderedText(timerText.str().c_str(), gTextColor);
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                gStartText.render((screenW - gStartText.getWidth())/2, 0);
                gPauseText.render((screenW - gPauseText.getWidth())/2, gStartText.getHeight());
                gCountText.render((screenW - gCountText.getWidth())/2, (screenH - gCountText.getWidth())/2);
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
