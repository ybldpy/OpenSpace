#version __CONTEXT__



layout(rgba32f,binding = 2) uniform image2D positionTexture;


out float vs_mass;
out vec3 posColor;
out vec3 vsPositionModel;

uniform float kpc;
uniform dmat4 transformation;
uniform int positionTextureWidth;
uniform  dmat4 modelMat;

void main() {

    int id = gl_VertexID;
    ivec2 textureIndex = ivec2(int(id%positionTextureWidth),int(id/positionTextureWidth));
    vec4 pixel = imageLoad(positionTexture,textureIndex);
    vec3 p = pixel.rgb * kpc;
    vs_mass = pixel.a;
    gl_PointSize = vs_mass * 40;
    vsPositionModel = vec4(modelMat * dvec4(p,1)).xyz;
    gl_Position = vec4(transformation * dvec4(p,1));
}
