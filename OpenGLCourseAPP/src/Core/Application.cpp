#include "Core/Application.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

Application::Application()
    : deltaTime(0.0f), lastTime(0.0f)
{
}

Application::~Application()
{
    glfwTerminate();
}

void Application::Init()
{
    mainWindow = Window(1366, 768);
    mainWindow.initialise();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f, 0.0f, 5.0f, 0.5f);

    plainTexture = Texture("assets/textures/plain.png");
    plainTexture.loadTextureA();

    road = Model();
    road.loadModel("assets/models/g.glb", "road_texture/");

    car = Model();
    car.loadModel("assets/models/car.fbx", "car_textures/");

    shinyMaterial = Material(0.8f, 256);
    dullMaterial = Material(0.3f, 4);

    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
        0.8f, 0.3f,
        0.0f, 0.0f, -1.0f);
}

void Application::Run()
{
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        glfwPollEvents();
        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader* shader = shaderList[0];
        shader->useShader();

        GLuint uniformModel = shader->getModelLocation();
        GLuint uniformProjection = shader->getProjectionLocation();
        GLuint uniformView = shader->getViewLocation();
        GLuint uniformEyePosition = shader->getEyePositionLocation();
        GLuint uniformSpecularIntensity = shader->getSpecularIntensityLocation();
        GLuint uniformShininess = shader->getShininessLocation();

        glm::vec3 lowerLight = camera.getCameraPosition();
        lowerLight.y -= 0.3f;
        spotLights[0].setFlash(lowerLight, camera.getCameraDirection());

        shader->setDirectionalLight(&mainLight);
        shader->setPointLights(pointLights, 0);
        shader->setSpotLights(spotLights, 2);

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition, camera.getCameraPosition().x,
            camera.getCameraPosition().y,
            camera.getCameraPosition().z);

        // --- Road ---
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.004f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
        road.renderModel();

        // --- Car ---
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.4f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
        car.renderModel();

        glUseProgram(0);
        mainWindow.swapBuffers();
    }
}

void Application::CreateObjects()
{
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        0, 2, 3,
        0, 1, 2
    };

    GLfloat vertices[] = {
        -1.0f, -1.0f, -0.6f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         0.0f, -1.0f,  1.0f, 0.5f, 0.0f,  0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, -0.6f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         0.0f,  1.0f,  0.0f, 0.5f, 1.0f,  0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = { 0, 2, 1, 1, 2, 3 };
    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         10.0f, 0.0f, -10.0f, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f,  10.0f, 0.0f, 10.0f, 0.0f, -1.0f, 0.0f,
         10.0f, 0.0f,  10.0f, 10.0f, 10.0f, 0.0f, -1.0f, 0.0f
    };

    Mesh* obj1 = new Mesh();
    obj1->createMesh(vertices, indices, 32, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->createMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(obj2);
}

void Application::CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->createFromFiles("assets/shaders/shader.vert", "assets/shaders/shader.frag");
    shaderList.push_back(shader1);
}