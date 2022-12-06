#version 330 core               
//in vec4 vertexColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
//uniform sampler2D ourTexture;
//uniform sampler2D ourFace;
uniform vec3 objColor;
uniform vec3 ambientColor;

uniform vec3 cameraPos;
uniform float matrixlight;
uniform float matrixmove;
out vec4 FragColor;

struct LightDirectional{
    vec3 pos;
    vec3 color;
    vec3 dirToLight;
};

struct LightSpot{
    vec3 pos;
    vec3 color;
    vec3 dirToLight;
    float constant;
    float linear;
    float quadratic;
    float cosPhyInner;
    float cosPhyOutter;
};

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
uniform Material material;
uniform LightSpot lightS;
uniform LightDirectional lightD;
uniform LightPoint lightDFront;
uniform LightPoint lightDBack;
uniform LightPoint lightDLeft;
uniform LightPoint lightDRight;
uniform LightPoint lightP0;
uniform LightPoint lightP1;
uniform LightPoint lightP2;
uniform LightPoint lightP3;

vec3 CalclightDirectional(LightDirectional light, vec3 uNormal, vec3 dirToCamera){
    //diffuse max(dot(L, N),0)
    float diffuseIntensity = max(dot(light.dirToLight, uNormal),0);
    vec3 diffColor = diffuseIntensity * light.color * texture(material.diffuse, TexCoord).rgb;
    
    //specular pow(max(dot(R, Cam),0),shininess)
    vec3 R = normalize(reflect(-light.dirToLight, uNormal));
    float specIntensity = pow(max(dot(R, dirToCamera),0),material.shininess);
    vec3 specColor = specIntensity * light.color;// * texture(material.specular, TexCoord).rgb;
    vec3 result = diffColor + specColor;
    return result;
}

vec3 CalclightPoint(LightPoint light,vec3 uNormal, vec3 dirToCamera){
    //attenuation
    float dist = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
    //diffuse
    float diffuseIntensity = max(dot(normalize(light.pos - FragPos), uNormal),0) ;
    vec3 diffColor = diffuseIntensity * light.color * texture(material.diffuse, TexCoord).rgb;
    //specular
    vec3 R = normalize(reflect(-normalize(light.pos - FragPos), uNormal));
    float specIntensity = pow(max(dot(R, dirToCamera),0),material.shininess) ;
    vec3 specColor = specIntensity * light.color ;//* texture(material.specular, TexCoord).rgb;   
    
    vec3 result = diffColor + specColor;
    return result;
}
vec3 CalclightSpot(LightSpot light,vec3 uNormal, vec3 dirToCamera){
    //attenuation
    float dist = length(light.pos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
    float spotRatio;
    float cosTheta = dot(normalize(FragPos-light.pos),-light.dirToLight);
    if(cosTheta > light.cosPhyInner){
        spotRatio = 1.0f;
    }else if(cosTheta > lightS.cosPhyOutter){
        spotRatio = 1.0f -(cosTheta - lightS.cosPhyInner)/(lightS.cosPhyOutter - lightS.cosPhyInner);
    }else{
        spotRatio = 0.0f;
    }

    //diffuse
    float diffuseIntensity = max(dot(normalize(light.pos - FragPos), uNormal),0) * attenuation * spotRatio;
    vec3 diffColor = diffuseIntensity * light.color * texture(material.diffuse, TexCoord).rgb;
    //specular
    vec3 R = normalize(reflect(-normalize(light.pos - FragPos), uNormal));
    float specIntensity = pow(max(dot(R, dirToCamera),0),material.shininess) * attenuation * spotRatio;
    vec3 specColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;   
    
    vec3 result = diffColor + specColor;

    return result;
}


void main()                     
{   
    vec3 finalResult = vec3(0,0,0);
    vec3 uNormal = normalize(Normal);
    vec3 dirToCamera = normalize(cameraPos - FragPos);
    LightPoint lightP;
    finalResult += CalclightDirectional(lightD, uNormal, dirToCamera);
    finalResult += CalclightPoint(lightDFront, uNormal, dirToCamera);
    finalResult += CalclightPoint(lightDBack, uNormal, dirToCamera);
    finalResult += CalclightPoint(lightDLeft, uNormal, dirToCamera);
    finalResult += CalclightPoint(lightDRight, uNormal, dirToCamera);
    finalResult += CalclightPoint(lightP0, uNormal, dirToCamera);

    //finalResult += CalclightPoint(lightP2, uNormal, dirToCamera);   
    //finalResult += CalclightPoint(lightP3, uNormal, dirToCamera);
    //finalResult += CalclightSpot(lightS, uNormal, dirToCamera);
    FragColor = vec4(finalResult, 1.0);
    //FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
//    float distance    = length(lightPos - FragPos);
//    float attenuation = 1.0 / (lightP.constant + lightP.linear * distance + lightP.quadratic * (distance * distance));
//    //FragColor = vertexColor;
//    //FragColor = mix(texture(ourTexture, TexCoord), texture(ourFace, TexCoord), 0.2 );
//    //FragColor = vec4(objColor * ambientColor, 1.0) * mix(texture(ourTexture, TexCoord), texture(ourFace, TexCoord), 0.2 );
//    
//    vec3 lightDir = normalize(lightDirUniform - FragPos);
//    vec3 reflectVec = reflect(-lightDir, Normal);
//    vec3 cameraVec = normalize(cameraPos - FragPos);
//
//    float specularAmount = pow(max( dot(reflectVec, cameraVec),0),material.shininess);
//      
//    vec3 specular = texture(material.specular, TexCoord).rgb * specularAmount * lightColor;
//    //vec3 diffuse = material.diffuse * max(dot(lightDir, Normal),0) * lightColor;
//    vec3 diffuse = texture(material.diffuse,TexCoord).rgb *  max(dot(lightDir, Normal),0) * lightColor; 
//    vec3 ambient = texture(material.diffuse,TexCoord).rgb * ambientColor;
//    vec3 emission = matrixlight*texture(material.emission,vec2(TexCoord.x,TexCoord.y+matrixmove)).rgb;
//    //FragColor = vec4(objColor * ambientColor, 1.0);
//    
//    float cosTheta = dot(normalize(FragPos - lightPos), -1 * lightDirUniform);
//
//    float spotRation;
//    if(cosTheta > lightS.cosPhyInner){
//        //inside
//        spotRation = 1.0f;
//    }else if(cosTheta > lightS.cosPhyOutter){
//        //middle
//        spotRation = 1.0f -(cosTheta - lightS.cosPhyInner)/(lightS.cosPhyOutter - lightS.cosPhyInner);
//    }else{
//        //outside
//        spotRation = 0.0f;
//       
//    }
//     FragColor = vec4(( ambient + (diffuse + specular) * spotRation) * objColor, 1.0);
//    if(cosTheta > lightS.cosPhy){
//        //FragColor = vec4(( ambient + (diffuse + specular) * attenuation) * objColor, 1.0);
//        FragColor = vec4(( ambient + diffuse + specular) * objColor, 1.0);
//    }else{
//        FragColor = vec4(( ambient ) * objColor, 1.0);
//    }
    
}                               