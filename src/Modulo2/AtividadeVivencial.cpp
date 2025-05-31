#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

GLuint setupShader();
GLuint setupSprite();
GLuint setupBackground();
GLuint loadTexture(const char *filePath, bool isCloudLayer);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

const GLuint WIDTH  = 800;
const GLuint HEIGHT = 800;

float spriteOffsetX = 0.0f;
float spriteOffsetY = 0.0f;

float cloudOffset2 = 0.0f;
float cloudOffset3 = 0.0f;
float cloudOffset4 = 0.0f;
float cloudOffset5 = 0.0f;
float cloudOffset6 = 0.0f;

const float SPRITE_SPEED = 0.001f;
const float CLOUD_SPEED  = 0.001f;

const GLchar *vertexShaderSource = R"(
    #version 400
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec2 texc;
    out vec2 tex_coord;
    uniform vec2 offset;
    uniform vec2 texOffset;

    void main()
    {
        tex_coord = texc + texOffset;
        gl_Position = vec4(position.xy + offset, position.z, 1.0);
    }
)";

const GLchar *fragmentShaderSource = R"(
    #version 400
    in vec2 tex_coord;
    out vec4 color;
    uniform sampler2D tex_buff;
    void main()
    {
        color = texture(tex_buff, tex_coord);
    }
)";

int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_SAMPLES, 8);
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Sprite + Nuvens em Movimento Sem Tiling", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { glfwTerminate(); return -1; }
    glViewport(0, 0, WIDTH, HEIGHT);

    GLuint shaderProgram = setupShader();
    GLuint spriteVAO    = setupSprite();
    GLuint bgVAO        = setupBackground();

    GLuint bgTex1    = loadTexture("../assets/backgrounds/1.png", false);
    GLuint bgTex2    = loadTexture("../assets/backgrounds/2.png", true);
    GLuint bgTex3    = loadTexture("../assets/backgrounds/3.png", true);
    GLuint bgTex4    = loadTexture("../assets/backgrounds/4.png", true);
    GLuint bgTex5    = loadTexture("../assets/backgrounds/5.png", true);
    GLuint bgTex6    = loadTexture("../assets/backgrounds/6.png", true);
    GLuint vampirTex = loadTexture("../assets/sprites/Vampirinho.png", false);

    if (!bgTex1 || !bgTex2 || !bgTex3 || !bgTex4 || !bgTex5 || !bgTex6 || !vampirTex)
    {
        cout << "Falha ao carregar textura" << endl;
        glfwTerminate();
        return -1;
    }

    glUseProgram(shaderProgram);
    GLint texUniform      = glGetUniformLocation(shaderProgram, "tex_buff");
    glUniform1i(texUniform, 0);

    GLint offsetLocation    = glGetUniformLocation(shaderProgram, "offset");
    GLint texOffsetLocation = glGetUniformLocation(shaderProgram, "texOffset");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        // seta pra esquerda
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            spriteOffsetX -= SPRITE_SPEED;
            cloudOffset2  += CLOUD_SPEED;
            cloudOffset3  += CLOUD_SPEED * 1.2f;
            cloudOffset4  += CLOUD_SPEED * 1.4f;
            cloudOffset5  += CLOUD_SPEED * 1.6f;
            cloudOffset6  += CLOUD_SPEED * 1.8f;
        }
        // seta pra direita
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            spriteOffsetX += SPRITE_SPEED;
            cloudOffset2  -= CLOUD_SPEED;
            cloudOffset3  -= CLOUD_SPEED * 1.2f;
            cloudOffset4  -= CLOUD_SPEED * 1.4f;
            cloudOffset5  -= CLOUD_SPEED * 1.6f;
            cloudOffset6  -= CLOUD_SPEED * 1.8f;
        }
        // seta pra cima = só sobe o sprite, nuvens ficam no mesmo lugar
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            spriteOffsetY += SPRITE_SPEED;
        }
        // seta pra baixo = só desce sprite
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            spriteOffsetY -= SPRITE_SPEED;
        }

        if (spriteOffsetX >  1.0f) spriteOffsetX = 1.0f;
        if (spriteOffsetX < -1.0f) spriteOffsetX = -1.0f;
        if (spriteOffsetY >  1.0f) spriteOffsetY = 1.0f;
        if (spriteOffsetY < -1.0f) spriteOffsetY = -1.0f;

        auto clampOffset = [](float &val) {
            if (val >  1.0f) val = 1.0f;
            if (val < -1.0f) val = -1.0f;
        };
        clampOffset(cloudOffset2);
        clampOffset(cloudOffset3);
        clampOffset(cloudOffset4);
        clampOffset(cloudOffset5);
        clampOffset(cloudOffset6);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // fundo base
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(bgVAO);
        glBindTexture(GL_TEXTURE_2D, bgTex1);
        glUniform2f(offsetLocation,    0.0f, 0.0f);
        glUniform2f(texOffsetLocation, 0.0f, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // nuvem 2
        glBindTexture(GL_TEXTURE_2D, bgTex2);
        glUniform2f(offsetLocation,    0.0f, 0.0f);
        glUniform2f(texOffsetLocation, cloudOffset2, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // nuvem 3
        glBindTexture(GL_TEXTURE_2D, bgTex3);
        glUniform2f(offsetLocation,    0.0f, 0.0f);
        glUniform2f(texOffsetLocation, cloudOffset3, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // nuvem 4
        glBindTexture(GL_TEXTURE_2D, bgTex4);
        glUniform2f(offsetLocation,    0.0f, 0.0f);
        glUniform2f(texOffsetLocation, cloudOffset4, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // nuvem 5
        glBindTexture(GL_TEXTURE_2D, bgTex5);
        glUniform2f(offsetLocation,    0.0f, 0.0f);
        glUniform2f(texOffsetLocation, cloudOffset5, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // nuvem 6
        glBindTexture(GL_TEXTURE_2D, bgTex6);
        glUniform2f(offsetLocation,    0.0f, 0.0f);
        glUniform2f(texOffsetLocation, cloudOffset6, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // desenha o vampirinho
        glBindVertexArray(spriteVAO);
        glBindTexture(GL_TEXTURE_2D, vampirTex);
        glUniform2f(offsetLocation,    spriteOffsetX, spriteOffsetY);
        glUniform2f(texOffsetLocation, 0.0f, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &bgVAO);
    glDeleteVertexArrays(1, &spriteVAO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &bgTex1);
    glDeleteTextures(1, &bgTex2);
    glDeleteTextures(1, &bgTex3);
    glDeleteTextures(1, &bgTex4);
    glDeleteTextures(1, &bgTex5);
    glDeleteTextures(1, &bgTex6);
    glDeleteTextures(1, &vampirTex);

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

GLuint setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        cout << "Falha no vertex shader: " << infoLog << endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        cout << "Falha no fragment shader: " << infoLog << endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        cout << "Falha no link do shader: " << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint setupBackground()
{
    GLfloat bgVertices[] = {
        -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  0.0f,  1.0f, 0.0f
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgVertices), bgVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

GLuint setupSprite()
{
    GLfloat vertices[] = {
        -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.0f,  1.0f, 0.0f
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

GLuint loadTexture(const char *filePath, bool isCloudLayer)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if (isCloudLayer) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)      format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        else                      format = GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Falha ao carregar textura: " << filePath << endl;
        stbi_image_free(data);
        return 0;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}
