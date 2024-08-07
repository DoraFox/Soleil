#include "learnopengl.h"
#include "midware/define/pathdefine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <midware/define/basedefine.h>


// 顶点数组对象：Vertex Array Object，VAO
// 顶点缓冲对象：Vertex Buffer Object，VBO
// 元素缓冲对象：Element Buffer Object，EBO 或 索引缓冲对象 Index Buffer Object，IBO



void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


GLFWwindow* CreateWindow(int width, int length, const char* title);


int ShowWindow();

int drawTriangle();

int drawRectangle();

int drawTwoTrianglesByArray();

int drawTwoTrianglesSeparately();

int drawColorfulTriangle();

int drawTextures();

void initOpengl(int majorVersion = 3, int minorVersion = 3);

LearnOpengl::LearnOpengl(QObject *parent)
    : QObject{parent}
{
    QFutureWatcher<int>* watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcher<int>::finished, watcher, [watcher](){
        DEBUGPREFIX << "opengl exit:" << watcher->result();
        QCoreApplication::quit();
    });

    watcher->setFuture(QtConcurrent::run(drawTextures));

}

void initOpengl(int majorVersion, int minorVersion)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* CreateWindow(int width, int length, const char* title)
{
    GLFWwindow* window = glfwCreateWindow(width, length, title, NULL, NULL);
    if (window == NULL)
    {
        DEBUGPREFIX << "Failed to create GLFW window";
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        DEBUGPREFIX << "Failed to initialize GLAD";
        return nullptr;
    }

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    DEBUGPREFIX << "Maximum nr of vertex attributes supported: " << nrAttributes;


    glViewport(0, 0, width, length);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

int ShowWindow()
{
    initOpengl();
    GLFWwindow* window = CreateWindow(800, 600, "LearnOpenGL");
    if(nullptr == window)
    {
        return -1;
    }

    while(!glfwWindowShouldClose(window))
    {
        //检测并处理用户按键输入
        processInput(window);
        //交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
        glfwSwapBuffers(window);
        //检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glfwTerminate();

    return 0;
}

int drawTriangle()
{
    initOpengl();
    GLFWwindow* window = CreateWindow(800, 600, "DrawTrigle");
    if(nullptr == window)
    {
        return -1;
    }

    // 顶点着色器
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform float xOffset;\n"
        "uniform float yOffset;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x + xOffset, aPos.y + yOffset, aPos.z, 1.0);\n"
        "}\n\0";


    // 片段着色器
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 ourColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = ourColor;\n"
        "}\n\0";

    // 顶点着色器
    unsigned int vertexShader;  // 顶点着色器的ID
    vertexShader = glCreateShader(GL_VERTEX_SHADER);  // 创建一个着色器：GL_VERTEX_SHADER 代表顶点着色器
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const int Info_Log_Size = 512;
    int success;
    char infoLog[Info_Log_Size];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // 片段着色器
    unsigned int fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // 着色器程序：用于链接着色器
    unsigned int shaderProgram = 0;
    shaderProgram = glCreateProgram();
    // 把着色器附加到了程序上，然后用glLinkProgram 链接
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::PROGRAM::LINKINGFAILED" << infoLog;
        return -1;
    }
    // 在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    // 创建一个顶点数组对象
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    // 绑定VAO
    glBindVertexArray(VAO);

    // 创建一个顶点缓冲对象
    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 将新创建的缓冲绑定到GL_ARRAY_BUFFER 目标上
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 把之前定义的顶点数据复制到缓冲的内存中

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    //glBindVertexArray(0);

    // draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Loop until the user closes the window // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 输入
        processInput(window);

        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //设置清空屏幕所用的颜色
        glClear(GL_COLOR_BUFFER_BIT); // 清除颜色缓冲，并且整个颜色缓冲都会被填充为glClearColor里所设置的颜色

        // be sure to activate the shader before any calls to glUniform
        glUseProgram(shaderProgram);
/*
        // update shader uniform
        double timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        if(vertexColorLocation != -1)
        {
            glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        }

        float xOffset = static_cast<float>(sin(timeValue) / 2.0);
        int vertexXOffsetLocation = glGetUniformLocation(shaderProgram, "xOffset");
        if(vertexXOffsetLocation != -1)
        {
            glUniform1f(vertexXOffsetLocation, xOffset);
        }

        float yOffset = static_cast<float>(cos(timeValue) / 2.0);
        int vertexYOffsetLocation = glGetUniformLocation(shaderProgram, "yOffset");
        if(vertexYOffsetLocation != -1)
        {
            glUniform1f(vertexYOffsetLocation, yOffset);
        }
*/
        //glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        // 检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
        glfwPollEvents();

    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}

int drawRectangle()
{
    initOpengl();
    GLFWwindow* window = CreateWindow(800, 600, "DrawTrigle");
    if(nullptr == window)
    {
        return -1;
    }

    // 顶点着色器
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\n\0";


    // 片段着色器
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(0.26f, 0.56f, 0.90f, 1.00f);\n"
        "}\n\0";

    // vertexShader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const int Info_Log_Size = 512;
    int success;
    char infoLog[Info_Log_Size];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER:VERTEX::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << infoLog;
        return -1;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
    };

    unsigned int indices[] = {
        // 注意索引从0开始!
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the vertex array object first, the bind and set vertex buffer(s), and then configure vertex attributes(s)
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL or GL_LINE, default mode is GL_FILL

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;

}

int drawTwoTrianglesByArray()
{
    initOpengl();
    GLFWwindow* window = CreateWindow(800, 600, "DrawTwoTrianglesByArray");
    if(nullptr == window)
    {
        return -1;
    }

    // 顶点着色器
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\n\0";


    // 片段着色器
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(0.26f, 0.56f, 0.90f, 1.00f);\n"
        "}\n\0";

    // 顶点着色器
    unsigned int vertexShader;  // 顶点着色器的ID
    vertexShader = glCreateShader(GL_VERTEX_SHADER);  // 创建一个着色器：GL_VERTEX_SHADER 代表顶点着色器
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const int Info_Log_Size = 512;
    int success;
    char infoLog[Info_Log_Size];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // 片段着色器
    unsigned int fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // 着色器程序：用于链接着色器
    unsigned int shaderProgram = 0;
    shaderProgram = glCreateProgram();
    // 把着色器附加到了程序上，然后用glLinkProgram 链接
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::PROGRAM::LINKINGFAILED" << infoLog;
        return -1;
    }
    // 在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,  // left
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top
        // second triangle
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top
    };


    // 创建一个顶点数组对象
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // 绑定VAO
    glBindVertexArray(VAO);

    // 创建一个顶点缓冲对象
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 将新创建的缓冲绑定到GL_ARRAY_BUFFER 目标上
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 把之前定义的顶点数据复制到缓冲的内存中

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    //glBindVertexArray(0);

    // draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Loop until the user closes the window // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 输入
        processInput(window);

        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //设置清空屏幕所用的颜色
        glClear(GL_COLOR_BUFFER_BIT); // 清除颜色缓冲，并且整个颜色缓冲都会被填充为glClearColor里所设置的颜色

        glUseProgram(shaderProgram);
        //glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        // 检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
    return 0;
}

int drawTwoTrianglesSeparately()
{
    initOpengl();
    GLFWwindow* window = CreateWindow(800, 600, "DrawTwoTrianglesByArray");
    if(nullptr == window)
    {
        return -1;
    }

    // 顶点着色器
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\n\0";


    // 片段着色器
    const char *fragmentShader1Source =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";
    const char *fragmentShader2Source =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
        "}\n\0";


    // 顶点着色器
    unsigned int vertexShader;  // 顶点着色器的ID
    vertexShader = glCreateShader(GL_VERTEX_SHADER);  // 创建一个着色器：GL_VERTEX_SHADER 代表顶点着色器
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const int Info_Log_Size = 512;
    int success;
    char infoLog[Info_Log_Size];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // 片段着色器
    unsigned int fragmentShader_1 = 0;
    fragmentShader_1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_1, 1, &fragmentShader1Source, NULL);
    glCompileShader(fragmentShader_1);

    glGetShaderiv(fragmentShader_1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader_1, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    unsigned int fragmentShader_2 = 0;
    fragmentShader_2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_2, 1, &fragmentShader2Source, NULL);
    glCompileShader(fragmentShader_2);

    glGetShaderiv(fragmentShader_2, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader_2, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // 着色器程序：用于链接着色器
    unsigned int shaderProgram_1 = 0;
    shaderProgram_1 = glCreateProgram();
    // 把着色器附加到了程序上，然后用glLinkProgram 链接
    glAttachShader(shaderProgram_1, vertexShader);
    glAttachShader(shaderProgram_1, fragmentShader_1);
    glLinkProgram(shaderProgram_1);

    glGetProgramiv(shaderProgram_1, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram_1, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::PROGRAM::LINKINGFAILED" << infoLog;
        return -1;
    }

    unsigned int shaderProgram_2 = 0;
    shaderProgram_2 = glCreateProgram();
    // 把着色器附加到了程序上，然后用glLinkProgram 链接
    glAttachShader(shaderProgram_2, vertexShader);
    glAttachShader(shaderProgram_2, fragmentShader_2);
    glLinkProgram(shaderProgram_2);

    glGetProgramiv(shaderProgram_2, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram_2, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::PROGRAM::LINKINGFAILED" << infoLog;
        return -1;
    }

    // 在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader_1);
    glDeleteShader(fragmentShader_2);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f,
        -0.0f, -0.5f, 0.0f,
        -0.45f, 0.5f, 0.0f
    };

    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f,
        0.9f, -0.5f, 0.0f,
        0.45f, 0.5f, 0.0f
    };

    unsigned int VAOs[2];
    unsigned int VBOs[2];

    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);

    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram_1);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(shaderProgram_2);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteProgram(shaderProgram_1);
    glDeleteProgram(shaderProgram_2);

    glfwTerminate();
    return 0;
}

int drawColorfulTriangle()
{
    initOpengl();
    GLFWwindow* window = CreateWindow(800, 600, "DrawTrigle");
    if(nullptr == window)
    {
        return -1;
    }

    // 顶点着色器
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"   // 位置变量的属性位置值为 0
        "layout (location = 1) in vec3 aColor;\n" // 颜色变量的属性位置值为 1
        "out vec3 ourColor;\n"

        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos, 1.0);\n"
        "    ourColor = aColor;\n"   // 将ourColor设置为我们从顶点数据那里得到的输入颜色
        "}\n\0";


    // 片段着色器
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"

        "void main()\n"
        "{\n"
        "    FragColor = vec4(ourColor, 1.0);\n"
        "}\n\0";

    // 顶点着色器
    unsigned int vertexShader;  // 顶点着色器的ID
    vertexShader = glCreateShader(GL_VERTEX_SHADER);  // 创建一个着色器：GL_VERTEX_SHADER 代表顶点着色器
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const int Info_Log_Size = 512;
    int success;
    char infoLog[Info_Log_Size];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::VERTEX::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // 片段着色器
    unsigned int fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // 着色器程序：用于链接着色器
    unsigned int shaderProgram = 0;
    shaderProgram = glCreateProgram();
    // 把着色器附加到了程序上，然后用glLinkProgram 链接
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::PROGRAM::LINKINGFAILED" << infoLog;
        return -1;
    }
    // 在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        // 位置              // 颜色
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
    };
    // 创建一个顶点数组对象
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    // 绑定VAO
    glBindVertexArray(VAO);

    // 创建一个顶点缓冲对象
    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 将新创建的缓冲绑定到GL_ARRAY_BUFFER 目标上
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // 把之前定义的顶点数据复制到缓冲的内存中

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    //glBindVertexArray(0);

    // draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Loop until the user closes the window // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        // 输入
        processInput(window);

        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //设置清空屏幕所用的颜色
        glClear(GL_COLOR_BUFFER_BIT); // 清除颜色缓冲，并且整个颜色缓冲都会被填充为glClearColor里所设置的颜色

        // be sure to activate the shader before any calls to glUniform
        glUseProgram(shaderProgram);

        // update shader uniform
        double timeValue = glfwGetTime();
        float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        if(vertexColorLocation != -1)
        {
            glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        }

        //glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        // 检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}

int drawTextures()
{
    initOpengl();
    GLFWwindow* window = CreateWindow(800, 600, "DrawTrigle");
    if(nullptr == window)
    {
        return -1;
    }

    // 顶点着色器
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"

        "out vec3 ourColor;\n"
        "out vec2 TexCoord;\n"

        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos, 1.0);\n"
        "    ourColor = aColor;\n"
        "    TexCoord = aTexCoord;\n"
        "}\n\0";


    // 片段着色器
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"

        "in vec3 ourColor;\n"
        "in vec2 TexCoord;\n"

        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(ourTexture, vec2(1-TexCoord.x, TexCoord.y)) * vec4(ourColor, 0.6);"
        "}\n\0";

    // vertexShader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const int Info_Log_Size = 512;
    int success;
    char infoLog[Info_Log_Size];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER:VERTEX::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << infoLog;
        return -1;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, Info_Log_Size, NULL, infoLog);
        DEBUGPREFIX << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << infoLog;
        return -1;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // add texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 0;
    int height = 0;
    int nrChannels = 0;
    QByteArray iconPath = (APP_ICON).toUtf8();

    stbi_set_flip_vertically_on_load(true); // 加载图片时翻转y轴
    unsigned char *data = stbi_load(iconPath.data(), &width, &height, &nrChannels, 0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        DEBUGPREFIX << "failed to load picture:" << iconPath;
        return -1;
    }

    stbi_image_free(data);

    float vertices[] = {
        //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   // 右上
        0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f    // 左上
    };

    unsigned int indices[] = {
        // 注意索引从0开始!
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the vertex array object first, the bind and set vertex buffer(s), and then configure vertex attributes(s)
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL or GL_LINE, default mode is GL_FILL

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 在绑定纹理之前先激活纹理单元(纹理单元GL_TEXTURE0默认总是被激活)
        glActiveTexture(GL_TEXTURE0);
        // 激活纹理单元之后，接下来的glBindTexture函数调用会绑定这个纹理到当前激活的纹理单元
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;

}





















