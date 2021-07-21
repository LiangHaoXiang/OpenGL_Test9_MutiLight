#version 330 core
//材质属性结构
struct Material {
    sampler2D diffuse;      //漫反射贴图
    sampler2D specular;     //镜面反射贴图
    float shininess;    //反光度
};

uniform Material material;  //材质
in vec2 TexCoords;

//光源属性结构
struct Light {
    vec3 direction;     //平行光照射方向
    vec3 ambient;       //环境光
    vec3 diffuse;       //漫反射
    vec3 specular;      //镜面反射
};

uniform Light light;        //光源

out vec4 FragColor;

in vec3 FragPos;        //片段的坐标位置
in vec3 Normal;         //片段的法向量

void main()
{
    //环境光
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    //漫反射光
    vec3 norm = normalize(Normal);  //标准化法向量
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0); //向量点乘 得到cos余弦值
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    //镜面反射光
    vec3 viewDir = normalize(-FragPos);    //观察方向
    vec3 reflectDir = reflect(-lightDir, norm);     //反射光方向
    float specularStrength = 0.5;   //镜面强度
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);   //pow是次幂函数：x的y次方
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    //各种光的分量相加
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
