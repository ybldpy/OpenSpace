#version __CONTEXT__


layout(rgba32f,binding = 3) uniform image1D colorTexture;

in float mass;
out vec4 color;

uniform float minMass;
uniform float maxMass;
void main() {

int width = imageSize(colorTexture).x;
int idx = int(((mass - minMass)/(maxMass-minMass)) * (width-1));

color = vec4(imageLoad(colorTexture,idx).xyz,mass);
// color = vec4(1,0,0,1);

}
