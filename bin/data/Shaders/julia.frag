uniform float a;
uniform float b;
uniform float density;

vec4 julia(void)
{
    float x = gl_TexCoord[0].x;
    float y = gl_TexCoord[0].y;
    float x2;
    float y2;
    float xy;
    float r = 1.0;
    float k = density;

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
    
    float j = k / density;
    return 1.0 - vec4(j, j*j, j*j*j, 0.0);
}

void main(void)
{
    gl_FragColor = julia() * gl_TexCoord[1];
}
