#include <GL/glew.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>

namespace VCTII
{
	class Mesh {
	public:
		Mesh::Mesh() : hasNormals(false), hasTexCoords(false), hasTangentsAndBitangents(false), totalIndices(0), materialIndex(0)
		, indexBufferObject(0), vertexBufferObject(0), normalBufferObject(0), tangentBufferObject(0), bitangentBufferObject(0), texCoordBufferObject(0){}
		~Mesh() {}

		void DrawMesh();
		void LoadMesh(const aiMesh* mesh);

	private:
		std::vector<glm::vec3> storeVertices;
		std::vector<glm::vec2> storeTexCoords;
		std::vector<glm::vec3> storeNormals;
		std::vector<glm::vec3> storeTangents;
		std::vector<glm::vec3> storeBitangents;
		std::vector<unsigned int> storeIndices;

	private:
		GLuint vertexArraies;

		GLuint indexBufferObject;
		GLuint totalIndices;
		GLuint vertexBufferObject;
		GLuint normalBufferObject;
		GLuint tangentBufferObject;
		GLuint bitangentBufferObject;
		GLuint texCoordBufferObject;

		bool hasNormals;
		bool hasTangentsAndBitangents;
		bool hasTexCoords;
		unsigned int materialIndex;
	};
}