#version 120
#extension GL_EXT_gpu_shader4 : enable

varying vec4 	positionWithAO;
varying vec3 	eyePos;

uniform float 	isolevel;
uniform vec3 	dataStep;
uniform vec3 	rayDir[36];
uniform vec3 	vertDecals[8];
uniform vec3 	vertDecalsNorm[8];

uniform sampler3D dataFieldTex;

const vec3 diffuseMaterial = vec3(0.7, 0.7, 0.7);
const vec3 specularMaterial = vec3(0.99, 0.99, 0.99);
const vec3 ambientMaterial = vec3(0.3, 0.2, 0.3);

float computeMipmap(vec3 texStep, vec3 voxelStep)
{
	float averageTexStep   = (dataStep.x + dataStep.y + dataStep.z) / 3.0;
	float averageVoxelStep = (voxelStep.x + voxelStep.y + voxelStep.z) / 3.0;
	float mipmap = 1.0;

	while ( averageTexStep < averageVoxelStep )
	{
		averageTexStep *= 2.0;
		mipmap += 1.0;
	}

	return mipmap - 1.0;
}

vec3 cubePos(vec3 pos, int i, float mipmap) {
	return pos + vertDecalsNorm[i] * dataStep * (mipmap + 1.0);
}

float cubeVal(vec3 pos, int i, float mipmap) {
	return texture3DLod( dataFieldTex, 0.5 * (cubePos(pos, i, mipmap) + 1.0), mipmap ).a;
}

float computeAmbientOcclusion(vec3 pos)
{
	const float numRays = 32.0;
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
	// var
	vec3 eyeDirNorm = normalize(positionWithAO.xyz - eyePos);
	vec3 lightVec = normalize(gl_LightSource[0].position.xyz - positionWithAO.xyz);

#ifdef USE_RAYCAST_CORRECTION
	const float numSteps = 15.0;
	vec3 step = 2.0 * vertDecals[6] * eyeDirNorm / numSteps;
	vec3 pos = positionWithAO.xyz - 0.5 * numSteps * step;

	float i = 0.0;
	float prev = texture3D(dataFieldTex, 0.5 * (pos - step) + 0.5).a;
	for(; i<numSteps; i += 1.0, pos += step)
	{
		float cur = texture3D(dataFieldTex, 0.5 * pos + 0.5).a;
		if ( (cur - isolevel) * (prev - isolevel) < 0.0 )
		{
			pos = mix(pos - step, pos, abs( (prev - isolevel) / (cur - prev) ) );
			break;
		}
	}

	// intersection not found;
	if ( i == numSteps ) {
		pos = positionWithAO.xyz;
	}

	vec3 grad = vec3( cubeVal(pos.xyz, 2, 0.0) - cubeVal(pos.xyz, 1, 0.0),
					  cubeVal(pos.xyz, 4, 0.0) - cubeVal(pos.xyz, 3, 0.0),
					  cubeVal(pos.xyz, 5, 0.0) - cubeVal(pos.xyz, 4, 0.0) );
	if ( length(grad) < 0.00001 ) {
		grad = vec3(1.0, 0.0, 0.0);
	}
	vec3 normalNorm = normalize(grad);
#else
	vec3 grad = vec3( cubeVal(positionWithAO.xyz, 2, 0.0) - cubeVal(positionWithAO.xyz, 1, 0.0),
					  cubeVal(positionWithAO.xyz, 4, 0.0) - cubeVal(positionWithAO.xyz, 3, 0.0),
					  cubeVal(positionWithAO.xyz, 5, 0.0) - cubeVal(positionWithAO.xyz, 4, 0.0) );
	if ( length(grad) < 0.00001 ) {
		grad = vec3(1.0, 0.0, 0.0);
	}
	vec3 normalNorm = normalize(grad);
#endif


 /*
 		texture3D(dataFieldTex, (position.xyz+vec3(dataStep.x, 0, 0)+1.0f)/2.0f).a - texture3D(dataFieldTex, (position.xyz+vec3(-dataStep.x, 0, 0)+1.0f)/2.0f).a,
		texture3D(dataFieldTex, (position.xyz+vec3(0, dataStep.y, 0)+1.0f)/2.0f).a - texture3D(dataFieldTex, (position.xyz+vec3(0, -dataStep.y, 0)+1.0f)/2.0f).a,
		texture3D(dataFieldTex, (position.xyz+vec3(0,0,dataStep.z)+1.0f)/2.0f).a - texture3D(dataFieldTex, (position.xyz+vec3(0,0,-dataStep.z)+1.0f)/2.0f).a);

	const float start_mipmap = 0.0;
	const float end_mipmap   = 5.0;

	float averageDensity = 0.0;
	for(float mipmap = start_mipmap; mipmap < end_mipmap; mipmap += 1.0)
	{
		averageDensity += float( cubeVal(pos, 1, mipmap) )
						  + float( cubeVal(pos, 2, mipmap) )
						  + float( cubeVal(pos, 3, mipmap) )
						  + float( cubeVal(pos, 4, mipmap) )
						  + float( cubeVal(pos, 5, mipmap) )
						  + float( cubeVal(pos, 6, mipmap) );
	}
	averageDensity = 0.16 * averageDensity / (end_mipmap - start_mipmap) + 0.01;
	float ambientOcclusion = clamp( isolevel / averageDensity, 0.0, 1.0 );
*/

	// calculate diffuse component
	vec3 diffuse = abs( dot(normalNorm, lightVec) ) * diffuseMaterial;

	// calculate specular component
	vec3 reflectVec = reflect(eyeDirNorm, normalNorm);
	//vec3 halfVec = normalize(lightVec - eyeDirNorm);
	vec3 specular = pow( abs( dot(normalNorm, reflectVec) ), 32.0 ) * specularMaterial;

	// combine diffuse and specular contributions and output final vertex color
#ifdef USE_AMBIENT_OCCLUSION
	gl_FragColor.rgb = positionWithAO.w * (ambientMaterial + diffuse + specular);
#else
	gl_FragColor.rgb = 0.7 * (ambientMaterial + diffuse + specular);
#endif
	gl_FragColor.a = 1.0;
}


/*
	//eyePos = gl_ModelViewMatrixInverse[3].xyz;
	//positionWithAO = vec4(-1.0, -1.0, 0.0, 1.0);
	gl_Position = positionWithAO;//gl_ModelViewProjectionMatrix * vec4(-1.0, -1.0, 1.0, 1.0);
	EmitVertex();
	
	eyePos = gl_ModelViewMatrixInverse[3].xyz;
	positionWithAO = vec4(0.0, 1.0, 0.0, 1.0);
	gl_Position = gl_ModelViewProjectionMatrix * vec4(0.0, 1.0, 1.0, 1.0);
	EmitVertex();

	eyePos = gl_ModelViewMatrixInverse[3].xyz;
	positionWithAO = vec4(1.0, 0.0, 0.0, 1.0);
	gl_Position = gl_ModelViewProjectionMatrix * vec4(1.0, -1.0, 1.0, 1.0);
	EmitVertex();
	
	return;
		
	int cubeindex=0;

	//Determine the index into the edge table which
	//tells us which vertices are inside of the surface
	vec2 mipmap = computeMipmap(dataStep, vertDecals[6]);
	if (cubeVal(0, mipmap) < isolevel) cubeindex = cubeindex | 1;
	if (cubeVal(1, mipmap) < isolevel) cubeindex = cubeindex | 2;
	if (cubeVal(2, mipmap) < isolevel) cubeindex = cubeindex | 4;
	if (cubeVal(3, mipmap) < isolevel) cubeindex = cubeindex | 8;
	if (cubeVal(4, mipmap) < isolevel) cubeindex = cubeindex | 16;
	if (cubeVal(5, mipmap) < isolevel) cubeindex = cubeindex | 32;
	if (cubeVal(6, mipmap) < isolevel) cubeindex = cubeindex | 64;
	if (cubeVal(7, mipmap) < isolevel) cubeindex = cubeindex | 128;

	//Cube is entirely in/out of the surface
	int edgeTableValue = edgeTable[cubeindex];
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
	
	for(int i = 0; i < 16; i += 3)
	{
		if( triTable[cubeindex*16 + i] != -1 )
		{
			// Generate first vertex of triangle
			eyePos = gl_ModelViewMatrixInverse[3].xyz;
			positionWithAO.xyz = vertlist[ triTable[cubeindex*16 + i] ];
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

			// Generate second vertex of triangle
			eyePos = gl_ModelViewMatrixInverse[3].xyz;
			positionWithAO.xyz = vertlist[ triTable[cubeindex*16 + i + 1] ];
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

			// Generate last vertex of triangle
			eyePos = gl_ModelViewMatrixInverse[3].xyz;
			positionWithAO.xyz = vertlist[ triTable[cubeindex*16 + i + 2] ];
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

			//End triangle strip at firts triangle
			EndPrimitive();
		} else {
			break;
		}
	}
*/