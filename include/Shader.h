#include <GL/glew.h>

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>
#include <string.h>
using namespace std;
namespace VCTII
{
    class Shader
    {
    public:
        static std::string LoadShaderFile(const char* shaderPath) {
            std::string shaderCode;
            std::ifstream shaderStream(shaderPath, std::ios::in);
            if (shaderStream.is_open()) {
                std::string Line = "";
                while (getline(shaderStream, Line))
                    shaderCode += "\n" + Line;
                shaderStream.close();
            }
            else {
                std::cout << "Open Shader failed! " << shaderPath << std::endl;
                getchar();
                exit(-1); // Terminate the program
            }
            return shaderCode;
        }

        static GLuint CompileShader(GLuint shader, const std::string& shaderCode) {
            GLint compileStatus = GL_FALSE;
            int infoLogLength;
            const char* shaderSourcePointer = shaderCode.c_str();
            glShaderSource(shader, 1, &shaderSourcePointer, NULL);
            glCompileShader(shader);
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0) {
                std::vector<char> errorInfo(infoLogLength + 1);
                glGetShaderInfoLog(shader, infoLogLength, NULL, &errorInfo[0]);
                std::cout << errorInfo[0] << std::endl;
            }
            return shader;
        }

        static GLuint LoadShaders(const char* vertPath, const char* fragPath, const char* geomPath = nullptr) {
            // Create the shaders
            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            GLuint geometryShader;
            if (geomPath) {
                geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            }

            // Read the Shader code from the file
            std::string vertexCode = LoadShaderFile(vertPath);
            std::string fragCode = LoadShaderFile(fragPath);
            std::string geomCode;
            if (geomPath) {
                geomCode = LoadShaderFile(geomPath);
            }

            // Compile Shaders
            std::cout << "Compiling shader :" << vertPath << std::endl;
            CompileShader(vertexShader, vertexCode);
            std::cout << "Compiling shader :" << fragPath << std::endl;
            CompileShader(fragmentShader, fragCode);
            if (geomPath) {
                std::cout << "Compiling shader :" << geomPath << std::endl;
                CompileShader(geometryShader, geomCode);
            }

            // Link the program
            std::cout << "Linking......." << std::endl;
            GLuint program = glCreateProgram();
            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            if (geomPath) {
                glAttachShader(program, geometryShader);
            }
            glLinkProgram(program);

            // Check the program
            GLint compileStatus = GL_FALSE;
            int infoLogLength;
            glGetProgramiv(program, GL_LINK_STATUS, &compileStatus);
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0) {
                std::vector<char> programErrorInfo(infoLogLength + 1);
                glGetProgramInfoLog(program, infoLogLength, NULL, &programErrorInfo[0]);
                std::cout << programErrorInfo[0] << std::endl;
            }

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            if (geomPath) {
                glDeleteShader(geometryShader);
            }

            return program;
        }
    };
}
