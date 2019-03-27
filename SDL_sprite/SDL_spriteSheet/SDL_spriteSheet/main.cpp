//
//  main.cpp
//  SDL_spriteSheet
//  Toy program to load different images from the same sheet
//  Created by William Cole on 11/5/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

//Some global variables
int screenW=640;
int screenH=480;
//wrapper class for SDL_textures
class lTexture{
public:
    lTexture();
    
    ~lTexture();
    
    bool loadTexture(std::string path);
    
    void render( int x, int y, SDL_Rect* clippedSprtite=NULL);
    
    void free();
    
    int getWidth(){return textW;};
    int getHeight(){return textH;};
private:
    //class variables
    int textW;
    int textH;
    SDL_Texture* mTexture;
};

SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
lTexture spriteSheet;
//finally an array defining where the clipped sprites are
SDL_Rect spriteClips[4];

//Method forward declarations
bool init();
bool loadMedia();
void close();



//Fill out the methods
//wrapper constructor
lTexture::lTexture(){
    textH=0;
    textW=0;
    mTexture=NULL;
}
//the deconstructor
lTexture::~lTexture(){
    //handle deallocation in free()
    free();
}
//method to load a texture from file
bool lTexture::loadTexture(std::string path){
    //start by freeing mTexture to make sure its availible
    free();
    SDL_Surface* loadedSurface=IMG_Load(path.c_str());
    SDL_Texture* newTexture=NULL;
    if(loadedSurface==NULL){
        printf("Could not load surface from %s! IMG error: %s\n",path.c_str(), IMG_GetError());
        
    }
    else{
        //need to set cyan as color key
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 255, 255));
    }
    //if the surface was loaded then we want to put it into newTexture
    newTexture=SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if(newTexture==NULL){
        printf("Could not create Texture form Surface! SDL error: %s\n", SDL_GetError());
    }
    //if the texture was properly created then we can put it into mTexture
    mTexture=newTexture;
    //dont forget to tell the lTexture the size of the texture!
    textH=loadedSurface->h;
    textW=loadedSurface->w;
    //then free the surface
    SDL_FreeSurface(loadedSurface);
    //The return statement is essentially return true if mTexture is not NULL
    return mTexture!=NULL;
}
//method to render texture to specific screen position
void lTexture::render(int x, int y, SDL_Rect* clippedSprite){
    //Need to create a Rect to tell the renderer where to render
    SDL_Rect locRect={x,y,textW,textH};
    //if the clipping rect is not null we need to change the above values to reflect
    //which portion of the sprite sheet to render
    if(clippedSprite!=NULL){
        //the size of the texture will be changed from the entire sprite sheet to the size of the
        //actual sprite which is defined by the clipped Sprite rect
        locRect.w=clippedSprite->w;
        locRect.h=clippedSprite->h;
    }
    
    //then use that as the render target, the clippedSprite rect tells us which sprite to use
    SDL_RenderCopy(gRenderer, mTexture, clippedSprite, &locRect);
}
//method to destroy class texture
void lTexture::free(){
    //first look if there is a mTexture
    if(mTexture!=NULL){
        SDL_DestroyTexture(mTexture);
        mTexture=NULL;
        textH=0;
        textW=0;
    }
}

//now the method to initialize everything
bool inti(){
    bool successFlag=true;
    //start up SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n",SDL_GetError());
        successFlag=false;
    }
    else{
        //if that all starts we can create a window and renderer
        SDL_CreateWindowAndRenderer(screenW, screenH, SDL_WINDOW_SHOWN, &gWindow, &gRenderer);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }
        if(gRenderer==NULL){
            printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            //once those are set up we want to set the default draw color
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
        }
    }
    //that finishes the SDL startup now do the same for IMG
    int IMG_Startup_Flag=IMG_INIT_PNG;
    if(!(IMG_Init(IMG_Startup_Flag)&IMG_Startup_Flag)){
        printf("Could not start SDL image! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    
    return successFlag;
}

//method to shutdown and destroy allocated variables
void close(){
    spriteSheet.free();
    SDL_DestroyRenderer(gRenderer);
    gRenderer=NULL;
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    //shutdown
    SDL_Quit();
    IMG_Quit();
}
//method to load sprite sheet and setup the clipped sprite rectangle
bool loadMedia(){
    bool successFlag=true;
    if(!spriteSheet.loadTexture("dots.png")){
        printf("Could not load sprite sheet");
        successFlag=false;
    }else{
        //if that goes okay we need to setup the clippedSprite rect given that each sprite is a 100 pixel sq
        //topleft
        spriteClips[0]={0,0,100,100};
        //topright
        spriteClips[1]={spriteSheet.getWidth()-100,0,100,100};
        //botleft
        spriteClips[2]={0,spriteSheet.getHeight()-100,100,100};
        //botright
        spriteClips[3]={spriteSheet.getWidth()-100,spriteSheet.getHeight()-100,100,100};
    }
    
    return successFlag;
}

int main(int argc, const char * argv[]) {
    //lets get down to business
    if(!inti()){
        printf("Failed to initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load Media!");
        }else{
            //now do things
            bool quit=false;
            SDL_Event e;
            int i=4;
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    if(e.type==SDL_QUIT){
                        //user wants to quit
                        quit=true;
                    }
                }
                //if the user doesnt want to quit then we can start rendering things
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                //then render the four sprites in the 4 corners
                //top left
                spriteSheet.render(0, 0, &spriteClips[i%4]);
                //top right
                spriteSheet.render(screenW-spriteClips[(i-1)%4].w, 0, &spriteClips[(i-1)%4]);
                //bot left
                spriteSheet.render(0, screenH-spriteClips[(i-2)%4].h, &spriteClips[(i-2)%4]);
                //bot right
                spriteSheet.render(screenW-spriteClips[(i-3)%4].w, screenH-spriteClips[(i-3)%4].h, &spriteClips[(i-3)%4]);
                //then render
                SDL_RenderPresent(gRenderer);
                i++;
                SDL_Delay(1000);
            }
        }
    }
    close();
    return 0;
}
