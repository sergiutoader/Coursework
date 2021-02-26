#version 330

// TODO: get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 3) in vec3 v_normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 1) in vec3 v_color;


// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;

// TODO: output values to fragment shader
out vec3 frag_color;
out vec2 frag_tex_coord;
out vec3 frag_normal;


void main()
{

	// TODO: compute gl_Position
	gl_Position = Projection * View * Model * vec4(v_position, 1.0);

	/////////////////////// bonus ////////////////////////////////

	// variatia pozitiei in functie de timp
	vec3 aux = v_position + vec3(cos(Time), 0, 0);
	gl_Position = Projection * View * Model * vec4(aux, 1.0);

	// variatia culorii in functie de timp
	vec3 aux_color = v_color + vec3(cos(Time), sin(Time), cos(Time));
	frag_color = aux_color;

	///////////////////////////////////////////////////////////////


	// TODO: send output to fragment shader
	//frag_color = v_color;
	frag_tex_coord = tex_coord;
	frag_normal = v_normal;
}
