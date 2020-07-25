#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 uv_varying;
layout(location = 1) in vec4 color_varying;

layout(location = 0) out vec4 outColor;

struct UniformData
{
  vec2 vertices[4];
  vec2 border_radius[4];
};

layout(set = 0, binding = 0) uniform UBO
{
  UniformData payload[20];
} ubo;

void main()
{
    float a = 1;
    float r = 0.1;
    if(
    (distance(uv_varying, vec2(r,   r))     > r &&      uv_varying.x < r &&     uv_varying.y < r) ||
    (distance(uv_varying, vec2(1-r, r))     > r && 1 -  uv_varying.x < r &&     uv_varying.y < r) ||
    (distance(uv_varying, vec2(r,   1-r))   > r &&      uv_varying.x < r && 1 - uv_varying.y < r) ||
    (distance(uv_varying, vec2(1-r, 1-r))   > r && 1 -  uv_varying.x < r && 1 - uv_varying.y < r)
    )
    {
        a = 0;
    }

    outColor = vec4(color_varying.rgb, a);
}