#include "Core/Application.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

Application::Application()
    : deltaTime(0.0f), lastTime(0.0f)
{
    floor = nullptr;
    circle = nullptr;
    wall = nullptr;
    trail = nullptr;
    restartBtn = nullptr;
}

Application::~Application()
{
    glfwTerminate();

    delete trail;
    trail = nullptr;

    delete floor;
    floor = nullptr;

    delete circle;
    circle = nullptr;

    delete wall;
    wall = nullptr;

    delete restartBtn;
    restartBtn = nullptr;
}

void Application::Init()
{
    mainWindow = Window(1366, 768);
    mainWindow.initialise();

    input.init(mainWindow.getWindow());
    input.setWindowSize((int)mainWindow.getBufferWidth(), (int)mainWindow.getBufferHeight());

    createShaders();

    camera = Camera(glm::vec3(0.0f, 5.0f, 20.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f, 0.0f, 0.0f, 0.0f);

    plainTexture = Texture("assets/textures/plain.png");
    plainTexture.loadTextureA();
    
    resetTexture = Texture("assets/textures/reset.png");
    resetTexture.loadTextureA();

    ball = Ball(Simulation::throwerPos, Simulation::v0, 0.2f);

    floor = PrimitiveFactory::createQuad(40.0f, 2.0f);
    circle = PrimitiveFactory::createCircle(ball.GetRadius(), 50);
    wall = PrimitiveFactory::createQuad(0.2f, 40.0f);

    shinyMaterial = Material(0.8f, 256);
    dullMaterial = Material(0.3f, 4);

    trail = new Trail(2000);


    float w = (float)mainWindow.getBufferWidth();
    float h = (float)mainWindow.getBufferHeight();
    float btnW = 120.0f, btnH = 50.0f, margin = 20.0f;

    restartBtn = new Button(304.0f, 304.0f, shinyMaterial, resetTexture);
    restartBtn->setPos(glm::vec3(100.0f, h - 60.0f, 0.0f));
    restartBtn->setScale(glm::vec3(0.5f, 0.5f, 1.0f));
}

void Application::Run()
{
    float w = (float)mainWindow.getBufferWidth();
    float h = (float)mainWindow.getBufferHeight();
    float aspect = w / h;
    float halfHeight = 10.0f;
    float halfWidth = halfHeight * aspect;

    glm::mat4 worldProjection = glm::ortho(
        -halfWidth, halfWidth,
        -halfHeight, halfHeight,
        -1.0f, 1.0f
    );

    glm::mat4 uiProjection = glm::ortho(
        0.0f, w,
        0.0f, h,
        -1.0f, 1.0f
    );

    glm::mat4 worldView = glm::mat4(1.0f);
    glm::mat4 uiView = glm::mat4(1.0f);

    Shader* shader = shaderList[0];
    Shader* lineShader = shaderList[1];
    Shader* uiShader = shaderList[2];

    lastTime = glfwGetTime();
    deltaTime = 0.0f;

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        // --- Mouse Input ---
        float mx = input.getMouseXUI();
        float my = input.getMouseYUI();

        if (input.mousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
            if (restartBtn->buttonPressed(mx, my)) {
                trail->clear();
                ball.restart();
                trajectoryTimer = 0.0f;
            }
        }

        ball.update(deltaTime * 1.0f, Simulation::gravity);
        ball.bounceOnFloor(Simulation::floorTopY,Simulation::restitution);
        ball.bounceOnWall(Simulation::wallX, Simulation::restitution);

        trajectoryTimer += deltaTime;
        if (trajectoryTimer > 0.01f)
        {
            trail->addPoint(glm::vec2(ball.GetPos().x, ball.GetPos().y - Simulation::floorOffset));
            trajectoryTimer = 0.0f;
        }

        glfwPollEvents();
        input.update();

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->useShader();

        GLuint uniformModel = shader->getModelLocation();
        GLuint uniformProjection = shader->getProjectionLocation();
        GLuint uniformView = shader->getViewLocation();
        GLuint uniformEyePosition = shader->getEyePositionLocation();
        GLuint uniformSpecularIntensity = shader->getSpecularIntensityLocation();
        GLuint uniformShininess = shader->getShininessLocation();

        glm::vec3 lowerLight = camera.getCameraPosition();
        lowerLight.y -= 0.3f;

        // camera
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(worldProjection));

        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(worldView));
        glUniform3f(uniformEyePosition, 0.0f, 0.0f, 0.0f);

        // --- Trajectory ---
        lineShader->useShader();

        lineShader->setMat4("projection", worldProjection);
        lineShader->setMat4("view", worldView);

        GLint colorLoc = glGetUniformLocation(lineShader->getShaderId(), "lineColor");
        glUniform4f(colorLoc, 0.4f, 0.5f, 1.0f, 1.0f);

        trail->render();
        // change back to the normal shader
        shader->useShader();

        // --- Ball ---
        glm::mat4 model(1.0f);
        
        glm::vec2 p = ball.GetPos();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(p.x,p.y - Simulation::floorOffset, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
        plainTexture.useTexture();
        circle->renderMesh();

        // --- Floor ---
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f - Simulation::floorOffset, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
        plainTexture.useTexture();
        floor->renderMesh();

        // --- Wall ---
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(Simulation::wallX, 20.0f - Simulation::floorOffset, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
        plainTexture.useTexture();
        wall->renderMesh();

        // --- UI ---
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);

        uiShader->useShader();

        uiShader->setMat4("projection", uiProjection);
        uiShader->setMat4("view", uiView);

        GLint btnColorLoc = glGetUniformLocation(uiShader->getShaderId(), "btnColor");
        GLuint uiModelLoc = glGetUniformLocation(uiShader->getShaderId(), "model");

        restartBtn->render(uiModelLoc, btnColorLoc);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(0);
        mainWindow.swapBuffers();
    }
}

void Application::createShaders()
{
    Shader* shader1 = new Shader();
    shader1->createFromFiles("assets/shaders/shader.vert", "assets/shaders/shader.frag");
    shaderList.push_back(shader1);

    Shader* shader2 = new Shader();
    shader2->createFromFiles("assets/shaders/line.vert", "assets/shaders/line.frag");
    shaderList.push_back(shader2);

    Shader* shader3 = new Shader();
    shader3->createFromFiles("assets/shaders/btn.vert", "assets/shaders/btn.frag");
    shaderList.push_back(shader3);
}