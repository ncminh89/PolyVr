#version 120

#extension GL_ARB_texture_rectangle : require
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect     texBufPos;
uniform sampler2DRect     texBufNorm;
uniform sampler2DRect     texBufDiff;
uniform vec2              vpOffset;
uniform int               channel;

vec3 pos;
vec3 norm;
vec4 color = vec4(0);

vec4 OSG_SSME_FP_calcShadow(in vec4 ecFragPos);

void computeSpotLight() {
    vec3  lightDirUN = gl_LightSource[0].position.xyz - pos;
    vec3  lightDir   = normalize(lightDirUN);
    float spotEffect = dot(-lightDir, gl_LightSource[0].spotDirection);

    if(spotEffect > gl_LightSource[0].spotCosCutoff) {
        float NdotL = max(dot(lightDir, norm), 0.);
        
        if(NdotL > 0.) {
            vec4  shadow    = OSG_SSME_FP_calcShadow(vec4(pos, 1.));
            float lightDist = length(lightDirUN);
            float att       = dot(vec3(gl_LightSource[0].constantAttenuation,
                                       gl_LightSource[0].linearAttenuation,
                                       gl_LightSource[0].quadraticAttenuation),
                                  vec3(1., lightDist, lightDist * lightDist)       );
            spotEffect = pow(spotEffect, gl_LightSource[0].spotExponent);
            att        = spotEffect / att;
            color = shadow * att * NdotL * color * gl_LightSource[0].diffuse;
        } else color = vec4(0);
    } else color = vec4(0);
}

void main(void) {
    vec2 lookup = gl_FragCoord.xy - vpOffset;
    norm = texture2DRect(texBufNorm, lookup).xyz;

    if (dot(norm, norm) < 0.95) discard;
    else {
        vec4  posAmb = texture2DRect(texBufPos,  lookup);
        pos = posAmb.xyz;
        color = texture2DRect(texBufDiff, lookup);

	if (channel == 0) computeSpotLight();
	if (channel == 1) color = vec4(posAmb.xyz, 1.0);
	if (channel == 2) color = vec4(norm.xyz, 1.0);
	if (channel == 3) color = vec4(color.xyz, 1.0);
        gl_FragColor = color;
    }
}
