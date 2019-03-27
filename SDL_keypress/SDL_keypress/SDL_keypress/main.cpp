//
//  main.cpp
//  SDL_keypress
//  Toy program to use SDL to use key presses to change a window
//
//
//  Created by William Cole on 10/4/18.
//  Copyright © 2018 William Cole. All rights reserved.
//
#include <stdio.h>
#include <SDL2/SDL.h>
#include <iostream>

//Start with enumeration to determine the possible key presses
//This will be used to label the surfaces that correspond to key presses
enum KeyPressSurfaces{
    KEY_PRESS_DEFAULT,
    KEY_PRESS_UP,
    KEY_PRESS_DOWN,
    KEY_PRESS_LEFT,
    KEY_PRESS_RIGHT,
    TOTAL_KEY_PRESSES
};
//small variables for screen size
int screenWidth =640;
int screenHeight=480;

//Start SDl and load a window
bool init();
//method to load bmp files for the screens
bool loadMedia();
//Shutdown method, deallocate resources
void close();
//method to return a specific surface corresponding to a key press, this will be used in loadMedia
SDL_Surface* loadSurface(std::string path);
//Window to be rendered to
SDL_Window* gWindow=NULL;
//surface to blit to window
SDL_Surface* gScreenSurface=NULL;
//Surfaces corresponding to key press, this is an array
SDL_Surface* gKeySurface[TOTAL_KEY_PRESSES];
//Finally a pointer to the current screen
SDL_Surface* gCurrentScreen;

//First the initialization, here we want to load the SDL libaray and create our main window
bool init(){
    //want a flag to determine if the method ran successfully
    bool successFlag = true;
    //first call the SDL_Init function, this loads the library and returns negative if there is a problem
    //the SDL_Init_Video flag tells it to load the event system too
    if( SDL_Init(SDL_INIT_VIDEO)<0){
        //if you are here the program did not load SDL correctly
        printf("SDL could not initialize: %s\n",SDL_GetError());
        successFlag=false;
    }
    //if it does initialize we want to load up a window
    else{
        //create a window with undefined window position
        gWindow=SDL_CreateWindow("Press a key!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth , screenHeight, SDL_WINDOW_SHOWN);
        //want to make sure the window was created
        if(gWindow == NULL){
            //if the window is still null we have a problem
            printf("The window could not be created: %s\n", SDL_GetError());
            successFlag=false;
        }
        //if the window was loaded correctly we want to get the screen's surface for later
        else{
            //get the surface of the window
            gScreenSurface=SDL_GetWindowSurface(gWindow);
        }
            
    }
    return successFlag;
}

//Now a method to load a specific surface at a specific path
SDL_Surface* loadSurface(std::string path){
    //first need the surface to be returned
    SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
    //want a section to catch if there was a problem with the load
    if(loadedSurface==NULL){
        //if youre here the surface wasn't loaded from a bmp file correctly
        printf("Error loading image at :%s! SDL error: %s\n", path.c_str(),SDL_GetError());
    }
    return loadedSurface;
}

//Now implement the loadMedia method which loads all of the images from the directory to be used for key presses
bool loadMedia(){
    bool successFlag=true;
    //want to load the surface associated with each key press; labeled by the values in gKeySurface[]
    //start with the default screen; these addresses are hard coded in but probably dont have to be
    gKeySurface[KEY_PRESS_DEFAULT] = SDL_LoadBMP("SDL_keypress/press.bmp");
    //catch loading error
    if(gKeySurface[KEY_PRESS_DEFAULT]==NULL){
        //print out error
        printf(" Problem loading default screen! SDL Error: %s\n",SDL_GetError());
        successFlag=false;
    }
    //Do that for each other possible key press
    gKeySurface[KEY_PRESS_UP] = SDL_LoadBMP("SDL_keypress/up.bmp");
    if(gKeySurface[KEY_PRESS_UP]==NULL){
        printf(" Problem loading UP screen! SDL Error: %s\n",SDL_GetError());
        successFlag=false;
    }
    gKeySurface[KEY_PRESS_DOWN] = SDL_LoadBMP("SDL_keypress/down.bmp");
    if(gKeySurface[KEY_PRESS_DOWN]==NULL){
        printf(" Problem loading DOWN screen! SDL Error: %s\n",SDL_GetError());
        successFlag=false;
    }
    gKeySurface[KEY_PRESS_LEFT] = SDL_LoadBMP("SDL_keypress/left.bmp");
    if(gKeySurface[KEY_PRESS_LEFT]==NULL){
        printf(" Problem loading LEFT screen! SDL Error: %s\n",SDL_GetError());
        successFlag=false;
    }
    gKeySurface[KEY_PRESS_RIGHT] = SDL_LoadBMP("SDL_keypress/right.bmp");
    if(gKeySurface[KEY_PRESS_RIGHT]==NULL){
        printf(" Problem loading RIGHT screen! SDL Error: %s\n",SDL_GetError());
        successFlag=false;
    }
    //since we explictly load all of these here we need to be sure to kill them at the end!
    return successFlag;
}

//Close method to kill all resources
void close(){
    //deallocate surfaces in use from loadMedia
    for(SDL_Surface* x : gKeySurface){
        SDL_FreeSurface(x);
        x=NULL;
    }
    //deallocate the globals
    SDL_FreeSurface(gScreenSurface);
    gScreenSurface=NULL;
    //gCurrentScreen doesnt need to be deallocated?
    //Kill the window
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    //And quit SDL
    SDL_Quit();
    
}

int main(int argc, const char * argv[]) {
    //Start by initializing everything
    if(!init()){
        //If the initialization fails
        printf("Could not Initialize\n");
    }
    else{
        //Once initialized we can continue by loading the media
        if(!loadMedia()){
            //If youre here there was a problem loading the bmp images
            printf("Could not load image files\n");
        }
        else{
            //Now we want to enter the main loop
            bool quit = false;
            //need an SDL event to get key presses
            SDL_Event e;
            
            //First set the screen to the default image
            gCurrentScreen=gKeySurface[KEY_PRESS_DEFAULT];
            //to the game loop
            while(!quit){
                
                //need to test the key press to see what happens
                //SDL_PollEvent returns 1 if an event is pending or 0 if there are none
                //Since e is an SDL_event we pass the address since SDL_PollEvent takes values
                //as arguements
                while(SDL_PollEvent(&e)!=0){
                    //if there is an event pending we need to figure out what it is
                    //the first possible event is for the user to quit
                    if( e.type == SDL_QUIT ){
                        quit=true;
                    }
                    //if not a quit we want to check to see if it is a key press
                    else if(e.type == SDL_KEYDOWN){
                        //if it is a key press we need to figure out what kind of key press
                        //We will use a switch function for this
                        switch (e.key.keysym.sym) {
                            //depending on which key is press be sure to set the current screen to the appropriate surface
                            case SDLK_UP:
                                gCurrentScreen=gKeySurface[KEY_PRESS_UP];
                                break;
                            case SDLK_DOWN:
                                gCurrentScreen=gKeySurface[KEY_PRESS_DOWN];
                                break;
                            case SDLK_LEFT:
                                gCurrentScreen=gKeySurface[KEY_PRESS_LEFT];
                                break;
                            case SDLK_RIGHT:
                                gCurrentScreen=gKeySurface[KEY_PRESS_RIGHT];
                                break;
                            default:
                                gCurrentScreen=gKeySurface[KEY_PRESS_DEFAULT];
                                break;
                        }
                    }
                }
                //Once you break out of here you want to apply the current screen to the window
                SDL_BlitSurface(gCurrentScreen, NULL, gScreenSurface, NULL);
                //And update the surface
                SDL_UpdateWindowSurface(gWindow);
            }
        }
    }
    //When down be sure to close everything
    close();
    return 0;
}
