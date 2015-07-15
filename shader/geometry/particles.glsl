##start
##vertex

#version 400
layout(location=0) in vec3 in_position;
layout(location=1) in vec4 in_velocity;
layout(location=2) in vec3 in_force;
layout(location=3) in vec3 in_right;
layout(location=4) in vec4 in_scale;
layout(location=5) in vec4 data;
layout(location=6) in ivec3 idata;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 MV;
uniform mat4 MVP;

uniform int timer;
uniform float timestep;
uniform float interpolation;

out vec2 scale;

out vec3 direction;
out vec3 right;

out float fade;
flat out int layer;
flat out int orientation;

float getTime(){
    int time = idata.x;
    float t = float(timer-time)*timestep;
    t = t+(interpolation*timestep);
    return t;
}

void setBasicOutputs(float t){
    float fadetime = data.z;
    float lifetime = data.y;

    if(t>=lifetime)
        fade = 0;
    else
        fade = clamp(1-((t-fadetime)/(lifetime-fadetime)),0,1);

    layer = idata.y;
    orientation = idata.z;


    scale = in_scale.xy + in_scale.zw * t;
    right = in_right;
}

void main() {
    float t = getTime();
    setBasicOutputs(t);

    float drag = data.x;
    vec3 force = in_force;
//    force -= 3.0f * in_velocity.xyz * in_velocity.w;

    //exact integration. (no approximation, because 't' is the total time and not a timestep)
    vec3 v0 = in_velocity.xyz * in_velocity.w ;
    vec3 velocity = v0*exp(-t*drag)+ force * t;
    vec3 position = in_position.xyz + velocity * t - 0.5f * t * t * force;

//    position = in_position.xyz + v0 * t + v0*(1.0f-exp(-t));


    direction = normalize(vec3(model*vec4(in_velocity.xyz + in_force * t,0)));
    gl_Position = model* vec4(position,1);

}


##end


##start
##geometry
#version 400

layout(points) in;

in vec3[1] direction;
in vec3[1] right;
in vec2[1] scale;

in float[1] fade;
flat in int[1] layer;
flat in int[1] orientation;

layout(triangle_strip, max_vertices=4) out;

uniform mat4 proj;
uniform mat4 view;

out vec3 tc;
out float fade2;

vec3 getTC(int id){
    const vec4 tx=vec4(0,1,0,1);
    const vec4 ty=vec4(0,0,1,1);
    return vec3(tx[id],ty[id],layer[0]);
}

vec4 getOffset(int id){
    const vec4 ix=vec4(-1,1,-1,1);
    const vec4 iy=vec4(-1,-1,1,1);

    vec4 pos;
    pos.x =ix[id]*scale[0].x;
    pos.y =iy[id]*scale[0].y;
    pos.z = 0;
    pos.w = 0;
    return pos;
}

vec4 getOffset(int i, vec3 right, vec3 dir){
    const vec4 ix=vec4(1,-1,1,-1);
    const vec4 iy=vec4(1,1,-1,-1);

    vec3 offset =  ix[i]*dir*scale[0].x+iy[i]*right*scale[0].y;

    return vec4(offset,0);
}



void createVelocityParticle(){
    vec4 position = view*gl_in[0].gl_Position;


    vec3 dir = normalize(vec3(view*vec4(direction[0],0)));

    vec3 right = normalize(cross(dir,vec3(0,0,-1)));

    for(int i =0; i<4;i++){
        vec4 pos = getOffset(i,right,dir)+position;
        tc = getTC(i);
        fade2 = fade[0];
        gl_Position = proj*pos;
        EmitVertex();
    }
}

void createBillboardParticle(){
    vec4 position = view*gl_in[0].gl_Position;


    vec3 dir = normalize(cross(right[0],vec3(0,0,-1)));


    for(int i =0; i<4;i++){
        vec4 pos = getOffset(i,dir,right[0])+position;
        tc = getTC(i);
        fade2 = fade[0];
        gl_Position = proj*pos;
        EmitVertex();
    }
}


void createFixedParticle(){
    vec4 position = view*gl_in[0].gl_Position;


//    vec3 dir = normalize(vec3(view*vec4(direction[0],0)));

    vec3 dir = direction[0];
//    vec3 right2 = normalize(cross(dir,vec3(0,0,-1)));
    vec3 front = normalize(cross(dir,right[0]));

    for(int i =0; i<4;i++){
        vec4 pos = view*getOffset(i,front,right[0])+position;
        tc = getTC(i);
        fade2 = fade[0];
        gl_Position = proj*pos;
        EmitVertex();
    }
}

void main() {
    if(fade[0]==0.0f)
        return;




     if(orientation[0]==0){
         createBillboardParticle();
     }else if(orientation[0]==1){
         createVelocityParticle();
     }else if(orientation[0]==2){
         createFixedParticle();
     }


}


##end
##start
##fragment

#version 400

uniform sampler2DArray image;

uniform mat4 view;

in vec3 tc;
in float fade2;

layout(location=0) out vec4 out_color;

void main() {

    float d = distance(tc.xy,vec2(0.5f,0.5f))*2;
//    d = 0;
    vec4 c = texture(image,tc);
    c.a *= fade2;
    out_color = c;
//    out_color = vec4(1);

//    out_color = vec4(color2.xyz,(1-d)*fade2);

}

##end