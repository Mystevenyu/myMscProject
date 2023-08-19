#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <string>
#include <iostream>

class Application; // Forward declaration

#include "Texture.h"
#include "Mesh.h"
#include "Material.h"
namespace VCTII
{
	class Object {
	public:
		Object() : materials(nullptr), cameraPosition(glm::vec3(0.f)), objScale(1.f) {}
		~Object() {delete meshes;}
		inline void GetMesh(Mesh* mesh) { meshes = mesh; }
		inline void GetMaterial(Material* material) { materials = material; }
		void SetObjectPosition(glm::vec3 pos);
		void SetObjectScale(float scale);
		void DrawObject(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightModelViewProjectionMatrix, GLuint shader);
		void DrawObjectToDepthBuffer(glm::mat4& lightViewProjectionMatrix, GLuint shader);
		void DrawObjectTo3DTextureWithLight(GLuint shader, glm::mat4& lightViewProjectionMatrix);



	private:
		glm::vec3 cameraPosition;
		float objScale;

		Mesh* meshes;
		Material* materials;
	};
}

#endif // OBJECT_H