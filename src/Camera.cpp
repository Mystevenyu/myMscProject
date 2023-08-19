#include <iostream>

#include "Camera.h"
namespace VCTII
{
	void Camera::UpdateCameraState() {
		// Update camera coordinate system vectors
		dirFront.x = cos(cameraYaw) * cos(cameraPitch);
		dirFront.y = sin(cameraPitch);
		dirFront.z = sin(cameraYaw) * cos(cameraPitch);
		dirFront = glm::normalize(dirFront);
		dirRight = glm::normalize(glm::cross(dirFront, worldUpVector));
		dirUp = glm::normalize(glm::cross(dirRight, dirFront));

		// Update view matrix
		viewMatrix = glm::lookAt(cameraPosition, cameraPosition + dirFront, dirUp);

		// Update projection matrix
		projectionMatrix = glm::perspective(viewField, aspect, nearPlane, farPlane);
	}

	glm::mat4 Camera::GetViewMatrix() {
		return viewMatrix;
	}

	glm::mat4 Camera::GetProjectionMatrix() {
		return projectionMatrix;
	}

	glm::vec3 Camera::GetPosition() {
		return cameraPosition;
	}

	glm::vec3 Camera::GetDirection() {
		return dirFront;
	}

	void Camera::SetObjectPosition(glm::vec3 pos) {
		cameraPosition = pos;
	}

	void Camera::SetDirection(glm::vec3 dir) {
		dirFront = dir;
	}

	void Camera::MoveForward(float move) {
		cameraPosition += dirFront * move;
	}

	void Camera::MoveBackward(float move) {
		cameraPosition -= dirFront * move;
	}

	void Camera::MoveRight(float move) {
		cameraPosition += dirRight * move;
	}

	void Camera::MoveLeft(float move) {
		cameraPosition -= dirRight * move;
	}

	void Camera::MoveUp(float move) {
		cameraPosition += worldUpVector * move;
	}

	void Camera::MoveDown(float move) {
		cameraPosition -= worldUpVector * move;
	}

	void Camera::AddYaw(float yaw) {
		cameraYaw += yaw;
	}

	void Camera::AddPitch(float pitch) {
		cameraPitch += pitch;
		if (cameraPitch > PI / 2.f)
			cameraPitch = PI / 2.f;
		if (cameraPitch < -PI / 2.f)
			cameraPitch = -PI / 2.f;
	}
}