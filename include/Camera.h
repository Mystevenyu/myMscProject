#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#ifndef CAMERA_H
#define CAMERA_H
namespace VCTII
{
	class Camera 
	{
	public:
		Camera::Camera(glm::vec3 pos, float yaw, float pitch, glm::vec3 up, float fieldOfView, float aspectRatio, float near, float far) {
			cameraPosition = pos;
			worldUpVector = up;

			cameraYaw = yaw;
			cameraPitch = pitch;
			cameraRoll = 0.f;

			viewField = fieldOfView;
			aspect = aspectRatio;
			nearPlane = near;
			farPlane = far;
		}
		~Camera() {}

		void UpdateCameraState();

		glm::mat4 GetViewMatrix();
		glm::mat4 GetProjectionMatrix();
		glm::vec3 GetPosition();
		glm::vec3 GetDirection();

		void SetObjectPosition(glm::vec3 pos);
		void SetDirection(glm::vec3 dir);
		void MoveForward(float move);
		void MoveBackward(float move);
		void MoveRight(float move);
		void MoveLeft(float move);
		void MoveUp(float move);
		void MoveDown(float move);
		void AddYaw(float yaw);
		void AddPitch(float pitch);

	private:
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;

		// Camera position and rotation
		glm::vec3 cameraPosition;
		float cameraYaw;
		float cameraPitch;
		float cameraRoll;

		const float PI = 3.1415926535;
		// Camera coordinate system vectors
		glm::vec3 dirFront;
		glm::vec3 dirRight;
		glm::vec3 dirUp;

		glm::vec3 worldUpVector;
		float viewField;
		float aspect;
		float nearPlane;
		float farPlane;

	};
}

#endif