#include "learnopengl.h"

#include "shader.h"

#include <QtConcurrent>
#include <midware/define/pathdefine.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef Q_OS_WIN32
#include <minwindef.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

#ifdef Q_OS_WIN

extern "C" {
// specify the Nvidia to render
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
// specify the AMD to render
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#elif defined(Q_OS_LINUX)

extern "C" {
// specify the Nvidia to render
__attribute__((visibility("default"))) unsigned int NvOptimusEnablement = 0x00000001;
// specify the AMD to render
__attribute__((visibility("default"))) int AmdPowerXpressRequestHighPerformance = 1;
}

#endif


// 顶点数组对象：Vertex Array Object，VAO
// 顶点缓冲对象：Vertex Buffer Object，VBO
// 元素缓冲对象：Element Buffer Object，EBO 或 索引缓冲对象 Index Buffer Object，IBO

#define USE_KEY_CALL_BACK 1

static float mixValue = 0.2f;

//标准的45度FoV和16:9长宽比
static constexpr float gl_aspectRatioArray[] = {
    16.0f/9.0f,
    21.0f/9.0f,
    4.0f/3.0f
};

static float gl_fov = 45.0f;

static unsigned char aspectRatioIdx = 0;

static float radius = 10.0f;

inline float GetAspectRatio(){ return gl_aspectRatioArray[aspectRatioIdx]; }

inline auto GetProjection(){
    return glm::perspective(glm::radians(gl_fov), GetAspectRatio(), 0.1f, 100.0f);
}

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

static glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
static glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    if (width > 0 && height > 0) {
        // 只有当宽度和高度都大于0时，才设置视口
        glViewport(0, 0, width, height);
        DEBUGPREFIX << "Setting viewport to " << width << "*" << height;
    } else {
        // 窗口可能已被最小化或发生了其他异常情况
        DEBUGPREFIX << "Framebuffer size callback called with invalid dimensions: " << width << "*" << height;
    }
}

void processInput(GLFWwindow *window)
{
    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

/* 键盘回调函数 */
void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    float cameraSpeed = 0.08f; // adjust accordingly

    constexpr bool isRepeat = true;
    if (action == GLFW_PRESS || (isRepeat && action == GLFW_REPEAT))
    {
        switch(key)
        {
        case GLFW_KEY_ESCAPE:
        {
            glfwSetWindowShouldClose(window, true); // 按下ESC键时关闭窗口
        }
        break;
        case GLFW_KEY_UP:
        {
            // mixValue += 0.001f;
            // if(mixValue >= 1.0f) mixValue = 1.0f;

            // gl_fov += 15.0f;
            radius++;
        }
        break;
        case GLFW_KEY_DOWN:
        {
            // mixValue -= 0.001f;
            // if (mixValue <= 0.0f) mixValue = 0.0f;

            // gl_fov -= 15.0f;
            radius--;
        }
        break;
        case GLFW_KEY_LEFT:
        {
            if(aspectRatioIdx > 0)
            {
                aspectRatioIdx--;
            }
        }
        break;
        case GLFW_KEY_RIGHT:
        {
            constexpr unsigned char bound = sizeof(gl_aspectRatioArray) / sizeof(gl_aspectRatioArray[0]) - 1;
            if(aspectRatioIdx < bound)
            {
                aspectRatioIdx++;
            }
        }
        break;
        case GLFW_KEY_W:
        {
            cameraPos += cameraSpeed * cameraFront;
        }
        break;
        case GLFW_KEY_S:
        {
            cameraPos -= cameraSpeed * cameraFront;
        }
        break;
        case GLFW_KEY_A:
        {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        break;
        case GLFW_KEY_D:
        {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        break;
        default:
            break;
        }
        //cameraPos.y = 0.0f;
        DEBUGPREFIX << cameraPos.x << cameraPos.y << cameraPos.z;
    }
    else if (action == GLFW_RELEASE)
    {

    }
    else
    {

    }


}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            // 隐藏光标
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (action == GLFW_RELEASE)
        {

        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            double cursorX;
            double cursorY;
            // 获取当前光标位置
            glfwGetCursorPos(window, &cursorX, &cursorY);

            DEBUGPREFIX << cursorX << cursorY;
            // 恢复光标
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            // 恢复光标后光标的位置会变为光标隐藏之前的位置，所以需要重新设置为当前的位置
            glfwSetCursorPos(window, cursorX, cursorY);

            glfwGetCursorPos(window, &cursorX, &cursorY);
            DEBUGPREFIX << cursorX << cursorY;
        }
        else if (action == GLFW_RELEASE)
        {

        }
    }
}


void mouseCallback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scrollCallback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
{
    gl_fov -= (float)yoffset;
    if (gl_fov < 1.0f)
        gl_fov = 1.0f;
    if (gl_fov > 45.0f)
        gl_fov = 45.0f;
}

GLFWwindow* CreateWindow(int width, int length, const char* title);

void MoveWindowToCenter(GLFWwindow* window);

int ShowWindow();

int drawTriangle();

int drawRectangle();

int drawTwoTrianglesByArray();

int drawTwoTrianglesSeparately();

int drawColorfulTriangle();

int drawTextures();

int drawTextures3D();

void initOpengl(int majorVersion = 3, int minorVersion = 3);

LearnOpengl::LearnOpengl(QObject *parent)
    : QObject{parent}
{
    QFutureWatcher<int>* watcher = new QFutureWatcher<int>(this);
    connect(watcher, &QFutureWatcher<int>::finished, watcher, [watcher](){
        DEBUGPREFIX << "opengl exit:" << watcher->result();
        QCoreApplication::quit();
    });

    watcher->setFuture(QtConcurrent::run(drawTextures3D));

    char a[10]={0,1,2,3,4,5,6,7};
    int*p=(int*)a;
    DEBUGPREFIX << *p;
    auto cc = *(short int*)(p++);
    int k = 6;
    int q =  sizeof(k + 1);
    DEBUGPREFIX << q << sizeof(k + 1) << cc << *p;
    DEBUGPREFIX << k;
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

    int nrAttributes = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    DEBUGPREFIX << "Maximum nr of vertex attributes supported: " << nrAttributes;

    glViewport(0, 0, width, length);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

#if USE_KEY_CALL_BACK
    glfwSetKeyCallback(window, keyCallback);
#endif

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    return window;
}

void MoveWindowToCenter(GLFWwindow* window)
{
    // 获取屏幕尺寸
    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int screenWidth = vidmode->width;
    int screenHeight = vidmode->height;

    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // 计算窗口中心位置
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    // 设置窗口位置
    glfwSetWindowPos(window, posX, posY);
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
        // //检测并处理用户按键输入
        // processInput(window);
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

    GLFWwindow* window = CreateWindow(800, 600, "drawTextures");
    if(nullptr == window)
    {
        return -1;
    }

    MoveWindowToCenter(window);

    // 顶点着色器
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"

        "out vec3 ourColor;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 transform;\n"

        "void main()\n"
        "{\n"
        "    gl_Position = transform * vec4(aPos, 1.0f);\n"
        "    ourColor = aColor;\n"
        "    TexCoord = aTexCoord;\n"
        "}\n\0";


// 片段着色器
#if 0
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"

        "in vec3 ourColor;\n"
        "in vec2 TexCoord;\n"

        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(ourTexture, vec2(1-TexCoord.x, TexCoord.y)) * vec4(ourColor, 0.6);\n"
        "}\n\0";
#else
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"

        "in vec3 ourColor;\n"
        "in vec2 TexCoord;\n"

        "uniform float mixValue;\n"
        "uniform sampler2D texture1;\n"
        "uniform sampler2D texture2;\n"
        "void main()\n"
        "{\n"
        // linearly interpolate between both textures (20% texture2, 80% texture1)
        "   FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);\n"
        "}\n\0";
#endif
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

    // add texture1
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /*
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

    // add texture2
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    iconPath = (Get_APP_ICON("1.jpg")).toUtf8();

    stbi_set_flip_vertically_on_load(true); // 加载图片时翻转y轴

    data = stbi_load(iconPath.data(), &width, &height, &nrChannels, 0);
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
*/
    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);


#if 0
    float vertices[] = {
        // positions          // colors           // texture coords (note that we changed them to 2.0f!)
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f  // top left
    };
#else
    float vertices[] = {
        // positions          // colors           // texture coords (note that we changed them to 'zoom in' on our texture image)
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.75f, 0.75f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.75f, 0.25f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.25f, 0.25f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.25f, 0.75f  // top left
    };

#endif

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

    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 IdentityMatrix = glm::mat4(1.0f);
    glm::mat4 trans = glm::translate(IdentityMatrix, glm::vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;
    DEBUGPREFIX << vec.x << vec.y << vec.z << vec.w;

    // trans = glm::rotate(IdentityMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.4f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        /*
        // 在绑定纹理之前先激活纹理单元(纹理单元GL_TEXTURE0默认总是被激活)
        glActiveTexture(GL_TEXTURE0);
        // 激活纹理单元之后，接下来的glBindTexture函数调用会绑定这个纹理到当前激活的纹理单元
        glBindTexture(GL_TEXTURE_2D, texture1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
*/
        // trans = glm::translate(IdentityMatrix, glm::vec3(0.5f, -0.5f, 0.0f));
        // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        trans = glm::rotate(IdentityMatrix, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)); // Switched the order
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f)); // Switched the order

        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glUniform1f(glGetUniformLocation(shaderProgram, "mixValue"), mixValue);
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // second transformation
        // ---------------------
        float scaleAmount = static_cast<float>(sin(glfwGetTime()));
        trans = glm::scale(IdentityMatrix, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &trans[0][0]); // this time take the matrix value array's first element as its memory pointer value

        // now with the uniform matrix being replaced with new transformations, draw it again.
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

int drawTextures3D()
{
    initOpengl();

    GLFWwindow* window = CreateWindow(800, 600, "drawTextures");
    if(nullptr == window)
    {
        return -1;
    }

    MoveWindowToCenter(window);

    // 顶点着色器
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"

        "out vec3 ourColor;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"

        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
        "    TexCoord = aTexCoord;\n"
        "}\n\0";


// 片段着色器
#if 0
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"

        "in vec3 ourColor;\n"
        "in vec2 TexCoord;\n"

        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(ourTexture, vec2(1-TexCoord.x, TexCoord.y)) * vec4(ourColor, 0.6);\n"
        "}\n\0";
#else
    const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"

        "in vec2 TexCoord;\n"

        "uniform float mixValue;\n"
        "uniform sampler2D texture1;\n"
        "uniform sampler2D texture2;\n"
        "void main()\n"
        "{\n"
        // linearly interpolate between both textures (20% texture2, 80% texture1)
        "   FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);\n"
        "}\n\0";
#endif


    Shader shader;
    shader.createShaderProgram(vertexShaderSource, fragmentShaderSource);

    QByteArray iconPath = (APP_ICON).toUtf8();
    shader.addTexture(iconPath.data());

    iconPath = (Get_APP_ICON("1.jpg")).toUtf8();
    shader.addTexture(iconPath.data());

    unsigned int shaderProgram = shader.shaderProgram();

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

#if 0
    float vertices[] = {
        // positions          // colors           // texture coords (note that we changed them to 2.0f!)
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f  // top left
    };
#elif 0
    float vertices[] = {
        // positions          // colors           // texture coords (note that we changed them to 'zoom in' on our texture image)
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.75f, 0.75f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.75f, 0.25f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.25f, 0.25f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.25f, 0.75f  // top left
    };
#else
    float vertices[] = {
        // positions          // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
#endif

    unsigned int VBO = 0;
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the vertex array object first, the bind and set vertex buffer(s), and then configure vertex attributes(s)
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL or GL_LINE, default mode is GL_FILL

    glEnable(GL_DEPTH_TEST);

    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    while (!glfwWindowShouldClose(window))
    {
#if !USE_KEY_CALL_BACK
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
#endif

        glClearColor(0.4f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.drawAllTexture();

        glUseProgram(shaderProgram);

        static glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };

        static const glm::mat4 IdentityMatrix(1.0f);

        //glm::mat4 model = glm::rotate(IdentityMatrix, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //glm::mat4 model = glm::rotate(IdentityMatrix, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // 注意，我们将矩阵向我们要进行移动场景的反方向移动。
        //glm::mat4 view = glm::translate(IdentityMatrix, glm::vec3(0.0f, 0.0f, -3.0f));

        // float camX = sin(glfwGetTime()) * radius;
        // float camZ = cos(glfwGetTime()) * radius;

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        //glm::mat4 projection = glm::perspective(glm::radians(45.0f), windowWidth / static_cast<float>(windowHeight), 0.1f, 100.0f);

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniform1f(glGetUniformLocation(shaderProgram, "mixValue"), mixValue);

        glBindVertexArray(VAO);

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 projection = GetProjection();
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

            glm::mat4 model = glm::translate(IdentityMatrix, cubePositions[i]);
            float angle = 20.0f * i + 20;

            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            constexpr int verticesCount = sizeof(vertices) / sizeof(vertices[0]) / 5;
            glDrawArrays(GL_TRIANGLES, 0, verticesCount);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;

}

