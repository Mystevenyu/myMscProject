#include <iostream>

#include "Mesh.h"
namespace VCTII
{

	void Mesh::LoadMesh(const aiMesh* mesh) {
		const unsigned int numVertices = mesh->mNumVertices;
		const unsigned int numFaces = mesh->mNumFaces;

		// Reserve space
		std::vector<glm::vec3> vertices(numVertices);
		std::vector<glm::vec2> iTexCoords;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<unsigned int> indices(numFaces * 3);

		// Load vertices
		for (unsigned int i = 0; i < numVertices; ++i) {
			const aiVector3D& pos = mesh->mVertices[i];
			vertices[i] = glm::vec3(pos.x, pos.y, pos.z);
		}

		// Load indices
		for (unsigned int i = 0; i < numFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			indices[i * 3] = face.mIndices[0];
			indices[i * 3 + 1] = face.mIndices[1];
			indices[i * 3 + 2] = face.mIndices[2];
		}

		hasTexCoords = mesh->HasTextureCoords(0);
		hasNormals = mesh->HasNormals();
		hasTangentsAndBitangents = mesh->HasTangentsAndBitangents();

		auto loadVectorArray = [&](const aiVector3D* source, std::vector<glm::vec3>& target) {
			target.reserve(numVertices);
			for (unsigned int i = 0; i < numVertices; ++i) {
				const aiVector3D& v = source[i];
				target.push_back(glm::vec3(v.x, v.y, v.z));
			}
		};

		if (hasTexCoords) {
			iTexCoords.reserve(numVertices);
			for (unsigned int i = 0; i < numVertices; ++i) {
				const aiVector3D& texCoord = mesh->mTextureCoords[0][i];
				iTexCoords.push_back(glm::vec2(texCoord.x, -texCoord.y));
			}
		}

		if (hasNormals) loadVectorArray(mesh->mNormals, normals);
		if (hasTangentsAndBitangents) {
			loadVectorArray(mesh->mTangents, tangents);
			loadVectorArray(mesh->mBitangents, bitangents);
		}

		// Create VAO
		glGenVertexArrays(1, &vertexArraies);
		glBindVertexArray(vertexArraies);

		// Load VBOs
		auto createBuffer = [&](GLuint& buffer, const void* data, GLsizei size) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		};

		createBuffer(vertexBufferObject, vertices.data(), vertices.size() * sizeof(glm::vec3));
		if (hasTexCoords) createBuffer(texCoordBufferObject, iTexCoords.data(), iTexCoords.size() * sizeof(glm::vec2));
		if (hasNormals) createBuffer(normalBufferObject, normals.data(), normals.size() * sizeof(glm::vec3));
		if (hasTangentsAndBitangents) {
			createBuffer(tangentBufferObject, tangents.data(), tangents.size() * sizeof(glm::vec3));
			createBuffer(bitangentBufferObject, bitangents.data(), bitangents.size() * sizeof(glm::vec3));
		}

		glGenBuffers(1, &indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// Unbind vertex array
		glBindVertexArray(0);

		// Move data to class members
		storeVertices = std::move(vertices);
		storeTexCoords = std::move(iTexCoords);
		storeNormals = std::move(normals);
		storeTangents = std::move(tangents);
		storeBitangents = std::move(bitangents);
		storeIndices = std::move(indices);
		totalIndices = 3 * numFaces;
		materialIndex = mesh->mMaterialIndex;
	}


	void Mesh::DrawMesh() {
		glBindVertexArray(vertexArraies);

		// Bind the buffers and set the vertex attributes
		struct Attribute {
			GLuint index;
			GLuint buffer;
			GLint size;
		};

		const Attribute attributes[] = {
			{0, vertexBufferObject, 3},     // vertices
			{1, texCoordBufferObject, 2},   // UVs
			{2, normalBufferObject, 3},     // normals
			{3, tangentBufferObject, 3},    // tangents
			{4, bitangentBufferObject, 3}   // bitangents
		};

		for (const Attribute& attr : attributes) {
			glEnableVertexAttribArray(attr.index);
			glBindBuffer(GL_ARRAY_BUFFER, attr.buffer);
			glVertexAttribPointer(
				attr.index,      // attribute
				attr.size,       // size
				GL_FLOAT,        // type
				GL_FALSE,        // normalized?
				0,               // stride
				(void*)0         // array buffer offset
			);
		}

		// Bind index buffer and draw
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		glDrawElements(GL_TRIANGLES, totalIndices, GL_UNSIGNED_INT, (void*)0);

		// Disable attribute arrays
		for (const Attribute& attr : attributes) {
			glDisableVertexAttribArray(attr.index);
		}

		glBindVertexArray(0);
		glUseProgram(0);
	}

}