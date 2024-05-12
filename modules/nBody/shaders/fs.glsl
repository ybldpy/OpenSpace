#version __CONTEXT__



#include "fragment.glsl"
layout(rgba32f,binding = 3) uniform image1D colorTexture;

in float mass;
in float vs_screenDepth;
in vec4 vs_gPosition;

out vec4 color;
layout(depth_less) out float gl_FragDepth;

uniform sampler2D spriteTexture;
uniform float minMass;
uniform float maxMass;
uniform float kpc;
void main() {
gl_FragDepth = vs_screenDepth/kpc;
int width = imageSize(colorTexture).x;
float normalizedMass = (mass - minMass)/(maxMass-minMass);
int idx = int(normalizedMass * (width-1));
//float luminosity = pow(5.0, 1.5 - 0.4 * mass);
//float distance = length(vsPosition);
//luminosity /= pow(distance / kpc,2);

float depth = gl_FragDepth * 100;
float attenuation = 1.0 / (1.0 + 5 * depth);
color = vec4(imageLoad(colorTexture,idx).xyz,1) * texture(spriteTexture,gl_PointCoord) * attenuation*2;
color = vec4(color.xyz,normalizedMass);
//color = vec4(vs_screenDepth,0,0,1);
//color = vec4(imageLoad(colorTexture,idx).xyz,texture(spriteTexture,gl_PointCoord).a * mass);
}

//Fragment getFragment() {
  //Fragment frag;
  //int width = imageSize(colorTexture).x;
  //float normalizedMass = (mass - minMass)/(maxMass-minMass);
  //int idx = int(normalizedMass * (width-1));
  //vec4 color = vec4(imageLoad(colorTexture,idx).xyz,normalizedMass) * texture(spriteTexture,gl_PointCoord) * pow(50,normalizedMass) * 2;
  // G-Buffer
  //frag.depth = vs_screenDepth;
  //frag.gPosition = vs_gPosition;
  //frag.color = color;
  //frag.gNormal = vec4(0,0,0,1);
  //
  //return frag;
//}
