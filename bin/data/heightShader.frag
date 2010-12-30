
// texture unit defined by the c++ program
uniform sampler2D texture;

void main(void) {

	// mandatory output of fragment shader: pixel color
	gl_FragColor = texture2D(texture, gl_TexCoord[0].s);
	gl_FragColor.a = 1.0;
}
