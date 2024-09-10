#ifndef SHADER_H
#define SHADER_H

#include <midware/define/basedefine.h>
#include <QObject>

class Shader
{
public:
    Shader();

    bool createShaderProgram(const char *vertexShaderSource, const char *fragmentShaderSource);

    bool addTexture(const char *filename);

    void drawAllTexture();


private:

    int checkCompileErrors(unsigned int shader, const QString &name);

    int checkLinkErrors(unsigned int program, const QString &name);


private:

    DECLARE_PROPERTY_READ_ONLY(unsigned int, shaderProgram)

    DECLARE_PROPERTY_READ_ONLY(QVector<unsigned int>, textureArray)

};

#endif // SHADER_H
