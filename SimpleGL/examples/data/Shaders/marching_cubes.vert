varying vec4 positionWithAO;
varying vec3 eyePos;

void main(void)
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor  = gl_Color;

	eyePos = gl_ModelViewMatrixInverse[3].xyz;

#ifdef GENERATE_VOLUME
	positionWithAO = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = gl_Vertex;
#else
	positionWithAO = gl_Vertex;
	gl_Position    = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
#endif
}
