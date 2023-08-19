#ifndef MATERIAL_H
#define MATERIAL_H
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

#include "Texture.h"
namespace VCTII
{
	class Material {
	public:
		enum TEXTURES_TYPES {
			DIFFUSETEX,
			SPECULARTEX,
			HEIGHTTEX
		};

		Material();
		~Material() {}

		void LoadMaterial(const aiMaterial* material, std::string path);
		Texture2D LoadMaterialTexture(const aiMaterial* mat, aiTextureType type, std::string typeName, std::string path);
		Texture2D LoadTexture(std::string filenameString);
		void SetTextureParameters(const Texture2D& tex);
		void BindMaterial(GLuint shader);

		std::string materialName;

	private:
		// Material properties
		glm::vec3 ambientColor;
		glm::vec3 diffuseColor;
		glm::vec3 specularColor;
		glm::vec3 emissiveColor;
		float shininess;
		float opacity;

		Texture2D diffuseTexture;
		Texture2D specularTexture;
		Texture2D heightTexture;

	};
}

#endif