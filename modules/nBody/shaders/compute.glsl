#version __CONTEXT__


layout (local_size_x = 10,local_size_y = 10) in;
layout(rgba32f, binding = 0) uniform image2D startPos;
layout(rgba32f, binding = 1) uniform image2D targetPos;
layout(rgba32f,binding = 2) uniform image2D  currentPos;

uniform float displacement;
uniform int maxIndex;

void main(){

ivec2 index = ivec2(gl_GlobalInvocationID.xy);
vec3 target = imageLoad(targetPos,index).xyz;
vec4 start = imageLoad(startPos,index);
vec4 updatedPos = vec4(start.xyz+(target - start.xyz) * displacement,start.w);
imageStore(currentPos,index,updatedPos);

}

