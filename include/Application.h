#ifndef VCTAPPLICATION_H
#define VCTAPPLICATION_H

#include <GLFW/glfw3.h>

#include <vector>
#include <map>

#include "Object.h"
#include "Material.h"
#include "Camera.h"
#include "Controls.h"
#include "Texture.h"
#include "Application.h"

namespace VCTII
{
	class Application {
	public:
		Application::Application(const int width, const int height, GLFWwindow* window) : windowWidth(width), windowHeight(height), appWindow(window), appCamera(nullptr), appControls(nullptr) {}

		~Application() {
			if (appCamera)delete appCamera; if (appControls)delete appControls;
			for (std::vector<Object*>::iterator obj = storeObjects.begin(); obj != storeObjects.end(); ++obj) {
				delete (*obj);
			}
			storeObjects.clear();
		}

		int GetAppWindowWidth();
		int GetAppWindowHeight();
		GLFWwindow* GetAppWindow();
		Camera* GetCamera();

		bool InitCameraControls();
		bool LoadShaders();
		bool InitApp();
		void UpdateScene(float deltaTime);
		void SwitchMode();
		void DrawScene();

		GLint GetUniformLocation(GLuint program, const std::string& name);

		void SetUniforms(GLuint shader);
		bool loadObject(std::string path, std::string name, glm::vec3 pos = glm::vec3(0.0f), float scale = 1.0f);
		void DrawVoxels();
		void RenderDepthTexture();
		void Voxelization();
	private:
		
		std::map<std::string, GLint> uniformLocations_;

		int windowWidth, windowHeight;
		Camera* appCamera;
		Controls* appControls;
		GLFWwindow* appWindow;

		std::vector<Object*> storeObjects;
		std::map<int, Material*> storeMaterials;

		GLuint defaultShader;

		const float modelScale = 0.05f;
		glm::vec3 lightDirection = glm::vec3(0.f, 1.f, 0.25f);

		// Stuff for shadow mapping
		GLuint depthFramebuffer;
		Texture2D depthTexture;
		GLuint shadowShader;
		glm::mat4 lightViewProjectionMatrix;

		// Voxelization
		GLuint voxelizationShader;
		Texture3D voxelTexture;
		const int dimensionOfVoxels = 256;
		const float voxelGridSize = 160.0f;
		glm::mat4 projX, projY, projZ;

		// Render voxels
		GLuint renderVoxelsShader;
		GLuint texture3DVertexArray;


		// Inputs
		bool switchKey = false;
		bool showVoxels = false;

		bool showDiffuseColor = true;
		bool showIndirectDiffuseColor = true; 
		bool showIndirectSpecularColor = true; 
		bool showAmbientOcculisionColor = true;
	};
}
#endif // VCTAPPLICATION_H