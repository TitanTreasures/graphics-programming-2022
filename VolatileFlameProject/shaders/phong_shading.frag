#version 330 core

uniform vec3 camPosition; // so we can compute the view vector
out vec4 FragColor; // the output color of this fragment

// TODO exercise 5.4 setup the 'uniform' variables needed for lighting
// light uniform variables
uniform vec3 ambientLightColor;
uniform vec3 light1Position;
uniform vec3 light1Color;
uniform vec3 light2Position;
uniform vec3 light2Color;

// material properties
uniform vec3 reflectionColor;
uniform float ambientReflectance;
uniform float diffuseReflectance;
uniform float specularReflectance;
uniform float specularExponent;

//uniform float mixValue;

// The 'in' variables to receive the interpolated Position and Normal from the vertex shader
in vec4 worldPos;
in vec3 worldNormal;
in vec2 TexCoord;
in float vertexTextureMixValue;

uniform sampler2D texture1, texture2;

void main()
{
   vec4 P = worldPos;
   vec3 N = normalize(worldNormal);

   // Phong shading (i.e. Phong reflection model computed in the fragment shader)
   // ambient component
   vec3 ambient = ambientLightColor * ambientReflectance * reflectionColor;

   // LIGHT 1:
   // diffuse component for light 1
   vec3 L = normalize(light1Position - P.xyz);
   float diffuseModulation = max(dot(N, L), 0.0);
   vec3 diffuse = light1Color * diffuseReflectance * diffuseModulation * reflectionColor;
   // specular component for light 1
   vec3 V = normalize(camPosition - P.xyz);
   vec3 H = normalize(L + V);
   float specModulation = pow(max(dot(H, N), 0.0), specularExponent);
   vec3 specular = light1Color * specularReflectance * specModulation;
   // attenuation component for light 1 (Lighting loses intensity as the distance between the light emitter and the surface increases.)
   float distToLight = distance(light1Position, P.xyz);
   float attenuation = 1.0f / (distToLight * distToLight);
   // combined phong light 1
   vec3 light1 = (diffuse + specular) * attenuation;


   // LIGHT 2:
   // diffuse component for light 2
   vec3 L2 = normalize(light2Position - P.xyz);
   float diffuseModulation2 = max(dot(N, L2), 0.0);
   vec3 diffuse2 = light2Color * diffuseReflectance * diffuseModulation2 * reflectionColor;
   // specular component for light 2
   vec3 H2 = normalize(L2 + V);
   float specModulation2 = pow(max(dot(H2, N), 0.0), specularExponent);
   vec3 specular2 = light2Color * specularReflectance * specModulation2;
   // calculate light 2 attenuation
   float distToLight2 = distance(light2Position, P.xyz);
   float attenuation2 = 1.0f / (distToLight2 * distToLight2);
   // combined phong light 2
   vec3 light2 = (diffuse2 + specular2)*attenuation2;

   // Compute the final shaded color
   vec4 lighting = vec4(ambient + light1 + light2, 1.0);

   // mix the textures
   vec4 textureMix = mix(texture(texture2, TexCoord), texture(texture1, TexCoord), (vertexTextureMixValue * vertexTextureMixValue * vertexTextureMixValue * 100000000.0)-1);

   // add the final texture and lighting
   FragColor = textureMix * lighting;
}
