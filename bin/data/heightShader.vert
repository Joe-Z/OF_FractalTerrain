
void main() {

	// customized output of vertex shader:
	gl_TexCoord[0].s = gl_Vertex.z;
	gl_TexCoord[0].t = 0;
	
	// mandatory output of vertex shader: transformed and projected vertex position
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
