//
//  main.cpp
//  SDL_tiling
//  Toy program to generate a map from tiles and then a dot to move around the level
//  Created by William Cole on 1/12/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <string>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include "lUtilityFunc.h"
#include "lRigidDot.h"
#include "lTimer.h"
#include "lTexture.h"
#include "lWindow.h"
#include "lTile.h"

//some screen and level constants
const int CAMERA_WIDTH = 640;
const int CAMERA_HEIGHT = 480;
extern const int LEVEL_WIDTH;
extern const int LEVEL_HEIGHT;

//sdl constants
lWindow gWindow;
lRigidDot player;
//need a holder for all of the tiles
lTile* tiles[TOTAL_TILES];
//the sprite textures
lTexture gTileSpriteTexture;
//camera object
SDL_Rect camera;

//forward declarations
bool init();
//the load media function is a little different since we need to load in each tile object
bool loadMedia(lTile* tiles[]);
//same for the close function
void close(lTile* tiles[]);

bool init(){
    bool successFlag = true;
    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0){
        printf("Could not start SDL! SDL error: %s\n", SDL_GetError());
        successFlag = false;
    }else{
        if(!gWindow.init(CAMERA_WIDTH, CAMERA_HEIGHT)){
            printf("Could not create window!");
            successFlag = false;
        }else{
            //set up player
            player = lRigidDot(LEVEL_WIDTH, LEVEL_HEIGHT, gWindow.getRenderer());
            gTileSpriteTexture = lTexture(gWindow.getRenderer());
        }
    }
    //set up IMG
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        printf("Could not start IMG! %s\n", IMG_GetError());
        successFlag = false;
    }
    return successFlag;
}

bool loadMedia(lTile* tiles[]){
    bool successFlag = true;
    //load the sprite texture
    if(!gTileSpriteTexture.loadFromFile("tiles.png", SDL_FALSE)){
        printf("Could not load sprite texture!");
        successFlag = false;
    }
    //load player texture
    SDL_Color cyanKey = {0,255,255};
    if(!player.loadFromFile("dot.bmp", SDL_TRUE, cyanKey)){
        printf("Could not load player texture!");
        successFlag = false;
    }
    //now set the tiles
    if(!setTiles(tiles)){
        printf("Could not set tiles!");
        successFlag = false;
    }
    return successFlag;
}

void close(lTile* tiles[]){
    //need to delete the tile objects
    for(int i = 0; i < TOTAL_TILES; i++){
        delete tiles[i];
    }
    //free other resources
    gTileSpriteTexture.free();
    player.free();
    gWindow.free();
    SDL_Quit();
    IMG_Quit();
}



int main(int argc, const char * argv[]) {
    // lets get down to business
    if(!init()){
        printf("Could not initialize!");
    }else{
        if(!loadMedia(tiles)){
            printf("Could not load media!");
        }else{
            //start game loop
            bool quit = false;
            SDL_Event e;
            //setup the camera
            camera = {0,0,CAMERA_WIDTH,CAMERA_HEIGHT};
            //setup timer
            lTimer ticker = lTimer();
            while(!quit){
                while(SDL_PollEvent(&e)){
                    if(e.type == SDL_QUIT){
                        quit = true;
                    }
                    //handle player events
                    player.handleEvent(e);
                }
                //now move player, set time, and render
                float timeStep = (ticker.getTime() / 1000.f);
                player.move(timeStep, tiles);
                ticker.start();
                //then set the camera
                player.setCamera(camera);
                //now we can render
                gWindow.render();
                
                //render each tile
                for(int i = 0; i < TOTAL_TILES; i++){
                    tiles[i]->render(camera);
                }
                //render player on top of surface
                player.render(camera);
                //after render everything, show it on screen
                SDL_RenderPresent(gWindow.getRenderer());
            }
        }
    }
    
    close(tiles);
    return 0;
}
