#pragma once

#include "ofMain.h"

#define IMG_SIZE 64

class ofApp : public ofBaseApp
{

public:
				ofApp();
	virtual		~ofApp();

	void		setup();
	void		update();
	void		draw();

	void		keyPressed(int key);
	void		keyReleased(int key);



private:
	bool*			keys;

	ofVec2f			pos;
	ofVec2f			right;
	ofVec2f			dir;
	float			FOV;
	float			rot;

	ofTexture		tex;

	unsigned char*	colorBuffer;

	ofImage*		images;

	unsigned char*	textureData;
	unsigned char*	textureData2;
	unsigned char*	textureData3;

	int				width;
	int				height;
	ofImage			img1;

	bool			useTextures;

	unsigned char*	screenBuffer;

	int worldMap[15][24] =
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,1},
		{1,0,0,0,0,5,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,5,0,5,0,5,0,5,0,5,0,5,0,5,0,5,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
};
