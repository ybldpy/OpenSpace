#version __CONTEXT__

layout(points) in;
layout(points,max_vertices = 1) out;

in float vs_mass[];
in vec4 vs_gPositions[];

out vec4 vs_gPosition;
out float mass;
out float vs_screenDepth;

void main(){

vec4 position = gl_in[0].gl_Position;
vs_gPosition = vs_gPositions[0];
gl_Position = position;
vs_screenDepth = position.w;
gl_Position.z = 0;
mass = vs_mass[0];
EmitVertex();
EndPrimitive();

}
