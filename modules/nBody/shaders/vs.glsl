#version __CONTEXT__

struct PositionTextureSize{
int width;
};

layout(rgba32f,binding = 2) uniform image2D positionTexture;


out float vs_mass;
out vec3 posColor;

uniform float kpc;
uniform dmat4 transformation;
uniform PositionTextureSize positionTextureSize;

void main() {

    int id = gl_VertexID;
    ivec2 textureIndex = ivec2(int(id%positionTextureSize.width),int(id/positionTextureSize.width));
    vec4 pixel = imageLoad(positionTexture,textureIndex);
    vec3 p = pixel.rgb * kpc;
    vs_mass = pixel.a;
    gl_Position = vec4(transformation * dvec4(p,1));
}
