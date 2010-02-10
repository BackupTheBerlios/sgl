uniform mat4 modelViewProjectionMatrix;

void main()
{
	gl_Position    = modelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_Vertex;
	gl_TexCoord[1] = gl_Color;
}
