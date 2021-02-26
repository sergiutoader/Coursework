#version 330
#extension GL_EXT_gpu_shader4: enable


layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;


uniform vec3 object_color;
uniform int render_type;
uniform int is_deformed;
uniform float seed;

// Output value to fragment shader
out vec3 color;
out vec2 texcoord; 

// sursa functie zgomot: https://www.geeks3d.com/20100831/shader-library-noise-and-pseudo-random-number-generator-in-glsl/

int LFSR_Rand_Gen(in int n)
{
  // <<, ^ and & require GL_EXT_gpu_shader4.
  n = (n << 13) ^ n; 
  return (n * (n*n*15731+789221) + 1376312589) & 0x7fffffff;
}

float LFSR_Rand_Gen_f( in int n )
{
  return float(LFSR_Rand_Gen(n));
}

float noise3f(in vec3 p)
{
  ivec3 ip = ivec3(floor(p));
  vec3 u = fract(p);
  u = u*u*(3.0-2.0*u);

  int n = ip.x + ip.y*57 + ip.z*113;

  float res = mix(mix(mix(LFSR_Rand_Gen_f(n+(0+57*0+113*0)),
                          LFSR_Rand_Gen_f(n+(1+57*0+113*0)),u.x),
                      mix(LFSR_Rand_Gen_f(n+(0+57*1+113*0)),
                          LFSR_Rand_Gen_f(n+(1+57*1+113*0)),u.x),u.y),
                 mix(mix(LFSR_Rand_Gen_f(n+(0+57*0+113*1)),
                          LFSR_Rand_Gen_f(n+(1+57*0+113*1)),u.x),
                      mix(LFSR_Rand_Gen_f(n+(0+57*1+113*1)),
                          LFSR_Rand_Gen_f(n+(1+57*1+113*1)),u.x),u.y),u.z);

  return 1.0 - res*(1.0/1073741824.0);
}


// ~~~~~~~~ ILUMINARE ~~~~~~~~~

out vec3 world_position;
out vec3 world_normal;

void main()
{

	texcoord = v_texture_coord;

	float disp = noise3f(v_position * seed);
	vec3 P = v_position + (v_normal * disp * 0.5);
	
	color = object_color;
	if(render_type == 1) {
		if(is_deformed == 1 ) {
			gl_Position = Projection * View * Model * vec4(P, 1.0);
		} else {
			gl_Position = Projection * View * Model * vec4(v_position, 1.0);
		}
		
	} else {
		gl_Position = Model * vec4(v_position, 1.0);
	}

	// iluminare
	world_position = (Model * vec4(v_position, 1)).xyz;
	world_normal = normalize( mat3(Model) * v_normal );
	
}
