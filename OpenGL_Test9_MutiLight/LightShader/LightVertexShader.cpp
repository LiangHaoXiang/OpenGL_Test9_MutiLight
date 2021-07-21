#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;   //片段的坐标位置
out vec3 Normal;    //片段的法向量

out vec2 TexCoords;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    //使用inverse和transpose函数自己生成这个法线矩阵
    //法线矩阵让法向量转换在世界空间坐标中
    //inverse函数：得到逆矩阵。      transpose函数：得到转置矩阵
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    TexCoords = aTexCoords;
}
