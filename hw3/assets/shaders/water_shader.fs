#version 330 core

out vec4 o_frag_color;

in vec3 Position;
in vec4 ClipCoords;
in vec2 TexCoords;
in vec4 ProjectorSpaceCoords;

uniform sampler2D water_normal;
uniform sampler2D water_dudv;
uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;
uniform vec3 sunPosition;
uniform vec3 projectorPosition;
uniform vec3 projectorDirection;
uniform float projectorAngle;
uniform float windFactor;
uniform vec3 cameraPosition;

void main()
{
    float specularStrength = 1.5;
    float windStrength = 0.05;    

    vec2 _dudv = texture(water_dudv, vec2(TexCoords.x + windFactor, TexCoords.y)).xy * 0.1;
    _dudv = TexCoords + vec2(_dudv.x, _dudv.y + windFactor);
    vec2 dudv = (texture(water_dudv, _dudv).xy * 2.0 - 1.0) * windStrength;

    vec4 normalMap = texture(water_normal, _dudv);
    vec3 Normal = normalize(vec3(normalMap.x * 2.0 - 1.0, normalMap.y, normalMap.z * 2.0 - 1.0));

    vec3 color = vec3(1, 1, 1);
    float projectorAttenuation = 0.25;

    vec3 projectorRay = Position - projectorPosition;
    float pointAngle = dot(normalize(projectorRay), normalize(projectorDirection));
    projectorAttenuation = 1.0 / (1.0 + projectorAttenuation * pow(length(projectorRay), 3));
    if (pointAngle < cos(projectorAngle)) {
        projectorAttenuation = 0;
    }

    vec3 sunAmbient = 0.1 * color;
    vec3 sunDirection = normalize(vec3(1, -0.5, -1));
    vec3 sunDiffuse = max(dot(normalize(Normal), normalize(vec3(1, 0.8, 1))), 0.0) * 0.7 * color;

    vec3 projectorDiffuse = vec3(0.3, 0.3, 0.3);

    vec3 I = normalize(cameraPosition - Position);    

    vec3 sunSpecular = pow(max(dot(I, reflect(-sunDirection, normalize(Normal))), 0.0), 10) * specularStrength * color;

    vec3 result = clamp(sunAmbient + sunDiffuse + projectorAttenuation * projectorDiffuse, 0.0, 1.0);
    

    vec2 texCoords = ClipCoords.xy / ClipCoords.w / 2 + 0.5;
    texCoords.x *= -1;
    texCoords = mod(mod(texCoords, 1.0) + 1.0, 1.0);
    texCoords = clamp(texCoords, 0.001, 0.999);
    
    
    vec2 ndc = (ClipCoords.xy / ClipCoords.w) / 2.0 + 0.5;
    vec2 R = clamp(vec2(ndc.x, 1 - ndc.y) + dudv, 0.001, 0.999);

    vec4 waterColor = vec4(texture(reflection_texture, texCoords).rgb, 1.0);
    o_frag_color = 
        vec4(result * mix(vec4(texture(reflection_texture, R).rgb + sunSpecular, 1.0)
        , vec4(116.0 / 255, 204.0 / 255, 244.0 / 255, 1.0), 0.2).xyz
        , 1.0);

    o_frag_color = clamp(o_frag_color, 0.0, 1.0);
}
