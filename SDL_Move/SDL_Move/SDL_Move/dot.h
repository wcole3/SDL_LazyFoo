//
//  dot.h
//  SDL_Move
//  texture wrapper that inherts from lTexture
//  This texture can move around the screen
//  Created by William Cole on 12/4/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#ifndef dot_h
#define dot_h
#include "lTexture.h"
//want to extend lTexture with a dot that can move
class dot : public lTexture{
public:
    //a constant definition of velocity
    static const int DOT_VEL=640;
    
    dot(SDL_Renderer* renderer=NULL);
    void handleEvent(SDL_Event& e);
    void move(float time);
    int getXPos(){return xPos;};
    int getYPos(){return yPos;};
    void setScreenSize(int width, int height);
private:
    float xPos, yPos;
    float xVel, yVel;
    int screenW, screenH;
};

dot::dot(SDL_Renderer* renderer){
    xPos=0;
    yPos=0;
    xVel=0;
    yVel=0;
    screenW=0;
    screenH=0;
    //set up the rest normally
    lTexture();
    //and specifically set the renderer
    setRenderer(renderer);
}
//Check for what is happening an respond
void dot::handleEvent(SDL_Event& e){
    //check for key downs that arent repeats
    if(e.type==SDL_KEYDOWN && e.key.repeat==0){
        switch (e.key.keysym.sym) {
            case SDLK_UP: yVel-=DOT_VEL; break;
            case SDLK_DOWN: yVel+=DOT_VEL; break;
            case SDLK_RIGHT: xVel+=DOT_VEL; break;
            case SDLK_LEFT: xVel-=DOT_VEL; break;
            default:
                break;
        }
    }
    //cancel the motion when the key is released
    if(e.type==SDL_KEYUP && e.key.repeat==0){
        switch(e.key.keysym.sym){
            case SDLK_UP: yVel+=DOT_VEL; break;
            case SDLK_DOWN: yVel-=DOT_VEL; break;
            case SDLK_RIGHT: xVel-=DOT_VEL; break;
            case SDLK_LEFT: xVel+=DOT_VEL; break;
            default:
                break;
        }
    }
}
//want to change the movement to be per second instead of per frame
void dot::move(float time){
    //the time is going to be in microseconds so account for that
    //move the dots screen position
    xPos+=xVel*time;
    //check if the dot is off screen
    if(xPos<0){
        xPos=0;
    }
    else if(xPos > screenW-this->getWidth()){
        xPos=screenW-this->getWidth();
    }
    
    yPos+=yVel*time;
    if(yPos<0){
        yPos=0;
    }
    else if(yPos > screenH-this->getHeight()){
        yPos=screenH-this->getHeight();
    }
    
}

//method to set the screen size for the dot that is moving around
void dot::setScreenSize(int width, int height){
    screenW=width;
    screenH=height;
}

#endif /* dot_h */
