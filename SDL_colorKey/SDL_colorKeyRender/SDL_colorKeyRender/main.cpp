//
//  main.cpp
//  SDL_colorKeyRender

//  Toy program to render a texture on top of another after color keying it

//  Created by William Cole on 10/23/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2_image/SDl_image.h>


//A wrapper class for the textures
class lTexture{
public:
    //public methods go here
    //Constructor
    lTexture();
    //Deconstructor
    ~lTexture();
    //method to get texture from file
    bool loadTexture(std::string path);
    //method to deallocate resources
    void free();
    //mehtod to render texture to screen
    void render(int x, int y);
    //methods to get width and height
    int getWidth(){return texWidth;}
    int getHeight(){return texHeight;}
private:
    //Class variables
    //Texture size
    int texWidth;
    int texHeight;
    //the actual texture
    SDL_Texture* mTexture;
};
//Some global variables
SDL_Window* gWindow=NULL;
SDL_Renderer* gRender=NULL;
//the figure texture using wrapper class
lTexture figTexture;
//the background using wrapper class
lTexture backTexture;

//General variables
int screenW=640;
int screenH=480;

//some method forward decs
bool init();
bool loadMedia();
void close();

//methods for the wrapper class
//The simple constructor
lTexture::lTexture(){
    //initialize
    texWidth=0;
    texHeight=0;
    mTexture=NULL;
}
//Deconstructor will just call free()
lTexture::~lTexture(){
    //Call free()
    free();
}
void lTexture::free(){
    //Look to see if there is something there
    if(mTexture!=NULL){
        //If there is them destroy it
        SDL_DestroyTexture(mTexture);
        mTexture=NULL;
        texHeight=0;
        texWidth=0;
    }
}
//need to load a texture from file
bool lTexture::loadTexture(std::string path){
    //free the texture if one already exists
    free();
    //first try loading the surface from file
    SDL_Surface* loadedSurface=NULL;
    loadedSurface=IMG_Load(path.c_str());
    if(loadedSurface==NULL){
        //if youre here there is a problem loading the surface
        printf("Could not load image at %s! IMG error: %s\n",path.c_str(),IMG_GetError());
    }else{
        //want to color key the surface
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 255, 255));
    }
    SDL_Texture* newTexture=NULL;
    //If we did load the surface convert it to texture
    newTexture=SDL_CreateTextureFromSurface(gRender, loadedSurface);
    if(newTexture==NULL){
        printf("Could not create texture from surface! SDL error: %s\n", SDL_GetError());
    }else{
        //if we do have the texture want to put the relevant info into the lTexture
        texWidth=loadedSurface->w;
        texHeight=loadedSurface->h;
    }
    //once everthing is taken care of we can relaese the surface and put the texture into our mTexture
    SDL_FreeSurface(loadedSurface);
    mTexture=newTexture;
    //the return value is true if mTexture is not null
    return mTexture!=NULL;
}
//Now the render method which will render to a specific position on screen and will not stretch the texture
void lTexture::render(int x, int y){
    //Need a rect to define where we render to
    SDL_Rect renderLocation={x,y,texWidth,texHeight};
    SDL_RenderCopy(gRender, mTexture, NULL, &renderLocation);
}

//Some general methods

//Start up the SDL and SDL_image libraries
bool init(){
    bool successFlag =true;
    //First start up sdl
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not initialize SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        SDL_CreateWindowAndRenderer(screenW, screenH, SDL_WINDOW_SHOWN, &gWindow, &gRender);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }
        if(gRender==NULL){
            printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
            successFlag=false;
        }else{
            SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
        }
        
    }
    //once SDL is loaded we need to load up IMG
    int IMG_start_key= IMG_INIT_PNG;
    if(!(IMG_Init(IMG_start_key)&IMG_INIT_PNG)){
        //if the flag loaded is not the one we wanted
        printf("Could not initialize IMG! IMG error: %s\n",IMG_GetError());
        successFlag=false;
    }
    return successFlag;
}

bool loadMedia(){
    bool successFlag=true;
    //load the two images from file
    
    if(!figTexture.loadTexture("foo.png")){
        printf("Could not load figure texture!\n");
        successFlag=false;
    }
    
    if(!backTexture.loadTexture("Background.png")){
        printf("Could not load background image\n!");
        successFlag=false;
    }
    return successFlag;
}
//method to kill and deallocate resources
void close(){
    //deallocate the textures
    figTexture.free();
    backTexture.free();
    
    //kill the window and renderer
    SDL_DestroyRenderer(gRender);
    SDL_DestroyWindow(gWindow);
    gRender=NULL;
    gWindow=NULL;
    
    //and quit SDL and SDL_image
    IMG_Quit();
    SDL_Quit();
    
}

int main(int argc, const char * argv[]) {
    //Start up everything
    if(!init()){
        printf("Could not start SDL!\n");
    }else{
        if(!loadMedia()){
            printf("Could not load Media!\n");
        }else{
            bool quit=false;
            SDL_Event e;
            int screenX=0;
            while(!quit){
                //check event poll
                while(SDL_PollEvent(&e)!=0){
                    //check to see if the user wants to quit
                    if(e.type==SDL_QUIT){
                        quit=true;
                    }
                }
                //if the user doesnt want to quit we can start rendering
                SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
                SDL_RenderClear(gRender);
                //start by rendering the background
                backTexture.render(0, 0);
                //one that is rendered we want to render the figure ontop
                figTexture.render(screenX%screenW, 190);
                //then update the window
                SDL_RenderPresent(gRender);
                screenX+=10;
                SDL_Delay(100);
                
            }
        }
    }
    
    //once that is going and the textures are loaded we can start doing things
    
    
    close();
    return 0;
}
