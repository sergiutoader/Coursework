#version 330
#define MAX_LIGHT_COUNT 20

in vec3 color;
in vec2 texcoord;

uniform sampler2D texture;
uniform int is_textured;
uniform int is_color_mixed;
uniform int render_type;

layout(location = 0) out vec4 out_color;

// calculeaza culoarea obiectului + aplica texturare daca este cazul
vec4 getObjectColor() {
	vec4 object_color;

	if(is_textured == 1) {
		object_color = texture2D(texture, texcoord);

		if(is_color_mixed == 1) {
			object_color = mix(object_color, vec4(color, 1), 0.5f);
		}

		if(object_color.a < 0.5) discard;

	} else {
		object_color = vec4(color, 1);
	}

	return object_color;
}

// ~~~~~~~ ILUMINARE ~~~~~~~

struct Light {
	vec3 position;
	vec3 direction;
	vec3 color;

	int is_spot;
};

in vec3 world_position;
in vec3 world_normal;

uniform float material_kd;
uniform float material_ks;
uniform float material_ke;
uniform int material_shininess;

uniform vec3 eye_position;


// proprietati lumina spot
uniform vec3 spotlight_position;
uniform vec3 spotlight_direction;
uniform vec3 spotlight_color;


// proprietati lumina point
uniform int point_count;						// numarul de lumini
uniform vec3 point_coord[MAX_LIGHT_COUNT];		// coordonatele luminilor
uniform vec3 point_color[MAX_LIGHT_COUNT];		// culorile luminilor


// initializare structura pentru lumina de tip spot
Light spotlightInit() {
	Light spotlight;
	spotlight.position = spotlight_position;
	spotlight.direction = spotlight_direction;
	spotlight.color = spotlight_color;
	spotlight.is_spot = 1;
	
	return spotlight;
}

// initializare structura pentru luminile de tip light
void pointlightsInit(inout Light pointlights[MAX_LIGHT_COUNT]) {
	
	for(int i = 0; i < point_count; i++) {
		pointlights[i].position = point_coord[i];
		pointlights[i].direction = vec3(0);			// nu este folosita
		pointlights[i].color = point_color[i];
		pointlights[i].is_spot = 0;					// nu este de tip spot
	}
}

// calculul luminii pentru ambele tipuri de lumina
vec3 computeLight(Light l) {
	vec3 light_intensity = vec3(0);

	vec3 L = normalize( l.position - world_position );
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );
	vec3 R = reflect (L, world_normal);

	vec3 diffuse_light = l.color * (material_kd * max(dot(world_normal, L), 0));

	// diffuse light

	float lightReceived = 0;
	if(diffuse_light.x > 0 || diffuse_light.y > 0 || diffuse_light.z > 0) {
		lightReceived = 1;
	}

	vec3 specular_light = l.color * (material_ks * lightReceived * pow(max(dot(world_normal, H), 0), material_shininess));

	// compute light

	float dist = distance(world_position, l.position);
	float attenuation_factor = 100 / (dist * dist + 1);


	if(l.is_spot == 1) {
		float cut_off = radians(45.0);
		float spot_light = dot(-L, l.direction);
		float spot_light_limit = cos(cut_off);
		
		if (spot_light > cos(cut_off))
		{
			// Quadratic attenuation
			float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
			float light_att_factor = pow(linear_att, 2);

			light_intensity = light_intensity + light_att_factor * attenuation_factor * (diffuse_light + specular_light);
		}

	} else {
	
		light_intensity = light_intensity + attenuation_factor * ( diffuse_light + specular_light);	
	}
	

	return light_intensity;
}


void main()
{	

	// setare culoare obiect
	vec4 object_color = getObjectColor();

	// elementele de interfata grafica nu sunt afectate de iluminare
	if(render_type == 1) {

		// setare proprietati spotlight
		Light spotlight = spotlightInit();

		// setare proprietati pointlight
		Light pointlights[MAX_LIGHT_COUNT];
		pointlightsInit(pointlights);
		

		vec3 ambient_light = vec3(material_kd * 0.25); 
		vec3 emissive_light = material_ke * object_color.xyz;

		// componente ambientale si emisive
		out_color = vec4(ambient_light, 1);
		out_color.xyz += emissive_light.xyz;

		// CALCUL LUMINA DE TIP SPOT
		vec3 light_intensity = computeLight(spotlight);
		out_color.xyz += light_intensity.xyz * object_color.xyz;


		// CALCUL LUMINA DE TIP POINT = comenteaza 169 - 173 pentru a creste FPS (a se inmulti emissive_light cu 3 atunci cand se dezactiveaza lumina de tip point - linia 156)
		
		vec3 point_light_intensity;
		for(int i = 0; i < point_count; i++) {
			point_light_intensity += computeLight(pointlights[i]);
		}
		out_color.xyz += point_light_intensity.xyz * object_color.xyz;
		
	} else {
		out_color = object_color;	
	}
	
}