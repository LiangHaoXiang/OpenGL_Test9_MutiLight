#version 330 core
out vec4 FragColor;
//材质属性结构
struct Material {
    sampler2D diffuse;      //漫反射贴图
    sampler2D specular;     //镜面反射贴图
    float shininess;    //反光度
};

//光源属性结构
struct Light {
    vec3 position;
    vec3 direction;     //平行光照射方向
    vec3 ambient;       //环境光
    vec3 diffuse;       //漫反射
    vec3 specular;      //镜面反射
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;        //片段的坐标位置
in vec3 Normal;         //片段的法向量
in vec2 TexCoords;

uniform Material material;  //材质
uniform Light light;        //光源
uniform vec3 viewPos;
uniform int useType;    //使用哪种光源

void main()
{
    //平行光
    if (useType == 1)
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
    //点光源
    else if (useType == 2)
    {
        //环境光
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
        //漫反射光
        vec3 norm = normalize(Normal);  //标准化法向量
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0); //向量点乘 得到cos余弦值
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
        //镜面反射光
        vec3 viewDir = normalize(viewPos - FragPos);    //观察方向
        vec3 reflectDir = reflect(-lightDir, norm);     //反射光方向
        float specularStrength = 0.5;   //镜面强度
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);   //pow是次幂函数：x的y次方
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        
        float distance = length(light.position - FragPos);//距离
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));//光线衰减公式
        //各种光的分量相加
        vec3 result = (ambient + diffuse + specular) * attenuation;
        FragColor = vec4(result, 1.0);
    }
}
