#version 330
 
uniform sampler2D texture_1;
uniform sampler2D texture_2;

uniform float Time;
uniform int rotating;
 
in vec2 texcoord;

layout(location = 0) out vec4 out_color;

void main()
{	
	vec2 texcoord_aux = texcoord;
	if(rotating == 1) {
		texcoord_aux.x = texcoord_aux.x - (1.0/20) * Time;
	} 
	
	vec4 color1 = texture2D(texture_1, texcoord_aux);
	vec4 color2 = texture2D(texture_2, texcoord_aux);
	
	vec4 color = mix(color1, color2, 0.5f);
	if(color.a < 0.5f) discard;

	out_color = color;
}