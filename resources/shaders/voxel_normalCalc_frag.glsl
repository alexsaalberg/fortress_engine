#version 330 core

in vec3 fPosition_World;
in vec4 block_color;

//to send the color to a frame buffer
layout(location = 0) out vec4 color;

//uniform sampler2D Texture0;

uniform mat4 P;
uniform mat4 V;
uniform mat4 trans_inv_V;

/*LIGHTING*/
uniform vec3 eyePosition;
uniform vec3 directionTowardsLight;

uniform vec3 mDiffusionCoefficient;
uniform vec3 mAmbientCoefficient;
uniform vec3 mSpecularCoefficient;

uniform float mSpecularAlpha;

void main()
{
    vec3 fNormal_world = cross(dFdy(fPosition_World.xyz), dFdx(fPosition_World.xyz));
    fNormal_world = normalize(fNormal_world);
    
    //vec4 fNormal_camera_nohomo = P * V * vec4(fNormal_world, 0.0f);
    vec4 fNormal_camera_nohomo = P * trans_inv_V * vec4(fNormal_world, 0.0f);
    vec3 fNormal_camera = fNormal_camera_nohomo.xyz / fNormal_camera_nohomo.w;
    fNormal_camera = normalize(fNormal_camera);
    fNormal_camera = abs(fNormal_camera);
    
    vec3 directionTowardsEye = normalize(eyePosition - fPosition_World);
    
    vec3 L = normalize(directionTowardsLight);
    vec3 N = normalize(abs(fNormal_world));
    vec3 H = normalize(directionTowardsLight + directionTowardsEye);
    
    vec3 lightColor = vec3(0.0f);
    
    vec3 ambientLight = mAmbientCoefficient;
    vec3 diffusionLight = block_color.xyz * clamp(dot(N, L), 0.0, 1.0);
    vec3 specularLight = mSpecularCoefficient * pow( clamp(dot(H, N), 0.0, 1.0), mSpecularAlpha);
    
    lightColor = ambientLight + diffusionLight + specularLight;
    
    color = vec4(lightColor, block_color.w);
    //color = (vec4(lightColor, 1.0) + vec4(abs(fNormal_world), 1.0f)) / 2.0f;
    //color = vec4(fNormal_world, 1.0);
    //color = vec4(abs(fNormal_camera), 1.0);
    //color = vec4(H, 1.0) * 0.5 + (abs(fNormal_world), 1.0) * 0.5;
}
