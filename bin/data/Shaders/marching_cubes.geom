//GLSL version 1.20
#extension GL_EXT_geometry_shader4 : require
#extension GL_EXT_gpu_shader4 : require

//Volume data field texture
uniform sampler3D dataFieldTex;

// Constant tables
uniform isampler1D edgeTableTex;
uniform isampler2D triTableTex;

uniform vec3 dataStep;

// Global iso level
uniform float isolevel;
// Random directions
uniform vec3 rayDir[24];
// Marching cubes vertices decal
uniform vec3 vertDecals[8];

//Vertices position for fragment shader
varying vec3 eyePos;
varying vec4 positionWithAO; // w stores ambient occlusion

//Get vertex i position within current marching cube
vec3 cubePos(int i){
	return gl_PositionIn[0].xyz + vertDecals[i];
}

//Get vertex i value within current marching cube
float cubeVal(int i, vec2 mipmapVal) {
	//return sampleFunction(cubePos(i)).a;
	#ifdef USE_MIPMAP_CORRECTION
	return mix( texture3DLod( dataFieldTex, 0.5 * (cubePos(i) + 1.0), mipmapVal.x ).a,
				texture3DLod( dataFieldTex, 0.5 * (cubePos(i) + 1.0), mipmapVal.x - 1.0 ).a,
				mipmapVal.y );
	#else
	return texture3D( dataFieldTex, 0.5 * (cubePos(i) + 1.0) ).a;
	#endif
}

//Compute interpolated vertex along an edge
vec3 vertexInterp(float isolevel, vec3 v0, float l0, vec3 v1, float l1) {
	return mix( v0, v1, (isolevel - l0) / (l1 - l0) );
}

vec2 computeMipmap(vec3 texStep, vec3 voxelStep)
{
	float averageTexStep   = 2.0 * (dataStep.x + dataStep.y + dataStep.z) / 3.0;
	float averageVoxelStep = (voxelStep.x + voxelStep.y + voxelStep.z) / 3.0;
	float mipmap = 1.0;

	while ( averageTexStep < averageVoxelStep )
	{
		averageTexStep *= 2.0;
		mipmap += 1.0;
	}

	return vec2(mipmap, (averageVoxelStep - 0.5 * averageTexStep) / (averageTexStep - averageVoxelStep) );
}

float computeAmbientOcclusion(vec3 pos)
{
	const float numRays = 24.0;
	const float numShortRangeSteps = 8.0;
	const float numLongRangeSteps  = 4.0;

	float visibility = 0.0;
	for (int i = 0; i<numRays; ++i)
	{
		float rayVisibility = 1.0;

		// short range
		vec3 step = 0.15 * rayDir[i] / numShortRangeSteps;
		vec3 rayPos = pos + step;
		for (float i = 0.0; i < numShortRangeSteps; i += 1.0, rayPos += step)
		{
			float density = texture3D(dataFieldTex, rayPos * 0.5 + 0.5).a + 0.01;
	     	rayVisibility *= clamp( isolevel / density, 0.0, 1.0);//clamp( (isolevel - density) * 10000.0, 0.5, 1.0);
		}

		// long range
		step = 0.4 * rayDir[i] / numLongRangeSteps;
		rayPos = pos + step;
		for (float i = 0.0; i < numLongRangeSteps; i += 1.0, rayPos += step)
		{
			float density = texture3D(dataFieldTex, rayPos * 0.5 + 0.5).a + 0.01;
	     	rayVisibility *= clamp( isolevel / density, 0.0, 1.0);//clamp( (isolevel - density) * 10000.0, 0.5, 1.0);
		}

		visibility += rayVisibility;
	}

	return visibility / numRays;
}

void main(void)
{
	//Determine the index into the edge table which
	//tells us which vertices are inside of the surface
	vec2 mipmap = computeMipmap(dataStep, vertDecals[6]);
	int cubeindex = 0;
	if (cubeVal(0, mipmap) < isolevel) cubeindex = cubeindex | 1;
	if (cubeVal(1, mipmap) < isolevel) cubeindex = cubeindex | 2;
	if (cubeVal(2, mipmap) < isolevel) cubeindex = cubeindex | 4;
	if (cubeVal(3, mipmap) < isolevel) cubeindex = cubeindex | 8;
	if (cubeVal(4, mipmap) < isolevel) cubeindex = cubeindex | 16;
	if (cubeVal(5, mipmap) < isolevel) cubeindex = cubeindex | 32;
	if (cubeVal(6, mipmap) < isolevel) cubeindex = cubeindex | 64;
	if (cubeVal(7, mipmap) < isolevel) cubeindex = cubeindex | 128;

	//Cube is entirely in/out of the surface
	int edgeTableValue = texelFetch(edgeTableTex, cubeindex, 0).a;
	if (edgeTableValue == 0)
		return;
		
	vec3 vertlist[12];

	//Find the vertices where the surface intersects the cube
	if ( (edgeTableValue & 1) != 0 )
	  vertlist[0] = vertexInterp( isolevel, cubePos(0), cubeVal(0, mipmap), cubePos(1), cubeVal(1, mipmap) );
	if ( (edgeTableValue & 2) != 0 )
	  vertlist[1] = vertexInterp( isolevel, cubePos(1), cubeVal(1, mipmap), cubePos(2), cubeVal(2, mipmap) );
	if ( (edgeTableValue & 4) != 0 )
	  vertlist[2] = vertexInterp( isolevel, cubePos(2), cubeVal(2, mipmap), cubePos(3), cubeVal(3, mipmap) );
	if ( (edgeTableValue & 8) != 0 )
	  vertlist[3] = vertexInterp( isolevel, cubePos(3), cubeVal(3, mipmap), cubePos(0), cubeVal(0, mipmap) );
	if ( (edgeTableValue & 16) != 0 )
	  vertlist[4] = vertexInterp( isolevel, cubePos(4), cubeVal(4, mipmap), cubePos(5), cubeVal(5, mipmap) );
	if ( (edgeTableValue & 32) != 0 )
	  vertlist[5] = vertexInterp( isolevel, cubePos(5), cubeVal(5, mipmap), cubePos(6), cubeVal(6, mipmap) );
	if ( (edgeTableValue & 64) != 0 )
	  vertlist[6] = vertexInterp( isolevel, cubePos(6), cubeVal(6, mipmap), cubePos(7), cubeVal(7, mipmap) );
	if ( (edgeTableValue & 128) != 0 )
	  vertlist[7] = vertexInterp( isolevel, cubePos(7), cubeVal(7, mipmap), cubePos(4), cubeVal(4, mipmap) );
	if ( (edgeTableValue & 256) != 0 )
	  vertlist[8] = vertexInterp( isolevel, cubePos(0), cubeVal(0, mipmap), cubePos(4), cubeVal(4, mipmap) );
	if ( (edgeTableValue & 512) != 0 )
	  vertlist[9] = vertexInterp( isolevel, cubePos(1), cubeVal(1, mipmap), cubePos(5), cubeVal(5, mipmap) );
	if ( (edgeTableValue & 1024) != 0 )
	  vertlist[10] = vertexInterp( isolevel, cubePos(2), cubeVal(2, mipmap), cubePos(6), cubeVal(6, mipmap) );
	if ( (edgeTableValue & 2048) != 0 )
	  vertlist[11] = vertexInterp( isolevel, cubePos(3), cubeVal(3, mipmap), cubePos(7), cubeVal(7, mipmap) );

	// Create the triangle
	gl_FrontColor = gl_FrontColorIn[0]; //vec4(cos(isolevel*10.0-0.5), sin(isolevel*10.0-0.5), cos(1.0-isolevel),1.0);
	
	for(int i = 0; i < 16; ++i)
	{
		int triTableIndex = texelFetch(triTableTex, ivec2(i, cubeindex), 0).a;
		if (triTableIndex != -1)
		{
			// Generate first vertex of triangle
			eyePos = gl_ModelViewMatrixInverse[3].xyz;
			positionWithAO.xyz = vertlist[triTableIndex];
			#ifdef USE_AMBIENT_OCCLUSION
			positionWithAO.w = computeAmbientOcclusion(positionWithAO.xyz);
			#else
			positionWithAO.w = 0.7;
			#endif
			gl_Position =
				#ifdef TRANSFORM_POSITION
				gl_ModelViewProjectionMatrix *
				#endif
				vec4(positionWithAO.xyz, 1.0);
			EmitVertex();
		}
		else {
			break;
		}
	}
}
