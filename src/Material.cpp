#include <iostream>
#include <assimp/scene.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Material.h"
namespace VCTII
{
	Material::Material() {
		Texture2D tex;
		tex.texWidth = tex.texHeight = tex.textureID = 0;

		diffuseTexture = tex;
		specularTexture = tex;
		heightTexture = tex;
	}

	void Material::LoadMaterial(const aiMaterial* mat, std::string path) {
		aiString name;
		mat->Get(AI_MATKEY_NAME, name);
		materialName = name.data;

		std::cout << "\tLoading material : " << materialName << std::endl;

		aiColor3D color;
		mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
		ambientColor = glm::vec3(color.r, color.g, color.b);

		mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
		specularColor = glm::vec3(color.r, color.g, color.b);

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		diffuseColor = glm::vec3(color.r, color.g, color.b);

		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		emissiveColor = glm::vec3(color.r, color.g, color.b);

		mat->Get(AI_MATKEY_OPACITY, opacity);
		mat->Get(AI_MATKEY_SHININESS, shininess);

		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			diffuseTexture = LoadMaterialTexture(mat, aiTextureType_DIFFUSE, "diffuseTexture", path);
		}

		if (mat->GetTextureCount(aiTextureType_AMBIENT) > 0) {
			specularTexture = LoadMaterialTexture(mat, aiTextureType_AMBIENT, "specularTexture", path);
		}

		if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
			heightTexture = LoadMaterialTexture(mat, aiTextureType_HEIGHT, "heightTexture", path);
		}
	}

	Texture2D Material::LoadMaterialTexture(const aiMaterial* mat, aiTextureType type, std::string typeName, std::string path) {
		std::cout << "\t\t" << typeName << " loaded" << std::endl;
		aiString texturePath;
		if (mat->GetTexture(type, 0, &texturePath) == AI_SUCCESS) {
			std::string fullPath = path + texturePath.data;
			std::replace(fullPath.begin(), fullPath.end(), '\\', '/'); // replace all '\' with '/'
			return LoadTexture(fullPath);
		}
		return Texture2D();
	}

	Texture2D Material::LoadTexture(std::string filenameString) {
		Texture2D tex;

		const char* filename = filenameString.c_str();
		GLubyte* textureData = stbi_load(filename, &tex.texWidth, &tex.texHeight, &tex.colorChannels, 0);

		if (!textureData) {
			std::cout << "Couldn't load image: " << filename << std::endl;
			return tex;
		}

		// Generate a texture ID and bind to it
		glGenTextures(1, &tex.textureID);
		glBindTexture(GL_TEXTURE_2D, tex.textureID);

		// Define the format array
		GLenum formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };

		if (tex.colorChannels >= 1 && tex.colorChannels <= 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, formats[tex.colorChannels], tex.texWidth, tex.texHeight, 0, formats[tex.colorChannels], GL_UNSIGNED_BYTE, textureData);
			SetTextureParameters(tex);
		}


		stbi_image_free(textureData);

		return tex;
	}

	void Material::SetTextureParameters(const Texture2D& tex) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}


	void Material::BindMaterial(GLuint shader) {
		glUseProgram(shader);

		glUniform1f(glGetUniformLocation(shader, "Shininess"), shininess);
		glUniform1f(glGetUniformLocation(shader, "Opacity"), opacity);

		glActiveTexture(GL_TEXTURE0 + DIFFUSETEX);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture.textureID);
		glUniform1i(glGetUniformLocation(shader, "uniformDiffuse"), DIFFUSETEX);
		glUniform2f(glGetUniformLocation(shader, "DiffuseTextureSize"), diffuseTexture.texWidth, diffuseTexture.texHeight);

		glActiveTexture(GL_TEXTURE0 + SPECULARTEX);
		glBindTexture(GL_TEXTURE_2D, specularTexture.textureID);
		glUniform1i(glGetUniformLocation(shader, "uniformSpecular"), SPECULARTEX);
		glUniform2f(glGetUniformLocation(shader, "SpecularTextureSize"), specularTexture.texWidth, specularTexture.texHeight);

		glActiveTexture(GL_TEXTURE0 + HEIGHTTEX);
		glBindTexture(GL_TEXTURE_2D, heightTexture.textureID);
		glUniform1i(glGetUniformLocation(shader, "HeightTexture"), HEIGHTTEX);
		glUniform2f(glGetUniformLocation(shader, "HeightTextureSize"), heightTexture.texWidth, heightTexture.texHeight);
	}
}