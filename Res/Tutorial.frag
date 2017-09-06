
#version 410
layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inWorldPosition;

uniform sampler2D colorSampler;
struct PointLight
{
  vec3 position; 
  vec3 color;
};
const int maxLightCount = 4;
layout(std140) uniform LightData
{
 vec3 ambientColor;
 PointLight light[maxLightCount]; 
}lightData;
out vec4 fragColor;
void main() {
  vec3 lightColor = lightData.ambientColor.rgb;
  for (int i = 0; i < maxLightCount; ++i) {
    vec3 lightVector = lightData.light[i].position.xyz - inWorldPosition;
    float lightPower = 1.0 / dot(lightVector, lightVector);
	float cosTheta = clamp(dot(inWorldNormal, normalize(lightVector)), 0, 1);
    lightColor += lightData.light[i].color.rgb * cosTheta * lightPower;
  }
  fragColor = inColor * texture(colorSampler, inTexCoord);
  fragColor.rgb *= lightColor;
}