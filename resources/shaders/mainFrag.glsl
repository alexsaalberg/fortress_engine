#version 330 core

in vec3 fNormal;
in vec2 fTextureCoordinates;
in vec3 fPosition_World;

//to send the color to a frame buffer
layout(location = 0) out vec4 color;

//uniform sampler2D Texture0;

uniform vec3 eyePosition;
uniform vec3 directionTowardsLight;

uniform vec3 mDiffusionCoefficient;
uniform vec3 mAmbientCoefficient;
uniform vec3 mSpecularCoefficient;
uniform float mSpecularAlpha;

/* Very simple Diffuse shader with a directional light*/
void main()
{
    vec3 directionTowardsEye = normalize(eyePosition - fPosition_World); // L
    
    vec3 L = normalize(directionTowardsLight);
    vec3 N = normalize(fNormal);
    vec3 H = normalize(directionTowardsLight + directionTowardsEye);
    
    vec3 lightColor = vec3(0.0f);
    
    lightColor += mAmbientCoefficient;
    lightColor += mDiffusionCoefficient * clamp(dot(L, N), 0.0, 1.0);
    lightColor += mSpecularCoefficient * pow( clamp(dot(H, N), 0.0, 1.0), mSpecularAlpha);
    
    color = vec4(lightColor, 1.0);
    //color = vec4(fNormal, 1.0);
    //vec4 textureColor = texture(Texture0, fTextureCoordinates);
    //color = 0.5 * (color+textureColor);
}
