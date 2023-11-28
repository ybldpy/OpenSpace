#version __CONTEXT__


layout(rgba32f,binding = 3) uniform image1D colorTexture;

in float mass;
in vec3 vsPosition;

out vec4 color;

uniform sampler2D spriteTexture;
uniform float minMass;
uniform float maxMass;
uniform float kpc;
void main() {
int width = imageSize(colorTexture).x;
float normalizedMass = (mass - minMass)/(maxMass-minMass);
int idx = int(normalizedMass * (width-1));
float luminosity = pow(5.0, 1.5 - 0.4 * mass);
float distance = length(vsPosition);
luminosity /= pow(distance / kpc,2);
color = vec4(imageLoad(colorTexture,idx).xyz,1.0) * texture(spriteTexture,gl_PointCoord) * pow(100,normalizedMass) * 2.5;
//color = vec4(imageLoad(colorTexture,idx).xyz,texture(spriteTexture,gl_PointCoord).a * mass);

}
