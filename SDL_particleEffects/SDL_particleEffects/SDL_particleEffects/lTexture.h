//
//  lTexture.h
//  SDL_renderFlip
//  wrapper class for SDL_texture
//  Created by William Cole on 11/11/18.
//  Copyright © 2018 William Cole. All rights reserved.
//


#ifndef lTexture_h
#define lTexture_h


class lTexture{
public:
    //need a way to set the renderer and font externally
    
    
    lTexture(SDL_Renderer* renderer=NULL);
    ~lTexture();
    bool loadFromFile(std::string path, SDL_bool colorKey, SDL_Color keyColor = {0,0,0});
#ifdef _SDL_TTF_H
    bool loadFromRenderedText(std::string text, SDL_Color color);
    void setFont(TTF_Font* font);
#endif
    void free();
    void setRenderer(SDL_Renderer* renderer);
    void setBlend(SDL_BlendMode blend);
    void setColor(Uint8 red, Uint8 green, Uint8 blue);
    void setAlpha(Uint8 alpha);
    void render(int x, int y, SDL_Rect* clip=NULL,double angle=0, SDL_Point* center=NULL,SDL_RendererFlip flip=SDL_FLIP_NONE );
    int getWidth(){return textW;};
    int getHeight(){return textH;};
    
private:
    int textW;
    int textH;
    SDL_Texture* mTexture;
    SDL_Renderer* lRenderer;
#ifdef _SDL_TTF_H
    TTF_Font* lFont;
#endif
};

//define methods
//constructor
lTexture::lTexture(SDL_Renderer* render){
    textH=0;
    textW=0;
    mTexture=NULL;
    lRenderer=render;
    
}
//destrcutor
lTexture::~lTexture(){
    free();
}
//deallocation of variables
void lTexture::free(){
    if(mTexture!=NULL){
        SDL_DestroyTexture(mTexture);
        mTexture=NULL;
        textW=0;
        textH=0;
        //Since lRenderer is a passed variable it will be destroyed in main
    }
}
//method to set renderer to specific reference (used for inheritance classes)
void lTexture::setRenderer(SDL_Renderer* renderer){
    lRenderer=NULL;
    lRenderer=renderer;
}
//method to set the blend mode
void lTexture::setBlend(SDL_BlendMode blend){
    SDL_SetTextureBlendMode(mTexture, blend);
}
//set the color modulation of the texture
void lTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}
//set alpha modulation
void lTexture::setAlpha(Uint8 alpha){
    SDL_SetTextureAlphaMod(mTexture, alpha);
}
//method to load image from file path
bool lTexture::loadFromFile(std::string path, SDL_bool colorKey, SDL_Color keyColor){
    free();
    SDL_Surface* loadedSurface=NULL;
    SDL_Texture* loadedTexture=NULL;
    loadedSurface=IMG_Load(path.c_str());
    if(loadedSurface==NULL){
        printf("Could not load image at path: %s! IMG error: %s\n",path.c_str(),IMG_GetError());
    }else{
        //color key here if need; should change this to make it possible in the call to specify the color (DO THIS!!)
        SDL_SetColorKey(loadedSurface, colorKey, SDL_MapRGB(loadedSurface->format, keyColor.r, keyColor.g, keyColor.b));
        //if the surface is loaded put it in a texture
        loadedTexture=SDL_CreateTextureFromSurface(lRenderer, loadedSurface);
        if(loadedTexture==NULL){
            printf("Could not create Texture from Surface! SDL error: %s\n",SDL_GetError());
        }else{
            textW=loadedSurface->w;
            textH=loadedSurface->h;
        }
    }
    SDL_FreeSurface(loadedSurface);
    mTexture=loadedTexture;
    //return true if mTexture is not NULL
    return mTexture!=NULL;
}
#ifdef _SDL_TTF_H
//method to load an image text string, this works pretty similar to above just using TTF
bool lTexture::loadFromRenderedText(std::string text, SDL_Color color){
    //start with freeing things
    free();
    SDL_Surface* loadedSurface=NULL;
    //using TTF and our loaded font, create an image of the text string
    loadedSurface=TTF_RenderText_Solid(lFont, text.c_str(), color);
    if(loadedSurface==NULL){
        printf("Could not create text image! TTF error: %s\n",TTF_GetError());
    }else{
        //if the surface is loaded we need to create a texture
        mTexture=SDL_CreateTextureFromSurface(lRenderer, loadedSurface);
        if(mTexture==NULL){
            printf("Could not create texture from surface! SDL error: %s\n", SDL_GetError());
        }else{
            //set the dimensions
            textH=loadedSurface->h;
            textW=loadedSurface->w;
        }
    }
    //free surface
    SDL_FreeSurface(loadedSurface);
    //return true if mTexture is not NULL
    return mTexture!=NULL;
}

void lTexture::setFont(TTF_Font* font){
    lFont=font;
}
#endif
//finally the rendering method
void lTexture::render(int x, int y, SDL_Rect* clip,double angle, SDL_Point* center, SDL_RendererFlip flip ){
    //first setup screen loaction argument
    SDL_Rect screenLoc={x,y,getWidth(),getHeight()};
    if(clip!=NULL){
        screenLoc.w=clip->w;
        screenLoc.h=clip->h;
    }
    //render with rendercopyex
    SDL_RenderCopyEx(lRenderer, mTexture, clip, &screenLoc, angle, center, flip);
}

#endif /* lTexture_h */
