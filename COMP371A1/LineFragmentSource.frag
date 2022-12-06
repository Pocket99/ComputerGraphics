#version 330 core
in vec2 TexCoord;
in vec3 vertexColor;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;
out vec4 FragColor;

uniform vec3 cameraPos;
uniform vec3 myColor = vec3(1.0f);
const float PI = 3.1415926535897932384626433832795;
const float shading_ambient_strength    = 0.1;
const float shading_diffuse_strength    = 0.6;
const float shading_specular_strength   = 0.3;
struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 

struct LightPoint {
    vec3 pos;
    vec3 color;
    vec3 dirToLight;
    float constant;
    float linear;
    float quadratic;
};

in vec3 fragment_position;
in vec4 fragment_position_light_space;
in vec3 fragment_normal;
uniform sampler2D shadow_map;
uniform float light_cutoff_outer;
uniform float light_cutoff_inner;
uniform float nearPlane;
uniform float farPlane;

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



uniform Material material;
uniform LightPoint lightP;
uniform vec3 lightPos;
uniform int isTextured = 0;

float calculateShadow() {
    // this function returns 1.0 when the surface receives light, and 0.0 when it is in a shadow
    // perform perspective divide
    vec3 normalized_device_coordinates = fragment_position_light_space.xyz / fragment_position_light_space.w;
    // transform to [0,1] range
    normalized_device_coordinates = normalized_device_coordinates * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragment_position_light_space as coords)
    float closest_depth = texture(shadow_map, normalized_device_coordinates.xy).r;
    // get depth of current fragment from light's perspective
    float current_depth = normalized_device_coordinates.z;

//    float bias = 0.003;  // bias applied in depth map: see shadow_vertex.glsl
//    return ((current_depth - bias) < closest_depth) ? 1.0 : 0.0;
    // check whether current frag pos is in shadow
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightP.pos - FragPos);
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
vec3 ambient_color(vec3 light_color_arg) {
    return shading_ambient_strength * light_color_arg;
}

vec3 diffuse_color(vec3 light_color_arg, vec3 light_position_arg) {
    vec3 light_direction = normalize(light_position_arg - fragment_position);
    return shading_diffuse_strength * light_color_arg * max(dot(normalize(fragment_normal), light_direction), 0.0f);
}

vec3 specular_color(vec3 light_color_arg, vec3 light_position_arg) {
    vec3 light_direction = normalize(light_position_arg - fragment_position);
    vec3 view_direction = normalize(cameraPos - fragment_position);
    vec3 reflect_light_direction = reflect(-light_direction, normalize(fragment_normal));
    return shading_specular_strength * light_color_arg * pow(max(dot(reflect_light_direction, view_direction), 0.0f),32);
}

vec3 getPointLightEffect(LightPoint light, vec3 normal, vec3 dirToCamara, float shadow) {
    vec3 diffuseTexture,specularTexture;

    if(toggleGround == 1){
        diffuseTexture = texture(gridMap, TexCoord).rgb;

    }else if(toggleNose == 1){
        diffuseTexture = texture(noseMap, TexCoord).rgb;

    }else if(toggleEye == 1){
        diffuseTexture = texture(eyeMap, TexCoord).rgb;

    }else if(toggleHat == 1){
        diffuseTexture = texture(hatMap, TexCoord).rgb;

    }else if(toggleArm == 1){
        diffuseTexture = texture(armMap, TexCoord).rgb;

    }else{
        diffuseTexture = vec3(1);
    }
    diffuseTexture = vec3(1);
    specularTexture = vec3(1);

    vec3 dirToLight = normalize(light.pos - FragPos);
    // attenuation
    float dist = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear* dist + light.quadratic * (dist * dist));

    //diffuse
    float diffuseIntensity = max(dot(dirToLight, normal), 0);// * attenuation;
    vec3 diffuseColor = diffuseIntensity * light.color * diffuseTexture;

    //specular
    vec3 reflectVec = reflect(-dirToLight, normal);
    float specularIntensity = pow(max(dot(reflectVec, dirToCamara), 0.0), 32) ;//* attenuation;
    vec3 specularColor = specularIntensity * light.color * specularTexture;

    //ambient
    vec3 ambient = shading_ambient_strength * diffuseTexture;
    float shadowStrength = 0.85;
    return  ambient + (1.0 - shadow * shadowStrength) * (diffuseColor + specularColor);
}

void main(){
//    vec3 lightEffect;
//    vec3 normal = normalize(Normal);
//
//    // shadow
//
//    float shadow = calculateShadow();
//
//    // light
//    vec3 dirToCamara = normalize(cameraPos - FragPos);
//    lightEffect = getPointLightEffect(lightP, normal, dirToCamara, shadow);
//
//
//
//    FragColor = vec4(lightEffect, 1.0);

//    vec3 finalResult = vec3(0,0,0);
//    vec3 uNormal = normalize(Normal);
//    vec3 dirToCamera = normalize(cameraPos - FragPos);
//    
//
//    finalResult += CalclightPoint(lightP, uNormal, dirToCamera);
//    finalResult += vertexColor;
    //FragColor = vec4(finalResult, 1.0);


    vec3 finalResult = vec3(0,0,0);
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


    //attenuation
    float dist = length(lightP.pos - FragPos);
    float attenuation = 1.0 / (lightP.constant + lightP.linear * dist + lightP.quadratic * (dist * dist));

    //Ambient light
    float ambientStrength =0.1;
    vec3 ambientLight = ambientStrength * vec3(1.0f, 1.0f, 1.0f);

    //Diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightP.pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0) ;
    vec3 diffuseColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 diffuse = diff * diffuseColor ;

   //Specular light
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specularColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 specular = specularStrength * spec * specularColor ;
    finalResult += ambientLight + diffuse + specular;




    if(isTextured==1){
        FragColor =  textureColor * vec4(finalResult, 1.0f);
    }
    else{
        FragColor =  vec4(vertexColor,1.0f) * vec4(finalResult, 1.0f);
    }

}
