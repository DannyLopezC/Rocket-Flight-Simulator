#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/CommonValues.h"

#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Core/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/Texture.h"
#include "Graphics/Lightning/DirectionalLight.h"
#include "Graphics/Lightning/PointLight.h"
#include "Graphics/Material.h"
#include "Graphics/Lightning/SpotLight.h"

#include "Graphics/Model.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

Model robot;
Model road;
Model car;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

float curAngle = 0.0f;
bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex Shader
static const char* vShader = "assets/shaders/shader.vert";

// Fragment Shader
static const char* fShader = "assets/shaders/shader.frag";

void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat* vertices,
	unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset) {

	for (size_t i = 0; i < indiceCount; i+=3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;

		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;

		vertices[in0] += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;

		vertices[in1] += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;

		vertices[in2] += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount/vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x;
		vertices[nOffset + 1] = vec.y;
		vertices[nOffset + 2] = vec.z;
	}
}

void createObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		0, 2, 3,
		0, 1, 2
	};

	GLfloat vertices[] =
	{
		// x     y     z     u      v		nx	   ny	 nz
		-1.0f, -1.0f, -0.6f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,// left down
		0.0f, -1.0f, 1.0f, 0.5f, 0.0f,		0.0f, 0.0f, 0.0f,// depth
		1.0f, -1.0f, -0.6f, 1.0f, 0.0f,		0.0f, 0.0f, 0.0f,// right down
		0.0f, 1.0f, 0.0f, 0.5f, 1.0f,		0.0f, 0.0f, 0.0f// up
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};
	
	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f, 0.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f, 10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f, 0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f, 10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->createMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->createMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj2);
}

void createShaders()
{
	Shader* shader1 = new Shader();
	shader1->createFromFiles(vShader, fShader);
	shaderList.push_back(shader1);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.initialise();

	createObjects();
	createShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	plainTexture = Texture("assets/textures/plain.png");
	plainTexture.loadTextureA();

	road = Model();
	road.loadModel("assets/models/road.obj");

	car = Model();
	car.loadModel("assets/models/car.fbx", "car_textures/");

	shinyMaterial = Material(0.8f, 256);
	dullMaterial = Material(0.3f, 4);

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.8f, 0.3f,
		0.0f, 0.0f, -1.0f);

	unsigned int pointLightCount = 0;

	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
		0.0f, 0.1f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1f);

	//pointLightCount++;

	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
		0.0f, 0.1f,
		-4.0f, 2.0f, 0.0f,
		0.3f, 0.1f, 0.1f);

	//pointLightCount++;

	unsigned int spotLightCount = 0;

	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);

	spotLightCount++;

	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, -1.5f, 0.0f,
		-100.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);

	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformAmbientIntensity = 0, uniformAmbientColor = 0, uniformDiffuseIntensity = 0, uniformDiffuseDirection = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// loop until window closed
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// get and handle user input
		glfwPollEvents();

		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// clear window
		glClearColor(0, 0, 0, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (curSize >= maxSize || curSize <= minSize)
		{
			sizeDirection = !sizeDirection;
		}

		curAngle += 1;
		curAngle = fmod(curAngle, 360.0f);

		shaderList[0]->useShader();
			uniformModel = shaderList[0]->getModelLocation();
			uniformProjection = shaderList[0]->getProjectionLocation();
			uniformView = shaderList[0]->getViewLocation();
			uniformEyePosition = shaderList[0]->getEyePositionLocation();
			uniformSpecularIntensity = shaderList[0]->getSpecularIntensityLocation();
			uniformShininess = shaderList[0]->getShininessLocation();

			glm::vec3 lowerLight = camera.getCameraPosition();
			lowerLight.y -= 0.3f;
			spotLights[0].setFlash(lowerLight, camera.getCameraDirection());

			shaderList[0]->setDirectionalLight(&mainLight);
			shaderList[0]->setPointLights(pointLights, pointLightCount);
			shaderList[0]->setSpotLights(spotLights, spotLightCount);

			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

			glm::mat4 model(1.0f);

			model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0f));
			//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
			road.renderModel();

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
			//model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
			car.renderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	glfwTerminate();
	return 0;
}