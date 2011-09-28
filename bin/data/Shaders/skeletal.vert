//
// GLSL vertex shader for skeletal animation
//

#define N     100
#define EPS   0.001

// attributes
attribute vec4  weights;
attribute vec4  bone0;
attribute vec4  bone1;
attribute vec4  bone2;


// uniforms
uniform mat4 modelViewProjectionMatrix;

uniform vec4 boneQuat[N];
uniform vec3 bonePos[N];

//
// Quaternion multiplication
//
vec4 quatMul(in vec4 q1, in vec4 q2)
{
    vec3 v1 = q1.xyz;
    vec3 v2 = q2.xyz;
    return vec4( cross(v1, v2) + q1.w * v2 + q2.w * v1, q1.w * q2.w - dot(v1, v2) );
}

//
// vector rotation via quaternion
//
vec4 quatRotate(in vec3 p, in vec4 q)
{
    vec4 temp = quatMul( q, vec4(p, 0.0) );
    return quatMul( temp, vec4(-q.x, -q.y, -q.z, q.w) );
}

vec3 boneTransf(int index, vec3 pos)
{
    return bonePos[index].xyz + quatRotate(pos, boneQuat[index]).xyz;
}

void main()
{
    vec3 pos = vec3(0.0);
    int  index;

    if (weights.x > EPS)
    {
        index = int(bone0.w);
        pos  += weights.x * boneTransf(index, bone0.xyz);
    }

    if (weights.y > EPS)
    {
        index = int(bone1.w);
        pos  += weights.y * boneTransf(index, bone1.xyz);
    }

    if (weights.z > EPS)
    {
        index = int(bone2.w);
        pos  += weights.z * boneTransf(index, bone2.xyz);
    }
/*
    if (weights.w > EPS)
    {                                          
        index = int(bone3.w);
        pos  += weights.w * boneTransf(index, bone3.xyz);
    }
*/

    gl_Position = modelViewProjectionMatrix * vec4(pos, 1.0);
}
