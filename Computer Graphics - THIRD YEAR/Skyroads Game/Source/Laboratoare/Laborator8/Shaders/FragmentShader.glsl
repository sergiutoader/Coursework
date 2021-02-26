#version 330

// TODO: get color value from vertex shader
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

uniform int is_spotlight;
uniform float cut_off_angle;

layout(location = 0) out vec4 out_color;

void main()
{
	float intensitateLumina = 1;

	// TODO: define ambient light component
	float ambient_light = material_kd * 0.25;

	vec3 L = normalize( light_position - world_position );
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );

	// TODO: compute diffuse light component
	float diffuse_light = intensitateLumina * material_kd * max(dot(world_normal, L), 0);

	// TODO: compute specular light component
	vec3 R = reflect (L, world_normal);


	float primesteLumina = 0;
	if(diffuse_light > 0) {
		primesteLumina = 1;
	}
	
	//float specular_light = intensitateLumina * material_ks * primesteLumina * pow(max(dot(V, R), 0), material_shininess);
	float specular_light = intensitateLumina * material_ks * primesteLumina * pow(max(dot(world_normal, H), 0), material_shininess);

	// TODO: compute light
	// factor atenuare:
	float dist = distance(world_position, light_position);
	float factor_atenuare = 1 / (dist * dist + 1);

	float intensitate = ambient_light;

	if(is_spotlight == 1) {
		float cut_off = radians(cut_off_angle);
		float spot_light = dot(-L, light_direction);
		float spot_light_limit = cos(cut_off);
		
		if (spot_light > cos(cut_off))
		{
			// Quadratic attenuation
			float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
			float light_att_factor = pow(linear_att, 2);

			intensitate = intensitate + light_att_factor;
		}

	} else {
		intensitate = intensitate + factor_atenuare * ( diffuse_light + specular_light);	
	}

	out_color = vec4(object_color * intensitate, 1);
	
	
}