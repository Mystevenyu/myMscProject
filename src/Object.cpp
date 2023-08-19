#include <iostream>

#include "Camera.h"
#include "Application.h"

#include "Object.h"
namespace VCTII
{
	void Object::SetObjectPosition(glm::vec3 pos) {
		cameraPosition = pos;
	}

	void Object::SetObjectScale(float scale) {
		objScale = scale;
	}

	void Object::DrawObject(glm::mat4& viewMatrix, glm::mat4& projectionMatrix, glm::mat4& lightViewProjectionMatrix, GLuint shader) {
		glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(objScale)), cameraPosition);
		glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
		glm::mat4 lightModelViewProjectionMatrix = lightViewProjectionMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(shader, "ViewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "LightModelViewProjectionMatrix"), 1, GL_FALSE, &lightModelViewProjectionMatrix[0][0]);

		if (materials) {
			materials->BindMaterial(shader);
		}

		meshes->DrawMesh();
	}

	void Object::DrawObjectToDepthBuffer(glm::mat4& lightViewProjectionMatrix, GLuint shader) {
		glUseProgram(shader);

		glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(objScale)), cameraPosition);
		glm::mat4 modelViewProjectionMatrix = lightViewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjectionMatrix"), 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);

		meshes->DrawMesh();
	}

	void Object::DrawObjectTo3DTextureWithLight(GLuint shader, glm::mat4& lightViewProjectionMatrix) {
		materials->BindMaterial(shader);

		// Matrix to transform to light position
		glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(objScale)), cameraPosition);
		glm::mat4 lightModelViewProjectionMatrix = lightViewProjectionMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(shader, "LightModelViewProjectionMatrix"), 1, GL_FALSE, &lightModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMatrix"), 1, GL_FALSE, &modelMatrix[0][0]);

		meshes->DrawMesh();
	}
}