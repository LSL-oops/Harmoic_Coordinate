//
// Created by shuliang on 19-7-18.
//

#ifndef HARMONICCOORDINATEDEMO_INTERACTIONS_H
#define HARMONICCOORDINATEDEMO_INTERACTIONS_H

#include <stdio.h>
#include <GLFW/glfw3.h>
#include "kernel.cuh"
#include <iostream>

const unsigned int W = 800;
const unsigned int H = 600;
const GLfloat DT = 1.0f;
GLfloat *d_temp = 0;
int iterationsCount = 0;
BC bc = {W/2,H/2,W/10.f,150,212.f,70.f,0.f};

#define MAX(x,y) ((x) > (y) ? (x) : (y))


void processInput(GLFWwindow *window){
    bool shift_press = (glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS){
        if(shift_press)
            bc.t_s += DT;
        else
            bc.t_s -= DT;
    }
    if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS){
        if(shift_press)
            bc.t_a += DT;
        else
            bc.t_a -= DT;
    }
    if(glfwGetKey(window,GLFW_KEY_G) == GLFW_PRESS){
        if(shift_press)
            bc.t_g += DT;
        else
            bc.t_g -= DT;
    }
    if(glfwGetKey(window,GLFW_KEY_R) == GLFW_PRESS){
        if(shift_press)
            bc.rad += DT;
        else
            bc.rad = MAX(0.f,bc.rad - DT);
    }
    if(glfwGetKey(window,GLFW_KEY_C) == GLFW_PRESS){
        if(shift_press)
            ++bc.chamfer;
        else
            --bc.chamfer;
    }
    if(glfwGetKey(window,GLFW_KEY_Z) == GLFW_PRESS) {
        resetTemperature(d_temp, W, H, bc);
        iterationsCount = 0;
    }
    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window,GLFW_TRUE);
}

void cursor_pos_callback(GLFWwindow *window,int x,int y){
    bc.x = x;
    bc.y = y;
}

void printInstructions(){
    printf("Temperature visualize:\n"
           "Relocate source with mouse click\n"
           "Change source temperature (-/+) : s/S\n"
           "Change air temperature    (-/+) : a/A\n"
           "Change ground temperature (-/+) : g/G\n"
           "Change pipe radius        (-/+) : r/R\n"
           "Change chamfer            (-/+) : c/C\n"
           "Reset to air temperature  (-/+) : z\n"
           "Exit                            : Esc\n");
}

void framebuffer_size_callback(GLFWwindow *window,int width,int height){
    glViewport(0,0,width,height);
}
#endif //HARMONICCOORDINATEDEMO_INTERACTIONS_H
