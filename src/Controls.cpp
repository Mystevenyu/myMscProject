#include <GLFW/glfw3.h>
#include "Controls.h"
#include "Application.h"

namespace VCTII
{
	void Controls::UpdateInputState(Application* app, float deltaTime)
	{
		GLFWwindow* window = app->GetAppWindow();
		Camera* camera = app->GetCamera();

		static bool cameraControlEnabled = false;
		static bool lastRightButtonState = false;

		bool rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

		if (rightButtonState && !lastRightButtonState) { // if the right button is just pressed
			cameraControlEnabled = !cameraControlEnabled;
			glfwSetInputMode(window, GLFW_CURSOR, cameraControlEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
			if (cameraControlEnabled) { // Add this check
				// Get the current mouse position and set it as preMousePosition
				glfwGetCursorPos(window, &preMousePosition.x, &preMousePosition.y);
			}
		}


		lastRightButtonState = rightButtonState;

		if (cameraControlEnabled) {
			glm::dvec2 mousePos;
			glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
			glm::dvec2 mousePosChange = mousePos - preMousePosition;
			preMousePosition = mousePos;

			camera->AddYaw(mousePosChange.x * mouseSensitive);
			camera->AddPitch(-1.0 * mousePosChange.y * mouseSensitive);
		}

		float baseTranslationDelta = deltaTime * mouseSpeed;
		float speedModifier = 1.0f;

		// Adjust speed if either Shift or Control is pressed
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			speedModifier = 2.5f; // Faster when Shift is pressed
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			speedModifier = 0.2f; // Slower when Control is pressed
		}

		float translationDelta = baseTranslationDelta * speedModifier;

		// Move forward
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera->MoveForward(translationDelta);
		}
		// Move backward
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera->MoveBackward(translationDelta);
		}
		// Strafe right
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera->MoveRight(translationDelta);
		}
		// Strafe left
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera->MoveLeft(translationDelta);
		}
		// Move up
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera->MoveUp(translationDelta);
		}
		// Move down
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera->MoveDown(translationDelta);
		}
	}
}
