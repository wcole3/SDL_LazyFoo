//
//  lDataStream.h
//  SDL_textureStreaming
//  wrapper class for data streaming
//  Created by William Cole on 1/20/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#ifndef lDataStream_h
#define lDataStream_h

class lDataStream{
public:
    lDataStream();
    ~lDataStream();
    //method to free all surfaces
    void free();
    //method to load all the images
    bool loadImages();
    //method to get current frame data
    void* getBuffer();
private:
    //the surfaces that define the texture stream
    SDL_Surface* mImages[4];
    //the index of the current image
    int currentImage;
    //the counter for how many frames to delay before changing images
    int delayFrames;
};

lDataStream::lDataStream(){
    for(int i = 0; i < 4; i++){
        mImages[i] = NULL;
    }
    currentImage = 0;
    //we will decrement the delay so start it at 4
    delayFrames = 500;
}

lDataStream::~lDataStream(){
    free();
}

void lDataStream::free(){
    //free up all of the resources
    for(int i = 0; i < 4; i++){
        if(mImages[i] != NULL){
            SDL_FreeSurface(mImages[i]);
            mImages[i] = NULL;
        }
    }
    currentImage = 0;
    delayFrames = 0;
}
//setup the images
bool lDataStream::loadImages(){
    bool successFlag = true;
    //need to get the string that loads each image
    for(int i = 0; i < 4; i++){
        char path[64] = " ";
        //get the path
        sprintf(path, "foo_walk_%d.png", i);
        //and load the surface
        SDL_Surface* loadedSurface = IMG_Load(path);
        if(loadedSurface == NULL){
            printf("Could not load the image! IMG error: %s\n", IMG_GetError());
            successFlag = false;
        }else{
            //set the image
            mImages[i] = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGB888, NULL);
        }
        //free the loadedsurface
        SDL_FreeSurface(loadedSurface);
    }
    return successFlag;
}
//method to get the pixels from the current image
void* lDataStream::getBuffer(){
    //need to check which image is current
    --delayFrames;
    if(delayFrames == 0){
        //reset delay and increase the current image index
        ++currentImage;
        delayFrames = 500;
    }
    //reset current image
    if(currentImage == 4){
        currentImage = 0;
    }
    return mImages[currentImage]->pixels;
}

#endif /* lDataStream_h */
