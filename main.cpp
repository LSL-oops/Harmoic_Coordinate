#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include "interactions.h"
#include "openGLHelper/Shader.h"
#include "openGLHelper/Camera.h"
#include "kernel.cuh"
#include <iostream>

GLuint pbo = 0;
GLuint tex = 0;
struct cudaGraphicsResource *cuda_pbo_resource;
GLFWwindow *window;

GLuint VAO;
GLuint VBO;
GLuint EBO;

#define ITERS_PER_RENDER 50

Shader *shader;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

GLdouble vertices[] = {
         1.,  1., 0., 1.,1.,  // top right
         1., -1., 0., 1.,0., // bottom right
        -1., -1., 0., 0.,0., // bottom left
        -1.,  1., 0., 0.,1. // top left
};

GLuint indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
};

void initRenderingWindowSystem(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

    window = glfwCreateWindow(W,H,"Harmonic_Coordinate_Demo",NULL,NULL);
    if(window == NULL){
        std::cerr << "FAILED CREATING WINDOW" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
}

void initShaderingSystem(){
    glGenBuffers(1,&pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,pbo);
    glBufferData(GL_PIXEL_UNPACK_BUFFER,W*H*sizeof(GLubyte)*4,0,GL_STREAM_DRAW);

    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D,tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource,pbo,cudaGraphicsMapFlagsWriteDiscard);

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_DOUBLE,GL_FALSE,5*sizeof(GLdouble),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_DOUBLE,GL_FALSE,5*sizeof(GLdouble),(void*)(sizeof(GLdouble)*3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    shader = new Shader("../shader/shader.vert","../shader/shader.frag");
}

void render(){
    uchar4 *d_out = 0;
    cudaGraphicsMapResources(1,&cuda_pbo_resource,0);
    cudaGraphicsResourceGetMappedPointer((void**)&d_out,NULL,cuda_pbo_resource);

    iterationsCount += 1;
    for(int i = 0;i < ITERS_PER_RENDER;++i){
        kernelLauncher(d_out,d_temp,W,H,bc);
    }

    cudaGraphicsUnmapResources(1,&cuda_pbo_resource,0);
    char title[128];
    sprintf(title,"Temperature Visualization - Iterations=%4d, "
                  "T_s=%3.0f, T_a=%3.0f, T_g=%3.0f",
                  iterationsCount,bc.t_s,bc.t_a,bc.t_g);

    glfwSetWindowTitle(window,title);
}

void draw_texture(){
    glBindTexture(GL_TEXTURE_2D,tex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,W,H,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);

    glClearColor(1.f,1.f,1.f,1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader->use();
    shader->setInt("sampler",0);
    shader->setMat4("model",glm::mat4(1.0f));
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),(float)W/(float)H,0.1f,100.f);
    shader->setMat4("projection",projection);

    glm::mat4 view = camera.GetViewMatrix();
    shader->setMat4("view",view);

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
}


void exit_system(){
    if(pbo) {
        cudaGraphicsUnregisterResource(cuda_pbo_resource);
        glDeleteBuffers(1,&pbo);
        glDeleteTextures(1,&tex);
    }

    cudaFree(d_temp);
    delete shader;
    glfwTerminate();
}

int main() {
    initRenderingWindowSystem();

    cudaMalloc(&d_temp,W*H*sizeof(float));
    resetTemperature(d_temp,W,H,bc);
    printInstructions();

    initShaderingSystem();

    while(!glfwWindowShouldClose(window)){
        processInput(window);
        render();
        draw_texture();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    exit_system();
    return 0;
}
