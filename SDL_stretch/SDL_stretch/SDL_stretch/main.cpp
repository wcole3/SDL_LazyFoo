//
//  main.cpp
//  SDL_stretch
//  Toy program to load, convert, and strecth bmp images on a screen
//  Created by William Cole on 10/5/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <SDL2/SDL.h>

//Some globals to declare
int screenWidth=640;
int screenHeight=420;
//A window to render to
SDL_Window* gWindow=NULL;
//the screen surface
SDL_Surface* gScreenSurface=NULL;
//The image that iis to be blited
SDL_Surface* gCurrentScreen=NULL;
//method to initialize
bool init();
//method to load media from file
bool loadMedia();
//method to load a bmp and convert to native format
SDL_Surface* loadScreen(std::string path);
//Final method to kill everything
void close();

//Start with initialization
bool init(){
    bool successFlag=true;
    //Start up SDL with event system
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        //if youre here the init failed
        printf("Failed to initialize SDL! SDL error: %s\n",SDL_GetError());
        successFlag=false;
    }
    else{
        //If initialization works then we want to create the window
        gWindow =SDL_CreateWindow("Screen Stretching", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
        //test to make sure the window was made
        if(gWindow==NULL){
            printf("Failed to create window! SDL error: %s\n",SDL_GetError());
            successFlag=false;
        }
        else{
            //if the window was created corrected then get the surface
            gScreenSurface=SDL_GetWindowSurface(gWindow);
        }
    }
    
    return successFlag;
}

//Now the method to load a image from file and convert it to screens' native format
SDL_Surface* loadScreen(std::string path){
    //this will be the screen we eventually return after optimize
    SDL_Surface* convertedSurface=NULL;
    //First get the surface normally
    SDL_Surface* loadedSurface=SDL_LoadBMP(path.c_str());
    //Check to see if we loaded it
    if(loadedSurface==NULL){
        printf("The image was not loaded! SDL error: %s\n", SDL_GetError());
    }
    else{
        //convert the image and send out
        convertedSurface=SDL_ConvertSurface(loadedSurface, gScreenSurface->format, NULL);
        //check if the surface was converted
        if(convertedSurface==NULL){
            printf("Failed to convert image! SDL error: %s\n",SDL_GetError());
        }
        
    }
    //if we get here then we need to free the allocated surface for the load
    SDL_FreeSurface(loadedSurface);
    //returned the optimized Surface
    return convertedSurface;
}

//Need a method to load the image from file
bool loadMedia(){
    bool successFlag=true;
    //get the the needed screen
    gCurrentScreen=loadScreen("SDL_stretch/SDL_stretch/stretch.bmp");
    if(gCurrentScreen==NULL){
        printf("Could not load image! SDL error: %s\n",SDL_GetError());
        successFlag=false;
    }
    
    return successFlag;
}

//method to close out the program
void close(){
    //get rid of surfaces and window in use
    SDL_FreeSurface(gCurrentScreen);
    gCurrentScreen=NULL;
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    //quit SDL
    SDL_Quit();
}

int main(int argc, const char * argv[]) {
    //Now to start the program
    if(!init()){
        printf("Failed to initialize! SDL error: %s\n",SDL_GetError());
    }
    else{
        if(!loadMedia()){
            printf("Failed to load media! SDL error: %s\n",SDL_GetError());
        }
        else{
            //Here is where we can start doing things
            bool quit=false;
            while(!quit){
                SDL_Event e;
                //need a game loop
                while(SDL_PollEvent(&e)!=0){
                    if(e.type==SDL_QUIT){
                        //the user wants to quit
                        quit=true;
                    }
                    //Can play around with a sdl rect to customize the size of the image
                    SDL_Rect rect;
                    rect.x=50;
                    rect.y=100;
                    rect.h=screenHeight/2;
                    rect.w=screenWidth/2;
                    
                    //if not blit the surface
                    SDL_BlitScaled(gCurrentScreen,NULL,gScreenSurface,&rect);
                    SDL_UpdateWindowSurface(gWindow);
                }
            }
            
        }
    }
    
    close();
    return 0;
}
