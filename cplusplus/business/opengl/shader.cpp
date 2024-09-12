#include "shader.h"

#include <midware/define/pathdefine.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef Q_OS_WIN32
#include <minwindef.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Shader::Shader()
{

}

bool Shader::createShaderProgram(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    // vertexShader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    if(!checkCompileErrors(vertexShader, "vertexShader"))
    {
        return false;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    if(!checkCompileErrors(fragmentShader, "vertexShader"))
    {
        return false;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    if(!checkLinkErrors(shaderProgram, "shaderProgram"))
    {
        return false;
    }

    m_shaderProgram = shaderProgram;

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

bool Shader::addTexture(const char *filename)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_textureArray.append(texture);

    int width = 0;
    int height = 0;
    int nrChannels = 0;

    stbi_set_flip_vertically_on_load(true); // 加载图片时翻转y轴
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        DEBUGPREFIX << "failed to load picture:" << QString(filename);
        return false;
    }

    stbi_image_free(data);

    return true;
}

void Shader::drawAllTexture()
{
    int size = m_textureArray.size();
    for(int i = 0; i < size; ++i)
    {
        // 在绑定纹理之前先激活纹理单元(纹理单元GL_TEXTURE0默认总是被激活)
        glActiveTexture(GL_TEXTURE0 + i);
        // 激活纹理单元之后，接下来的glBindTexture函数调用会绑定这个纹理到当前激活的纹理单元
        glBindTexture(GL_TEXTURE_2D, m_textureArray.at(i));
    }
}

int Shader::checkCompileErrors(unsigned int shader, const QString& name)
{
    int success = 0;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        constexpr int Info_Log_Size = 512;
        char infoLog[Info_Log_Size];
        glGetShaderInfoLog(shader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << name << "COMPILATION_FAILED" << infoLog;
    }

    return success;
}

int Shader::checkLinkErrors(unsigned int program, const QString &name)
{
    int success = 0;

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        constexpr int Info_Log_Size = 512;
        char infoLog[Info_Log_Size];
        glGetProgramInfoLog(program, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << name << "LINKING_FAILED" << infoLog;

    }

    return success;
}



