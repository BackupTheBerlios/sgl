uniform float density;
uniform float a;
uniform float b;
uniform float c;

// consts
const float PI_2   =  6.2831853;
/*
const float startX = -2.336723163;
const float startY =  3.410099999;
const float sizeX  =  3.522937852;
const float sizeY  =  4.589900001;
*/

const float startX = -7.336723163;
const float startY =  3.410099999;
const float sizeX  =  0.1;
const float sizeY  =  0.1;

vec2 div_c(vec2 a, vec2 b)
{
    return vec2( dot(a,b), a.y*b.x - a.x*b.y ) / dot(b,b);
}

vec2 exp_c(vec2 z)
{
    return exp(z.x) * vec2( cos(z.y), sin(z.y) );
}

vec2 sin_c(vec2 z)
{
    vec2 iz = vec2(-z.y, z.x);
    return ( exp_c(iz) - exp_c(-iz) ) / 2.0;
}

vec2 cos_c(vec2 z)
{
    vec2 iz = vec2(-z.y, z.x);
    return ( exp_c(iz) + exp_c(-iz) ) / 2.0;
}

vec2 cot_c(vec2 z)
{
    vec2 iz = vec2(-z.y, z.x);
    vec2 ez0 = exp_c(iz);
    vec2 ez1 = exp_c(-iz);
    return div_c(ez0 + ez1, ez0 - ez1);
}

vec2 pow_c(vec2 z, float n)
{
    float angle = atan(z.y, z.x);
    if (angle < 0.0) {
        angle += 2.0 * PI_2;
    }
    float arg = angle * n;
    return pow(length(z), n) * vec2(cos(arg), sin(arg));
}

// map from [-1, 1]x[-1, 1] to requred rectangle
vec2 map(vec2 z)
{
    return vec2( startX + (z.x + 1.0) * 0.5 * sizeX,
                 startY + (z.y + 1.0) * 0.5 * sizeY );
}

vec4 tailor()
{
    vec3 maximum    = vec3(0.0);
    vec3 average    = vec3(0.0);
    vec3 total      = vec3(0.0);

    vec2  zSum        = vec2(0.0);
    float denominator = 1.0;
    vec2  current     = map(gl_TexCoord[0].xy);
    vec2  z           = exp_c(current);

    float count = 0.0;
    const float tolerance = 1e-6;
    while (count < density && length(zSum - z) > tolerance)
    {
        zSum += exp_c(z) * pow_c(current - z, count) / denominator;

        denominator *= count + 1.0;
        float value = length( cot_c(sin_c(zSum + a) + b) + c );
        vec3  vec = vec3( value * 0.1,
                          value * 0.2,
                          value * 0.4 );
/*
        vec2  value = cot_c(zSum);
        vec3  vec = vec3( length( pow_c(value, a) ),
                          length( pow_c(value, b) ),
                          length( pow_c(value, c) ) );
*/
        average = (vec + average * count) / (count + 1.0);
        vec3 derivative = abs(vec - average);
        maximum = max(maximum, derivative);

        total += derivative;

        ++count;
    }

    if (count > 0.0) {
        return vec4(total/count/maximum, 1.0);
        //return vec4( vec3(length(cot_c(current))), 1.0 );
    }

    return vec4(0.0, 0.0, 0.0, 1.0);
}

void main()
{
    gl_FragColor = tailor() * gl_TexCoord[1];
}
