#include "Texture.h"

#include "ofMain.h"

Texture::Texture() {
	glGenTextures(1, &textureId);
}
Texture::~Texture() {
	glDeleteTextures(1, &textureId);
}

void Texture::load(ofImage *image, int wrapS, int wrapT) {
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	GLint glType;
	switch (image->type) {
		case OF_IMAGE_GRAYSCALE:
			glType = GL_LUMINANCE;
			break;
		case OF_IMAGE_COLOR_ALPHA:
			glType = GL_RGBA;
			break;
		default:
		case OF_IMAGE_COLOR:
			glType = GL_RGB;
			break;
	}

	unsigned char *pixels = image->getPixels();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild2DMipmaps(
		GL_TEXTURE_2D,
		glType,
		image->getWidth(),
		image->getHeight(),
		glType,
		GL_UNSIGNED_BYTE,
		pixels
	);
	unbind();
}

void Texture::bind() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::unbind() {
	glDisable(GL_TEXTURE_2D);
}

int Texture::getId() {
	return textureId;
}
