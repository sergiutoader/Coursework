#version 330

// TODO: get values from fragment shader
in vec3 frag_color;
in vec2 frag_tex_coord;
in vec3 frag_normal;

layout(location = 0) out vec4 out_color;

void main()
{


	//ex 5 - decomenteaza asta
	//out_color = vec4(frag_normal, 1.0);

	// pentru bonus - refolosim frag_color pentru calcularea out_color
	out_color = vec4(frag_color, 1.0);

}