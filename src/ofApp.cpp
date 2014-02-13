#include "ofApp.h"

// <3

ofImage* img;

unsigned char* textureData;
unsigned char* textureData2;
unsigned char* textureData3;


//--------------------------------------------------------------
void ofApp::setup(){
    pos.set(2, 2);
    FOV = 60;
    rot = PI;
    
    keys = new bool[512];
    for(int i = 0; i < 512; i++){
        keys[i] = false;
    }
    
    tex = ofTexture();
    tex.allocate(width, height, GL_RGB8);
    screenBuffer = new unsigned char[width * height * 8];

    
    width = ofGetWidth();
    height = ofGetHeight();

    pixels = new unsigned char[width * height * 3];
    

    
    
    
    
    
    img1.allocate(width, height, OF_IMAGE_COLOR);
    //img1.allocate(width, height, OF_IMAGE_COLOR);
    
    
    img = new ofImage[5];
    img[0].allocate(64, 64, OF_IMAGE_COLOR);
   // img[1].allocate(64, 64, OF_IMAGE_COLOR);
   // img[2].allocate(64, 64, OF_IMAGE_COLOR);
    
    img[0].loadImage("bricks.gif");
    img[1].loadImage("bricks1.png");
    img[2].loadImage("wall_plain.gif");
    img[3].loadImage("bricks1.gif");
    img[4].loadImage("bricks.gif");
    
    textureData =  new unsigned char[64 * 64 * 3];
    textureData2 = new unsigned char[64 * 64 * 3];
    textureData3 = new unsigned char[64 * 64 * 3];
    
  //  for(int x = 0; x < img[0].width; x++){
//        for(int y = 0; y < img[0].height; y++ ){
    for(int c = 0; c < 64 * 64 * 3; c++){
        textureData[c] = img[0].getPixels()[c];
    }

    for(int c = 0; c < 64 * 64 * 3; c++){
        textureData2[c] = img[1].getPixels()[c];
    }

    for(int c = 0; c < 64 * 64 * 3; c++){
        textureData3[c] = img[2].getPixels()[c];
    }
    
     //       textureData[y * width *3 + (x*3)] = img[0].getPixels()[
            
            //pixels[((int(startY) + yTexel) * width * 3) + (x*3)] = c.r;
    //    }
   // }

    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    float speed = 0.0f;
    
    if(keys[OF_KEY_UP] == true){
        speed = 0.1f;
    }
    if(keys[OF_KEY_DOWN] == true){
        speed = -0.1f;
    }
    
    if(keys[OF_KEY_RIGHT] == true){
        rot += 0.051f;
    }
    if(keys[OF_KEY_LEFT] == true){
        rot -= 0.051f;
    }
    
    pos = pos + dir * speed;
    
    dir.x = cos(rot);
    dir.y = sin(rot);
    
    right.x = -dir.y;
    right.y = dir.x;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    printf("fps %f\n", ofGetFrameRate());
    
    ofSetColor(64, 128, 255);
    ofRect(0, 0, width, height/2);
    
    ofSetColor(0, 255, 0);
    ofRect(0, height/2, width, height);
    
    float camPlaneMag = tan( FOV / 2.0f * (PI / 180.0) );
    
    ofVec2f rayDir;
    
    
    for(int c = 0; c < width * height * 3; c++){
        pixels[c] = 0;
    }
    
    
    unsigned long long timing = ofGetElapsedTimeMillis();
    
    for(int x = 0; x < width; x++){
        
        // Map screen coordinates [0 to width-1] to [-1 to 1]
        float currCamScale = (2.0f * x / float(width)) - 1.0f;
        
        rayDir.set(dir.x + (right.x * camPlaneMag) * currCamScale,
                   dir.y + (right.y * camPlaneMag) * currCamScale);
        
        
        /*
         What we are trying figure out is for each ray, when does it hit an edge? to answer that
         question we'll need to figure out some things:
         
         1. What is the magnitude from the players position travelling along the current ray to the nearest
         x edge (and y edge). Note I say magnitude because we will be working with scalar values.
         
         2. What is the magnitude from one x edge to the next x edge? (And y edge to the next y edge.)
         We aren't trying to figure out the vertical/horizontal distance, that's just a unit of 1. Instead, we
         need to calculate the hypotenues of the triangle fromed from the ray going from one
         edge to the other. Again, when calculating the magnitude for x the horizontal base leg length of the
         triangle formed will be 1. And when calculating the magnitude for y, the vertical height of the triangle will be 1.
         
         Once we have these values. We can start running an algorithm. Selecting the nearest x or y edge
         testing if that edge has a filled block associated with it, and if not, incrementing.
         We'll have two scalar values 'racing' one another. The algorithm will compare the their values
         and see which one is shorter and test if that shorter one has hit an edge.
         
         Okay, let's answer the second question first. Based on the direction, what is the magnitude/hypotenue length
         from one x edge to another? We know horizontally, the length is 1 unit. That means we
         need to scale our ray such that the x component will be 1.
         
         What value multilied by x will result in 1? The answer is the inverse of x! So if we scale the x component
         of the ray by 1/x, we'll need to do the same for y to maintain the ray direction. This gives us a new vector:
         
         v = [ rayDir.x * 1 / rayDir.x , rayDir.y * 1 / rayDir.x]
         
         We already know the result of the calculation for the first component, so we can simplify:
         
         v = [ 1, rayDir.y / rayDir.x ]
         
         Now we need to get the magnitude using the pythagorean theorm.
         */
        float magBetweenXEdges = ofVec2f(1, rayDir.y * (1.0 / rayDir.x) ).length();
        float magBetweenYEdges = ofVec2f(rayDir.x * (1.0 / rayDir.y), 1).length();
        
        float magToXedge;
        float magToYedge;
        
        int dirStepX;
        int dirStepY;
        
        int worldIndexX = int(pos.x);
        int worldIndexY = int(pos.y);
        
        /*
         - Keep track of which direction the ray is traveling
         - Get relative position of player in the current cell
         - Find out how far player is from edge
         - Use that as a value to determine how much to scale the magnitude
         
         For x, we have a triangle with a base horizontal length of 1 and some magnitude for its
         hypotenuse which we recently figured out. Now, based on the relative position of the user
         within a cell, how long is the hypotenuse of this triangle?
         
         If the x position of the player was 0.2,
         magToXedge = (0 + 1 - 0.2) * magBetweenXedges
         = 0.8 * magBetweenXedges
         */
        if(rayDir.x > 0){
            magToXedge = (worldIndexX + 1.0 - pos.x) * magBetweenXEdges;
            dirStepX = 1;
        }
        else{
            magToXedge = (pos.x - worldIndexX) * magBetweenXEdges;
            dirStepX = -1;
        }
        
        if(rayDir.y > 0){
            magToYedge = (worldIndexY + 1.0 - pos.y) * magBetweenYEdges;
            dirStepY = 1;
        }
        else{
            magToYedge = (pos.y - worldIndexY) * magBetweenYEdges;
            dirStepY = -1;
        }
        
        int sideHit;
        
        do{
            if(magToXedge < magToYedge){
                magToXedge += magBetweenXEdges;
                worldIndexX += dirStepX;
                sideHit = 0;
            }
            else{
                magToYedge += magBetweenYEdges;
                worldIndexY += dirStepY;
                sideHit = 1;
            }
        }while(worldMap[worldIndexX][worldIndexY] == 0);
        
        ofColor wallColor;
        switch(worldMap[worldIndexX][worldIndexY]){
            case 1:wallColor = ofColor(255,255,255);break;
            case 2:wallColor = ofColor(0,255,255);break;
            case 3:wallColor = ofColor(0,0,255);break;
            case 4:wallColor = ofColor(255,255,255);break;
            case 5:wallColor = ofColor(255,0,255);break;
        }
        
        // Basic lighting
        wallColor = sideHit == 0 ? wallColor : wallColor/2.0f;
        ofSetColor(wallColor);
        
        float wallDist;
        
        // Distortion Correction
        if(sideHit == 0){
            wallDist = fabs((worldIndexX - pos.x + (1.0 - dirStepX) / 2.0) / rayDir.x);
        }
        else{
            wallDist = fabs((worldIndexY - pos.y + (1.0 - dirStepY) / 2.0) / rayDir.y);
        }
        //printf("wall dist: %f\n", wallDist);
        
//        if(wallDist == 0)return;
        
        float t;
        if(sideHit == 0){
         //float fullAmt = pos.x + worldX;
         //float texCoordx = fullAmt - (pos.x + worldX);
         
         //length of vector from pos/ direction
         
         ofVec2f originToX = pos + (rayDir * magToXedge);// - rayDir;
         float texCoordX = originToX.y - int(originToX.y);
         
         t = pos.y + ((worldIndexX - pos.x + (1.0f - dirStepX) / 2.0) / rayDir.x) * rayDir.y;
         t = t - int(t);
         }
         else{
         //float fullAmt = pos.x + worldX;
         //float texCoordx = fullAmt - (pos.x + worldX);
         
         //length of vector from pos/ direction
         
         //                ofVec2f originToX = pos + (rayDir * magToXedge);// - rayDir;
         //              float texCoordX = originToX.y - int(originToX.y);
         
         t = pos.x + ((worldIndexY - pos.y + (1.0f - dirStepY) / 2.0) / rayDir.y) * rayDir.x;
         t = t - int(t);
         
         //                col = img.getColor(t * img.getWidth(), 0);
         }
        
        
        /*
         If ray was 1 unit from the wall, we draw a line the fills the entire length of the screen.
         
         To provide a proper FPS feel, we center the vertical lines in screen space along Y.
         
         To calculate how long the vertical line needs to be on the screen, we divide the viewport
         height by the distance from the wall. So if the wall was 1 unit from the player, it will fill
         up the entire vertical slide.
         
         At this point we need to center our line height in our viewport line. Draw two vertical
         lines down, the smaller one centered next to the larger one. To get the position of top of the smaller one
         relative to the larger one, you just have to divide both lines halfway horizontally then subtract from that
         the half that remained from the smaller line. This gives us the starting point for the line we have to
         draw.
         */
        //wallDist = wallDist == 0 ? 0.01f : wallDist;
        
        float lineHeight = fabs(height/wallDist);
        
        lineHeight = lineHeight > height ? height : lineHeight;
        
        float startY = height/2.0 - lineHeight/2.0;
        
        startY = startY <0 ? 0 : startY;
        
        int linesDrawn = 0 ;

       // ofColor lastColor = ofColor(0,0,0);
        int lastPosition;
        int usingCache;
        
        //
        
        // fill the buffer
        /*for(int xWidth = 0; xWidth < width; xWidth++){
            for(int yHeight = 0; yHeight < height; yHeight++){
                //screenBuffer[xWidth* yHeight] = 255;
                
//                screenBuffer[xWidth][yHeight] = ;
            }
        }*/
        

       // tex.loadData(screenBuffer, width, height, GL_RGB8);
        

        for(int yTexel = 0; yTexel < lineHeight; yTexel++){
            ofColor c = img[worldMap[worldIndexX][worldIndexY] -1].getColor(t * img[0].getWidth(),
              
          //  ofColor c = ofColor(0,0,0);
            
            int(yTexel/(float)lineHeight * (img[0].getHeight()-1)) );
//            pixels[height * ]
            
            
/*
            getColor(t * img[0].getWidth(),
                     int(yTexel/(float)lineHeight * (img[0].getHeight()-1)) );
                     
                     */
            
            int ximg = t * img[0].getWidth();
            int yimg = int(yTexel/(float)lineHeight * (img[0].getHeight()-1)) ;

            unsigned char* texData;
            
            switch(worldMap[worldIndexX][worldIndexY] -1){
                case 1: texData = textureData;break;
                case 2: texData = textureData2; break;
                case 4:
                default:
                case 3: texData = textureData3;break;
            }
            
           // ofColor c = texData[ yimg * 64 *3 + (ximg*3)];
            
//            ofColor c = textureData[int(yTexel/(float)lineHeight * (img[0].getHeight()-1)) + t*img[0].getWidth()];
            
            pixels[((int(startY) + yTexel) * width * 3) + (x*3)] = c.r;
            pixels[((int(startY) + yTexel) * width * 3) + (x*3) +1] = c.g;
            pixels[((int(startY) + yTexel) * width * 3) + (x*3) +2] = c.b;
//            pixels[(yTexel * width * 3) + (x*3) + 1] = c.g;
  //          pixels[(yTexel * width * 3) + (x*3) + 2] = c.b;
            
//            pixels[yTexel+ 3 + x+1] = c.g;
  //          pixels[yTexel+ (3 + x+2] = c.b;
            
            //img1.setColor(x, int(startY) + yTexel, c);
                  //  printf("%d  %d\n", x, int(startY) + yTexel);
                    
//            if(c == lastColor){
  //              lastPosition = yTexel;
    //            ofLine(x, yTexel, x, yTexel);
      //      }
        //    else{
              //  ofSetColor(c);
          //      ofRect(x, startY + yTexel, 1, 1);
            //    lastColor = c;
            //}
            
            //c = sideHit == 0 ? c/2.0f : c;
        }
      //  tex.draw(0,0);
        
        
        int i = 0;
        
        
        //while ( i < img1.getPixelsRef().size() ) {
        //img1.getPixelsRef()[i] = abs(sin( float(i) / 18.f)) * 255.f; // make some op-art
        
          //  i++;
        //}


        
        //printf("%f  -  %d\n", ofGetFrameRate(), 0);
        
        //img[worldMap[worldX][worldY] -1].drawSubsection(x, startY,   1, lineHeight,   t * 63, 0,  1, 63);
        
        // Used for solid colors
        //ofLine(x, startY, x, startY + lineHeight);
    }
   // ofSetColor(255,255,255);
    /*
    setFromPixels(...)
    
    void ofImage_::setFromPixels(const PixelType *pixels, int w, int h, ofImageType type, bool bOrderIsRGB=true)
    
    Set the pixels of the image from an array of values, for an ofFloatImage these need to be floats, for an ofImage these need to be unsigned chars. The w and h values are important so that the correct dimensions are set in the image. This assumes that you're setting the pixels from 0,0 or the upper left hand corner of the image. The bOrderIsRGB flag allows you pass in pixel data that is BGR by setting bOrderIsRGB=false.
        
        Copies in the pixel data from the 'pixels' array. Specify the corresponding width and height of the image you are passing in with 'w' and 'h'. The image type can be OF_IMAGE_GRAYSCALE, OF_IMAGE_COLOR, or OF_IMAGE_COLOR_ALPHA.
        
        Note: that your array has to be at least as big as [ width * height * bytes per pixel ].
        
        If you have a grayscale image, you will have (widthheight) number of pixels. Color images will have (widthheight3) number of pixels (interlaced R,G,B), and coloralpha images will have (widthheight*4) number of pixels (interlaced R,G,B,A).
        
        Note: You do not need to call allocate() before calling setFromPixels() as setFromPixels() re-allocates itself if needed.

            
            */
    
ofSetColor(255,255,255);
img1.setFromPixels(pixels, width, height, OF_IMAGE_COLOR);
img1.draw(0,0);
    
}

ofApp::~ofApp(){
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    keys[key] = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    keys[key] = false;
}