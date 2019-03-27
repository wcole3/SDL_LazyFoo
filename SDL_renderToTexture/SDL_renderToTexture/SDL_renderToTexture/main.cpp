//
//  main.cpp
//  SDL_renderToTexture
//  Toy program to render to a texture and then spin the texture instead of rendering to window and having to change the coordinates each time
//  Created by William Cole on 1/21/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"
#include <stdio.h>
#include <string>

//screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//sdl constants
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
lTexture gTexture;

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
        gWindow = SDL_CreateWindow("You spin my box right round", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag = false;
        }else{
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag = false;
            }else{
                //setup renderer and texture
                SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
                gTexture = lTexture(gRenderer);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! IMG error: %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag = true;
    //create blank texture that can be set as the streaming target
    if(!gTexture.createTexture(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SDL_TEXTUREACCESS_TARGET)){
        printf("Could not create blank texture!");
        successFlag = false;
    }
    return successFlag;
}

void close(){
    gTexture.free();
    
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
        printf("Could not initialize!\n");
    }else{
        if(!loadMedia()){
            printf("Could not laod media!\n");
        }else{
            //start main loop
            bool quit = false;
            SDL_Event e;
            //we are going to spin the texture so need an angle
            double angle = 0;
            
            while(!quit){
                while(SDL_PollEvent(&e) != 0){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                }
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                //set the texture as the render target
                gTexture.setRenderTarget();
                //do the rendering here
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                
                //draw a solid rectanlge
                SDL_Rect redRect = {gTexture.getWidth()/4, gTexture.getHeight()/4, gTexture.getWidth()/2, gTexture.getHeight()/2};
                SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
                SDL_RenderFillRect(gRenderer, &redRect);
                //draw a grenn outline
                SDL_Rect greenRect = {gTexture.getWidth()/6, gTexture.getHeight()/6, (gTexture.getWidth()*2)/3, (gTexture.getHeight()*2)/3};
                SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
                SDL_RenderDrawRect(gRenderer, &greenRect);
                //now a blue horizontal line
                SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
                SDL_RenderDrawLine(gRenderer, 0, gTexture.getHeight()/2, gTexture.getWidth(), gTexture.getHeight()/2);
                //draw a yellow dotted vertical line
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 255);
                for(int i = 0; i < gTexture.getHeight(); i += 4){
                    SDL_RenderDrawPoint(gRenderer, gTexture.getWidth()/2, i);
                }
                //now set the render target back to normal window
                SDL_SetRenderTarget(gRenderer, NULL);
                //render the texture
                gTexture.render((SCREEN_WIDTH - gTexture.getWidth())/2, (SCREEN_HEIGHT - gTexture.getHeight())/2, NULL, angle, NULL);
                SDL_RenderPresent(gRenderer);
                //rotate the texture
                angle += 2;
                if(angle > 360){
                    angle -= 360;
                }
                SDL_Delay(100);
            }
        }
    }
    close();
    return 0;
}
