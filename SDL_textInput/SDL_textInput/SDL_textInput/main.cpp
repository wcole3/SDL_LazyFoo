//
//  main.cpp
//  SDL_textInput
//  Toy program to use keyboard to enter text and render to screen
//  Created by William Cole on 1/2/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#include "lTexture.h"
#include <string>
#include <stdio.h>
#include <sstream>

//Screen constants
int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//SDL globals
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;
SDL_Color gTextColor = {0,0,0,0};
lTexture gPrompt;
lTexture gInputText;

//Forward Dec
bool init();
bool loadMedia();
void close();

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Could not initialize SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Enters some text", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                gPrompt=lTexture(gRenderer);
                gInputText=lTexture(gRenderer);
            }
        }
    }
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not initialize IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    if(TTF_Init() < 0){
        printf("Could not initialize TTF! TTF error: %s\n", TTF_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag = true;
    if((gFont=TTF_OpenFont("lazy.ttf", 20))==NULL){
        printf("Could not open font! TTF error: %s\n", TTF_GetError());
        successFlag=false;
    }else{
        gPrompt.setFont(gFont);
        gInputText.setFont(gFont);
    }
    //we can load the prompt text since it doesnt change
    if(!gPrompt.loadFromRenderedText("Enter some text:", gTextColor)){
        printf("Could not render prompt text!");
        successFlag=false;
    }
    return successFlag;
}

void close(){
    gPrompt.free();
    gInputText.free();
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer=NULL;
    gWindow=NULL;
    TTF_CloseFont(gFont);
    gFont=NULL;
    
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
            //enter the game loop
            bool quit = false;
            SDL_Event e;
            //need a hold for text entry
            std::string textInput = "Some Text";
            //load that into texture
            gInputText.loadFromRenderedText(textInput.c_str(), gTextColor);
            //start keyboard input
            SDL_StartTextInput();
            while(!quit){
                while(SDL_PollEvent(&e) != 0){
                    //want to know if we need to render text
                    bool renderText = false;
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                    //need to handle some special events like copy and pasting and deleting
                    if(e.type == SDL_KEYDOWN){
                        if(e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL){
                            SDL_SetClipboardText(textInput.c_str());
                        }
                        else if(e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL){
                            textInput = SDL_GetClipboardText();
                            renderText = true;
                        }
                        else if(e.key.keysym.sym == SDLK_BACKSPACE){
                            //remove a character
                            textInput.pop_back();
                            renderText= true;
                        }
                    }
                    else if(e.type == SDL_TEXTINPUT){
                        //need to check if the entry is not copy/paste
                        //There is a less verbose way for this logic, but this makes more sense to me
                        if(!(((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (SDL_GetModState() & KMOD_CTRL)) || ((e.text.text[0]=='v' || e.text.text[0] == 'V') && (SDL_GetModState() & KMOD_CTRL)))){
                            //need to add characters
                            textInput += e.text.text;
                            renderText = true;
                        }
                    }
                    if(renderText){
                        if(textInput != ""){
                            //render the input text
                            gInputText.loadFromRenderedText(textInput.c_str(), gTextColor);
                        }
                        else{
                            //render space if empty
                            gInputText.loadFromRenderedText(" ", gTextColor);
                        }
                        
                    }
                }
                //here we render
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                gPrompt.render((SCREEN_WIDTH - gPrompt.getWidth())/2, 0);
                gInputText.render((SCREEN_WIDTH - gInputText.getWidth())/2, gPrompt.getHeight());
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    SDL_StopTextInput();
    close();
    return 0;
}
