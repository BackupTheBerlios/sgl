#version 120
#extension GL_EXT_geometry_shader4 : enable

void main(void)
{
	for(int i = 0; i < gl_VerticesIn; i++)
	{
		gl_Position =  gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();
}