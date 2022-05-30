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

uniform sampler2D heightmap1;
uniform float time;
uniform float frequency;
uniform float amplitude;
void main() {

	vertexTextureMixValue =  (texture(heightmap1, textCoord.xy).r/255)*(sin(time * frequency) * amplitude) * 100;
	
   // vertex in world space (for lighting computat ion)
   vec3 N = normalize(modelToWorldSpace * vec4(normal, 0.0)).xyz;
   vec4 P = modelToWorldSpace * vec4(vertex + normalize(normal.xyz) * vertexTextureMixValue, 1.0);
   // normal in world space (for lighting computation)
   // Pass the positions in world space to the fragment shader
   worldPos = P;
   worldNormal = N;
   // final vertex position (for opengl rendering, not for lighting)
   gl_Position = viewProjection * P;
   TexCoord = textCoord;
}