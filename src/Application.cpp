#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Shader.h"
#include "Application.h"

namespace VCTII {
	
	int Application::GetAppWindowWidth() {
		return windowWidth;
	}

	int Application::GetAppWindowHeight() {
		return windowHeight;
	}

	GLFWwindow* Application::GetAppWindow() {
		return appWindow;
	}

	Camera* Application::GetCamera() {
		return appCamera;
	}

	bool Application::loadObject(std::string path, std::string name, glm::vec3 pos, float scale) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path + name, aiProcess_Triangulate |
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices);

		if (!scene) {
			std::cerr << "Mesh: " << importer.GetErrorString() << std::endl;
			return false;
		}

		// Create a materials from the loaded assimp materials
		for (unsigned int m = 0; m < scene->mNumMaterials; m++) {
			Material* mat = new Material();
			mat->LoadMaterial(scene->mMaterials[m], path);
			storeMaterials[m] = mat;
		}

		// Create objects and add to storeObjects vector.
		for (size_t m = 0; m < scene->mNumMeshes; m++) {
			Object* obj = new Object();
			Mesh* mesh = new Mesh();
			mesh->LoadMesh(scene->mMeshes[m]);
			obj->GetMesh(mesh);
			obj->GetMaterial(storeMaterials[scene->mMeshes[m]->mMaterialIndex]);
			obj->SetObjectScale(scale);
			obj->SetObjectPosition(pos);
			storeObjects.emplace_back(obj);
		}

		return true;
	}


	bool Application::InitCameraControls() {
		glm::vec3 pos = glm::vec3(1.0, 1.0, 1.0);
		glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
		float yaw = -3.1415f / 2.0f;
		float pitch = 0.0f;
		appCamera = new Camera(pos, yaw, pitch, up, 45.0f, (float)windowWidth / windowHeight, 0.1f, 1000.0f);
		appControls = new Controls(10.0f, 0.0015f);
		return true;
	}

	bool Application::LoadShaders() {
		defaultShader = VCTII::Shader::LoadShaders("../shaders/default.vert", "../shaders/default.frag");
		voxelizationShader = VCTII::Shader::LoadShaders("../shaders/voxelization.vert", "../shaders/voxelization.frag", "../shaders/voxelization.geom");
		shadowShader = VCTII::Shader::LoadShaders("../shaders/shadow.vert", "../shaders/shadow.frag");
		renderVoxelsShader = VCTII::Shader::LoadShaders("../shaders/renderVoxels.vert", "../shaders/renderVoxels.frag", "../shaders/renderVoxels.geom");
		return true;
	}



	bool Application::InitApp() {
		std::cout << "Initializing Application......" << std::endl;

		if (!InitCameraControls() || !LoadShaders()) {
			return false;
		}

		// Load objects
		std::cout << "Loading objects... " << std::endl;
		loadObject("../models/", "sponza.obj", glm::vec3(0.0f), modelScale);
		std::cout << "Loading done! " << storeObjects.size() << " objects loaded" << std::endl;


		// Create VAO for 3D texture. Won't really store any information but it's still needed.
		glGenVertexArrays(1, &texture3DVertexArray);

		// Create framebuffer for shadow map
		glGenFramebuffers(1, &depthFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);

		// Depth texture
		depthTexture.texWidth = depthTexture.texHeight = 4096;

		glm::mat4 viewMatrix = glm::lookAt(lightDirection, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 projectionMatrix = glm::ortho	<float>(-120, 120, -120, 120, -500, 500);
		lightViewProjectionMatrix = projectionMatrix * viewMatrix;

		glGenTextures(1, &depthTexture.textureID);
		glBindTexture(GL_TEXTURE_2D, depthTexture.textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, depthTexture.texWidth, depthTexture.texHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture.textureID, 0);
		// No color output
		glDrawBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Error creating framebuffer" << std::endl;
			return false;
		}

		voxelTexture.tex3DSize = dimensionOfVoxels;

		glEnable(GL_TEXTURE_3D);

		glGenTextures(1, &voxelTexture.textureID);
		glBindTexture(GL_TEXTURE_3D, voxelTexture.textureID);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Fill 3D texture with empty values
		int numVoxels = voxelTexture.tex3DSize * voxelTexture.tex3DSize * voxelTexture.tex3DSize;
		std::unique_ptr<GLubyte[]> data(new GLubyte[numVoxels * 4]);
		memset(data.get(), 0, numVoxels * 4);

		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, voxelTexture.tex3DSize, voxelTexture.tex3DSize, voxelTexture.tex3DSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

		glGenerateMipmap(GL_TEXTURE_3D);

		// Create projection matrices used to project stuff onto each axis in the voxelization step
		float size = voxelGridSize;
		// left, right, bottom, top, zNear, zFar
		projectionMatrix = glm::ortho(-size * 0.5f, size * 0.5f, -size * 0.5f, size * 0.5f, size * 0.5f, size * 1.5f);
		projX = projectionMatrix * glm::lookAt(glm::vec3(size, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		projY = projectionMatrix * glm::lookAt(glm::vec3(0, size, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
		projZ = projectionMatrix * glm::lookAt(glm::vec3(0, 0, size), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));


		// Draw depth for shadow mapping and voxelize scene once
		RenderDepthTexture();
		Voxelization();

		return true;
	}

	void Application::UpdateScene(float deltaTime) {
		appControls->UpdateInputState(this, deltaTime);
		appCamera->UpdateCameraState();
		SwitchMode();

	}

	void Application::SwitchMode() {

		// This is a bit silly
		if (!switchKey && glfwGetKey(appWindow, GLFW_KEY_1) == GLFW_PRESS) {
			showVoxels = !showVoxels;
			switchKey = true;
		}

		if (glfwGetKey(appWindow, GLFW_KEY_1) == GLFW_RELEASE) {
			switchKey = false;
		}

		bool changeLightDir = false;
		if (glfwGetKey(appWindow, GLFW_KEY_UP) == GLFW_PRESS) {
			lightDirection.x += 0.015;
			changeLightDir = true;
		}
		if (glfwGetKey(appWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
			lightDirection.x -= 0.015;
			changeLightDir = true;
		}
		if (glfwGetKey(appWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
			lightDirection.z -= 0.015;
			changeLightDir = true;
		}
		if (glfwGetKey(appWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			lightDirection.z += 0.015;
			changeLightDir = true;
		}
		if (changeLightDir)
		{
			lightViewProjectionMatrix = glm::ortho	<float>(-120, 120, -120, 120, -100, 100) * glm::lookAt(lightDirection, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));;
			RenderDepthTexture();
			Voxelization();
		}
	}

	void Application::SetUniforms(GLuint shader) {
		const struct {
			const char* name;
			float value;
		} uniforms[] = {
			{ "lighting.ShowDiffuse", showDiffuseColor },
			{ "lighting.ShowIndirectDiffuse", showIndirectDiffuseColor },
			{ "lighting.ShowIndirectSpecular", showIndirectSpecularColor },
			{ "lighting.ShowAmbientOcculision", showAmbientOcculisionColor },
		};

		for (const auto& uniform : uniforms) {
			glUniform1f(GetUniformLocation(shader, uniform.name), uniform.value);
		}
	}

	void Application::DrawScene() {
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 viewMatrix = appCamera->GetViewMatrix();
		glm::mat4 projectionMatrix = appCamera->GetProjectionMatrix();

		glUseProgram(defaultShader);
		glm::vec3 camPos = appCamera->GetPosition();
		glUniform3f(GetUniformLocation(defaultShader, "CameraPosition"), camPos.x, camPos.y, camPos.z);
		glUniform3f(GetUniformLocation(defaultShader, "LightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
		glUniform1f(GetUniformLocation(defaultShader, "VoxelGridWorldSize"), voxelGridSize);
		glUniform1i(GetUniformLocation(defaultShader, "VoxelDimensions"), dimensionOfVoxels);

		SetUniforms(defaultShader);

		const GLuint textures[] = { depthTexture.textureID, voxelTexture.textureID };
		for (int i = 0; i < 2; ++i) {
			glActiveTexture(GL_TEXTURE0 + 5 + i);
			glBindTexture(i == 0 ? GL_TEXTURE_2D : GL_TEXTURE_3D, textures[i]);
			glUniform1i(GetUniformLocation(defaultShader, i == 0 ? "ShadowMap" : "VoxelTexture"), 5 + i);
		}

		for (Object* obj : storeObjects) {
			obj->DrawObject(viewMatrix, projectionMatrix, lightViewProjectionMatrix, defaultShader);
		}

		if (showVoxels) {
			DrawVoxels();
		}
	}


	GLint Application::GetUniformLocation(GLuint program, const std::string& name)
	{
		if (uniformLocations_.count(name) > 0) {
			return uniformLocations_[name];
		}
		else 
		{
			GLint location = glGetUniformLocation(program, name.c_str());
			uniformLocations_[name] = location;
			return location;
		}
	}

	void Application::RenderDepthTexture() {
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		// Draw to depth frame buffer instead of screen
		glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);
		// Set viewport of framebuffer size
		glViewport(0, 0, depthTexture.texWidth, depthTexture.texHeight);
		// Set clear color and clear
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto& obj : storeObjects) {
			obj->DrawObjectToDepthBuffer(lightViewProjectionMatrix, shadowShader);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, windowWidth, windowHeight);
	}

	void Application::Voxelization() {

		glViewport(0, 0, voxelTexture.tex3DSize, voxelTexture.tex3DSize);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(voxelizationShader);

		// Set uniforms
		glUniform1i(glGetUniformLocation(voxelizationShader, "VoxelDimensions"), voxelTexture.tex3DSize);
		glUniformMatrix4fv(glGetUniformLocation(voxelizationShader, "ProjX"), 1, GL_FALSE, &projX[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(voxelizationShader, "ProjY"), 1, GL_FALSE, &projY[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(voxelizationShader, "ProjZ"), 1, GL_FALSE, &projZ[0][0]);

		// Bind depth texture
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, depthTexture.textureID);
		glUniform1i(glGetUniformLocation(voxelizationShader, "ShadowMap"), 5);

		// Bind single level of texture to image unit so we can write to it from shaders
		glBindImageTexture(6, voxelTexture.textureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		glUniform1i(glGetUniformLocation(voxelizationShader, "VoxelTexture"), 6);

		for (auto& obj : storeObjects) {
			obj->DrawObjectTo3DTextureWithLight(voxelizationShader, lightViewProjectionMatrix);
		}

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_3D, voxelTexture.textureID);
		glGenerateMipmap(GL_TEXTURE_3D);

		// Reset viewport
		glViewport(0, 0, windowWidth, windowHeight);
	}

	void Application::DrawVoxels() {
		glUseProgram(renderVoxelsShader);

		int voxelCount = voxelTexture.tex3DSize * voxelTexture.tex3DSize * voxelTexture.tex3DSize;
		float voxelSize = voxelGridSize / voxelTexture.tex3DSize;
		glUniform1i(glGetUniformLocation(renderVoxelsShader, "Dimensions"), voxelTexture.tex3DSize);
		glUniform1i(glGetUniformLocation(renderVoxelsShader, "TotalNumVoxels"), voxelCount);
		glUniform1f(glGetUniformLocation(renderVoxelsShader, "VoxelSize"), voxelSize);
		glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(voxelSize)), glm::vec3(0, 0, 0));
		glm::mat4 viewMatrix = appCamera->GetViewMatrix();
		glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
		glm::mat4 projectionMatrix = appCamera->GetProjectionMatrix();
		glUniformMatrix4fv(glGetUniformLocation(renderVoxelsShader, "ModelViewMatrix"), 1, GL_FALSE, &modelViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(renderVoxelsShader, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, voxelTexture.textureID);
		glUniform1i(glGetUniformLocation(renderVoxelsShader, "VoxelsTexture"), 0);

		glBindVertexArray(texture3DVertexArray);
		glDrawArrays(GL_POINTS, 0, voxelCount);

		glBindVertexArray(0);
		glUseProgram(0);
	}
}