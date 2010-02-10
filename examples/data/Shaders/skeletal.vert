//
// GLSL vertex shader for skeletal animation
//

#define N     100
#define EPS   0.001

uniform mat4 modelViewProjectionMatrix;

uniform vec4 boneQuat [N];
uniform vec3 bonePos  [N];

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

void	main ()
{
    vec3    weights = gl_Vertex.xyz;            // weights for 3 bones
    vec3    pos     = vec3 ( 0.0 );
    int     index;

    if ( weights.x > EPS )                      // process 1st bone
    {                                           // get 1st bone index
        index = int ( gl_MultiTexCoord3.w );
        pos  += weights.x * boneTransf(index, gl_MultiTexCoord3.xyz);
    }

    if ( weights.y > EPS )                      // process 2nd bone
    {                                           // get 2nd bone index
        index = int ( gl_MultiTexCoord4.w );
        pos  += weights.y * boneTransf ( index, gl_MultiTexCoord4.xyz );
    }

    if ( weights.z > EPS )                      // process 3rd bone
    {                                           // get 3rd bone index
        index = int ( gl_MultiTexCoord5.w );
        pos  += weights.z * boneTransf ( index, gl_MultiTexCoord5.xyz );
    }
/*
    if ( weights.w > EPS )                      // process 4th bone
    {                                           // get 4th bone index
        index = int ( gl_MultiTexCoord7.w );
        pos  += weights.w * boneTransf ( index, gl_MultiTexCoord7.xyz );
    }
*/

    gl_Position     = modelViewProjectionMatrix * vec4 ( pos, 1.0 );
    gl_TexCoord [0] = gl_MultiTexCoord0;
}
