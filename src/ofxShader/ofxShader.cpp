#include "ofxShader.h"

ofxShader::ofxShader() {
}

ofxShader::~ofxShader() {
	unload();	
};

bool ofxShader::loadShader(char const *vertexFileName, char const *fragmentFileName) {
	loaded = false;

	if (!GLEE_ARB_shader_objects) {
		printf("Extension \"ARB_shader_objects\" not supported.\n");
		return false;
	}

	vertexShader = (GLhandleARB)glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = (GLhandleARB)glCreateShader(GL_FRAGMENT_SHADER);
	shader = glCreateProgramObjectARB();

	if (
		compileFile(vertexFileName, vertexShader, vertexFileName) &&
		compileFile(fragmentFileName, fragmentShader, fragmentFileName) &&
		link(fragmentFileName)
	) {
		return loaded = true;
	} else {
		unload();
		return false;
	}
}
		
bool ofxShader::loadShader(char const *shaderName) {
	loaded = false;
	string vertexFileName = ofToDataPath(shaderName) + ".vert";
	string fragmentFileName = ofToDataPath(shaderName) + ".frag";
	return loadShader(vertexFileName.c_str(), fragmentFileName.c_str());
}

void ofxShader::unload() {
	if (vertexShader) {
		glDetachObjectARB(shader, vertexShader);
		glDeleteObjectARB(vertexShader);
		vertexShader = NULL;
	}

	if (fragmentShader) {
		glDetachObjectARB(shader, fragmentShader);
		glDeleteObjectARB(fragmentShader);
		fragmentShader = NULL;
	}

	if (shader) {
		glDeleteObjectARB(shader);
		shader = NULL;
	}
}

void ofxShader::bind() {
	if (loaded)
		glUseProgramObjectARB(shader);
}

void ofxShader::unbind() {
	if (loaded)
		glUseProgramObjectARB(0);
}

void ofxShader::setUniformVariable1f(char const *name, float value) {
	if (loaded)
		glUniform1fARB(glGetUniformLocationARB(shader, name), value);
}
void ofxShader::setUniformVariable1i(char const *name, int value) {
	if (loaded)
		glUniform1iARB(glGetUniformLocationARB(shader, name), value);
}

void ofxShader::setUniformVariable1fv (char const *name, int count, float const *value) {
	if (loaded)
		glUniform1fvARB(glGetUniformLocationARB(shader, name), count, value);
}

void ofxShader::setUniformVariable3fv (char const *name, int count, float const *value) {
	if (loaded)
		glUniform3fvARB(glGetUniformLocationARB(shader, name), count, value);
}

void ofxShader::setUniformVariable4fv (char const *name, int count, float const *value) {
	if (loaded)
		glUniform4fvARB(glGetUniformLocationARB(shader, name), count, value);
}

void ofxShader::setUniformVariable2f (char const *name, float value, float value2) {
	if (loaded)
		glUniform2fARB(glGetUniformLocationARB(shader, name), value, value2);
}
void ofxShader::setUniformVariable3f (char const * name, float value, float value2, float value3) {
	if (loaded)
		glUniform3fARB(glGetUniformLocationARB(shader, name), value, value2, value3);
}


char *ofxShader::loadSourceFile(char const *fileName) {
	char *source;
	unsigned long position, length, lineLength;
	FILE *file;

	file = fopen(fileName, "r");
	if (file == NULL) return NULL;

	fseek(file, 0, SEEK_END);
	length = ftell(file) + 1;
	fseek(file, 0, SEEK_SET);
	source = (GLchar *)malloc(length);
	if (source != NULL) {
		position = 0;
		while (fgets(&source[position], length - position, file)) {
			lineLength = strlen(&source[position]);
			if (lineLength == 0) break;
			position += lineLength;
		}
		source[position] = 0;
	}
	fclose(file);

	return source;
}

bool ofxShader::compileFile(char const *shaderName, GLhandleARB shader, char const *fileName) {
	char *source;
	source = loadSourceFile(fileName);
	if (source == NULL) {
		printf("%s: Failed to open source file.\n", shaderName);
		return false;
	}
	bool result = compile(shaderName, shader, source);
	free(source);
	return result;
}

bool ofxShader::compile(char const *shaderName, GLhandleARB shader, char const *source) {
	GLint length = strlen(source);
	glShaderSourceARB(shader, 1, &source, &length);
	glCompileShaderARB(shader);
	return checkStatus(shaderName, shader, GL_COMPILE_STATUS);
}

bool ofxShader::link(char const *shaderName) {
	glAttachObjectARB(shader, vertexShader);
	glAttachObjectARB(shader, fragmentShader);
	glLinkProgramARB(shader);
	return checkStatus(shaderName, shader, GL_LINK_STATUS);
}

bool ofxShader::checkStatus(char const *shaderName, GLhandleARB shader, GLint action) {
	GLchar infoLog[1024];
	GLsizei infoLogLength = 0;
	GLint status = 0;
	glGetObjectParameterivARB(shader, action, &status);

	glGetInfoLogARB(shader, sizeof(infoLog), &infoLogLength, infoLog);
	if (infoLogLength > 0) {
		infoLog[infoLogLength] = 0;
		printf("%s: %s \n", shaderName, infoLog);
	}

	return status == 1;
}