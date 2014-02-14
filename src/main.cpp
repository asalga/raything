#include "ofMain.h"
#include "ofApp.h"


int main()
{
	ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);

	ofSetupOpenGL(640, 480, OF_WINDOW);
	ofRunApp(new ofApp());
}