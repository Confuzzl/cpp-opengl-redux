#pragma once

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace callback {
void error(int error_code, const char *description);

void framebufferSize(GLFWwindow *window, int width, int height);
void windowSize(GLFWwindow *window, int width, int height);

void APIENTRY debug(GLenum source, GLenum type, GLuint id, GLenum severity,
                    GLsizei length, const GLchar *message,
                    const void *userParam);
} // namespace callback