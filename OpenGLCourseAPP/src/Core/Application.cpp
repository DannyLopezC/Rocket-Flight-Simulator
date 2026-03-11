#include "Core/Application.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

Application::Application()
    : deltaTime(0.0f), lastTime(0.0f)
{
    floor = nullptr;
    rocket = nullptr;
    trail = nullptr;
    restartBtn = nullptr;
    velArrow = nullptr;
    accArrow = nullptr;
    maxHeightLine = nullptr;
}

Application::~Application()
{
    glfwTerminate();

    delete trail;
    trail = nullptr;

    delete floor;
    floor = nullptr;

    delete rocket;
    rocket = nullptr;

    delete restartBtn;
    restartBtn = nullptr;

    delete velArrow;
    velArrow = nullptr;

    delete accArrow;
    accArrow = nullptr;

    delete maxHeightLine;
    maxHeightLine = nullptr;
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

    simulation = Simulation();

    floor = PrimitiveFactory::createQuad(40.0f, 2.0f);
    rocket = PrimitiveFactory::createQuad(simulation.getRocketWidth(), simulation.getRocketHeight());
    maxHeightLine = PrimitiveFactory::createLine();

    shinyMaterial = Material(0.8f, 256);
    dullMaterial = Material(0.3f, 4);

    trail = new Trail(2000);
    velArrow = new Arrow(1.0f, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), 45);
    accArrow = new Arrow(1.0f, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), 45);

    float w = (float)mainWindow.getBufferWidth();
    float h = (float)mainWindow.getBufferHeight();
    float btnW = 120.0f, btnH = 50.0f, margin = 20.0f;

    restartBtn = new Button(273.0f, 108.0f, shinyMaterial, resetTexture);
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

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        deltaTime = glm::clamp(deltaTime, 0.0f, 0.05f);
        accumulator += deltaTime;

        // --- Mouse Input ---
        float mx = input.getMouseXUI();
        float my = input.getMouseYUI();

        bool mouseDown = input.mouseDown(GLFW_MOUSE_BUTTON_LEFT);
        bool mousePressed = input.mousePressed(GLFW_MOUSE_BUTTON_LEFT);

        restartBtn->updateButtonState(mx, my, mouseDown);

        if (restartBtn->getButtonHovered() && mousePressed) {
            trail->clear();
            simulation.restart();
            trajectoryTimer = 0.0f;
        }

        // --- Simulation ---
        while (accumulator >= fixedDt)
        {
            simulation.update(fixedDt);
            accumulator -= fixedDt;
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

        // --- Camera ---

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(worldProjection));

        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(worldView));
        glUniform3f(uniformEyePosition, 0.0f, 0.0f, 0.0f);

        // --- Trajectory ---

        trajectoryTimer += deltaTime;
        if (trajectoryTimer > 0.01f)
        {
            trail->addPoint(glm::vec2(simulation.getRocketPos().x, simulation.getRocketPos().y - simulation.getFloorOffset()));
            trajectoryTimer = 0.0f;
        }

        lineShader->useShader();

        lineShader->setMat4("projection", worldProjection);
        lineShader->setMat4("view", worldView);

        GLint colorLoc = glGetUniformLocation(lineShader->getShaderId(), "lineColor");
        GLuint lineModel = lineShader->getModelLocation();
        glUniform4f(colorLoc, 0.4f, 0.5f, 1.0f, 1.0f);

        glm::mat4 identity(1.0f);
        glUniformMatrix4fv(lineModel, 1, GL_FALSE, glm::value_ptr(identity));
        trail->render();
        // change back to the normal shader
        shader->useShader();

        // --- Rocket ---

        glm::vec2 p = simulation.getRocketPos();
        glm::vec2 vel = simulation.getRocketVel();
        glm::vec2 acc = simulation.getRocketAcc();

        glm::vec3 t = { p.x, p.y - simulation.getFloorOffset(), 0.0f };
        glm::vec3 r = { 0.0f, 0.0f, 1.0f };
        float rA;

        if (glm::length(vel) > 0.01f)
        {
            rA = glm::degrees(atan2(vel.y, vel.x)) + 90.0f;
        }
        else
        {
            rA = simulation.getRocketAngle() + 90.0f;
        }
        
        renderMesh(rocket, t, r, rA, uniformModel, shinyMaterial, plainTexture);

        // --- Arrows ---
        lineShader->useShader();
        lineShader->setMat4("projection", worldProjection);
        lineShader->setMat4("view", worldView);

        colorLoc = glGetUniformLocation(lineShader->getShaderId(), "lineColor");
         // --- Vel Arrow ---
        
        rA = glm::degrees(atan2(vel.y, vel.x));
        velArrow->setArrowPos(t);
        velArrow->setArrowAngle(rA);
        velArrow->setArrowLength(glm::clamp(glm::length(vel) * 0.4f, 0.0f, 5.0f));
        glUniform4f(colorLoc, velocityColor.r, velocityColor.g, velocityColor.b, velocityColor.a);
        velArrow->render(lineModel);
         // -- Acc Arrow ---

        float accMag = glm::length(acc);

        if (accMag > 0.0001f)
        {
            float rA = glm::degrees(atan2(acc.y, acc.x));

            accArrow->setArrowPos(t);
            accArrow->setArrowAngle(rA);
            accArrow->setArrowLength(accMag * 0.2f);

            glUniform4f(colorLoc, accelColor.r, accelColor.g, accelColor.b, accelColor.a);
            accArrow->render(lineModel);
        }

        // --- Height Line ---

        t = { simulation.getWallX(), 20.0f - simulation.getFloorOffset(), 0.0f };
        r = { 1.0f, 1.0f, 1.0f };
        rA = 0;
        //renderMesh(wall, t, r, rA, uniformModel, dullMaterial, plainTexture);

        // --- Floor ---
        shader->useShader();

        t = { 0.0f, -1.0f - simulation.getFloorOffset(), 0.0f };
        r = { 1.0f, 1.0f, 1.0f };
        rA = 0;
        renderMesh(floor, t, r, rA, uniformModel, dullMaterial, plainTexture);

        // --- Wall ---

        t = { -18.0f , simulation.getMaxHeight() - simulation.getFloorOffset(), 0.0f};
        r = { 1.0f, 1.0f, 1.0f };
        rA = 0;
        glm::vec3 scale = { 36.0f, 1.0f, 1.0f };
        renderLine(maxHeightLine, t, r, rA, uniformModel, scale);

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

void Application::renderMesh(Mesh* mesh, glm::vec3 translate, glm::vec3 rotate, float rotationAngle,
    GLuint uniformModel, Material mat, Texture tex, glm::vec3 scale)
{
    glm::mat4 model(1.0f);

    model = glm::mat4(1.0f);
    model = glm::translate(model, translate);
    model = glm::rotate(model, glm::radians(rotationAngle), rotate);
    model = glm::scale(model, scale);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    mat.useMaterial(0, 0);
    tex.useTexture();
    mesh->renderMesh();
}

void Application::renderLine(Mesh* mesh, glm::vec3 translate, glm::vec3 rotate, float rotationAngle, GLuint uniformModel, glm::vec3 scale)
{
    glm::mat4 model(1.0f);

    model = glm::mat4(1.0f);
    model = glm::translate(model, translate);
    model = glm::rotate(model, glm::radians(rotationAngle), rotate);
    model = glm::scale(model, scale);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    mesh->renderLines();
}
