#version 330 core

// Interpolated values from the vertex shaders
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace1;
in vec3 LightDirection_cameraspace2;
in vec3 LightDirection_cameraspace3;
in vec3 LightDirection_cameraspace4;
in vec3 LightDirection_cameraspace5;
in vec3 LightDirection_cameraspace6;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform vec3 LightPosition_worldspace1;
uniform vec3 LightPosition_worldspace2;
uniform vec3 LightPosition_worldspace3;
uniform vec3 LightPosition_worldspace4;
uniform vec3 LightPosition_worldspace5;
uniform vec3 LightPosition_worldspace6;
uniform vec4 material_color;
uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float n;

void main(){

	vec3 LightColor1 = vec3(1,1,1);
	vec3 LightColor2 = vec3(1,1,1);
	vec3 LightColor3 = vec3(1,1,1);
	vec3 LightColor4 = vec3(1,1,1);
	vec3 LightColor5 = vec3(1,1,1);
	vec3 LightColor6 = vec3(1,1,1);
	float LightPower = 500.0f;

	vec3 MaterialDiffuseColor = Kd * material_color.xyz;
	vec3 MaterialAmbientColor = Ka * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = Ks;

	float distance1 = length( LightPosition_worldspace1 - Position_worldspace );
	float distance2 = length( LightPosition_worldspace2 - Position_worldspace );
	float distance3 = length( LightPosition_worldspace3 - Position_worldspace );
	float distance4 = length( LightPosition_worldspace4 - Position_worldspace );
	float distance5 = length( LightPosition_worldspace5 - Position_worldspace );
	float distance6 = length( LightPosition_worldspace6 - Position_worldspace );

	vec3 normal = normalize( Normal_cameraspace );

	vec3 l1 = normalize( LightDirection_cameraspace1 );
	float cosTheta1 = clamp( dot( normal,l1 ), 0,1 );
	vec3 l2 = normalize( LightDirection_cameraspace2 );
	float cosTheta2 = clamp( dot( normal,l2 ), 0,1 );
	vec3 l3 = normalize( LightDirection_cameraspace3 );
	float cosTheta3 = clamp( dot( normal,l3 ), 0,1 );
	vec3 l4 = normalize( LightDirection_cameraspace4 );
	float cosTheta4 = clamp( dot( normal,l4 ), 0,1 );
	vec3 l5 = normalize( LightDirection_cameraspace5 );
	float cosTheta5 = clamp( dot( normal,l5 ), 0,1 );
	vec3 l6 = normalize( LightDirection_cameraspace6 );
	float cosTheta6 = clamp( dot( normal,l6 ), 0,1 );

	vec3 E = normalize(EyeDirection_cameraspace);

	vec3 R1 = reflect(-l1,normal);
	float cosAlpha1 = clamp( dot( E,R1 ), 0,1 );
	vec3 R2 = reflect(-l2,normal);
	float cosAlpha2 = clamp( dot( E,R2 ), 0,1 );
	vec3 R3 = reflect(-l3,normal);
	float cosAlpha3 = clamp( dot( E,R3 ), 0,1 );
	vec3 R4 = reflect(-l4,normal);
	float cosAlpha4 = clamp( dot( E,R4 ), 0,1 );
	vec3 R5 = reflect(-l5,normal);
	float cosAlpha5 = clamp( dot( E,R5 ), 0,1 );
	vec3 R6 = reflect(-l6,normal);
	float cosAlpha6 = clamp( dot( E,R6 ), 0,1 );

	color.rgb =
		MaterialAmbientColor +
		MaterialDiffuseColor * LightColor1 * LightPower * cosTheta1 / (distance1*distance1) +
		MaterialDiffuseColor * LightColor2 * LightPower * cosTheta2 / (distance2*distance2) +
		MaterialDiffuseColor * LightColor3 * LightPower * cosTheta3 / (distance3*distance3) +
		MaterialDiffuseColor * LightColor4 * LightPower * cosTheta4 / (distance4*distance4) +
		MaterialDiffuseColor * LightColor5 * LightPower * cosTheta5 / (distance5*distance5) +
		MaterialDiffuseColor * LightColor6 * LightPower * cosTheta6 / (distance6*distance6) +
		MaterialSpecularColor * LightColor1 * LightPower * pow(cosAlpha1,n) / (distance1*distance1) +
		MaterialSpecularColor * LightColor2 * LightPower * pow(cosAlpha2,n) / (distance2*distance2) +
		MaterialSpecularColor * LightColor3 * LightPower * pow(cosAlpha3,n) / (distance3*distance3) +
		MaterialSpecularColor * LightColor4 * LightPower * pow(cosAlpha4,n) / (distance4*distance4) +
		MaterialSpecularColor * LightColor5 * LightPower * pow(cosAlpha5,n) / (distance5*distance5) +
		MaterialSpecularColor * LightColor6 * LightPower * pow(cosAlpha6,n) / (distance6*distance6);
	color.a =  material_color.w;
}
