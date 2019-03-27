//
//  main.cpp
//  SDL_joystick
//  Toy program to use joysticks to control program
//  Created by William Cole on 11/23/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include "lTexture.h"

//screen globals
int screenWidth=640;
int screenHeight=480;

//sdl globals
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
lTexture arrowTexture;
SDL_Joystick* gGameController=NULL;
SDL_Haptic* gHaptic=NULL;

//controller dead space
const int controllerDeadZone=8000;

//forward declarations
bool init();
bool loadMedia();
bool loadController();
void close();

bool init(){
    bool successFlag=true;
    //need to start the event system and the joystick system and the haptic system
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC)<0){
        printf("Could not initialize SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        //create window and renderer
        gWindow=SDL_CreateWindow("Spin the arrow with the joystick", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
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
                arrowTexture=lTexture(gRenderer);
            }
        }
    }
    //now load IMG
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not intialize IMG! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag=true;
    if(!arrowTexture.loadFromFile("arrow.png", SDL_FALSE)){
        printf("Could not load media!");
        successFlag=false;
    }
    return successFlag;
}

//new file to load controller object
bool loadController(){
    bool successFlag=true;
    //check to see if any joysticks are connected
    if(SDL_NumJoysticks()==0){
        printf("There are no Joysticks connected!");
        successFlag=false;
    }
    //if there are any connected open the first one
    gGameController=SDL_JoystickOpen(0);
    if(gGameController==NULL){
        printf("Ccould not open joystick! SDL error: %s\n",SDL_GetError());
        successFlag=false;
    }
    gHaptic=SDL_HapticOpenFromJoystick(gGameController);
    if(gHaptic==NULL){
        printf("Could not open haptic! SDL error: %s\n",SDL_GetError());
        
    }else{
        //initialize haptic
        if(SDL_HapticRumbleInit(gHaptic)<0){
            printf("Could not initialize haptic rumble! SDl error: %s\n", SDL_GetError());
            
        }
    }
    return successFlag;
}

void close(){
    arrowTexture.free();
    
    SDL_JoystickClose(gGameController);
    gGameController=NULL;
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
            if(!loadController()){
                printf("Could not load controller!");
            }else{
                bool quit=false;
                
                SDL_Event e;
                
                //need some ints to hold the joystick direction
                int xDir=0;
                int yDir=0;
                while(!quit){
                    while(SDL_PollEvent(&e)!=0){
                        //check if user want to quit
                        if(e.type==SDL_QUIT){
                            quit=true;
                        }
                        else if(e.type==SDL_JOYAXISMOTION){
                            //check if the motion was from controller 1
                            if(e.jaxis.which==0){
                                //cehck to see whether the x and y axis values are out of the dead zone
                                //the xaxis is typically 0
                                if(e.jaxis.axis==0){
                                    if(e.jaxis.value > controllerDeadZone){
                                        xDir=1;
                                    }
                                    else if(e.jaxis.value < -controllerDeadZone){
                                        xDir=-1;
                                    }else{
                                        xDir=0;
                                    }
                                }
                                //same for the y axis
                                if(e.jaxis.axis==1){
                                    if(e.jaxis.value > controllerDeadZone){
                                        yDir=1;
                                    }else if( e.jaxis.value < -controllerDeadZone){
                                        yDir=-1;
                                    }else{
                                        yDir=0;
                                    }
                                }
                            }
                        }
                        else if(e.type==SDL_JOYBUTTONDOWN){
                            //if a button is pressed we want to rumble
                            if(SDL_HapticRumblePlay(gHaptic, 0.75, 100)){
                                printf("Could not rumble! SDL error: %s\n", SDL_GetError());
                            }
                        }
                    }
                    //here is where we render
                    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                    SDL_RenderClear(gRenderer);
                    //render the arrow texture with a rotation defined by the joystick
                    //reme,ber to convert to degrees
                    double angle=(atan2(double(yDir), double(xDir)) * (180/M_PI));
                    arrowTexture.render((screenWidth-arrowTexture.getWidth())/2, (screenHeight-arrowTexture.getHeight())/2,NULL,angle);
                    SDL_RenderPresent(gRenderer);
                }
            }
        }
    }
    close();
    return 0;
}
