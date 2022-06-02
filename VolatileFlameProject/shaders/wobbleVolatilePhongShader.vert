#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoord;

uniform mat4 modelToWorldSpace; // represents vetex coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

out vec4 worldPos;
out vec3 worldNormal;
out vec2 TexCoord;
out float vertexTextureMixValue;

uniform sampler2D waveHeightmap, noiseHeightmap;
uniform float time;
uniform float deltaTime;
uniform float frequency;
uniform float amplitude;

vec2 calcTextCoord1HorizScroll = textCoord.xy;
vec2 calcTextCoord2HorizScroll = textCoord.xy;

void main() {

   // Horizonal scroll on the texture coordinates
   calcTextCoord1HorizScroll.x = textCoord.x + time * 0.009 * (frequency*7.0);
   calcTextCoord2HorizScroll.x = textCoord.x - time * 0.003 * (frequency*7.0);
   // loop back when outside texture bounds
   if(calcTextCoord1HorizScroll.x > 1.0) calcTextCoord1HorizScroll.x -= 1.0; 
   if(calcTextCoord2HorizScroll.x < 0.0) calcTextCoord2HorizScroll.x += 1.0;
   // get heightmap values in the range 0-1
   float heightMap1DisplaceValue = texture(waveHeightmap, calcTextCoord1HorizScroll.xy).r/255.0;
   float heightMap2DisplaceValue = texture(noiseHeightmap, calcTextCoord2HorizScroll.xy).r/255.0;

   // Pass the values for mixing the textures to the fragment shader
	vertexTextureMixValue =  (heightMap1DisplaceValue + heightMap2DisplaceValue) * (amplitude/10.0) * 100;

   // calculate displacement
   vec3 displacement = normalize(normal.xyz) * vertexTextureMixValue;

   // calculate final texture mixing
   vertexTextureMixValue = vertexTextureMixValue * vertexTextureMixValue * vertexTextureMixValue * vertexTextureMixValue * 10.0;

   // vertex position in world space (for lighting computation and displacement)
   vec4 P = modelToWorldSpace * vec4(vertex + displacement, 1.0);
   // normal direction in world space (for lighting computation)
   vec3 N = normalize(modelToWorldSpace * vec4(normal, 0.0)).xyz;
	
   // Pass the positions in world space to the fragment shader
   worldPos = P;
   worldNormal = N;
   // final vertex position (for opengl rendering, not for lighting)
   gl_Position = viewProjection * P;
   TexCoord = textCoord;
}