//
//  main.cpp
//  SDL_renderFlip
//  Toy program using renderEx to rotate and flip an image
//  Created by William Cole on 11/11/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"

//globals
int screenW=640;
int screenH=480;

SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
lTexture arrowTex;

bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag=true;
    //setup SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n",SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("You spin my head right round...", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n",SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n",SDL_GetError());
                successFlag=false;
            }else{
                //set up sdl draw color
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                //start up the wrapper class
                arrowTex=lTexture(gRenderer);
            }
        }
    }
    //setup IMG
    int IMG_start=IMG_INIT_PNG;
    if(!(IMG_Init(IMG_start) & IMG_start)){
        printf("Could not start IMG! IMG error: %s\n",IMG_GetError());
        successFlag=false;
    }
    return successFlag;
}

//method to load media at start
bool loadMedia(){
    bool successFlag=true;
    if(!arrowTex.loadFromFile("arrow.png")){
        printf("Could not load media!");
        successFlag=false;
    }
    return successFlag;
}

//shutdown and deallocation
void close(){
    arrowTex.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer=NULL;
    gWindow=NULL;
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
            bool quit=false;
            SDL_Event e;
            double angle=0;
            SDL_RendererFlip flip=SDL_FLIP_NONE;
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    if(e.type==SDL_QUIT){
                        //user wants to quit
                        quit=true;
                    }
                    if(e.type==SDL_KEYDOWN){
                        switch (e.key.keysym.sym) {
                            case SDLK_a:
                                //rotate ccw
                                angle-=60;
                                break;
                            case SDLK_d:
                                //rotate cw
                                angle+=60;
                                break;
                            case SDLK_q:
                                //flip horz
                                flip=SDL_FLIP_HORIZONTAL;
                                break;
                            case SDLK_w:
                                //reset flip type
                                flip=SDL_FLIP_NONE;
                                break;
                            case SDLK_e:
                                //flip vert
                                flip=SDL_FLIP_VERTICAL;
                                break;
                            default:
                                break;
                        }
                    }
                }
                //render stuff here
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                //render the arrow
                arrowTex.render((screenW-arrowTex.getWidth())/2,(screenH-arrowTex.getHeight())/2,NULL,angle, NULL, flip);
                
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
