#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "Application.h"

const int windowWidth = 1280;
const int windowHeight = 720;
float timer = 0.0;


int main(void) {

    // Load GLFW and create a window
    if (!glfwInit()) {
        std::cerr << "Failed to InitApp GLFW\n";
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Voxel Cone Tracing Global Illumination", NULL, NULL);

    // Check for Valid Context
    if (window == NULL) {
        std::cerr << "failed to create opengl context.\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPos(window, windowWidth, windowHeight);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "failed to init app \n";
        return EXIT_FAILURE;
    }
    // Initialize other openGL stuff
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Draw front faces only

    double previousTime, currentTime;
    previousTime = glfwGetTime();

    VCTII::Application app(windowWidth, windowHeight, window);
    if (!app.InitApp()) {
        std::cerr << "Failed to Init Application\n";
        return EXIT_FAILURE;
    }

    // Rendering Loop
    while (glfwWindowShouldClose(window) == false) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Update timer
        currentTime = glfwGetTime();
        float deltaTime = float(currentTime - previousTime);
        previousTime = currentTime;

        if (timer > 2.0) {
            std::cout << "FPS: " << 1.0 / deltaTime << std::endl;
            timer = 0.0;
        }
        else {
            timer += deltaTime;
        }

        app.UpdateScene(deltaTime);
        app.DrawScene();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
