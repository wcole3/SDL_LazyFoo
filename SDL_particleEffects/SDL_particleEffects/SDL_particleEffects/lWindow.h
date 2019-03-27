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
    //method to set up window and renderer
    bool init(int screenWidth, int screenHeight);
    //method to handle window events
    void handleEvent(SDL_Event& e);
    //method to render the window
    void render();
    //method to focus on window
    void focus();
    //method to get reference to window renderer
    SDL_Renderer* getRenderer(){return mRenderer;};
    //some method to get private variable values
    int getWidth(){return mWidth;};
    int getHeight(){return mHeight;};
    int getWindowID(){return mWindowID;};
    bool hasMouseFocus(){return mMouseFocus;};
    bool hasKeyboardFocus(){return mKeyboardFocus;};
    bool isMinimized(){return mMinimized;};
    bool isFullScreen(){return mFullScreen;};
    bool isShown(){return mShown;};
    
private:
    //the window object
    SDL_Window* mWindow;
    //renderer object
    SDL_Renderer* mRenderer;
    //the window's width and height
    int mWidth;
    int mHeight;
    int mWindowID;
    int mWindowDisplayID;
    //an outside definition of the max number fo connected displays
    int TOTAL_DISPLAYS;
    //the bounds of each display
    SDL_Rect* mDisplayBounds = NULL;
    //bools to determine the state of the window
    bool mMouseFocus;
    bool mKeyboardFocus;
    bool mMinimized;
    bool mFullScreen;
    bool mShown;
    
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
    mShown = true;
    
}

lWindow::~lWindow(){
    free();
}

void lWindow::free(){
    //destroy the window
    if(mRenderer!=NULL){
        SDL_DestroyRenderer(mRenderer);
        mRenderer = NULL;
        if(mWindow!=NULL){
            SDL_DestroyWindow(mWindow);
            mWindow = NULL;
            mWidth = 0;
            mHeight = 0;
        }
    }
}
//setup window and renderer
bool lWindow::init(int width, int height){
    bool successFlag = true;
    //need to setup the window
    mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(mWindow == NULL){
        printf("Could not create window #%s! SDL error: %s\n", std::to_string(getWindowID()).c_str(),SDL_GetError());
        successFlag = false;
        
    }else{
        //get window id
        mWindowID = SDL_GetWindowID(mWindow);
        //get display id
        mWindowDisplayID = SDL_GetWindowDisplayIndex(mWindow);
        //get the total number of displays
        TOTAL_DISPLAYS = SDL_GetNumVideoDisplays();
        if(TOTAL_DISPLAYS < 2){
            printf("Only one display connected!");
        }
        //create renderer
        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
        if(mRenderer == NULL){
            printf("Could not create renderer for window %s! SDL error: %s\n", std::to_string(getWindowID()).c_str(), SDL_GetError());
            successFlag = false;
            //need to destory window if this fails
            SDL_DestroyWindow(mWindow);
            mWindow = NULL;
        }else{
            SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
            mMouseFocus = true;
            mKeyboardFocus = true;
            mShown = true;
            mWidth = width;
            mHeight = height;
            //setup the display bounds
            mDisplayBounds = new SDL_Rect[TOTAL_DISPLAYS];
            for(int i = 0; i < TOTAL_DISPLAYS; i++){
                SDL_GetDisplayBounds(i, &mDisplayBounds[i]);
            }
        }
        
    }
    return successFlag;
}

//need to hande window events
void lWindow::handleEvent(SDL_Event& e){
    //need to know if we need to change the caption
    bool updateCaption = false;
    //need to check if the event is window event
    if(e.type == SDL_WINDOWEVENT && e.window.windowID == getWindowID()){
        //check the type of window event
        switch (e.window.event) {
            //reset the display index if the window moved
            case SDL_WINDOWEVENT_MOVED:
                mWindowDisplayID = SDL_GetWindowDisplayIndex(mWindow);
                updateCaption = true;
                break;
            case SDL_WINDOWEVENT_SHOWN:
                mShown = true;
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                mShown = false;
                break;
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
                break;
            //need to hide window if the close event is shown
            case SDL_WINDOWEVENT_CLOSE:
                mShown = false;
                SDL_HideWindow(mWindow);
                break;
            default:
                break;
        }
        
    }
    //use return to enter/exit full screen
    if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && !isMinimized() && e.window.windowID == getWindowID()){
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
    else if(e.type == SDL_KEYDOWN){
        //want to use up and down to switch displays
        bool switchDisplay = false;
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                mWindowDisplayID++;
                switchDisplay = true;
                break;
            case SDLK_DOWN:
                mWindowDisplayID--;
                switchDisplay = true;
            default:
                break;
        }
        if(switchDisplay){
            //first check to make sure index is in range
            if(mWindowDisplayID < 0){
                //loop around
                mWindowDisplayID = TOTAL_DISPLAYS - 1;
            }
            if(mWindowDisplayID == TOTAL_DISPLAYS){
                mWindowDisplayID = 0;
            }
            //once range has been check set window position to center of new display
            SDL_SetWindowPosition(mWindow, mDisplayBounds[mWindowDisplayID].x +(mDisplayBounds[mWindowDisplayID].w - getWidth())/2, mDisplayBounds[mWindowDisplayID].y + (mDisplayBounds[mWindowDisplayID].h - getHeight())/2);
            
            updateCaption = true;
        }
    }
    //now redo the caption if needed
    if(updateCaption){
        std::stringstream caption;
        caption<<"Window #"<< mWindowID <<" : Display #"<<mWindowDisplayID<<" : Mouse Focus: "<<((mMouseFocus) ? "Yes" : "No")<<" Keyboard Focus: "<<((mKeyboardFocus) ? "Yes" : "No");
        SDL_SetWindowTitle(mWindow, caption.str().c_str());
    }
}

void lWindow::render(){
    //only render if shown
    if(!isMinimized()){
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
        SDL_RenderClear(mRenderer);
        //SDL_RenderPresent(mRenderer);
    }
}

void lWindow::focus(){
    //lets check to make sure they arent minimized
    if(!isMinimized()){
        //first show window if not shown
        if(!isShown()){
            SDL_ShowWindow(mWindow);
        }
        //move window forward
        SDL_RaiseWindow(mWindow);
    }
}
#endif /* lWindow_h */
