//
//  main.cpp
//  SDL_fpsCap
//  Toy program to cap fps to set value
//  Created by William Cole on 12/3/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include "lTimer.h"
#include "lTexture.h"

int screenW=640;
int screenH=480;
int FRAME_CAP=60;
int TICKS_PER_FRAME=1000/60;

SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
TTF_Font* gFont=NULL;
lTexture gFPSText;
SDL_Color gTextColor={0,0,0,255};

bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag=true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("No more than 60 fps!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                gFPSText=lTexture(gRenderer);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    if(TTF_Init()<0){
        printf("Could not open TTF! TTF error: %s\n", TTF_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag=true;
    gFont=TTF_OpenFont("lazy.ttf", 20);
    if(gFont==NULL){
        printf("Could not open font! TTF error: %s\n", TTF_GetError());
        successFlag=false;
    }else{
        gFPSText.setFont(gFont);
    }
    return successFlag;
}

void close(){
    gFPSText.free();
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
            printf("Could not load media");
        }else{
            //game loop
            bool quit=false;
            SDL_Event e;
            lTimer total_ticks;
            lTimer frame_ticks;
            int frames=0;
            std::stringstream fpsText;
            total_ticks.start();
            while(!quit){
                frame_ticks.start();
                ++frames;
                while(SDL_PollEvent(&e)!=0){
                    if(e.type == SDL_QUIT){
                        quit=true;
                    }
                }
                //calculate the fps
                float fps = frames / (total_ticks.getTime() / 1000.f);
                if(fps>200000){
                    fps=0;
                }
                //do rendering
                fpsText.str(" ");
                fpsText<<"Your average FPS is: "<<fps;
                if(!gFPSText.loadFromRenderedText(fpsText.str().c_str(), gTextColor)){
                    printf("Could not render fps text!");
                }
                
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                gFPSText.render((screenW - gFPSText.getWidth())/2, (screenH - gFPSText.getHeight())/2);
                SDL_RenderPresent(gRenderer);
                
                //now need to check to see if we finished early
                if(frame_ticks.getTime() < TICKS_PER_FRAME){
                    SDL_Delay(TICKS_PER_FRAME - frame_ticks.getTime());
                }
                frame_ticks.stop();
                
            }
            
        }
    }
    close();
    return 0;
}
