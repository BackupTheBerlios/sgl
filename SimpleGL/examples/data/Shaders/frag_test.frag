#extension GL_EXT_bindable_uniform : enable
#extension GL_EXT_gpu_shader4 : enable

bindable uniform ivec4 colors[256];

void main(void)
{
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
	for(int i = 30; i < 40; i++)
	{
		color += vec4(colors[i][2]) / 255.0;
	}
	gl_FragColor = vec4(1.0);//color;//colors[3];// + vec4(1.0, 0.0, 0.0, 0.0);
}