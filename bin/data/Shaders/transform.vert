attribute vec2 vp_position;

varying vec2 fp_texcoord;

uniform mat4 modelViewProjectionMatrix;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(vp_position, 0.0, 1.0);
	fp_texcoord = vp_position;
}
