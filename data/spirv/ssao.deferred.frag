#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform pcb {
   mat4 Proj;
   vec2 noise_scale;
   int num_samples;
   float radius;
   float bias;
   float intensity;
} PCB;

const int MAX_SAMPLES = 64;
layout(std140, set = 0, binding = 0) uniform SamplesUBO {
   vec3 samples[MAX_SAMPLES];
} S;

layout(set = 1, binding = 0) uniform sampler2D tex_position;
layout(set = 1, binding = 1) uniform sampler2D tex_normal;
layout(set = 1, binding = 2) uniform sampler2D tex_noise;

layout(location = 0) in vec2 in_uv;

layout(location = 0) out float out_ssao;

void main()
{
   /* Retrieve Position and Normal of the current pixel */
   vec3 position = texture(tex_position, in_uv).xyz;
   vec3 normal = texture(tex_normal, in_uv).xyz;

   /* Compute a rotated TBN transform based on the noise vector */
   vec3 noise = texture(tex_noise, in_uv * PCB.noise_scale).xyz;
   vec3 tangent = normalize(noise - normal * dot(noise, normal));
   vec3 bitangent = cross(normal, tangent);
   mat3 TBN = mat3(tangent, bitangent, normal);

   /* Accumulate occlusion for each kernel sample */
   float occlusion = 0.0;
   for(int i = 0; i < PCB.num_samples; ++i) {
      /* Compute sample position in view space */
      vec3 sample_i = TBN * S.samples[i];
      sample_i = position + sample_i * PCB.radius;

      /* Compute sample XY coordinates in NDC space */
      vec4 sample_i_clip = PCB.Proj * vec4(sample_i, 1.0);
      vec2 sample_i_ndc = sample_i_clip.xy / sample_i_clip.w;

      /* Convert sample XY to texture coordinate space and sample from the
       * Position texture to obtain the scene depth at that XY coordinate
       */
      vec2 sample_i_uv = sample_i_ndc * 0.5 + 0.5;
      float ref_depth = texture(tex_position, sample_i_uv.xy).z;

      /* If the depth for that XY position in the scene is larger than
       * the sample's, then the sample is occluded by scene's geometry and
       * contributes to the occlussion factor.
       */
      if (ref_depth >= sample_i.z + PCB.bias)
         occlusion +=
            smoothstep(0.0, 1.0, PCB.radius / abs(position.z - ref_depth));
   }

   /* We output ambient intensity in the range [0,1] */
   out_ssao = pow(1.0 - (occlusion / PCB.num_samples), PCB.intensity);
}
