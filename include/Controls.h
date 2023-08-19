#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#ifndef CONTROLS_H
#define CONTROLS_H
namespace VCTII
{
	class Application; // Forward declaration

	class Controls {
	public:
		Controls::Controls(float speed, float mouseSensitivity) : mouseSpeed(speed),preMousePosition(glm::dvec2(0.f)),mouseSensitive(mouseSensitivity){}
		~Controls() {}

		void UpdateInputState(Application* app, float deltaTime);

	private:
		// Settings
		float mouseSpeed;
		float mouseSensitive;

		glm::dvec2 preMousePosition;

	};
}
#endif // CONTROLS_H