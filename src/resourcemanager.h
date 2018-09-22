#pragma once
#include "glad/glad.h"
#include <map>
#include <memory>
#include <vector>
#include <string>
#include "texture2d.h"
#include "shader.h"


// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
class ResourceManager
{
public:
    std::map<std::string, std::shared_ptr<Shader>> m_shaderMap;
    std::map<std::string, std::shared_ptr<Texture2D>> m_textureMap;

    static ResourceManager* get() 
    {
        if (!m_instance)
            m_instance = new ResourceManager;
        return m_instance;
    }
    void setSearchPath(const std::vector<std::string>& paths);
    void addSearchPath(const std::string& path);
    std::shared_ptr<Shader> loadShader(const GLchar *vShaderFile, const GLchar* fShaderFile, const GLchar* pShaderFile, const std::string& name);
    std::shared_ptr<Shader> getShader(const std::string& name);
    std::shared_ptr<Texture2D> loadTexture(const GLchar* file, GLboolean alpha, const std::string& name);
    std::shared_ptr<Texture2D> getTexture(const std::string& name);
    void clear();

private:
    ResourceManager();
    Shader* loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
    Texture2D* loadTextureFromFile(const GLchar *file, GLboolean alpha);
    static ResourceManager* m_instance;
    std::vector<std::string> m_searchPath;
    std::string solveResourcePath(const std::string& path);
};