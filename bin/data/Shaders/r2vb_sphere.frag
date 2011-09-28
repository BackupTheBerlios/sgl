#define PI     	3.141592653
#define PI_2   	(2.0 * PI)
#define HALF_PI (0.5 * PI)

void main()
{
	float r = sin(gl_TexCoord[0].y * PI);

	vec3 position;
	position.x = cos(gl_TexCoord[0].x * PI_2) * r;
	position.y = cos(gl_TexCoord[0].y * PI);
	position.z = sin(gl_TexCoord[0].x * PI_2) * r;
	
	gl_FragData[0] = vec4( position, 1.0 );
	//gl_FragData[1] = vec4( normalize(position), 1.0 );
}
