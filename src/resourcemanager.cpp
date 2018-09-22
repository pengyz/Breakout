#include "resourcemanager.h"
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

ResourceManager* ResourceManager::m_instance = nullptr;

ResourceManager::ResourceManager()
{
    //stbi_set_flip_vertically_on_load(true);
}

std::shared_ptr<Shader> ResourceManager::loadShader(const GLchar * vShaderFile, const GLchar * fShaderFile, const GLchar * pShaderFile, const std::string & name)
{
    m_shaderMap[name] = std::shared_ptr<Shader>(loadShaderFromFile(vShaderFile, fShaderFile, pShaderFile));
    return m_shaderMap[name];
}

std::shared_ptr<Shader> ResourceManager::getShader(const std::string & name)
{
    if (m_shaderMap.find(name) == m_shaderMap.end())
        return std::shared_ptr<Shader>(nullptr);
    return m_shaderMap[name];
}

std::shared_ptr<Texture2D> ResourceManager::loadTexture(const GLchar * file, GLboolean alpha, const std::string & name)
{
    m_textureMap[name] = std::shared_ptr<Texture2D>(loadTextureFromFile(file, alpha));
    return m_textureMap[name];
}

std::shared_ptr<Texture2D> ResourceManager::getTexture(const std::string & name)
{
    if (m_textureMap.find(name) == m_textureMap.end())
        return std::shared_ptr<Texture2D>(nullptr);
    return m_textureMap[name];
}

void ResourceManager::clear()
{
    m_shaderMap.clear();
    m_textureMap.clear();
}

Shader* ResourceManager::loadShaderFromFile(const GLchar * vShaderFile, const GLchar * fShaderFile, const GLchar * gShaderFile)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try {
        // Open files
        std::string vShaderFilePath = solveResourcePath(vShaderFile);
        std::string fShaderFilePath = solveResourcePath(fShaderFile);

        std::ifstream vertexShaderFile(vShaderFilePath);
        std::ifstream fragmentShaderFile(fShaderFilePath);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // If geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr) {
            std::string gShaderFilePath = solveResourcePath(gShaderFile);
            std::ifstream geometryShaderFile(gShaderFilePath);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (std::exception e) {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    // 2. Now create shader object from source code
    Shader* shader = new Shader();
    shader->attachShaderSource(GL_VERTEX_SHADER, vertexCode);
    shader->attachShaderSource(GL_FRAGMENT_SHADER, fragmentCode);
    if (gShaderFile) {
        shader->attachShaderSource(GL_GEOMETRY_SHADER, geometryCode);
    }
    if (!shader->compile()) 
    {
        delete shader;
        return nullptr;
    }
    return shader;
}

Texture2D* ResourceManager::loadTextureFromFile(const GLchar * file, GLboolean alpha)
{
    // Create Texture object
    Texture2D* texture = new Texture2D();
    if (alpha) {
        texture->m_internalFormat = GL_RGBA;
        texture->m_imageFormat = GL_RGBA;
    }
    // Load image
    int width, height, nrChannels;
    std::string filePath = solveResourcePath(file);
    unsigned char* image = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    // Now generate texture
    texture->generate(width, height, image);
    // And finally free image data
    stbi_image_free(image);
    return texture;
}

void ResourceManager::setSearchPath(const std::vector<std::string>& paths)
{
    for (const auto& path : paths) {
        m_searchPath.emplace_back(path);
    }
}

void ResourceManager::addSearchPath(const std::string& path)
{
    m_searchPath.emplace_back(path);
}


std::string ResourceManager::solveResourcePath(const std::string& path)
{
    if (path.find(":") == 0) {
        std::string fileSuffix = path.substr(1);
        //solve it
        for (auto& tmpPath : m_searchPath) {
            std::string finalPath = tmpPath;
            if (finalPath.find_last_of("\\") != finalPath.length() - 1 && finalPath.find_last_of("/") != finalPath.length() - 1 &&
                fileSuffix.find_first_of("\\") != 0 && fileSuffix.find_first_of("/") != 0) {
                finalPath.append("/");
            }
            finalPath.append(fileSuffix);
            if (std::filesystem::exists(std::filesystem::path(finalPath))) {
                return finalPath;
            }
        }
    }
    return path;
}
