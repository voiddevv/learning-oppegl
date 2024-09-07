#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/gl.h"
#include "glm/vec2.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
const char* vertexSource = R"glsl(
    #version 330 core

    in vec2 texcoord;

    out vec3 Color;
    out vec2 Texcoord;
    in vec2 position;
    uniform mat4 transform;

    void main()
    {
        Texcoord = texcoord;
        gl_Position = transform * vec4(position, 0.0, 0.5);
    }
)glsl";

const char* fragmentSource = R"glsl(
    #version 330 core
    out vec4 frag_color;
    in vec2 Texcoord;
    uniform sampler2D tex;
    void main()
    {
        frag_color = texture(tex,Texcoord);
    }
)glsl";

float vertices[] = {
//  Position      Color             Texcoords
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
};
GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
};


int load_gl(){
    if(gladLoaderLoadGL() == 0){
        fprintf(stderr,"FAILED TO LOAD OPENGL");
        return 1;
    };
    printf("GL VERSION: %s\n",glGetString(GL_VERSION));
    return 0;
}

struct Image {
    int width,height;
    unsigned char * pixels;
    Image(const char* path){
        pixels = stbi_load(path,&width,&height,NULL,4);
        printf("LOADED IMAGE\n");
        printf("SIZE -> %d,%d\n",width,height);
        printf("PIXELS -> %d",(width*height));
    }
};

GLuint create_program(){
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader,1,&vertexSource,NULL);    
    glCompileShader(vertShader);
    GLint vert_status;
    glGetShaderiv(vertShader,GL_COMPILE_STATUS,&vert_status);
    if(vert_status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(vertShader,512,NULL,buffer);
        printf("VERTEX ERROR -> %s",buffer);

    }
    // frag shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader,1,&fragmentSource,NULL);
    glCompileShader(fragShader);
    GLint frag_status;
    glGetShaderiv(fragShader,GL_COMPILE_STATUS,&frag_status);
    if(frag_status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(fragShader,512,NULL,buffer);
        printf("FRAGMENT ERROR -> %s",buffer);
    }
    // program
    GLuint program =  glCreateProgram();
    glAttachShader(program,vertShader);
    glAttachShader(program,fragShader);
    glLinkProgram(program);
    return program;
}

void on_resize(GLFWwindow* window, int width,int height){
    glViewport(0,0,width,height);
}




int main(){
    glfwInit();
    GLFWwindow* win = glfwCreateWindow(1280,720,"window",NULL,NULL);
    glfwMakeContextCurrent(win);
    int load_err = load_gl();
    if(load_err){
        return load_err;
    }
    glfwSetFramebufferSizeCallback(win,on_resize);
    glClearColor(0.7,0.4,0.4,1.0);
    GLuint texturecool;
    
    // bufers
    unsigned int vbo;
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    // get program
    GLuint program = create_program();
    glUseProgram(program);


    glm::mat4 trans = glm::mat4(1.0);
    trans = glm::rotate(trans,glm::radians((float)glfwGetTime()*5.0f),glm::vec3(0.0,0.0,1.0));
    //VAO
    GLuint vao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    GLint posAttrib = glGetAttribLocation(program,"position");
    GLint texAttrib = glGetAttribLocation(program,"texcoord");
    GLint transuni = glGetUniformLocation(program,"transform");
    glUniformMatrix4fv(transuni,1,GL_FALSE,glm::value_ptr(trans));

    // EBO
    GLuint ebo;
    glGenBuffers(1,&ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(elements),elements,GL_STATIC_DRAW);
    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(posAttrib,2,GL_FLOAT,GL_FALSE,sizeof(float)*7,0);
    glVertexAttribPointer(texAttrib,2,GL_FLOAT,GL_FALSE,7*sizeof(float),(void*)(5*sizeof(float)));
    // texture
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    Image *img = new Image("scorched.png");
    glGenTextures(1,&texturecool);
    glBindTexture(GL_TEXTURE_2D,texturecool);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D,0,GL_COMPRESSED_RGBA,img->width,img->height,0,GL_RGBA,GL_UNSIGNED_BYTE,img->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(img->pixels);

    int i = 0;
    while (!glfwWindowShouldClose(win))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        glfwSwapBuffers(win);
        glfwPollEvents();
        trans = glm::rotate(trans,glm::radians(sinf(glfwGetTime())/(float)M_PI/2.0f),glm::vec3(0.0,0.0,1.0));
        GLint transuni = glGetUniformLocation(program,"transform");
        glUniformMatrix4fv(transuni,1,GL_FALSE,glm::value_ptr(trans));
        i ++;
    }
    delete img;
    gladLoaderUnloadGL();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}