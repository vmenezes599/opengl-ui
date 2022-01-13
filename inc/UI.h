#pragma once

#include <glad.h>

#include <stdexcept>

#include "linmath.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <vector>

class UI {
 public:
  UI(GLubyte* texture = nullptr, int width = 2560, int height = 1440)
      : texture(texture), width(width), height(height) {
    if (!glfwInit()) throw std::runtime_error("Failed to init glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(width, height, "Fuk my life", nullptr, nullptr);
    if (!window) throw std::runtime_error("Failed to create glfw window.");

    glfwMakeContextCurrent(window);

    gladLoadGL();

    glfwSwapInterval(1);

    glEnable(GL_TEXTURE_2D);

    std::ifstream vfile("vs.vert", std::ios::ate);
    if (!vfile.is_open()) {
      throw std::runtime_error(
          "Failed to open file: "
          "shaders/vs.vert");
    }

    size_t fileSize = static_cast<size_t>(vfile.tellg());

    std::vector<char> vertex_shader_buffer(fileSize);
    vfile.seekg(0);
    vfile.read(vertex_shader_buffer.data(), fileSize);

    vfile.close();

    std::ifstream ffile("fs.frag", std::ios::ate);
    if (!ffile.is_open()) {
      throw std::runtime_error(
          "Failed to open file: "
          "shaders/fs.frag");
    }

    fileSize = static_cast<size_t>(ffile.tellg());

    std::vector<char> fragment_shader_buffer(fileSize);
    ffile.seekg(0);
    ffile.read(fragment_shader_buffer.data(), fileSize);

    ffile.close();

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertex_data_text = vertex_shader_buffer.data();
    int sz = vertex_shader_buffer.size();
    glShaderSource(vertex_shader, 1, &vertex_data_text, &sz);
    glCompileShader(vertex_shader);
    GLint isCompiled = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
      GLint maxLength = 0;
      glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::vector<GLchar> errorLog(maxLength);
      glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]);

      // Provide the infolog in whatever manor you deem best.
      // Exit with failure.
      glDeleteShader(vertex_shader);  // Don't leak the shader.
      throw std::runtime_error("Vertex shader error:\n" +
                               std::string(errorLog.data()));
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragment_shader_text = fragment_shader_buffer.data();
    sz = fragment_shader_buffer.size();
    glShaderSource(fragment_shader, 1, &fragment_shader_text, &sz);
    glCompileShader(fragment_shader);
    isCompiled = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
      GLint maxLength = 0;
      glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

      // The maxLength includes the NULL character
      std::vector<GLchar> errorLog(maxLength);
      glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &errorLog[0]);

      // Provide the infolog in whatever manor you deem best.
      // Exit with failure.
      glDeleteShader(fragment_shader);  // Don't leak the shader.
      throw std::runtime_error("Fragment shader error:\n" +
                               std::string(errorLog.data()));
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGenVertexArrays(4, &VAO);

    Run();
  }
  UI(const UI& other) = delete;
  ~UI() {
    glfwDestroyWindow(window);

    glfwTerminate();
  }

  void Run() {
    glUseProgram(program);

    if (texture == nullptr) {
      texture = new GLubyte[3 * width * height];
      for (int i = 0; i < 3 * width * height; i += 3) {
        if (i > 1.51 * width * height) {
          texture[i + 0] = 255.0;
          texture[i + 1] = 0.0;
          texture[i + 2] = 0.0;
        } else if (i < 1.49 * width * height) {
          texture[i + 0] = 255.0;
          texture[i + 1] = 255.0;
          texture[i + 2] = 0.0;
        } else {
          texture[i + 0] = 255.0;
          texture[i + 1] = 255.0;
          texture[i + 2] = 255.0;
        }
      }
    }

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, texture);

    // glActiveTexture(texture);
    GLint testTextureLocation = glGetUniformLocation(program, "testTexture");
    glUniform1i(testTextureLocation, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    while (!glfwWindowShouldClose(window)) {
      float ratio;
      int width, height;
      mat4x4 m, p, mvp;

      glfwGetFramebufferSize(window, &width, &height);
      ratio = width / (float)height;

      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);

      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 0.5f, -1.f);

      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
    delete[] texture;
  }

 private:
  GLubyte* texture = nullptr;
  int width, height;
  GLFWwindow* window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program, VAO;
  GLint mvp_location, vpos_location, vcol_location;
};