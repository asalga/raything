#include "ofApp.h"

// <3

// 1 -fix getColor perf issue (thing is way too slow)
// 2 - try reallocating img1 per frame
// 3 -
// 4 -
// 5 -


/*
 ======================================================================
 setup()
 
 load images, set initial player pos and direction
 ====================================================================== */
void ofApp::setup()
{
	width = ofGetWidth();
	height = ofGetHeight();
	
	pos.set(2, 2);
	FOV = 60;
	rot = PI;

	useTextures = true;

	keys = new bool[512];
	for(int i = 0; i < 512; i++)
	{
		keys[i] = false;
	}

	tex = ofTexture();
	tex.allocate(width, height, GL_RGB8);
	screenBuffer = new unsigned char[width * height * 8];

	colorBuffer = new unsigned char[width * height * 3];

	
	
	images = new ofImage[5];
	images[0].loadImage("bricks.gif");
	images[1].loadImage("bricks1.png");
	images[2].loadImage("wall_plain.gif");
	images[3].loadImage("bricks1.gif");
	images[4].loadImage("bricks.gif");

	textureData =  new unsigned char[64 * 64 * 3];
	textureData2 = new unsigned char[64 * 64 * 3];
	textureData3 = new unsigned char[64 * 64 * 3];

	for(int c = 0; c < 64 * 64 * 3; c++)
	{
		textureData[c] = images[0].getPixels()[c];
		textureData2[c] = images[1].getPixels()[c];
		textureData3[c] = images[2].getPixels()[c];
	}
}

ofApp::ofApp(){
	
}

//--------------------------------------------------------------
void ofApp::update()
{

	float speed = 0.0f;

	if(keys[OF_KEY_UP] == true)
	{
		speed = 0.1f;
	}
	if(keys[OF_KEY_DOWN] == true)
	{
		speed = -0.1f;
	}

	if(keys[OF_KEY_RIGHT] == true)
	{
		rot += 0.051f;
	}
	if(keys[OF_KEY_LEFT] == true)
	{
		rot -= 0.051f;
	}

	pos = pos + dir * speed;

	dir.x = cos(rot);
	dir.y = sin(rot);

	right.x = -dir.y;
	right.y = dir.x;
}

//--------------------------------------------------------------
void ofApp::draw()
{
	printf("fps %f\n", ofGetFrameRate());

	
	
	
	// Blue sky, unused right now with our colorBuffer
	//ofSetColor(64, 128, 255);
	//ofRect(0, 0, width, height/2);

	// Green floor, unused right now
	//ofSetColor(0, 255, 0);
	//ofRect(0, height/2, width, height);

	// TODO: add comment
	float camPlaneMag = tan( FOV / 2.0f * (PI / 180.0) );

	//
	ofVec2f rayDir;

	// Clear background with black
	memset(colorBuffer, 0, width * height * 3);

	unsigned long long timing = ofGetElapsedTimeMillis();

	// A ray is cast for each 'column' of the screen
	for(int x = 0; x < width; x++)
	{
		// Map screen coordinates [0 to width-1] to [-1 to 1]
		float currCamScale = (2.0f * x / float(width)) - 1.0f;

		rayDir.set(dir.x + (right.x * camPlaneMag) * currCamScale,
							 dir.y + (right.y * camPlaneMag) * currCamScale);


		/*
		 What we are trying figure out is for each ray, when does it hit an edge? to answer that question we'll need to figure out some things:

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
		if(rayDir.x > 0)
		{
			magToXedge = (worldIndexX + 1.0 - pos.x) * magBetweenXEdges;
			dirStepX = 1;
		}
		else
		{
			magToXedge = (pos.x - worldIndexX) * magBetweenXEdges;
			dirStepX = -1;
		}

		if(rayDir.y > 0)
		{
			magToYedge = (worldIndexY + 1.0 - pos.y) * magBetweenYEdges;
			dirStepY = 1;
		}
		else
		{
			magToYedge = (pos.y - worldIndexY) * magBetweenYEdges;
			dirStepY = -1;
		}

		int sideHit;

		do
		{
			if(magToXedge < magToYedge)
			{
				magToXedge += magBetweenXEdges;
				worldIndexX += dirStepX;
				sideHit = 0;
			}
			else
			{
				magToYedge += magBetweenYEdges;
				worldIndexY += dirStepY;
				sideHit = 1;
			}
		}
		while(worldMap[worldIndexX][worldIndexY] == 0);

		ofColor wallColor;
		switch(worldMap[worldIndexX][worldIndexY])
		{
		case 1:
			wallColor = ofColor(255,255,255);
			break;
		case 2:
			wallColor = ofColor(0,255,255);
			break;
		case 3:
			wallColor = ofColor(0,0,255);
			break;
		case 4:
			wallColor = ofColor(255,255,255);
			break;
		case 5:
			wallColor = ofColor(255,0,255);
			break;
		}

		// Basic lighting - Need this if using textures?
		// wallColor = sideHit == 0 ? wallColor : wallColor/2.0f;
		// ofSetColor(wallColor);

		float wallDist;

		// Distortion Correction
		if(sideHit == 0)
		{
			wallDist = fabs((worldIndexX - pos.x + (1.0 - dirStepX) / 2.0) / rayDir.x);
		}
		else
		{
			wallDist = fabs((worldIndexY - pos.y + (1.0 - dirStepY) / 2.0) / rayDir.y);
		}

		float t;
		if(sideHit == 0)
		{
			//float fullAmt = pos.x + worldX;
			//float texCoordx = fullAmt - (pos.x + worldX);

			//length of vector from pos/ direction

			ofVec2f originToX = pos + (rayDir * magToXedge);// - rayDir;
			float texCoordX = originToX.y - int(originToX.y);

			t = pos.y + ((worldIndexX - pos.x + (1.0f - dirStepX) / 2.0) / rayDir.x) * rayDir.y;
			t = t - int(t);
		}
		else
		{
			//float fullAmt = pos.x + worldX;
			//float texCoordx = fullAmt - (pos.x + worldX);

			//length of vector from pos/ direction

			// ofVec2f originToX = pos + (rayDir * magToXedge);// - rayDir;
			// float texCoordX = originToX.y - int(originToX.y);

			t = pos.x + ((worldIndexY - pos.y + (1.0f - dirStepY) / 2.0) / rayDir.y) * rayDir.x;
			t = t - int(t);

			// col = img.getColor(t * img.getWidth(), 0);
		}

		// The farther away the wall sliver is, the smaller it will be faking foreshortening.
		// If the ray was 1 unit from the wall, the line would fill the entire height length of the screen.
		float lineHeight = height/wallDist;

		float lineHeightClamped = lineHeight;
		lineHeightClamped = CLAMP(lineHeightClamped, 0, height);

		// TODO: comment
		float yOffset = (lineHeight - height)/2.0;
		yOffset = CLAMP(yOffset, 0, yOffset);

		// Must center the slivers for proper FPS feel. So find
		// out where to start drawing in the colorBuffer.
		float startY = height/2.0 - lineHeight/2.0;
		startY = CLAMP(startY, 0, startY);

		// Max we can ever go is from 0 to screenHeight
		for(int yTexel = 0; yTexel < lineHeightClamped; yTexel++)
		{
			ofColor c = images[worldMap[worldIndexX][worldIndexY] -1].
									getColor(t * 64, int((yTexel+yOffset)/lineHeight * (64-1)) );

			int ximg = t * 64;
			int yimg = int(yTexel/(float)lineHeight * (64-1)) ;

			colorBuffer[((int(startY) + yTexel) * width * 3) + (x*3)] = c.r;
			colorBuffer[((int(startY) + yTexel) * width * 3) + (x*3) +1] = c.g;
			colorBuffer[((int(startY) + yTexel) * width * 3) + (x*3) +2] = c.b;
		}
	}

	ofSetColor(255,255,255);
	img1.setFromPixels(colorBuffer, width, height, OF_IMAGE_COLOR);
	img1.draw(0, 0);
}

ofApp::~ofApp()
{
	delete [] images;
	delete [] keys;
	delete screenBuffer;
	delete [] colorBuffer;

	delete textureData;
	delete textureData2;
	delete textureData3;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	keys[key] = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
	keys[key] = false;
}