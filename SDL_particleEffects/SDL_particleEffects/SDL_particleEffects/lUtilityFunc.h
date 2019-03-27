//
//  lUtilityFunc.h
//  SDL_circleCollision
//  Some utility functions used in SDL tutorials
//  Created by William Cole on 12/10/18.
//  Copyright © 2018 William Cole. All rights reserved.
//

#ifndef lUtilityFunc_h
#define lUtilityFunc_h

//a circle structure
struct circle{
    //center coordinates, they are floats for physics based movement
    float x, y;
    //radius
    int r;
};

//method to calculate the distance squared between two points
double distanceSquared(float x1, float x2, float y1, float y2){
    double distanceSq = ((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1));
    return distanceSq;
}

int getSign(float number){
    if(number < 0){
        return -1;
    }
    else if(number > 0){
        return 1;
    }
    else{
        return 0;
    }
}
#endif /* lUtilityFunc_h */
