#ifndef OFX_SHADER_H
#define OFX_SHADER_H

#include "ofMain.h"

class ofxShader {
public:
	ofxShader();
	~ofxShader();
	bool loadShader(char const *shaderName);
	bool loadShader(char const *fragmentShaderName, char const *vertexShaderName);
	void unload();

	void bind();
	void unbind();
	void setUniformVariable1f(char const *name, float value);
	void setUniformVariable1i(char const *name, int value);
	void setUniformVariable2f(char const *name, float value, float value2);
	void setUniformVariable3f(char const *name, float value, float value2, float value3);
	void setUniformVariable1fv(char const *name, int count, float const *value);
	void setUniformVariable3fv(char const *name, int count, float const *value);
	void setUniformVariable4fv(char const *name, int count, float const *value);

private:
	char *loadSourceFile(char const *fileName);
	bool compileFile(char const *shaderName, GLhandleARB shader, char const *fileName);
	bool compile(char const *shaderName, GLhandleARB shader, char const *source);
	bool link(char const *shaderName);
	bool checkStatus(char const *shaderName, GLhandleARB shader, GLint action);

	GLhandleARB vertexShader;
	GLhandleARB fragmentShader;
	GLhandleARB shader;

	bool loaded;
};

#endif	

