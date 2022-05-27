#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textCoord;

uniform mat4 position; // represents model coordinates in the world coord space
uniform mat4 viewProjection;  // represents the view and projection matrices combined

// TODO exercise 5.4 - make the 'out' variables that will be used in the fragment shader
out vec4 worldPos;
out vec3 worldNormal;
out vec2 TexCoord;
out float mixValue;

uniform sampler2D heightmap;

void main() {
	// https://stackoverflow.com/questions/45240292/pass-texture-with-float-values-to-shader#:~:text=float%20float_texel%20%3D%20float(%20texture2D(float_texture%2C%20tex_coords.xy)%20)%3B
	float float_texel = float(texture2D(heightmap, textCoord).r);
	mixValue = float_texel;
   // vertex in world space (for lighting computation)
   vec4 P = position * vec4(vertex, 1.0);
   // normal in world space (for lighting computation)
   vec3 N = normalize(position * vec4(normal, 0.0)).xyz;

   // Pass the positions in world space to the fragment shader
   worldPos = P;
   worldNormal = N;


   // final vertex position (for opengl rendering, not for lighting)
   gl_Position = viewProjection * P;
   TexCoord = textCoord;

}