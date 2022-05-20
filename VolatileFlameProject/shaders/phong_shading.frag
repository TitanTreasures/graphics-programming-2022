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

uniform float mixValue;

// TODO exercise 5.4 add the 'in' variables to receive the interpolated Position and Normal from the vertex shader
in vec4 worldPos;
in vec3 worldNormal;
in vec2 TexCoord;

uniform sampler2D texture1, texture2;

void main()
{
   vec4 P = worldPos;
   vec3 N = normalize(worldNormal);
   // TODO exercise 5.4 - phong shading (i.e. Phong reflection model computed in the fragment shader)
   // ambient component
   vec3 ambient = ambientLightColor * ambientReflectance * reflectionColor;

   // diffuse component for light 1
   vec3 L = normalize(light1Position - P.xyz);
   float diffuseModulation = max(dot(N, L), 0.0);
   vec3 diffuse = light1Color * diffuseReflectance * diffuseModulation * reflectionColor;

   // specular component for light 1
   vec3 V = normalize(camPosition - P.xyz);
   vec3 H = normalize(L + V);
   float specModulation = pow(max(dot(H, N), 0.0), specularExponent);
   // Alternative version: Same result, with different specularExponent values
   //vec3 R = 2 * dot(L, N) * N - L; // the same as reflect(-L, normal)
   //float specModulation = pow(max(dot(R, V), 0.0), specularExponent);
   vec3 specular = light1Color * specularReflectance * specModulation;

   // TODO exercuse 5.5 - attenuation - light 1 (Lighting loses intensity as the distance between the light emitter and the surface increases.)
   float distToLight = distance(light1Position, P.xyz);
   float attenuation = 1.0f / (distToLight * distToLight);

   // TODO exercise 5.6 - multiple lights, compute diffuse, specular and attenuation of light 2
   vec3 L2 = normalize(light2Position - P.xyz);
   float diffuseModulation2 = max(dot(N, L2), 0.0);
   vec3 diffuse2 = light2Color * diffuseReflectance * diffuseModulation2 * reflectionColor;

   vec3 H2 = normalize(L2 + V);
   float specModulation2 = pow(max(dot(H2, N), 0.0), specularExponent);
   vec3 specular2 = light2Color * specularReflectance * specModulation2;

   float distToLight2 = distance(light2Position, P.xyz);
   float attenuation2 = 1.0f / (distToLight2 * distToLight2);

   // TODO compute the final shaded color (e.g. add contribution of the attenuated lights 1 and 2)

   // TODO set the output color to the shaded color that you have computed 
   //* texture(ourTexture, TexCoord)
   FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue) * vec4(ambient + (diffuse + specular) * attenuation + (diffuse2 + specular2)*attenuation2, 1.0);
}
