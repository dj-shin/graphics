#version 330 core

// Interpolated values from the vertex shaders
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace1;
in vec3 LightDirection_cameraspace2;
in vec3 LightDirection_cameraspace3;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform vec3 LightPosition_worldspace1;
uniform vec3 LightPosition_worldspace2;
uniform vec3 LightPosition_worldspace3;
uniform vec4 material_color;

void main(){

	vec3 LightColor = vec3(1,1,1);
	float LightPower = 2500.0f;

	vec3 MaterialDiffuseColor = material_color.xyz;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.5,0.5,0.5);

	float distance1 = length( LightPosition_worldspace1 - Position_worldspace );
	float distance2 = length( LightPosition_worldspace2 - Position_worldspace );
	float distance3 = length( LightPosition_worldspace3 - Position_worldspace );

	vec3 n = normalize( Normal_cameraspace );

	vec3 l1 = normalize( LightDirection_cameraspace1 );
	float cosTheta1 = clamp( dot( n,l1 ), 0,1 );
	vec3 l2 = normalize( LightDirection_cameraspace2 );
	float cosTheta2 = clamp( dot( n,l2 ), 0,1 );
	vec3 l3 = normalize( LightDirection_cameraspace3 );
	float cosTheta3 = clamp( dot( n,l3 ), 0,1 );

	vec3 E = normalize(EyeDirection_cameraspace);

	vec3 R1 = reflect(-l1,n);
	float cosAlpha1 = clamp( dot( E,R1 ), 0,1 );
	vec3 R2 = reflect(-l2,n);
	float cosAlpha2 = clamp( dot( E,R2 ), 0,1 );
	vec3 R3 = reflect(-l3,n);
	float cosAlpha3 = clamp( dot( E,R3 ), 0,1 );

	color.rgb =
		MaterialAmbientColor +
		MaterialDiffuseColor * LightColor * LightPower * cosTheta1 / (distance1*distance1) +
		MaterialDiffuseColor * LightColor * LightPower * cosTheta2 / (distance2*distance2) +
		MaterialDiffuseColor * LightColor * LightPower * cosTheta3 / (distance3*distance3) +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha1,5) / (distance1*distance1) +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha2,5) / (distance2*distance2) +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha3,5) / (distance3*distance3);
	color.a =  material_color.w;
}
