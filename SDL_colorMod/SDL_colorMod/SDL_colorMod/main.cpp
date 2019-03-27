//
//  main.cpp
//  SDL_colorMod
//  Toy program to use key presses to change the color modulation of an image
//  Created by William Cole on 11/6/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <SDL2/SDl.h>
#include <SDL2_image/SDL_image.h>

//general globals
int screenW=640;
int screenH=480;

//wrapper class for textures with functionality of color mod
class lTexture{
public:
    
    lTexture();
    
    ~lTexture();
    
    bool loadFromFile(std::string path);
    
    void changeColor(Uint8 red, Uint8 green, Uint8 blue);
    
    void free();
    
    void render(int x, int y);
    
    int getWidth(){return textW;};
    int getHeight(){return textH;};
    
private:
    //class varibles
    int textW;
    int textH;
    SDL_Texture* mTexture;
};

//Some globals for the actual game loop
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
lTexture colors;

bool init();
bool loadMedia();
void close();

//forward declarations for the wrapper class
//general constructor
lTexture::lTexture(){
    textW=0;
    textH=0;
    mTexture=NULL;
}
//Deconstructor to deallocate resources
lTexture::~lTexture(){
    free();
}
//Free method to deallocate things
void lTexture::free(){
    //check to see if there is a texture
    if(mTexture!=NULL){
        //if so destroy it
        SDL_DestroyTexture(mTexture);
        mTexture=NULL;
        textW=0;
        textH=0;
    }
}
//method to load file from given path
bool lTexture::loadFromFile(std::string path){
    //first free the textures
    free();
    //then start by creating surface and holding texture
    SDL_Surface* loadedSurface=NULL;
    SDL_Texture* loadedTexture=NULL;
    loadedSurface=IMG_Load(path.c_str());
    if(loadedSurface==NULL){
        printf("Could not load file at %s! IMG error: %s\n",path.c_str(),IMG_GetError());
    }else{
        //we could do color keying here if needed
        loadedTexture=SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(loadedTexture==NULL){
            printf("Could not create texture from surface! SDL error: %s\n",SDL_GetError());
        }else{
            //if the texture is loaded then put it in mTexture along with the width and heigth
            mTexture=loadedTexture;
            textW=loadedSurface->w;
            textH=loadedSurface->h;
        }
    }
    //once done free the surface
    SDL_FreeSurface(loadedSurface);
    //return true if mTexture is not null
    return mTexture!=NULL;
}
//method to change the color of the texture
void lTexture::changeColor(Uint8 red, Uint8 green, Uint8 blue){
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

//method to render the texture at a screen position
void lTexture::render(int x, int y){
    //Need a rect to set rendering location
    SDL_Rect locRect={x,y,getWidth(),getHeight()};
    SDL_RenderCopy(gRenderer, mTexture, NULL, &locRect);
}

//Now some general methods
//startup method
bool init(){
    bool successFlag=true;
    //Start up SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n)", SDL_GetError());
        successFlag=false;
    }else{
        //create window and renderer
        SDL_CreateWindowAndRenderer(screenW, screenH, SDL_WINDOW_SHOWN, &gWindow, &gRenderer);
        //test if the window and render were made
        if(gWindow==NULL){
            printf("could not create window! SDL error: %s\n",SDL_GetError());
            successFlag=false;
        }
        else if(gRenderer==NULL){
            printf("Could not create renderer! SDL error: %s\n",SDL_GetError());
            successFlag=false;
        }else{
            //if youre here we are in business just need to set default render color
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
        }
    }
    //Now startup IMG
    int IMG_Start=IMG_INIT_PNG;
    //if the requested startup flag isnt the one that is actually started then we have a problem
    if(!(IMG_Init(IMG_Start)&IMG_Start)){
        printf("Could not start SDL_image! IMG error: %s\n",IMG_GetError());
        successFlag=false;
    }
    
    return successFlag;
}
//method to load in the media
bool loadMedia(){
    bool successFlag=true;
    //load in the img from file
    if(!colors.loadFromFile("colors.png")){
        printf("Could not load media!");
        successFlag=false;
    }
    return successFlag;
}

//method to deallocate and shutdown
void close(){
    //deallocate
    colors.free();
    SDL_DestroyRenderer(gRenderer);
    gRenderer=NULL;
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    //shutdown
    SDL_Quit();
    IMG_Quit();
}

int main(int argc, const char * argv[]) {
    //lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }else{
            //okay now we start things
            bool quit=false;
            SDL_Event e;
            //need the Uint8 variable to set color mod values
            //start then all at 255 so there is no modulation
            Uint8 red=255;
            Uint8 green=255;
            Uint8 blue=255;
            
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    //first look to see if the user wants to quit
                    if(e.type==SDL_QUIT){
                        quit=true;
                    }
                    //other wise check for key presses
                    if(e.type==SDL_KEYDOWN){
                        switch (e.key.keysym.sym) {
                            //test for q,w,e,a,s,d
                            case SDLK_q:
                                red+=32;
                                break;
                            case SDLK_w:
                                green+=32;
                                break;
                            case SDLK_e:
                                blue+=32;
                                break;
                            case SDLK_a:
                                red-=32;
                                break;
                            case SDLK_s:
                                green-=32;
                                break;
                            case SDLK_d:
                                blue-=32;
                                break;
                            default:
                                break;
                           
                        }
                    }
                }
                //regardless we want to render the image with the updated color each time
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                //first set the color
                colors.changeColor(red, green, blue);
                colors.render(0, 0);
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
