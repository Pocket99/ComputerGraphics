#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aColor;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;
uniform int index = 0;


uniform mat4 light_view_proj_matrix;

out vec3 fragment_normal;
out vec3 fragment_position;
out vec4 fragment_position_light_space;

out vec3 vertexColor;
out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;
void main(){
    if( index == 7 ){
        vertexColor = vec3( 1.0f, 0.0f,  0.0f);
    }else if(index == 3 ){
        vertexColor = vec3( 1.0f, 0.5f,  0.6f);
    }else if(index == 8 || index == 9 ){
        vertexColor = vec3( 0.0f, 0.0f,  0.0f);
    }else if(index == 10 || index == 11 || index ==12 ){
        vertexColor = vec3( 1.0f, 1.0f,  0.0f);
    }else{
        vertexColor = vec3( 1.0f, 1.0f,  1.0f);
    }
    

    

    
    FragPos = (modelMat * vec4(aPos, 1.0)).xyz;
   
    TexCoord = aTexCoord;
     Normal = mat3(transpose(inverse(modelMat))) * aNormal;
    FragPos = (modelMat * vec4(aPos.xyz, 1.0)).xyz;
    FragPosLightSpace = light_view_proj_matrix * vec4(FragPos, 1.0);
    gl_Position =  projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
}