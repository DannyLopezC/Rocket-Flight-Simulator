#include "Core/Application.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

Application::Application()
    : deltaTime(0.0f), lastTime(0.0f)
{
    floor = nullptr;
    circle = nullptr;
}

Application::~Application()
{
    glfwTerminate();
}

void Application::Init()
{
    mainWindow = Window(1366, 768);
    mainWindow.initialise();

    createShaders();

    camera = Camera(glm::vec3(0.0f, 5.0f, 20.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f, 0.0f, 0.0f, 0.0f);

    plainTexture = Texture("assets/textures/plain.png");
    plainTexture.loadTextureA();

    road = Model();
    road.loadModel("assets/models/g.glb", "road_texture/");

    car = Model();
    car.loadModel("assets/models/car.fbx", "car_textures/");

    primitiveFactory = PrimitiveFactory();
    floor = primitiveFactory.createQuad(40.0f, 2.0f);
    circle = primitiveFactory.createCircle(0.5f, 50);

    shinyMaterial = Material(0.8f, 256);
    dullMaterial = Material(0.3f, 4);

    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
        0.8f, 0.3f,
        0.0f, 0.0f, -1.0f);

    ball = Ball({ 0.0f, 8.0f }, { 0.0f, 0.0f }, 1.0f);
}

void Application::Run()
{
    float w = (float)mainWindow.getBufferWidth();
    float h = (float)mainWindow.getBufferHeight();
    float aspect = w / h;
    float halfHeight = 10.0f;
    float halfWidth = halfHeight * aspect;

    //glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        //(GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    glm::mat4 projection = glm::ortho(
        -halfWidth, halfWidth,
        -halfHeight, halfHeight,
        -1.0f, 1.0f
    );

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        ball.update(deltaTime, gravity);
        ball.bounceOnFloor(floorY, restitution);

        glfwPollEvents();
        //camera.keyControl(mainWindow.getKeys(), deltaTime);
        //camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

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

        // camera
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        //glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        //glUniform3f(uniformEyePosition, camera.getCameraPosition().x,
            //camera.getCameraPosition().y,
            //camera.getCameraPosition().z);

        glm::mat4 view = glm::mat4(1.0f);
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniform3f(uniformEyePosition, 0.0f, 0.0f, 0.0f);

        glm::mat4 model(1.0f);
        
        glm::vec2 p = ball.GetPos();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(p.x,p.y, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
        plainTexture.useTexture();
        circle->renderMesh();

        // --- Floor ---
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -9.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
        plainTexture.useTexture();
        floor->renderMesh();

        glUseProgram(0);
        mainWindow.swapBuffers();
    }
}

void Application::createShaders()
{
    Shader* shader1 = new Shader();
    shader1->createFromFiles("assets/shaders/shader.vert", "assets/shaders/shader.frag");
    shaderList.push_back(shader1);
}