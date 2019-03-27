//
//  lRigidDot.h
//  SDL_collidingDot
//  Classes that inherits from lTexture and adds rigid body behavior starting with
//  collision detection
//  Created by William Cole on 12/6/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#ifndef lRigidDot_h
#define lRigidDot_h
#include "lTexture.h"

class lRigidDot: public lTexture{
public:
    //constructor to setup screen size and renderer to use
    lRigidDot(int width=0, int height=0, SDL_Renderer* renderer=NULL);
    //handle events to set the velocity through key presses
    void handleEvent(SDL_Event& e);
    //move the dot's posiiton synced to time
    void move(float time, SDL_Rect& box);
    //detect if two rects are touching
    bool detectCollision(SDL_Rect& boxA, SDL_Rect& boxB);
    //going to overload the loadFromFile method of lTexture to setup collision box here
    bool loadFromFile(std::string path, SDL_bool colorKey);
    //functions to return the dot's position
    float getXPos(){return xPos;};
    float getYPos(){return yPos;};
    //constant static for the velocity
    static const int DOT_VEL=200; //PIXELS PER SECOND
    
private:
    //screen positions
    float xPos, yPos;
    //velocity
    float xVel, yVel;
    //SDL_rect which defines the collision zone for the texture
    SDL_Rect mCollisionBox;
    //screen size
    int screenW, screenH;
    
};
//constructor to set screen and renderer
lRigidDot::lRigidDot(int width, int height, SDL_Renderer* renderer){
    //initialize all variables
    xPos=0;
    yPos=0;
    xVel=0;
    yVel=0;
    //set screen size
    screenW=width;
    screenH=height;
    //setup rest
    lTexture();
    //set renderer
    this->setRenderer(renderer);
}

bool lRigidDot::loadFromFile(std::string path, SDL_bool colorKey){
    bool successFlag=true;
    //load the file as lTexture would normally
    successFlag=lTexture::loadFromFile(path, colorKey);
    //then setup the extras, I'm doing this since the texture's width and height are not set before this call
    mCollisionBox.w=getWidth();
    mCollisionBox.h=getHeight();
    mCollisionBox.x=(int)xPos;//remember these are floats
    mCollisionBox.y=(int)yPos;
    return successFlag;
}
//need to handle keypresses and set the velocity appropriately
void lRigidDot::handleEvent(SDL_Event& e){
    //make sure the key is down and we discard repeats
    if(e.type==SDL_KEYDOWN && e.key.repeat==0){
        switch (e.key.keysym.sym) {
            case SDLK_UP: yVel-=DOT_VEL; break;
            case SDLK_DOWN: yVel+=DOT_VEL; break;
            case SDLK_RIGHT: xVel+=DOT_VEL; break;
            case SDLK_LEFT: xVel-=DOT_VEL; break;
        }
    }
    //do the opposite on key up
    if(e.type==SDL_KEYUP && e.key.repeat==0){
        switch (e.key.keysym.sym) {
            case SDLK_UP: yVel+=DOT_VEL; break;
            case SDLK_DOWN: yVel-=DOT_VEL; break;
            case SDLK_RIGHT: xVel-=DOT_VEL; break;
            case SDLK_LEFT: xVel+=DOT_VEL; break;
        }
    }
}
//need to be able to detect collision
bool lRigidDot::detectCollision(SDL_Rect& boxA, SDL_Rect& boxB){
    //doing collison detection by axis projection, start with the box corners
    //Box A's corners
    int topA=boxA.y;
    int botA=boxA.y+boxA.h;
    int leftA=boxA.x;
    int rightA=boxA.x+boxA.w;
    //box B's corners
    int topB=boxB.y;
    int botB=boxB.y+boxB.h;
    int leftB=boxB.x;
    int rightB=boxB.x+boxB.w;
    
    //now we just have to test these
    //if BoxA is above boxB not colliding
    if(botA < topB){
        return false;
    }
    //if boxA is to the left of boxB not colliding
    else if(rightA < leftB){
        return false;
    }
    //if boxB is above boxA not colliding
    else if(botB < topA){
        return false;
    }
    //if boxB is to the left of boxA not colliding
    else if(rightB < leftA){
        return false;
    }
    //otherwise they are colliding
    return true;
}

//now need to move the dot wrt time passed and make sure it hasn't collided with anything
//right now the collision behaves like the box has inifite mass
void lRigidDot::move(float time, SDL_Rect& box){
    //here box is some external structure the dot could collide with
    //first update the x position
    float oldxPos=xPos;
    xPos+=xVel*time;
    //also need to update the collision box
    mCollisionBox.x=xPos;
    //check if the dot is off screen
    if(xPos < 0){
        //too far left
        xPos=0;
    }else if(xPos > screenW - getWidth()){
        //too far right
        xPos=screenW-getWidth();
    }else if(detectCollision(mCollisionBox, box)){
        //the dot has collided with the box along the x axis
        xPos=oldxPos;//not sure if there is a cleverer way to do this
        mCollisionBox.x=xPos;
    }
    //now do the same for the y direction
    float oldyPos=yPos;
    yPos+=yVel*time;
    mCollisionBox.y=yPos;
    if(yPos<0){
        //too far up
        yPos=0;
    }else if(yPos > screenH -getHeight()){
        //too far down
        yPos=screenH-getHeight();
    }else if(detectCollision(mCollisionBox, box)){
        //the dot has collided with the box along the y axis
        yPos=oldyPos;
        mCollisionBox.y=yPos;
    }
}

#endif /* lRigidDot_h */
