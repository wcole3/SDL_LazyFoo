//
//  main.cpp
//  SDL_viewport
//  Toy program to play with viewports in an SDL window
//  Created by William Cole on 10/22/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

//Some globals
int screenW=720;
int screenH=480;

SDL_Window* gWindow=NULL;
SDL_Renderer* gRender=NULL;
SDL_Texture* gTexture=NULL;

//Some forward method declarations
bool init();
void close();
bool loadMedia();
SDL_Texture* loadTexture(std::string path);

//Start with init, which starts up SDL and SDL_image
bool init(){
    bool successFlag=true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not initalialize SDL! SDL error: %s\n",SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Viewport Practice", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n",SDL_GetError());
            successFlag=false;
        }else{
            gRender=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRender==NULL){
                printf("Could not create renderer! SDL error: %s\n",SDL_GetError());
                successFlag=false;
            }
            SDL_SetRenderDrawColor(gRender, 0, 0, 0, 255);
            //Below is not needed
            SDL_SetRenderDrawBlendMode(gRender, SDL_BLENDMODE_BLEND);
            //if that all works then we need to initialize SDL_image
            int IMG_PNG_flag= IMG_INIT_PNG;
            if(!(IMG_Init(IMG_PNG_flag) & IMG_PNG_flag)){
                //If the IMG init return is not the png flag there is a problem
                printf("Problem initializing SDL_image! IMG error: %s\n",IMG_GetError());
                successFlag=false;
            }
        }
    }
    return successFlag;
}

void close(){
    //Kill the window, renderer, and texture
    //The order matters here for assertation control
    SDL_DestroyTexture(gTexture);
    gTexture=NULL;
    SDL_DestroyRenderer(gRender);
    gRender=NULL;
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    
    //quit SDL and SDL_image
    
    IMG_Quit();
    SDL_Quit();
}

//Now the method that will load a texture from file
SDL_Texture* loadTexture(std::string path){
    SDL_Texture* loadedTexture=NULL;
    //First we need to load a png file into a SDL_surface
    SDL_Surface* loadedSurface=IMG_Load(path.c_str());
    if(loadedSurface==NULL){
        printf("Could not load file at %s! IMG error: %s\n",path.c_str(),IMG_GetError());
    }
    //Once we have the surface we can convert it to a texture
    loadedTexture=SDL_CreateTextureFromSurface(gRender, loadedSurface);
    if(loadedTexture==NULL){
        printf("Could not convert surface to texture! SDL error: %s\n",SDL_GetError());
    }
    //finally realease the temp surface used
    SDL_FreeSurface(loadedSurface);
    //Does below need to be here?
    
    return loadedTexture;
}

//Final method will load all the media files
bool loadMedia(){
    bool successFlag=true;
    gTexture=loadTexture("viewport.png");
    if(gTexture==NULL){
        printf("Could not load media!\n");
        successFlag=false;
    }
    //below is not nedded
    SDL_SetTextureBlendMode(gTexture, SDL_BLENDMODE_BLEND);
    return successFlag;
}

int main(int argc, const char * argv[]) {
    //Must start by initializing things
    if(!init()){
        printf("Could not initialize!\n");
    }else{
        //then load media
        if(!loadMedia()){
            printf("Could not load Media in main!\n");
        }else{
            //If we get here we are ready to go to game loop
            bool quit=false;
            SDL_Event e;
            //if the user doesnt want to quit
            while(!quit){
                //and there is an impending event
                while(SDL_PollEvent(&e)!=0){
                    if(e.type==SDL_QUIT){
                        quit=true;
                        //the user wants to quit
                    }
                }
                
                //if not do the render
                SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
                SDL_RenderClear(gRender);
                
                
                //start with a viewport in the top left
                SDL_Rect topLeft={0,0,screenW/2,screenH/2};
                //Set the view port with the created rect
                SDL_RenderSetViewport(gRender, &topLeft);
                //then copy the loaded texture there
                SDL_RenderCopy(gRender, gTexture, NULL, NULL);
                
                //then do top right corner
                SDL_Rect topRight={screenW/2,0,screenW/2,screenH/2};
                SDL_RenderSetViewport(gRender, &topRight);
                SDL_RenderCopy(gRender, gTexture, NULL, NULL);
                //and finally bottom
                
                SDL_Rect bottom={0,screenH/2,screenW,screenH/2};
                SDL_RenderSetViewport(gRender, &bottom);
                SDL_RenderCopy(gRender, gTexture, NULL, NULL);
                //Want to see if I can draw a rect over half of the bottom viewport
                SDL_Rect coverBox={screenW/2,screenH/4,screenW/4,screenH/4};
                SDL_SetRenderDrawColor(gRender, 255, 0, 0, 255);
                SDL_RenderFillRect(gRender, &coverBox);
                
                //render to screen
                SDL_RenderPresent(gRender);
                
            }
        }
    }
    close();
    return 0;
}
