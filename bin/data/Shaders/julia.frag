precision mediump float;

uniform float 	  a;
uniform float     b;
uniform float 	  density;
uniform lowp vec4 color;

varying vec2 fp_texcoord;

vec4 julia(void)
{
    float x = fp_texcoord.x;
    float y = fp_texcoord.y;
    float x2;
    float y2;
    float xy;
    float r = 1.0;
#ifdef GL_ES
    float k = 16.0;
#else
	float k = density;
#endif

    while( (k > 0.0) && (r < 4.0) )
    {
        x2 = x * x;
        y2 = y * y;
        xy = x * y;
        x = x2 - y2 + a;
        y = 2.0 * xy + b;
        r = x2 + y2;
        k--;
    }
    
#ifdef GL_ES
    float j = k / 16.0;
#else
    float j = k / density;
#endif
    return 1.0 - vec4(j, j*j, j*j*j, 0.0);
}

void main(void)
{
    gl_FragColor = julia() * color;
}
