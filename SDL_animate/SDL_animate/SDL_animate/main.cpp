//
//  main.cpp
//  SDL_animate
//  Toy program to animate a sprite walking on screen,possibly with keypress input
//  Created by William Cole on 11/9/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

int screenW=640;
int screenH=480;

class lTexture{
public:
    lTexture();
    ~lTexture();
    void free();
    bool loadFromFile(std::string path);
    void render(int x, int y, SDL_Rect* spriteClip);
    int getWidth(){return textW;};
    int getHeight(){return textH;};
private:
    int textW;
    int textH;
    SDL_Texture* mTexture;
};

//globals
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
lTexture spriteSheet;
//need a RECT collection to define which sprite frame
const int totalSpriteFrames=4;
SDL_Rect spriteFrame[totalSpriteFrames];

//forward decs
bool init();
bool loadMedia();
void close();

//wrapper class methods
lTexture::lTexture(){
    textH=0;
    textW=0;
    mTexture=NULL;
}
lTexture::~lTexture(){
    free();
}
void lTexture::free(){
    //only if there is a texture
    if(mTexture!=NULL){
        SDL_DestroyTexture(mTexture);
        mTexture=NULL;
        textH=0;
        textW=0;
    }
}
bool lTexture::loadFromFile(std::string path){
    free();
    SDL_Surface* loadedSurface=NULL;
    
    loadedSurface=IMG_Load(path.c_str());
    if(loadedSurface==NULL){
        printf("Could not load file at path: %s! IMG error: %s\n", path.c_str(), IMG_GetError());
    }else{
        //the image has a cyan background
        
        if(SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 255, 255))>0){
            printf("Error color keying! SDL error: %s\n",SDL_GetError());
        }
        
    }
    SDL_Texture* loadedTexture=NULL;
    loadedTexture=SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if(loadedTexture==NULL){
        printf("Could not create texture from surface! SDL error: %s\n",SDL_GetError());
    }else{
        
        textW=loadedSurface->w;
        textH=loadedSurface->h;
    }
    SDL_FreeSurface(loadedSurface);
    mTexture=loadedTexture;
    return mTexture!=NULL;
}
void lTexture::render(int x, int y, SDL_Rect* frame){
    //the frame arguement defines what part of the texture to render but we still need a screen position rect
    SDL_Rect locRect={x,y,getWidth(),getHeight()};
    //need to change the render location size if the frame is a different size to the full texture
    if(frame!=NULL){
        locRect.w=frame->w;
        locRect.h=frame->h;
    }
    
    SDL_RenderCopy(gRenderer, mTexture, frame, &locRect);
}
//global method dec
void close(){
    spriteSheet.free();
    SDL_DestroyRenderer(gRenderer);
    gRenderer=NULL;
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    
    SDL_Quit();
    IMG_Quit();
}

bool init(){
    bool successFlag=true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n",SDL_GetError());
        successFlag=false;
    }else{
        gWindow=SDL_CreateWindow("Look at that walking foo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create SDL window! SDL error: %s\n",SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n",SDL_GetError());
                successFlag=false;
            }else{
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
            }
        }
    }
    int IMG_start=IMG_INIT_PNG;
    if(!(IMG_Init(IMG_start) & IMG_start)){
        printf("Could not start IMG! IMG error: %s\n",IMG_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag=true;
    if(!spriteSheet.loadFromFile("foo.png")){
        printf("Could not load foo image!");
        successFlag=false;
    }else{
        //need to set up the sprite frame collection
        //each sprite on the sheet is 64 pixels wide
        //first sprite
        spriteFrame[0]={0,0,spriteSheet.getWidth()/4,spriteSheet.getHeight()};
        //second sprite
        spriteFrame[1]={64,0,spriteSheet.getWidth()/4,spriteSheet.getHeight()};
        //thrid and fourth
        spriteFrame[2]={128,0,spriteSheet.getWidth()/4,spriteSheet.getHeight()};
        spriteFrame[3]={192,0,spriteSheet.getWidth()/4,spriteSheet.getHeight()};
    }
    
    return successFlag;
}

int main(int argc, const char * argv[]) {
    //lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }else{
            //okay here we go
            bool quit=false;
            SDL_Event e;
            int frame=0;
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    if(e.type==SDL_QUIT){
                        quit=true;
                        //the user wants to quit
                    }
                }
                //now render things
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                
                //now render in middle of screen
                SDL_Rect* currentSprite=&spriteFrame[frame/400];
                spriteSheet.render((screenW-currentSprite->w)/2, (screenH-currentSprite->h)/2, currentSprite);
                SDL_RenderPresent(gRenderer);
                //now need to increment the frame value
                frame++;
                //test to make sure the value isnt out of range
                if((frame/400)>=totalSpriteFrames){
                    //reset to zero
                    frame=0;
                }
                
            }
            
        }
    }
    
    close();
    return 0;
}
