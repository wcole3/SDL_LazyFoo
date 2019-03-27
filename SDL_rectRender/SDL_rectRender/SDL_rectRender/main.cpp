//
//  main.cpp
//  SDL_rectRender
//  Playing around with rendering the default shapes in the SDL2 lib
//  Created by William Cole on 10/18/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <stdio.h>

//Some globals
int screenH=400;
int screenW=700;

//The real players
SDL_Window* gWindow=NULL;
SDL_Renderer* gRenderer= NULL;
//Method forward decs
bool init();
void close();
void drawLoadingBox(int x);

//Start up the program with SDL init
bool init(){
    bool successFlag=true;
    //initialize the SDL lib with the event system
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        //If youre here the SDL lib did not initialize properly
        printf("Could not initialize SDL! SDL error %s\n",SDL_GetError());
        successFlag=false;
    }else{
        //Setup the window and render
        if(SDL_CreateWindowAndRenderer(screenW, screenH, SDL_WINDOW_SHOWN, &gWindow, &gRenderer)<0){
            //if youre here there was a problem creating the window and renderer
            printf("Could not create window and renderer! SDL error %s\n",SDL_GetError());
            successFlag=false;
        }else{
            //If the wondow and render were created we want to set the default draw color of white
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        }
    }
    
    return successFlag;
}

//need method to close everything down
void close(){
    //destroy renderer and window
    SDL_DestroyWindow(gWindow);
    gWindow=NULL;
    SDL_DestroyRenderer(gRenderer);
    gRenderer=NULL;
    //close down SDL2
    SDL_Quit();
}

//Now the method that will draw a loading box icon
void drawLoadingBox(int x){
    //the input x is an index telling which location to draw the box
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    //x will be zero or 1
    SDL_Rect loadBox={screenW/2,screenH/2-(50*(x)),25,25};
    SDL_RenderFillRect(gRenderer, &loadBox);
    
}

int main(int argc, const char * argv[]) {
    //Want to setup the window and then draw some rectangles
    if(!init()){
        //Could not initialize
        printf("Could not initialize!");
    }else{
        //Want to setup the loop
        bool quit=false;
        SDL_Event e;
        int x=1;
        while(!quit){
            //get the event queue
            while(SDL_PollEvent(&e)!=0){
                //Really we just want to check to see if the user wants to quit
                if(e.type==SDL_QUIT){
                    quit=true;
                }else{
                    break;
                }
                
                
            }
            //first clear the renderer, make sure this is white
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
            SDL_RenderClear(gRenderer);
            //Then we want to draw things
            SDL_Rect rectBK={screenW/4,screenH/4,screenW/2,screenH/2};
            //change the render color to red
            SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(gRenderer, &rectBK);
            SDL_Rect rectOut={screenW/6,screenH/6,screenW*2/3,screenH*2/3};
            SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(gRenderer, &rectOut);
            //lets also draw a line
            SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(gRenderer, 0, screenH/2, screenW, screenH/2);
            //draw some points
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
            for(int i=0;i<screenH;i+=5){
                SDL_RenderDrawPoint(gRenderer, screenW/2, i);
            }
            //Now I want to draw a classic rotating loading icon
            drawLoadingBox(x);
            //This works in terms of drawing boxes but causes the program to not be able to close
            SDL_RenderPresent(gRenderer);
            
            
            x=(x+1)%2;
        }
    }
    close();
    return 0;
}
