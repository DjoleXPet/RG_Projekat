//
// Created by djole on 14.3.22..
//

#ifndef RG_PROJEKAT_SHADER_H
#define RG_PROJEKAT_SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

std::string readFileContents(std::string path);

class Shader
{
public:
    unsigned int m_shaderProgramId;

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
    {
        std::string vertexPathString(vertexPath);
        std::string fragmentPathString(fragmentPath);

        vertexPath = vertexPathString.c_str();
        fragmentPath= fragmentPathString.c_str();
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if(geometryPath != nullptr)
            {
                std::string geometryPathString(geometryPath);
                geometryPath = geometryPathString.c_str();
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if(geometryPath != nullptr)
        {
            const char * gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // shader Program
        m_shaderProgramId = glCreateProgram();
        glAttachShader(m_shaderProgramId, vertex);
        glAttachShader(m_shaderProgramId, fragment);
        if(geometryPath != nullptr)
            glAttachShader(m_shaderProgramId, geometry);
        glLinkProgram(m_shaderProgramId);
        checkCompileErrors(m_shaderProgramId, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if(geometryPath != nullptr)
            glDeleteShader(geometry);

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(m_shaderProgramId);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------

    void setUniform4f(const char* varName, float v0, float v1, float v2, float v3){
        int varId = glGetUniformLocation(m_shaderProgramId, varName);
        glUniform4f(varId, v0, v1, v2, v3);
    }
    void setUniform3f(const char* varName, float v0, float v1, float v2){
        int varId = glGetUniformLocation(m_shaderProgramId, varName);
        glUniform3f(varId, v0, v1, v2);
    }
    void setInt(const char *varName, int i0) {
        int varId = glGetUniformLocation(m_shaderProgramId, varName);
        glUniform1d(varId, i0);
    }
    void setUniformMat4(const char* varName, glm::mat4 transform){
        int varId = glGetUniformLocation(m_shaderProgramId, varName);
        glUniformMatrix4fv(varId, 1, GL_FALSE, glm::value_ptr(transform));
    }


    void deleteProgram(){
        glDeleteProgram(m_shaderProgramId);
    }

    void setUniform3f(const char *varName, glm::vec3 vector) {
        int varId = glGetUniformLocation(m_shaderProgramId, varName);
        glUniform3f(varId, vector[0], vector[1], vector[2]);
    }

    void setFloat(const char *varName, float f0) {
        int varId = glGetUniformLocation(m_shaderProgramId, varName);
        glUniform1f(varId, f0);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//
//class Shader {
////    unsigned m_shaderProgramId;
//public:
//    unsigned m_shaderProgramId;
//    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
//    void use();
//    void deleteProgram();
//    void setUniform4f(const char* varName, float v0, float v1, float v2, float v3);
//    void setUniform3f(const char* varName, float v0, float v1, float v2);
//    void setUniform3f(const char* varName, glm::vec3 vector);
//    void setInt(const char* varName, int i0);
//    void setUniformMat4(const char* varName, glm::mat4);
//    void setFloat(const char* varName, float f0);
//
//};
//
//#include <glad/glad.h>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//
//std::string readFileContents(const char* path){
//    std::ifstream in(path);
//    std::stringstream ss;
//    ss << in.rdbuf();
//    return ss.str();
//}
//
//Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
//{
//    std::string vertexPathString(vertexPath);
//    std::string fragmentPathString(fragmentPath);
//
//    vertexPath = vertexPathString.c_str();
//    fragmentPath= fragmentPathString.c_str();
//    // 1. retrieve the vertex/fragment source code from filePath
//    std::string vertexCode;
//    std::string fragmentCode;
//    std::string geometryCode;
//    std::ifstream vShaderFile;
//    std::ifstream fShaderFile;
//    std::ifstream gShaderFile;
//    // ensure ifstream objects can throw exceptions:
//    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//    gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
//    try
//    {
//        // open files
//        vShaderFile.open(vertexPath);
//        fShaderFile.open(fragmentPath);
//        std::stringstream vShaderStream, fShaderStream;
//        // read file's buffer contents into streams
//        vShaderStream << vShaderFile.rdbuf();
//        fShaderStream << fShaderFile.rdbuf();
//        // close file handlers
//        vShaderFile.close();
//        fShaderFile.close();
//        // convert stream into string
//        vertexCode = vShaderStream.str();
//        fragmentCode = fShaderStream.str();
//        // if geometry shader path is present, also load a geometry shader
//        if(geometryPath != nullptr)
//        {
//            std::string geometryPathString(geometryPath);
//            geometryPath = geometryPathString.c_str();
//            gShaderFile.open(geometryPath);
//            std::stringstream gShaderStream;
//            gShaderStream << gShaderFile.rdbuf();
//            gShaderFile.close();
//            geometryCode = gShaderStream.str();
//        }
//    }
//    catch (std::ifstream::failure& e)
//    {
//        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
//    }
//    const char* vShaderCode = vertexCode.c_str();
//    const char * fShaderCode = fragmentCode.c_str();
//    // 2. compile shaders
//    unsigned int vertex, fragment;
//    // vertex shader
//    vertex = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertex, 1, &vShaderCode, NULL);
//    glCompileShader(vertex);
//    checkCompileErrors(vertex, "VERTEX");
//    // fragment Shader
//    fragment = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragment, 1, &fShaderCode, NULL);
//    glCompileShader(fragment);
//    checkCompileErrors(fragment, "FRAGMENT");
//    // if geometry shader is given, compile geometry shader
//    unsigned int geometry;
//    if(geometryPath != nullptr)
//    {
//        const char * gShaderCode = geometryCode.c_str();
//        geometry = glCreateShader(GL_GEOMETRY_SHADER);
//        glShaderSource(geometry, 1, &gShaderCode, NULL);
//        glCompileShader(geometry);
//        checkCompileErrors(geometry, "GEOMETRY");
//    }
//    // shader Program
//    ID = glCreateProgram();
//    glAttachShader(ID, vertex);
//    glAttachShader(ID, fragment);
//    if(geometryPath != nullptr)
//        glAttachShader(ID, geometry);
//    glLinkProgram(ID);
//    checkCompileErrors(ID, "PROGRAM");
//    // delete the shaders as they're linked into our program now and no longer necessery
//    glDeleteShader(vertex);
//    glDeleteShader(fragment);
//    if(geometryPath != nullptr)
//        glDeleteShader(geometry);
//
//}



std::string readFileContents(std::string path) {
    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}


#endif //RG_PROJEKAT_SHADER_H
