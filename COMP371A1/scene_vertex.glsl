#version 330 core

uniform vec3 view_position;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 light_view_proj_matrix;

out vec3 fragment_normal;
out vec3 fragment_position;
out vec4 fragment_position_light_space;
out vec3 vertexColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
uniform int index = 0;
void main()
{
  

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
    Normal = mat3(transpose(inverse(model_matrix))) * normals;
    FragPos = (model_matrix * vec4(position, 1.0)).xyz;
    fragment_normal = mat3(model_matrix) * normals;
    fragment_position = vec3(model_matrix * vec4(position, 1.0));
    fragment_position_light_space = light_view_proj_matrix * vec4(fragment_position, 1.0);
    TexCoord = aTexCoord;
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0);
}
