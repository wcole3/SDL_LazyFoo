//
//  main.cpp
//  SDL_fpsCounter
//  A simple fps counter
//  Created by William Cole on 12/3/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#include "lTimer.h"
#include "lTexture.h"
#include <stdio.h>
#include <string>
#include <sstream>

//screen globals
int screenW=640;
int screenH=480;

//sdl globals
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
lTexture gCounterText;
lTimer counter;
SDL_Color gTextColor={0,0,0,255};
TTF_Font* gFont=NULL;
//forward dec
bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag=true;
    //Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        printf( "Warning: Linear texture filtering not enabled!" );
    }
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Let me count the frames", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                gCounterText=lTexture(gRenderer);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
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
        printf("Could not open font! TTF error: %s\n", TTF_GetError());
        successFlag=false;
    }else{
        gCounterText.setFont(gFont);
    }
    return successFlag;
}

void close(){
    gCounterText.free();
    TTF_CloseFont(gFont);
    gFont=NULL;
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer=NULL;
    gWindow=NULL;
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
            //need a frame counter
            int frames=0;
            
            std::stringstream countText;
            //start the timer
            counter.start();
            
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    if(e.type == SDL_QUIT){
                        quit=true;
                    }
                }
                //calculate the fps
                
                
                float fps=(frames / (counter.getTime() / 1000.f));
                //want a catch clause to handle the first few frames
                if(fps > 20000){
                    fps=0;
                }
                //load the fps into text
                countText.str(" ");
                countText<<"Your FPS is: "<<fps;
                //render text
                if(!gCounterText.loadFromRenderedText(countText.str().c_str(), gTextColor)){
                    printf("Could not render count text!");
                }
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                gCounterText.render((screenW - gCounterText.getWidth())/2, (screenH - gCounterText.getHeight())/2);
                SDL_RenderPresent(gRenderer);
                ++frames;
                
            }
        }
    }
    close();
    return 0;
}
