//
//  main.cpp
//  SDL_music
//  Toy prgram to play music and sound effects
//  Created by William Cole on 11/29/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>
#include "lTexture.h"

//some screen parameters
int screenW=640;
int screenH=480;

//SDL globals
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
lTexture promptTexture;
//SDL mixer objects
Mix_Music* gBeat=NULL;
Mix_Chunk* gScratch=NULL;
Mix_Chunk* gLow=NULL;
Mix_Chunk* gMedium=NULL;
Mix_Chunk* gHigh=NULL;

//forward dec
bool init();
bool loadMedia();
void close();

//free resources and quit libs
void close(){
    promptTexture.free();
    Mix_FreeMusic(gBeat);
    Mix_FreeChunk(gScratch);
    Mix_FreeChunk(gLow);
    Mix_FreeChunk(gMedium);
    Mix_FreeChunk(gHigh);
    gBeat=NULL;
    gScratch=NULL;
    gLow=NULL;
    gMedium=NULL;
    gHigh=NULL;
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    gRenderer=NULL;
    
    //quit sdl, img and mix
    SDL_Quit();
    IMG_Quit();
    Mix_Quit();
    
}

//load libs and create texture wrapper
bool init(){
    bool successFlag=true;
    //start SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
        printf("Could not initialize SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        //create globals
        gWindow=SDL_CreateWindow("Get Funky", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDl error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                //initialize ltext and draw color
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                promptTexture=lTexture(gRenderer);
            }
        }
    }
    //start up IMG
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not initialize SDL IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    //now start up mixer, using default values, stereo output, and 2kb output size
    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024)<0){
        printf("Could not intialize SDl mixer! MIX error: %s\n", Mix_GetError());
        successFlag=false;
    }
    
    return successFlag;
}

//method to load all media
bool loadMedia(){
    bool successFlag=true;
    //load the texture
    if(!promptTexture.loadFromFile("prompt.png", SDL_FALSE)){
        printf("Could not load texture!");
        successFlag=false;
    }
    //load the music
    if((gBeat=Mix_LoadMUS("beat.wav"))==NULL){
        printf("Could not load music file! Mix error: %s\n", Mix_GetError());
        successFlag=false;
    }
    if((gScratch=Mix_LoadWAV("scratch.wav"))==NULL){
        printf("Could not load scratch file! Mix error: %s\n", Mix_GetError());
        successFlag=false;
    }
    if((gLow=Mix_LoadWAV("low.wav"))==NULL){
        printf("Could not load scratch file! Mix error: %s\n", Mix_GetError());
        successFlag=false;
    }
    if((gMedium=Mix_LoadWAV("medium.wav"))==NULL){
        printf("Could not load scratch file! Mix error: %s\n", Mix_GetError());
        successFlag=false;
    }
    if((gHigh=Mix_LoadWAV("high.wav"))==NULL){
        printf("Could not load scratch file! Mix error: %s\n", Mix_GetError());
        successFlag=false;
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
            //start game loop
            bool quit=false;
            SDL_Event e;
            
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    //check if user wants to quit
                    if(e.type==SDL_QUIT){
                        quit=true;
                    }
                    //check if the keys were pressed
                    if(e.type==SDL_KEYDOWN){
                        switch (e.key.keysym.sym) {
                            //chunks on 1,2,3,4
                            case SDLK_1:
                                Mix_PlayChannel(-1, gScratch, 0);
                                break;
                            case SDLK_2:
                                Mix_PlayChannel(-1, gLow, 0);
                                break;
                            case SDLK_3:
                                Mix_PlayChannel(-1, gMedium, 0);
                                break;
                            case SDLK_4:
                                Mix_PlayChannel(-1, gHigh, 0);
                                break;
                            //play/pause
                            case SDLK_9:
                                if(Mix_PlayingMusic()==0){
                                    Mix_PlayMusic(gBeat, -1);
                                }else{
                                    if(Mix_PausedMusic()==1){
                                        Mix_ResumeMusic();
                                    }else{
                                        Mix_PauseMusic();
                                    }
                                }
                                break;
                            //stop
                            case SDLK_0:
                                Mix_HaltMusic();
                                break;
                            default:
                                break;
                        }
                    }
                }
                //render here
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                promptTexture.render(0, 0);
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    
    close();
    return 0;
}
