uniform mat4      modelViewProjectionMatrix;

uniform sampler2D verticesMap;
uniform vec2      sizeFactor;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4( texture2D(verticesMap, gl_Vertex.xy * sizeFactor).xyz, 1.0 );
}
