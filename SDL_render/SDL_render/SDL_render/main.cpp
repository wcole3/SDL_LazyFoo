//
//  main.cpp
//  SDL_render
//  Toy program to use SDL to load an image with SDL_image as a surface and then render to texture using
//  hardware acceleration
//  Created by William Cole on 10/10/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>




//Declaring globals for the program
int screenW=700;
int screenH=400;

//window for the program
SDL_Window* gWindow = NULL;
//texture to be rendered
SDL_Texture* gTexture =NULL;
//render to be used
SDL_Renderer* gRender=NULL;
//now some method declarations
//startup
bool init();
//close
void close();
//Method to convert a surface to a texture from a file path
SDL_Texture* loadTexture(std::string path);
//Method to load all media files at start
bool loadMedia();

//Start with init
bool init(){
    bool successFlag=true;
    //start up SDL2
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        //Initialization failed
        successFlag=false;
        printf("Could not initialize SDL2! SDL error: %s\n",SDL_GetError());
    }
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1")){
        printf("Warning: linear texture filtering not enabled");
    }
    else{
        //SDL is initialized setup window and renderer
        gWindow=SDL_CreateWindow("Look at that rendered image!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            //the window wasnt created
            successFlag=false;
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
        }
        else{
            //Start up SDL_image
            //SDL_image starts particular variants depending on the flag passed
            int image_Flag=IMG_INIT_PNG;
            if(!(IMG_Init(image_Flag)&image_Flag)){
                //IMG_init returns the flag of the variant initialized
                //if that is different from the one we want there is a problem
                successFlag=false;
                printf("Could not initialize SDL_image! SDL_image error: %s\n",IMG_GetError());
            }
            else{
                //now that IMG is loaded we can make our renderer
                gRender=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
                if(gRender==NULL){
                    //the renderer could not be setup
                    successFlag=false;
                    printf("Could not initialize renderer! SDL error: %s\n",SDL_GetError());
                }
                else{
                    //if the renderer is setup we need to setup the default draw color
                    SDL_SetRenderDrawColor(gRender, 255, 255, 0, 255);
                    SDL_SetRenderDrawBlendMode(gRender, SDL_BLENDMODE_BLEND);
                }
            }
        }
    }
    
    return successFlag;
}

//Now the method to load an img from file and convert it to a texture
SDL_Texture* loadTexture(std::string path){
    SDL_Texture* gLoadedTexture=NULL;
    //first we need a surface to load into
    SDL_Surface* gLoadedSurface=IMG_Load(path.c_str());
    if(gLoadedSurface==NULL){
        //the image wasnt loaded from file
        printf("Could not load image at %s! IMG error: %s\n", path.c_str(), IMG_GetError());
    }else{
        gLoadedTexture=SDL_CreateTextureFromSurface(gRender, gLoadedSurface);
        if(gLoadedTexture==NULL){
            //could not convert to texture
            printf("Could not convert surface to texture! SDl error: %s\n",SDL_GetError());
        }
    }
    //once we have the texture we can free the loaded surface
    SDL_FreeSurface(gLoadedSurface);
    return gLoadedTexture;
}

//Method to load the media at start
bool loadMedia(){
    bool successFlag=true;
    //hard coded image loaction
    gTexture=loadTexture("texture.png");
    if(gTexture==NULL){
        //the texture was not loaded from file
        successFlag=false;
        printf("Could not load media!");
    }
    
    return successFlag;
}

//method to close everything down
void close(){
    //Free the texture
    SDL_DestroyTexture(gTexture);
    gTexture=NULL;
    //Kill window
   
    //kill renderer
    SDL_DestroyRenderer(gRender);
    gRender=NULL;
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    //exit SDL2 and SDL_image
    SDL_Quit();
    IMG_Quit();
}

int main(int argc, const char * argv[]) {
    //Lets make a window and render an image to it
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }
        
        else{
            
            //now that we are going its time to start the game loop
            bool quit=false;
            SDL_Event e;
            
            while(!quit){
                
                while(SDL_PollEvent(&e)!=0){
                    //test the event type
                    if(e.type == SDL_QUIT){
                        //is the user wants to quit make it so
                        quit=true;
                        
                    }
                    
                }
                //until then we want to render that image to the screen
                //first clear the screen
                SDL_RenderClear(gRender);
                //then render the texture
                SDL_RenderCopy(gRender, gTexture, NULL, NULL);
                //And finally update the screen
                SDL_RenderPresent(gRender);
                
            }
        }
    }
    close();
    return 0;
}
