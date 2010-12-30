/*********************************************/
/*                                           */
/* Computer Graphics 2, Assignment 2         */
/* 'Fractal Terrain' by Franz J. Lanzendorfer*/
/* with Camera Controls by Eberhard Gräther  */
/*                                           */
/*********************************************/

#include "testApp.h"
#include "stdio.h"

//--------------------------------------------------------------
testApp::testApp():
	textureImage(new ofImage()) {
}

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofDisableSetupScreen();
    ofEnableAlphaBlending();

	//here you can set your desired camer-position
	trackball.setEye(2.0, 2.0, 2.5);
	trackball.setUp(0.0, 0.0, 1.0);
	
	trackball.setWindow(ofGetWidth(), ofGetHeight());

	mapDimension = MAP_DIMENSION;
	squareSize = MAP_DIMENSION - 1;
	subDivs = 0;

	//roughness-factor
	randomRange = 10;

	//normally maxHeight would be two times the
	//highest possible difference but too make
	//it look a bit more mountain-like i reduce it a bit
	maxHeight = randomRange * 2 - randomRange / 4;

	drawScale = 2;

	customSeaLevel = 0;
	raiseSea = lowerSea = false;

	//initially set all height values to maxHeight / 2
	for(int row = 0; row < mapDimension; row++) {
		for(int col = 0; col < mapDimension; col++) {
			map[row][col] = maxHeight / 2;
		}
	}

	shader.loadShader("heightShader");

	//setup gradient texture
	if(textureImage->loadImage("gradient.png"))
		texture.load(textureImage, GL_CLAMP_TO_EDGE, GL_CLAMP);
	else
		cout << "Couldn't load image!" << endl;

	iterate = false;
	iterateBack = false;

	drawType = textur;

	//setup a little terrain for beginning:
	//get log2 of MAP_DIMENSION - 1
	int maxSubDivs = log(float(MAP_DIMENSION - 1)) /
					 log(2.0f);
	for(int i = 0; i < maxSubDivs / 2; i++) {
		iterativeSubDiv();
		cout << "subDivs: " << subDivs << " " <<
			"randomRange: " << randomRange << endl;

	}

	//setup the displaylist for the first time
	wireframe = new Displaylist();
	textured = new Displaylist();

	updateDisplayLists(false);
}

//--------------------------------------------------------------
void testApp::update(){

	eye = trackball.getEye();
	origin = trackball.getOrigin();
	up = trackball.getUp();

	if(raiseSea)
		customSeaLevel += 0.002;
	if(lowerSea)
		customSeaLevel -= 0.002;

	if(iterate) {
		iterativeSubDiv();

		cout << "subDivs: " << subDivs << " " <<
			"randomRange: " << randomRange << endl;

		iterate = false;
		iterateBack = false;

		updateDisplayLists(true);
	}
	else if(iterateBack && subDivs > 0) {
		squareSize *= 2;
		randomRange *= 2;
		subDivs--;

		cout << "subDivs: " << subDivs << " " <<
			"randomRange: " << randomRange << endl;

		iterateBack = false;

		updateDisplayLists(true);
	}
}

void testApp::iterativeSubDiv() {
	if(subDivs == 0) {
		//seed the first 4 corners
		//but not with the normal starting randomrange
		map[0][0] = maxHeight / 2 + ofRandomf();
		map[0][squareSize] = maxHeight / 2 + ofRandomf();
		map[squareSize][0] = maxHeight / 2 + ofRandomf();
		map[squareSize][squareSize] = maxHeight / 2 + ofRandomf();
	}
	if(squareSize > 1) {
		for(int y = 0; y + 1 < mapDimension; y += squareSize) {
			for(int x = 0; x + 1 < mapDimension; x += squareSize) {
				//diamond step
					//top-middle
					map[y][x + squareSize / 2] = avgHeight(map[y][x], map[y][x + squareSize]);
					map[y][x + squareSize / 2] += ofRandomf() * (randomRange / 2);
					//middle-left
					map[y + squareSize / 2][x] = avgHeight(map[y][x], map[y + squareSize][x]);
					map[y + squareSize / 2][x] += ofRandomf() * (randomRange / 2);
					//middle-right
					map[y + squareSize / 2][x + squareSize] = avgHeight(map[y][x + squareSize], map[y + squareSize][x + squareSize]);
					map[y + squareSize / 2][x + squareSize] += ofRandomf() * (randomRange / 2);;
					//bottom-middle
					map[y + squareSize][x + squareSize / 2] = avgHeight(map[y + squareSize][x], map[y + squareSize][x + squareSize]);
					map[y + squareSize][x + squareSize / 2] += ofRandomf() * (randomRange / 2);

				//average mid step + random
					float avg = avgHeight(map[y][x + squareSize / 2],
										  map[y + squareSize / 2][x],
										  map[y + squareSize / 2][x + squareSize],
										  map[y + squareSize][x + squareSize / 2]);

					map[y + squareSize / 2][x + squareSize / 2] = avg;
					map[y + squareSize / 2][x + squareSize / 2] += ofRandomf() * randomRange;
			}
		}
		subDivs++;
		squareSize /= 2;
		randomRange /= 2;
	}
}

float testApp::avgHeight(float arg1, float arg2) {
	return (arg1 + arg2) / 2;
}

float testApp::avgHeight(float arg1, float arg2,
						 float arg3, float arg4) {
	return (arg1 + arg2 + arg3 + arg4) / 4;
}

void testApp::printMap() {
	for(int row = 0; row < mapDimension; row++) {
		for(int col = 0; col < mapDimension; col++) {
			cout << "[" << row << "][" << col << "]: " << map[row][col] << " | ";
		}
		cout << endl;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	// set viewport
	int width, height;
	width = ofGetWidth();	height = ofGetHeight();
	glViewport(0, 0, width, height);

	// clear viewport
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// set perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)width / height, 0.1, 1000.0);

	// set camera position
	float t = ofGetElapsedTimef();
	gluLookAt(
		eye.x, eye.y, eye.z,
		origin.x, origin.y, origin.z,
		up.x, up.y, up.z
	);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();

	glScalef(drawScale, drawScale, drawScale);

	switch(drawType) {
		case lines:
			wireframe->call();
			break;
		case textur:
			textured->call();
			break;
	}

	//water-surface:
	glPushMatrix();
	glTranslatef(-1, -1, 0);

	float seaLevel = customSeaLevel +
					 (((maxHeight / 2) / maxHeight) / 2);
	
	float stepWidth = 0.02;

	float amplitude1 = 0.008;
	float frequency1 = 1.0;

	float amplitude2 = 0.009;
	float frequency2 = 32.0;

	//basically there's one big and slow wave (cos())
	//and then there are plenty of little waves (sin())
	//with all the parameters it got pretty big, but
	//I tried to format it as good as I could
	for(float x = 0; x < 2; x += stepWidth) {
		for(float y = 0; y < 2; y += stepWidth) {
			glBegin(GL_QUADS);
				glColor4f(0.125, 0.0, 1.0, .65);

				glVertex3f(x, y,
						   seaLevel +
						   cos(t +
							   x * frequency1 +
							   y * frequency1)
						   * amplitude1 +
						    sin(t +
								y * frequency2)
							* amplitude2);

				glVertex3f(x + stepWidth, y,
						   seaLevel +
						   cos(t +
							   (x * frequency1 +
							   stepWidth * frequency1) +
							   y * frequency1)
						   * amplitude1 +
						   sin(t +
							    y * frequency2)
						   * amplitude2);

				glVertex3f(x + stepWidth, y + stepWidth,
						   seaLevel +
						   cos(t +
							   (x * frequency1 +
							   stepWidth * frequency1) +
							   (y * frequency1 +
							   stepWidth * frequency1))
						   * amplitude1 +
						   sin(t +
							   (y * frequency2 +
							   stepWidth * frequency2))
							* amplitude2);

				glVertex3f(x, y + stepWidth,
						   seaLevel +
						   cos(t +
							   x * frequency1 +
							   (y * frequency1 +
							   stepWidth * frequency1))
							* amplitude1 +
						   sin(t +
							   (y * frequency2 +
							   stepWidth * frequency2))
						   * amplitude2);

			glEnd();
		}
	}
	glPopMatrix();
	glPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	switch(key) {
		case 'i':
			iterate = true;
			break;
		case 'b':
			iterateBack = true;
			break;
		case 'd':
			if(drawType == textur)
				drawType = lines;
			else
				drawType = textur;
			break;
		case 'p':
			printMap();
			break;
		case 'w':
			randomRange *= 2;
			cout << "randomRange: " << randomRange << endl;
			break;
		case 's':
			randomRange /= 2;
			cout << "randomRange: " << randomRange << endl;

			break;
	}
	handleKey(true, key);
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	handleKey(false, key);
}

//--------------------------------------------------------------
void testApp::handleKey(bool pressed, int key) {
	if(key == OF_KEY_UP)
		raiseSea = pressed;
	if(key == OF_KEY_DOWN)
		lowerSea = pressed;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}


//camera-controls:
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	if (button == GLUT_LEFT_BUTTON)
		trackball.mousePressed(x, y, TrackBall::LEFT_BUTTON);
	else if (button == GLUT_MIDDLE_BUTTON)
		trackball.mousePressed(x, y, TrackBall::MIDDLE_BUTTON);
	else if (button == GLUT_RIGHT_BUTTON)
		trackball.mousePressed(x, y, TrackBall::RIGHT_BUTTON);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	if (button == GLUT_LEFT_BUTTON)
		trackball.mouseDragged(x, y, TrackBall::LEFT_BUTTON);
	else if (button == GLUT_MIDDLE_BUTTON)
		trackball.mouseDragged(x, y, TrackBall::MIDDLE_BUTTON);
	else if (button == GLUT_RIGHT_BUTTON)
		trackball.mouseDragged(x, y, TrackBall::RIGHT_BUTTON);
}
//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//========================================================================
void testApp::windowResized(int w, int h) {
	trackball.setWindow(w, h);
}

void testApp::renderWireFrame() {
	for(int row = 0; row < mapDimension; row += squareSize) {
		int col = 0;
		while(col < mapDimension) {
			//map the x- and y-values between -1 and +1
			float currentY = ((float(row) / mapDimension) - 0.5) * 2;
			float currentX = ((float(col) / mapDimension) - 0.5) * 2;

			float nextRow = ((float(row + squareSize) / mapDimension) - 0.5) * 2;
			float nextCol = ((float(col + squareSize) / mapDimension) - 0.5) * 2;

			float currentHeight = map[row][col] / maxHeight;
	
			glColor3f(1.0, 0., 0.);
			glEnable(GL_LINE_SMOOTH);
			//downward line
			if(row + squareSize < mapDimension) {
				float nextRowHeight = map[row + squareSize][col] / maxHeight;

				glBegin(GL_LINES);
				glVertex3f(currentX, currentY, currentHeight);
				glVertex3f(currentX, nextRow, nextRowHeight);
				glEnd();
			}

			//sideward line
			if(col + squareSize < mapDimension) {
				float nextColHeight = map[row][col + squareSize] / maxHeight;

				glBegin(GL_LINES);
				glVertex3f(currentX, currentY, currentHeight);
				glVertex3f(nextCol, currentY, nextColHeight);
				glEnd();
			}

			col += squareSize;
		}
	}
}

void testApp::renderTextured() {
	for(int row = 0; row < mapDimension; row += squareSize) {
		int col = 0;
		while(col < mapDimension) {
			//map the x- and y-values between -1 and +1
			float currentY = ((float(row) / mapDimension) - 0.5) * 2;
			float currentX = ((float(col) / mapDimension) - 0.5) * 2;

			float nextRow = ((float(row + squareSize) / mapDimension) - 0.5) * 2;
			float nextCol = ((float(col + squareSize) / mapDimension) - 0.5) * 2;

			float currentHeight = map[row][col] / maxHeight;

			//bind shader
			shader.bind();
			//bind texture
			glActiveTexture(GL_TEXTURE0);
			texture.bind();
			shader.setUniformVariable1i("texture", 0);

			if(row + squareSize < mapDimension &&
				col + squareSize < mapDimension) {
				float nextRowHeight = map[row + squareSize][col] / maxHeight;
				float nextColHeight = map[row][col + squareSize] / maxHeight;
				float diagonalHeight = map[row + squareSize][col + squareSize] / maxHeight;
				
				glBegin(GL_QUADS);
				glVertex3f(currentX, currentY, currentHeight);
				glVertex3f(currentX, nextRow, nextRowHeight);
				glVertex3f(nextCol, nextRow, diagonalHeight);
				glVertex3f(nextCol, currentY, nextColHeight);
				glEnd();
			}

				//unbind texture
				glActiveTexture(GL_TEXTURE0);
				texture.unbind();

				//unbind shader
				shader.unbind();

			col += squareSize;
		}
	}
}

void testApp::updateDisplayLists(bool deleteFirst) {
	if(deleteFirst) {
		wireframe->erase();
		textured->erase();
	}
	
	//setup the displaylist for the wireframe
	wireframe->beginCompile();
		renderWireFrame();
	wireframe->endCompile();

	//setup the displaylist for textured drawing
	textured->beginCompile();
		renderTextured();
	textured->endCompile();
}