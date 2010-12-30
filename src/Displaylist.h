#ifndef DISPLAY_LIST_H
#define DISPLAY_LIST_H

#include "ofMain.h"


//glGenLists(1);
//glNewList(GL_COMPILE);
//
//glEndList();
//glDeleteLists();

class Displaylist {
public:
	Displaylist() {
		list = glGenLists(1);
	}
	~Displaylist() {
		glDeleteLists(list, 1);
	}
	
	void beginCompile() {
		glNewList(list, GL_COMPILE);
	}

	void endCompile() {
		glEndList();
	}

	void call() {
		glCallList(list);
	}

	void erase() {
		glDeleteLists(list, 1);
	}

private:
	GLuint list;
};


#endif //DISPLAY_LIST_H