#version 330 core

const float PI = 3.1415926535897932384626433832795;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 light_direction;

uniform vec3 object_color;

const float shading_ambient_strength    = 0.1;
const float shading_diffuse_strength    = 0.6;
const float shading_specular_strength   = 0.3;

uniform float light_cutoff_outer;
uniform float light_cutoff_inner;
uniform float light_near_plane;
uniform float light_far_plane;

uniform vec3 view_position;

uniform sampler2D shadow_map;

uniform int toggleGround = 0;
uniform int toggleNose = 0;
uniform int toggleEye = 0;
uniform int toggleHat = 0;
uniform int toggleArm = 0;

uniform sampler2D noseMap;
uniform sampler2D gridMap;
uniform sampler2D eyeMap;
uniform sampler2D hatMap;
uniform sampler2D armMap;

uniform int isTextured = 0;
uniform int isShadowed = 1;
in vec3 fragment_position;
in vec4 fragment_position_light_space;
in vec3 fragment_normal;
in vec3 vertexColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 gl_FragCoord;

out vec4 result;

vec3 ambient_color(vec3 light_color_arg) {
    return shading_ambient_strength * light_color_arg;
}

vec3 diffuse_color(vec3 light_color_arg, vec3 light_position_arg) {
    vec3 light_direction = normalize(light_position_arg - fragment_position);
    return shading_diffuse_strength * light_color_arg * max(dot(normalize(fragment_normal), light_direction), 0.0f);
}

vec3 specular_color(vec3 light_color_arg, vec3 light_position_arg) {
    vec3 light_direction = normalize(light_position_arg - fragment_position);
    vec3 view_direction = normalize(view_position - fragment_position);
    vec3 reflect_light_direction = reflect(-light_direction, normalize(fragment_normal));
    return shading_specular_strength * light_color_arg * pow(max(dot(reflect_light_direction, view_direction), 0.0f),32);
}

float shadow_scalar() {
    // this function returns 1.0 when the surface receives light, and 0.0 when it is in a shadow
    // perform perspective divide
    vec3 normalized_device_coordinates = fragment_position_light_space.xyz / fragment_position_light_space.w;
    // transform to [0,1] range
    normalized_device_coordinates = normalized_device_coordinates * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragment_position_light_space as coords)
    float closest_depth = texture(shadow_map, normalized_device_coordinates.xy).r;
    // get depth of current fragment from light's perspective
    float current_depth = normalized_device_coordinates.z;
    // check whether current frag pos is in shadow
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light_position- FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_map, normalized_device_coordinates.xy + vec2(x, y) * texelSize).r; 
            shadow += current_depth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(normalized_device_coordinates.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

float spotlight_scalar() {
    float theta = dot(normalize(fragment_position - light_position), light_direction);
    
    if(theta > light_cutoff_inner) {
        return 1.0;
    } else if(theta > light_cutoff_outer) {
        return (1.0 - cos(PI * (theta - light_cutoff_outer) / (light_cutoff_inner - light_cutoff_outer))) / 2.0;
    } else {
        return 0.0;
    }
}

void main()
{
    vec4 textureColor;
    if(toggleGround == 1){
        textureColor = texture(gridMap, TexCoord);

    }else if(toggleNose == 1){
        textureColor = texture(noseMap, TexCoord);

    }else if(toggleEye == 1){
        textureColor = texture(eyeMap, TexCoord);

    }else if(toggleHat == 1){
        textureColor = texture(hatMap, TexCoord);

    }else if(toggleArm == 1){
        textureColor = texture(armMap, TexCoord);

    }else{
        textureColor = vec4(1,1,1,1);
    }
    vec3 ambient = vec3(0.0f);
    vec3 diffuse = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    float scalar;

    if(isShadowed == 1){
        scalar = 1-shadow_scalar();
    }else{
        scalar = 1.0f;
    }

    ambient = ambient_color(light_color) ;


    diffuse = scalar * diffuse_color(light_color, light_position);
    specular = scalar * specular_color(light_color, light_position);
    
    vec3 color = (specular + diffuse + ambient)  ;
    

     if(isTextured==1){
        result =   textureColor * vec4(color, 1.0f) ;
    }
    else{
        result =  vec4(vertexColor,1.0f) * vec4(color, 1.0f);
    }

}

