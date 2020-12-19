#version 330 core

out vec4 o_frag_color;

in vec3 Position;
in vec2 TexCoords;
in vec4 LightPos1;
in vec4 LightPos2;
in vec4 LightPos3;
in float z;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform float threshold1;
uniform float threshold2;
uniform sampler2D shadowMap1;
uniform sampler2D shadowMap2;
uniform sampler2D shadowMap3;

uniform vec3 sunPosition;
uniform vec3 projectorPosition;
uniform vec3 projectorDirection;
uniform float projectorAngle;

uniform float plane1;
uniform float plane2;
uniform float plane3;

uniform vec3 cameraPosition;


float calc_shadow(int i, vec3 lightPos, vec3 Normal, vec3 sunDirection) {
    vec3 projCoords = lightPos * 0.5 + 0.5;
    float closestDepth = 0;

    if (i == 1) {
        closestDepth = texture(shadowMap1, projCoords.xy).r;
    } else if (i == 2) {
        closestDepth = texture(shadowMap2, projCoords.xy).r;
    } else if (i == 3) {
        closestDepth = texture(shadowMap3, projCoords.xy).r;
    }

    float currentDepth = projCoords.z;

    float bias = max(0.005 * (1.0 - dot(Normal, sunDirection)), 0.0005);
    float shadow = 0.0;
    vec2 texelSize = vec2(0, 0);
    if (i == 1) {
        texelSize = 1.0 / textureSize(shadowMap1, 0);
    } else if (i == 2) {
        texelSize = 1.0 / textureSize(shadowMap2, 0);
    } else if (i == 3) {
        texelSize = 1.0 / textureSize(shadowMap3, 0);
    }

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcfDepth = 0;
            if (i == 1) {
                pcfDepth =texture(shadowMap1, projCoords.xy + vec2(x, y) * texelSize).r;
            } else if (i == 2) {
                pcfDepth = texture(shadowMap2, projCoords.xy + vec2(x, y) * texelSize).r;
            } else if (i == 3) {
               pcfDepth = texture(shadowMap3, projCoords.xy + vec2(x, y) * texelSize).r;
            }
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

void main()
{
    vec3 Normal = vec3(0, 1, 0);

    vec3 sunColor = vec3(1, 1, 1);
    vec3 projectorColor = vec3(1, 1, 1);

    vec3 projectorRay = Position - projectorPosition;
    float pointAngle = dot(normalize(projectorRay), normalize(projectorDirection));
    float projectorAttenuation = 1.0 / (1.0 + 0.06 * pow(length(projectorRay), 3));
    if (pointAngle < cos(projectorAngle)) {
        projectorAttenuation = 0;
    }

    vec3 sunAmbient = 0.6 * sunColor;
    vec3 sunDirection = normalize(sunPosition);
    vec3 sunDiffuse = max(dot(normalize(Normal), sunDirection), 0.0) * 0.7 * sunColor;

    vec3 projectorDiffuse = max(dot(normalize(Normal), -projectorRay), 0.0) * projectorColor * projectorAttenuation * 0.7;
    vec3 I = normalize(cameraPosition - Position);
    vec3 sunSpecular = pow(max(dot(I, reflect(-sunDirection, normalize(Normal))), 0.0), 16) * sunColor;

    vec3 lightPos;
    int i;
    if (z <= plane1) {
        lightPos = LightPos1.xyz;
        i = 1;
    } else if (z <= plane2) {
       lightPos = LightPos2.xyz;
       i = 2;
    } else {
        lightPos = LightPos3.xyz;
        i = 3;
    }

    float shadow = calc_shadow(i, lightPos, Normal, sunDirection);

    vec4 result = vec4(clamp(sunAmbient + projectorDiffuse + (1.0 - shadow) * sunDiffuse, 0.0, 1.0), 1.0);

    vec3 color1 = texture(texture1, TexCoords).rgb;
    vec3 color2 = texture(texture2, TexCoords).rgb;
    vec3 color3 = texture(texture3, TexCoords).rgb;
    
    float thresh_size_2 = (threshold2 - threshold1) / 2;
    float thresh_mid = (threshold2 + threshold1) / 2;
    float alpha = min(1, max(0, abs(thresh_mid - Position.y) / thresh_size_2));

    if (Position.y < thresh_mid) {
        o_frag_color = vec4(mix(color2, color1, 1 - pow(1 - alpha, 5)), 1);
    } else {
        o_frag_color = vec4(mix(color2, color3, pow(alpha, 5)), 1);
    }

    o_frag_color *= result;
}
