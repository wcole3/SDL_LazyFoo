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
#include "lUtilityFunc.h"

class lRigidDot: public lTexture{
public:
    //constructor to setup screen size and renderer to use
    lRigidDot(int width=0, int height=0, SDL_Renderer* renderer=NULL, int startX=0, int startY=0);
    //handle events to set the velocity through key presses
    void handleEvent(SDL_Event& e);
    //a couple variations of movingt he dot around the screen
    //the first will just move the dot subject to the screen boundaries
    void move(float timeStep);
    //one moves the dot and background; the dot's movement controls the backgrounds movement
    void move(float timeStep, int& bckScrollIndex, lTexture* scrollingTexture);
    //move the dot's posiiton synced to time subject to two obstacles one box and one circle
    void move(float time, SDL_Rect& box, circle& circle);
    //detect if two circles are touching
    bool detectCollision(circle& circleA, circle& circleB);
    //method to see if a circle and a box are touching
    bool detectCollision(circle& circle, SDL_Rect& box);
    //going to overload the loadFromFile method of lTexture to setup collision box here
    bool loadFromFile(std::string path, SDL_bool colorKey);
    //need a function to render the circle taking into account the offset
    void render(int cameraX = 0, int cameraY = 0);
    //functions to return the dot's position
    float getXPos(){return xCenterPos;};
    float getYPos(){return yCenterPos;};
    circle& getCollider(){return mCollisionCircle;};
    //constant static for the velocity
    static const int DOT_VEL=200; //PIXELS PER SECOND
    static const int SCROLL_SPEED=2;
private:
    //screen positions of the CENTER of the circle
    float xCenterPos, yCenterPos;
    //velocity
    float xVel, yVel;
    //two ints specify the startting screen position
    int xStart, yStart;
    //SDL_rect which defines the collision zone for the texture
    circle mCollisionCircle;
    //screen size OR level size for scrolling around a level with fixed camera
    int screenW, screenH;
    //method to shift the collision circle with dot movement
    void shiftCollider();
    void setStartingPos(int x, int y);
};
//constructor to set screen and renderer
lRigidDot::lRigidDot(int width, int height, SDL_Renderer* renderer, int startX, int startY){
    //initialize all variables
    xCenterPos=0;
    yCenterPos=0;
    xVel=0;
    yVel=0;
    //set screen size
    screenW=width;
    screenH=height;
    //set starting position
    setStartingPos(startX, startY);
    //setup rest
    lTexture();
    //set renderer
    this->setRenderer(renderer);
}

//method to set the starting position and make sure it is on screen
void lRigidDot::setStartingPos(int x, int y){
    //check to make sure the desired started position is on screen
    if(x < 0 || x > screenW){
        //invalid set at x=0
        xStart=0;
    }else{
        xStart=x;
    }
    if(y < 0 || y > screenH){
        yStart=0;
    }else{
        yStart=y;
    }
}//the only real reason to do it this way instead of directly setting the xCenterPos and yCenterPos is we can check if the coordinates are on screen

bool lRigidDot::loadFromFile(std::string path, SDL_bool colorKey){
    bool successFlag=true;
    //load the file as lTexture would normally
    successFlag=lTexture::loadFromFile(path, colorKey);
    //then setup the extras, I'm doing this since the texture's width and height are not set before this call
    //we can then setup the circle
    mCollisionCircle.r=getWidth()/2;
    xCenterPos= xStart + mCollisionCircle.r;
    yCenterPos= yStart + mCollisionCircle.r;
    //move the collision circle's x and y positions to the xCenterPos and yCenterPos taking into account offsets
    shiftCollider();
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
//need to be able to detect collision between two circles
bool lRigidDot::detectCollision(circle& circleA, circle& circleB){
    //need to test if the distance squared between the two circles is less than the total radius squared if so then the objects are colliding
    //circle A parameter
    float xA=circleA.x;
    float yA=circleA.y;
    int rA=circleA.r;
    //circle B parameter
    float xB=circleB.x;
    float yB=circleB.y;
    int rB=circleB.r;
    int totalRadiusSQ= (rA+rB)*(rA+rB);
    if(distanceSquared(xA, xB, yA, yB) < totalRadiusSQ){
        //if the distance sq is smaller than the radius squared they are touching
        return true;
    }
    //if test gets here there is not collision
    return false;
}

//method to check collision between circle and rect
bool lRigidDot::detectCollision(circle& circle, SDL_Rect& box){
    //here we test the distance sq between the circle center and the closest point on the box agaisnt the circle's radius sq
    //we first need to find the closest point on the box (remember the coordinates are defined at the top left
    //test x and y seperately, x first
    float closestX, closestY;
    //if the circle center is to the left of the box the left most coordinate is box.x
    if((circle.x) < box.x){
        closestX=box.x;
    }
    //if the circle center is to right then right side is closest
    else if((circle.x) > (box.x + box.w)){
        closestX = box.x + box.w;
    }
    //the only other option is the x coordinate of the circle is somewhere between box.x and box.x+box.w
    //in that case the x coordinate of the box that is closest is the x coor of the circle
    else{
        closestX=circle.x;
    }
    //now do y axis
    //if circle center is above the top of the box is closest
    if(circle.y < box.y){
        closestY=box.y;
    }
    //if circle center is below then the bottom is closest
    else if( circle.y > (box.y + box.h)){
        closestY=box.y + box.h;
    }
    //otherwise the y coordinate of the circle is closest
    else{
        closestY=circle.y;
    }
    //now we know the closest point on the box we can check collision against radius sq
    if(distanceSquared(circle.x, closestX, circle.y, closestY) < (circle.r*circle.r)){
        //if the distance is smaller then there is collision
        return true;
    }
    //otherwise there is no collison
    return false;
}

//method to move the collision circle in sync with the xCenterPos and yCenterPos variables
void lRigidDot::shiftCollider(){
    mCollisionCircle.x=xCenterPos;
    mCollisionCircle.y=yCenterPos;
}
//move function subject to screen size
void lRigidDot::move(float timeStep){
    //move the dot then check boundaries
    //you can cut out either direction to keep dot rooted in that direction
    xCenterPos+=xVel*timeStep;
    shiftCollider();
    //check x
    if(xCenterPos-mCollisionCircle.r < 0){
        xCenterPos=mCollisionCircle.r;
        shiftCollider();
    }
    if(xCenterPos + mCollisionCircle.r > screenW){
        xCenterPos=screenW-mCollisionCircle.r;
        shiftCollider();
    }
    
    //check y
    yCenterPos+=yVel*timeStep;
    shiftCollider();
    if(yCenterPos - mCollisionCircle.r < 0){
        yCenterPos=mCollisionCircle.r;
        shiftCollider();
    }
    if(yCenterPos + mCollisionCircle.r > screenH){
        yCenterPos=screenH-mCollisionCircle.r;
        shiftCollider();
    }
}

//now a move fucntion that will move the background, need to pass the scroll index by reference
void lRigidDot::move(float timeStep, int &scrollIndex, lTexture* scrollingTexture){
    //first move the dot
    move(timeStep);
    //then index the background in the correct direction; only care about x
    int xDirection = getSign(xVel);
    //need to change this to move the background the same amount as the dot moves
    //move in the opposite direction that the dot moves
    scrollIndex-=xDirection;//if you wanted to keep the dot rooted in the middle you would need to change the move method
    //check to make sure the index resets correctly
    if(scrollIndex < -1*scrollingTexture->getWidth()){
        scrollIndex=0;
    }
    if(scrollIndex > scrollingTexture->getWidth()){
        scrollIndex=0;
    }
}
//now need to move the dot wrt time passed and make sure it hasn't collided with anything
//right now the collision behaves like the box/circle has inifite mass
void lRigidDot::move(float time, SDL_Rect& box, circle& circle){
    //here box is some external structure the dot could collide with
    //first update the x position
    float oldxPos=xCenterPos;
    xCenterPos+=xVel*time;
    //also need to update the collision box
    shiftCollider();
    //check if the dot is off screen
    if(xCenterPos - mCollisionCircle.r < 0){
        //too far left
        xCenterPos = 0 + mCollisionCircle.r;
        shiftCollider();
    }else if(xCenterPos + mCollisionCircle.r > screenW){
        //too far right
        xCenterPos = screenW - mCollisionCircle.r;
        shiftCollider();
    }else if(detectCollision(getCollider(), box) || detectCollision(getCollider(), circle)){
        //the dot has collided with the box along the x axis
        xCenterPos=oldxPos;//not sure if there is a cleverer way to do this
        shiftCollider();
    }
    //now do the same for the y direction
    float oldyPos=yCenterPos;
    yCenterPos+=yVel*time;
    shiftCollider();
    
    if(yCenterPos - mCollisionCircle.r < 0){
        //too far up
        yCenterPos = 0 + mCollisionCircle.r;
        shiftCollider();
    }else if(yCenterPos + mCollisionCircle.r > screenH){
        //too far down
        yCenterPos=screenH - mCollisionCircle.r;
        shiftCollider();
    }else if(detectCollision(getCollider(), box) || detectCollision(getCollider(), circle)){
        //the dot has collided with the box along the y axis
        yCenterPos=oldyPos;
        shiftCollider();
    }
}

//need to overload the render function to take into account that we render at top left instead of center
void lRigidDot::render(int CameraX, int CameraY){
    //apply the offset for dot and camera
    int topLeftX = (int)(xCenterPos-getCollider().r);
    int topLeftY = (int)(yCenterPos-getCollider().r);
    //render
    lTexture::render(topLeftX-CameraX, topLeftY-CameraY);
}

#endif /* lRigidDot_h */
