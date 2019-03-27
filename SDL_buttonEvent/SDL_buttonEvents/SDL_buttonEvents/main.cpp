//
//  main.cpp
//  SDL_buttonEvents
//  Toy program to test adn use mouse evetns on the creen
//  Created by William Cole on 11/19/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include "lTexture.h"

//some globals
int screenH=480;
int screenW=640;

//globals from SDL
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer=NULL;
//need a texture which defines the image
lTexture mouseImage;

//some genral methods
bool init();
bool loadMedia();
void close();

//some constatns for the button
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

//create an enumeration to alias each sprite
enum buttonSprite{
    MOUSE_OUT=0,
    MOUSE_OVER=1,
    MOUSE_DOWN=2,
    MOUSE_UP=3,
    TOTAL_SPRITE_CLIPS=4
};
//Rect array that will define the clips
SDL_Rect buttonSpriteClips[TOTAL_SPRITE_CLIPS];//note that total_sprite_clips is how many sprites there are
//total buttons is how many buttons there are, they are not the same

//gonna make a wrapper for buttons
class lButton{
public:
    lButton();
    //set the button's top left position
    void setPosition(int x, int y);
    
    void handleEvent(SDL_Event* e);
    //render using the lTexture method
    void render();
private:
    //point defining where the button is
    SDL_Point buttPos;
    
    buttonSprite currentSprite;
};
//now want an array of buttons to represent each button
lButton gButtons[TOTAL_BUTTONS];


//define the button class methods
lButton::lButton(){
    buttPos.x=0;
    buttPos.y=0;
    currentSprite=MOUSE_OUT;
}

//set the position of the button's top left corner
void lButton::setPosition(int x, int y){
    buttPos.x=x;
    buttPos.y=y;
}
//this is the method we will call to check an event to see what to change the current sprite to
void lButton::handleEvent(SDL_Event* e){
    if(e->type == SDL_MOUSEMOTION||e->type==SDL_MOUSEBUTTONDOWN||e->type==SDL_MOUSEBUTTONUP){
        //need to check to see if the mouse is inside the button
        bool inside=true;
        //check the mouse position
        int x,y;
        SDL_GetMouseState(&x, &y);
        //now check the x and y position
        if(x<buttPos.x){
            //mouse is to the left of the button
            inside=false;
        }else if(x>(buttPos.x+BUTTON_WIDTH)){
            //mouse is to the right of the button
            inside=false;
        }else if(y<buttPos.y){
            //the mouse is above the button
            inside=false;
        }else if(y>(buttPos.y+BUTTON_HEIGHT)){
            //mouse is below the button
            inside=false;
        }
        if(!inside){
            currentSprite=MOUSE_OUT;
        }else{
            //now we know that the mouse is inside the button, need to check if the mouse is clicked or not
            switch (e->type) {
                    //change the sprite to the appropraite case
                case SDL_MOUSEMOTION:
                    currentSprite=MOUSE_OVER;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    currentSprite=MOUSE_DOWN;
                    break;
                case SDL_MOUSEBUTTONUP:
                    currentSprite=MOUSE_UP;
                    break;
            }
        }
        
    }
}
//the render method will use the rendering function of the texture wrapper
void lButton::render(){
    mouseImage.render(buttPos.x, buttPos.y, &buttonSpriteClips[currentSprite]);
}

//now we can define our global methods
bool init(){
    bool successFlag=true;
    //start up SDL
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag=false;
    }else{
        //setup window and renderer
        gWindow=SDL_CreateWindow("Push my buttons!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenW, screenH, SDL_WINDOW_SHOWN);
        if(gWindow==NULL){
            printf("Could not create window! SDL error: %s\n",SDL_GetError());
            successFlag=false;
        }else{
            gRenderer=SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
            if(gRenderer==NULL){
                printf("Could not create renderer! SDL error: %s\n", SDL_GetError());
                successFlag=false;
            }else{
                //everything seems to be setup so we can set draw color and construct our texture
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                mouseImage=lTexture(gRenderer);
            }
        }
    }
    //now need to set up IMG
    int IMG_start=IMG_INIT_PNG;
    if(!(IMG_Init(IMG_start) & IMG_start)){
        printf("Could not start SDL image! IMG error: %s\n", IMG_GetError());
        successFlag=false;
    }
    
    //set up the gloabl buttons
    //topleft
    gButtons[0].setPosition(0, 0);
    //top right
    gButtons[1].setPosition(BUTTON_WIDTH, 0);
    //bottom left
    gButtons[2].setPosition(0, BUTTON_HEIGHT);
    //bottom right
    gButtons[3].setPosition(BUTTON_WIDTH, BUTTON_HEIGHT);
    
    return successFlag;
}

void close(){
    mouseImage.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer=NULL;
    gWindow=NULL;
    SDL_Quit();
    IMG_Quit();
}

//need to load the media and also set up the buttonSpriteClips array
bool loadMedia(){
    bool successFlag=true;
    //load the button images
    if(!mouseImage.loadFromFile("button.png")){
        printf("Could not load button image!");
        successFlag=false;
    }else{
        //set up buttonSpriteClips, it is a certical column of images 300x200
        buttonSpriteClips[MOUSE_OUT]={0, 0, BUTTON_WIDTH, BUTTON_HEIGHT};
        buttonSpriteClips[MOUSE_OVER]={0, BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT};
        buttonSpriteClips[MOUSE_DOWN]={0, 2*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT};
        buttonSpriteClips[MOUSE_UP]={0, 3*BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT};
    }
    return successFlag;
}

//now we can do things
int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia()){
            printf("Could not load media!");
        }else{
            //now here is the game loop
            bool quit=false;
            SDL_Event e;
            
            while(!quit){
                while(SDL_PollEvent(&e)!=0){
                    //check if the user wants to quit
                    if(e.type==SDL_QUIT){
                        quit=true;
                    }
                    for(int i=0; i<TOTAL_BUTTONS;i++){
                        //for each button handle events
                        gButtons[i].handleEvent(&e);
                    }
                }
                //after the events have been checked we can render
                SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
                SDL_RenderClear(gRenderer);
                //render each button
                for(int i=0; i<TOTAL_BUTTONS;i++){
                    gButtons[i].render();
                }
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
