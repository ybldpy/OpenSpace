#version __CONTEXT__

layout(points) in;
layout(points,max_vertices = 1) out;

in float vs_mass[];
out float mass;
void main(){

vec4 position = gl_in[0].gl_Position;
gl_Position = position;
gl_Position.z = 0;
mass = vs_mass[0];
EmitVertex();
EndPrimitive();

}
