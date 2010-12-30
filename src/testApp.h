#ifndef _TEST_APP
#define _TEST_APP

/*********************************************/
/*                                           */
/* Computer Graphics 2, Assignment 2         */
/* 'Fractal Terrain' by Franz J. Lanzendorfer*/
/* with Camera Controls by Eberhard Gräther  */
/*                                           */
/*********************************************/

#include "ofMain.h"

#include "Trackball/Vector.h"
#include "Trackball/Trackball.h"

#include "ofxShader/ofxShader.h"
#include "Texture/Texture.h"

#include "Displaylist.h"

//MAP_DIMENSION must be a multiple of 2 + 1
#define MAP_DIMENSION 1025

enum Drawtype {lines, textur};

class testApp : public ofBaseApp{

	public:

		testApp();
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void handleKey(bool pressed, int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		
		void iterativeSubDiv();
		void printMap();
		float avgHeight(float arg1, float arg2);
		float avgHeight(float arg1, float arg2,
						float arg3, float arg4);

		void renderWireFrame();
		void renderTextured();
		void updateDisplayLists(bool deleteFirst);

		TrackBall trackball;
		Vector eye, origin, up;
		
		bool iterate, iterateBack;

		float map[MAP_DIMENSION][MAP_DIMENSION];
		//mapDimension is the dimension of the map
		//within the array-borders
		float mapDimension;
		
		//current size of a square used for sub-dividing
		int squareSize;
		
		//subDivs is actually just used to know when you
		//reached zero and need to seed the 4 start-corners again
		int subDivs;

		//roughness-factor
		float randomRange;
		
		//a theoretical value to map the height-values with
		float maxHeight;

		//the factor you want your terrain to be scaled to
		float drawScale;

		float customSeaLevel;
		bool raiseSea, lowerSea;

		ofxShader shader;
		ofImage* textureImage;
		Texture texture;

		Drawtype drawType;

		Displaylist* wireframe;
		Displaylist* textured;

};

#endif
