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
    vec3 position;      //光源位置  （点光/聚光 变量）
    vec3 direction;     //光照射方向 （平行光/聚光 变量）
    vec3 ambient;       //环境光
    vec3 diffuse;       //漫反射
    vec3 specular;      //镜面反射
    
    float constant;     //点光源常数 c
    float linear;       //点光源一次项常数 l
    float quadratic;    //点光源二次项常数 q
    
    float cutOff;       //内圆锥切光角的余弦值
    float outerCutOff;  //外圆锥切光角的余弦值
};

in vec3 FragPos;        //片段的坐标位置
in vec3 Normal;         //片段的法向量
in vec2 TexCoords;

uniform Material material;  //材质
uniform Light light;        //光源
uniform vec3 viewPos;
uniform int lightType;    //使用哪种光源

vec4 directionLight();
vec4 pointLight();
vec4 spotLight();

void main()
{
    //平行光
    if (lightType == 1)
    {
        FragColor = directionLight();
    }
    //点光源
    else if (lightType == 2)
    {
        FragColor = pointLight();
    }
    //聚光灯
    else if (lightType == 3)
    {
        FragColor = spotLight();
    }
}

vec4 directionLight()
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
    return vec4(result, 1.0);
}

vec4 pointLight()
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
    return vec4(result, 1.0);
}

vec4 spotLight()
{
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    if (theta > light.cutOff)
    {
        // 执行光照计算
        
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
        
        //平滑/软化边缘
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   //聚光强度随内外圆锥余弦值差变化而变化
        //各种光的分量相加
        vec3 result = (ambient + diffuse + specular) * intensity;
        return vec4(result, 1.0);
    }
    else
    {
        // 否则，使用环境光，让场景在聚光之外时不至于完全黑暗
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
        return vec4(ambient, 1.0);
    }
}

