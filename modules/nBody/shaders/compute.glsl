#version __CONTEXT__


layout (local_size_x = 10,local_size_y = 10) in;
layout(rgba32f, binding = 0) uniform image2D startPos;
layout(rgba32f, binding = 1) uniform image2D targetPos;
layout(rgba32f,binding = 2) uniform image2D  currentPos;

uniform float displacement;
uniform int maxIndex;


vec3 interpolation(vec3 start,vec3 end,float t) {
//float e = (1/(1+exp(-6*t+3))-1/(1+exp(3))) * (exp(3)+1)/(exp(3)-1);
//float p0 = e/1+e;
//float p1 = 1/1+e;
//return start+e*(end-start);
float ts = t*t;
return (1-t)*(1-t)*start + 2*(1-t)*t*end+ts*end;
}
vec3 slerp(vec3 start,vec3 end,float t){

float dotProduct = dot(start,end);
float startLen = length(start);
float endLen = length(end);
float angle = acos(dotProduct / (startLen * endLen));
vec3 normalizedStart = normalize(start);
vec3 normalizedEnd = normalize(end);
float sinw = sin(angle);
vec3 v1 = sin((1-t)*angle)/sinw * normalizedStart;
vec3 v2 = sin(t*angle)/sinw * normalizedEnd;
vec3 dict = v1+v2;
return dict * mix(startLen,endLen,t);

}


void main(){

ivec2 index = ivec2(gl_GlobalInvocationID.xy);
vec4 target = imageLoad(targetPos,index);
vec4 start = imageLoad(startPos,index);

vec4 p0 = start;
vec4 p1 = imageLoad(currentPos, index);
vec4 p2 = target;
  
float t = displacement;
float mass = start.w + (target.w - start.w) * displacement;
//vec4 updatedPos = vec4((1-displacement)*start.xyz+displacement*target.xyz,mass);
vec4 updatedPos = vec4(slerp(start.xyz,target.xyz,displacement),mass);
imageStore(currentPos,index,updatedPos);

}

