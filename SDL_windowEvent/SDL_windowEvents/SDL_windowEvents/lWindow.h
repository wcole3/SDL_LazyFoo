//
//  lWindow.h
//  SDL_windowEvents
//  wrapper class for window to accomodate window events
//  Created by William Cole on 1/5/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#ifndef lWindow_h
#define lWindow_h
#include <sstream>

class lWindow{
public:
    lWindow();
    ~lWindow();
    //method to destroy window
    void free();
    //method to set up window
    bool init(int screenWidth, int screenHeight);
    //method to set SDL renderer to the window
    SDL_Renderer* createRenderer();
    //method to handle window events
    void handleEvent(SDL_Event& e);
    
    
    
    //some method to get private variable values
    int getWidth(){return mWidth;};
    int getHeight(){return mHeight;};
    bool hasMouseFocus(){return mMouseFocus;};
    bool hasKeyboardFocus(){return mKeyboardFocus;};
    bool isMinimized(){return mMinimized;};
    bool isFullScreen(){return mFullScreen;};
    
private:
    //the window object
    SDL_Window* mWindow;
    //renderer object
    SDL_Renderer* mRenderer;
    //the window's width and height
    int mWidth;
    int mHeight;
    //bools to determine the state of the window
    bool mMouseFocus;
    bool mKeyboardFocus;
    bool mMinimized;
    bool mFullScreen;
    
};

lWindow::lWindow(){
    //initialize all varibales
    mWindow = NULL;
    mRenderer = NULL;
    mWidth = 0;
    mHeight = 0;
    mMinimized = false;
    mFullScreen = false;
    mMouseFocus = false;
    mKeyboardFocus = false;
}

lWindow::~lWindow(){
    free();
}

void lWindow::free(){
    //destroy the window
    if(mWindow!=NULL){
        SDL_DestroyWindow(mWindow);
        mWindow = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

SDL_Renderer* lWindow::createRenderer(){
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
    return mRenderer;
}

bool lWindow::init(int width, int height){
    bool successFlag = true;
    //need to setup the window
    mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(mWindow == NULL){
        printf("Could not create window! SDL error: %s\n", SDL_GetError());
        successFlag = false;
        
    }else{
        mMouseFocus = true;
        mKeyboardFocus = true;
        mWidth = width;
        mHeight = height;
    }
    return successFlag;
}

//need to hande window events
void lWindow::handleEvent(SDL_Event& e){
    //need to check if the event is window event
    if(e.type == SDL_WINDOWEVENT){
        //need to know if we need to change the caption
        bool updateCaption = false;
        //check the type of window event
        switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                //the window has been resized
                //change the width and height
                mWidth=e.window.data1;
                mHeight=e.window.data2;
                //rerender the window
                SDL_RenderPresent(mRenderer);
                break;
            
            case SDL_WINDOWEVENT_EXPOSED:
                //the window has been exposed
                SDL_RenderPresent(mRenderer);
                break;
                
            //now need to check all the conditionals
            case SDL_WINDOWEVENT_ENTER:
                mMouseFocus = true;
                //whenever the conditionals change we change the caption
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_LEAVE:
                mMouseFocus = false;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                mKeyboardFocus = true;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                mKeyboardFocus = false;
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                mMinimized = true;
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
                mMinimized = false;
                break;
            case SDL_WINDOWEVENT_RESTORED:
                mMinimized = false;
            default:
                break;
        }
        //now redo the caption if needed
        if(updateCaption){
            std::stringstream caption;
            caption<<"SDL Tutorial: Mouse Focus: "<<((mMouseFocus) ? "Yes" : "No")<<" Keyboard Focus: "<<((mKeyboardFocus) ? "Yes" : "No");
            SDL_SetWindowTitle(mWindow, caption.str().c_str());
        }
    }
    //use return to enter/exit full screen
    if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && !isMinimized()){
        if(!mFullScreen){
            mFullScreen = true;
            mMinimized = false;
            SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
        }
        else{
            mFullScreen = false;
            SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
        }
    }
}

#endif /* lWindow_h */
