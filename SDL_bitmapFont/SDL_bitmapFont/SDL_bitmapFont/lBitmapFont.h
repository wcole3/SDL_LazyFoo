//
//  lBitmapFont.h
//  SDL_bitmapFont
//  wrapper class for a custom font
//  Created by William Cole on 1/18/19.
//  Copyright © 2019 William Cole. All rights reserved.
//

#ifndef lBitmapFont_h
#define lBitmapFont_h

class lBitmapFont{
public:
    //constructor
    lBitmapFont();
    //deconstructor
    ~lBitmapFont();
    //need a method to setup the font
    bool buildFont(lTexture *texture);
    //method to render a text string
    void render(int x, int y, std::string text);
    
private:
    //need a pointer to the image which contains the characters
    //We assume this will be a charactter sheets that is 16x16
    lTexture* mFontTexture;
    //list of RECTS that define each character
    SDL_Rect mChars[256];
    //values for new line and space
    int mSpace;
    int mNewLine;
};

lBitmapFont::lBitmapFont(){
    //initialize the variables
    mFontTexture = NULL;
    mSpace = 0;
    mNewLine = 0;
}

lBitmapFont::~lBitmapFont(){
    //free the resources
    mFontTexture = NULL;
    mSpace = 0;
    mNewLine = 0;
}

//method to build the font, we need to essentially setup the mChars array
bool lBitmapFont::buildFont(lTexture* texture){
    bool successFlag = true;
    //first need to lock the texture
    if(!texture->lockTexture()){
        printf("Could not lock texture before building font!");
        successFlag = false;
    }else{
        //now we need to setup to run through the texture and find the characters
        //the texture is assumed to be a 16x16 grid so we now the width and height of each grid cell
        int cellW = (texture->getWidth()/16);
        int cellH = (texture->getHeight()/16);
        //need an index for the current character
        int currentChar = 0;
        //also need to know what the background color is, which we assume is the pixel at 0,0
        Uint32 bgColor = texture->getPixel(0, 0);
        //also need to know the top of the heightest character and the bottom of the character 'A'
        int top = cellH;//note that the heightest figure will have the lowest value of top!
        int bottomA = cellH;
        //now we can begin looping
        //THIS ORDER IS VERY IMPORTANT AND MUST MATCH THE ACSII ORDER OF THE TEXTURE!!!!
        for(int cellRow = 0; cellRow < 16; ++cellRow){
            for(int cellCol = 0; cellCol < 16; ++cellCol){
                //now set the variables for the SDL_rect object
                mChars[ currentChar ].x = cellCol*cellW;
                mChars[ currentChar ].y = cellRow*cellH;
                
                mChars[ currentChar ].w = cellW;
                mChars[ currentChar ].h = cellH;
                //now what we want to do is test each grid cell to find the start and end of the figure to change the x and w value of mChar[i]
                //start by testing the left of the figure
                for(int pCol = 0; pCol < cellW; ++pCol){
                    for(int pRow = 0; pRow < cellH; ++pRow){
                        //need to get the actual pixel values
                        int pX = (cellCol * cellW) + pCol;
                        int pY = (cellRow * cellH) + pRow;
                        //test the pixel to see if it isnt the bg color
                        if(texture->getPixel(pX, pY) != bgColor){
                            //if it isnt then we have found the left of the image
                            mChars[ currentChar ].x = pX;
                            //then once we have found it we can break the loops
                            pCol = cellW;
                            pRow = cellH;
                        }
                    }
                }
                //now need to test the right of the figure
                for(int pCol = cellW - 1; pCol >= 0; --pCol){
                    for(int pRow = 0; pRow < cellH; ++pRow){
                        //get the pixels
                        int pX = (cellCol * cellW) + pCol;
                        int pY = (cellRow * cellH) + pRow;
                        //test pixel
                        if(texture->getPixel(pX, pY) != bgColor){
                            //we have found the width
                            mChars[ currentChar ].w = (pX - mChars[ currentChar ].x) + 1;//there is a 1 pixel padding
                            //then we can break the loop
                            pCol = -1;
                            pRow = cellH;
                        }
                    }
                }
                //now need to find the global top, recall the heightest top is the lowest value
                for(int pRow = 0; pRow < cellH; ++pRow){//change the order of the loop to move from left to right and top to bottom
                    for(int pCol = 0; pCol < cellW; ++pCol){
                        //get pixel
                        int pX = (cellCol * cellW) + pCol;
                        int pY = (cellRow * cellH) + pRow;
                        //test
                        if(texture->getPixel(pX, pY) != bgColor){
                            //also test if the y value is lower that the current global value for the top
                            if( pRow < top){
                                top = pRow;
                                //and break loops
                                pRow = cellH;
                                pCol = cellW;
                            }
                        }
                    }
                }
                //finally text to see if we are at character 'A' and if we are find the bottom of the figure
                if(currentChar == 'A'){
                    for(int pRow = cellH - 1; pRow >= 0; --pRow){
                        for(int pCol = 0; pCol < cellW; ++pCol){
                            //get pixels
                            int pX = (cellCol * cellW) + pCol;
                            int pY = (cellRow * cellH) + pRow;
                            //test
                            if(texture->getPixel(pX, pY) != bgColor){
                                //we have found the base of the character
                                bottomA = pRow;
                                //break the loop
                                pRow = -1;
                                pCol = cellW;
                            }
                        }
                    }
                }
                //after we have done that for a character move on to the next
                ++currentChar;
            }
        }
        //now calculate the values for the space and new line
        mSpace = cellW/2;
        mNewLine = bottomA - top;
        //now need to correct each y and h value of the mChars
        for(int i = 0; i < 256; i++){
            //we set the y value to top
            mChars[ i ].y += top;
            mChars[ i ].h -= top;
        }
        //now unlock the texture
        texture->unlockTexture();
        //and set the texture
        mFontTexture = texture;
    }
    
    return successFlag;
}
//method to render a text string to a position on the screen
void lBitmapFont::render(int x, int y, std::string text){
    //check to make sure the texture is set
    if(mFontTexture != NULL){
        //offsets
        int posX = x;
        int posY = y;
        //now need to get each character of the text string
        for(int i = 0; i < text.length(); i++){
            //need to see if the character is a space or newline
            if(text[i] == ' '){
                posX += mSpace;
            }
            else if( text[i] == '\n'){
                posX = x;
                posY += mNewLine;
            }else{
                //need to figure out the character to render
                int acsii = (unsigned char)text[i];
                //now that we know the character we can render the value
                mFontTexture->render(posX, posY, &mChars[ acsii ]);
                //need to move the position over
                posX += mChars[acsii].w + 1;//add one pixel of padding
            }
        }
    }
}
#endif /* lBitmapFont_h */
