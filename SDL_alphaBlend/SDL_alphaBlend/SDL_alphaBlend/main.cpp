//
//  main.cpp
//  SDL_alphaBlend
//  Toy program to use key presses to modulate that alpha of an image
//  Created by William Cole on 11/8/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <cstdio>

//Start with some screen parameters
int screenW=640;
int screenH=480;

//now a wrapper class for textures
class lTexture{
public:
    lTexture();
    
    ~lTexture();
    
    bool loadFromFile(std::string path);
    
    void free();
    
    void render(int x, int y);
    
    void setAlpha(Uint8 alpha);
    
    void setBlendMode(SDL_BlendMode mode);
    
    int getWidth(){return textW;};
    int getHeight(){return textH;};
private:
    int textW;
    int textH;
    SDL_Texture* mTexture;
};

//Some global variables
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
lTexture bckTexture;
lTexture modTexture;

//forward declarations
bool init();
bool loadMedia();
void close();

//lTexture method definitions
//constructor
lTexture::lTexture(){
    textH=0;
    textW=0;
    mTexture=NULL;
}
//deconstructor
lTexture::~lTexture(){
    free();
}
//deallocation method
void lTexture::free(){
    //do this only if something exists
    if(mTexture!=NULL){
        SDL_DestroyTexture(mTexture);
        mTexture=NULL;
        textW=0;
        textH=0;
    }
}
//Method to get the texture from file path
bool lTexture::loadFromFile(std::string path){
    //first free texture if something is there
    free();
    //holder surface and texture
    SDL_Surface* loadedSurface=NULL;
    SDL_Texture* loadedTexture=NULL;
    if((loadedSurface=IMG_Load(path.c_str()))==NULL){
        printf("Could not load file at path: %s! IMG error: %s\n",path.c_str(),IMG_GetError());
    }else{
        if((loadedTexture=SDL_CreateTextureFromSurface(gRenderer, loadedSurface))==NULL){
            printf("Could not create texture from surface! SDL error: %s\n",SDL_GetError());
        }else{
            //if we get here we can go ahead and put the color keying in and put it in mTexture
            
            mTexture=loadedTexture;
            textH=loadedSurface->h;
            textW=loadedSurface->w;
        }
    }
    SDL_FreeSurface(loadedSurface);
    //return true if mTexture isnt false
    return mTexture!=NULL;
}
//method to render image at certain screen location
void lTexture::render(int x, int y){
    //need a rect to define screen position
    SDL_Rect locRect={x,y,getWidth(),getHeight()};
    SDL_RenderCopy(gRenderer, mTexture, NULL, &locRect);
}
//method to set blending mode of texture
void lTexture::setBlendMode(SDL_BlendMode mode){
    SDL_SetTextureBlendMode(mTexture, mode);
}
//method to set alpha of the texture
void lTexture::setAlpha(Uint8 alpha){
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

//Now define global methods
bool init(){
    bool successFlag=true;
    //start up SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error %s\n",SDL_GetError());
        successFlag=false;
    }else{
        //create window and renderer
        //You should test them seperately but im banking on this working
        if(SDL_CreateWindowAndRenderer(screenW, screenH, SDL_WINDOW_SHOWN, &gWindow, &gRenderer)<0){
            printf("Could not create window/renderer! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
        }
    }
    //start up IMG
    int IMG_start=IMG_INIT_PNG;
    if(!(IMG_Init(IMG_start)&IMG_start)){
        //if the request start flag is not used there is a problem
        printf("Could not start IMG! IMG error %s\n", IMG_GetError());
        successFlag=false;
    }
    
    return successFlag;
}
//method to shutdown
void close(){
    bckTexture.free();
    modTexture.free();
    
    SDL_DestroyRenderer(gRenderer);
    gRenderer=NULL;
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    
    SDL_Quit();
    IMG_Quit();
}
//method to load media from file
bool loadMedia(){
    bool successFlag=true;
    //load bck texture
    if(!bckTexture.loadFromFile("fadein.png")){
        printf("Could not load background texture!");
        successFlag=false;
    }else{
        //set up alpha
        bckTexture.setAlpha(255);
    }
    //now do the same for the modulated texture
    if(!modTexture.loadFromFile("fadeout.png")){
        printf("Could not load the modulated texture!");
        successFlag=false;
    }else{
        modTexture.setAlpha(0);
        modTexture.setBlendMode(SDL_BLENDMODE_BLEND);
    }
    return successFlag;
}

int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load Media!");
        }else{
            bool quit=false;
            SDL_Event e;
            
            int alpha=0;
            //start game loop
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    //see if user wants to quit
                    if(e.type==SDL_QUIT){
                        quit=true;
                    }
                    //else test for key presses on w and s
                    if(e.type==SDL_KEYDOWN){
                        switch (e.key.keysym.sym) {
                            case SDLK_w:
                                //Turn the alpha up
                                alpha+=32;
                                //test alpha
                                if(alpha>255){
                                    alpha=255;
                                }
                                modTexture.setAlpha((Uint8)alpha);
                                break;
                            case SDLK_s:
                                //turn the alpha down
                                alpha-=32;
                                //test alpha
                                if(alpha<0){
                                    alpha=0;
                                }
                                modTexture.setAlpha((Uint8)alpha);
                            default:
                                break;
                        }
                    }
                }
                //here we render things
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                //first the background
                bckTexture.render(0, 0);
                //and the modulated texture on top
                modTexture.render(0, 0);
                
                SDL_RenderPresent(gRenderer);
            }
            
        }
    }
    
    close();
    return 0;
}
