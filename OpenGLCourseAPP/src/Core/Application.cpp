#include "Core/Application.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

Application::Application()
    : deltaTime(0.0f), lastTime(0.0f)
{
    floor = nullptr;
    rocket = nullptr;
    rocketFire = nullptr;
    trail = nullptr;
    restartBtn = nullptr;
    velArrow = nullptr;
    accArrow = nullptr;
    maxHeightLine = nullptr;
    textRenderer = nullptr;
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

    delete textRenderer;
    textRenderer = nullptr;

    delete rocketFire;
    rocketFire = nullptr;

    for (auto& field : configFields)
    {
        delete field.inputField;
    }
    configFields.clear();
}

void Application::Init()
{
    mainWindow = Window(1700, 950);
    mainWindow.initialise();

    input.init(mainWindow.getWindow());
    input.setWindowSize((int)mainWindow.getBufferWidth(), (int)mainWindow.getBufferHeight());

    createShaders();

    textRenderer = new TextRenderer();
    textRenderer->init((unsigned int)mainWindow.getBufferWidth(), (unsigned int)mainWindow.getBufferHeight(), shaderList[3]);
    textRenderer->loadFont(fontPath, 24);

    camera = Camera(glm::vec3(0.0f, 5.0f, 20.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f, 0.0f, 0.0f, 0.0f);

    plainTexture = Texture("assets/textures/plain.png");
    plainTexture.loadTextureA();
    
    resetTexture = Texture("assets/textures/reset.png");
    resetTexture.loadTextureA();

    inputFieldTexture = Texture("assets/textures/inputfield.png");
    inputFieldTexture.loadTextureA();

    rocketTex = Texture("assets/textures/rocket.png");
    rocketTex.loadTextureA();

    simulation = Simulation();

    floor = PrimitiveFactory::createQuad(40.0f, 2.0f);
    rocket = PrimitiveFactory::createQuad(simulation.getRocketWidth(), simulation.getRocketHeight());
    rocketFire = PrimitiveFactory::createQuad(simulation.getRocketWidth(), simulation.getRocketHeight());
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
    restartBtn->setPos(glm::vec3(w - 183.0f, h - 200.0f, 0.0f));
    restartBtn->setScale(glm::vec3(0.5f, 0.5f, 1.0f));

    auto addField = [&](const std::string& label, ConfigKey key, float x, float y)
        {
            ConfigField field;
            field.label = label;
            field.key = key;

            field.inputField = new InputField(70.0f, 40.0f, plainTexture);
            field.inputField->initTextRenderer(w, h, shaderList[3], fontPath, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 5);

            field.inputField->setPos(glm::vec3(x, y, 0.0f));
            field.inputField->setText(getFieldValue(field.key));

            configFields.push_back(field);
        };

    float startX = 170.0f;
    float startY = h - 50.0f;

    float columnSpacing = 200.0f;
    float rowSpacing = 60.0f;

    std::vector<std::pair<std::string, ConfigKey>> fields =
    {
        {"Gravity (m/s^2)", ConfigKey::Gravity},
        {"Angle (Degree)", ConfigKey::Angle},
        {"Mass (kg)", ConfigKey::Mass},
        {"Thrust (N)", ConfigKey::Thrust},
        {"Burn Time (s)", ConfigKey::BurnTime},
        {"Restitution", ConfigKey::Restitution}
    };

    for (int i = 0; i < fields.size(); i++)
    {
        int col = i % 3;
        int row = i / 3;

        float x = startX + col * columnSpacing;
        float y = startY - row * rowSpacing;

        addField(fields[i].first, fields[i].second, x, y);
    }
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
    Shader* fireShader = shaderList[4];

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        deltaTime = glm::clamp(deltaTime, 0.0f, 0.05f);
        accumulator += deltaTime;

        input.beginFrame();
        glfwPollEvents();
        input.update();

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
            
            for (auto& field : configFields)
            {
                applyFieldValue(field);
            }
        }

        for (auto& field : configFields)
        {
            field.inputField->updateInputFieldState(mx, my, mousePressed);
        }

        // --- Key Input ---
        InputField* activeField = nullptr;

        for (auto& field : configFields)
        {
            if (field.inputField->getInputFieldFocused())
            {
                activeField = field.inputField;
                break;
            }
        }

        if (activeField)
        {
            for (unsigned int c : input.getCharBuffer())
            {
                activeField->writeChar((char)c);
            }

            if (input.keyPressed(GLFW_KEY_BACKSPACE))
                activeField->writeKey(GLFW_KEY_BACKSPACE);

            if (input.keyPressed(GLFW_KEY_ENTER))
                activeField->writeKey(GLFW_KEY_ENTER);

            if (input.keyPressed(GLFW_KEY_ESCAPE))
                activeField->writeKey(GLFW_KEY_ESCAPE);
        }

        // --- Simulation ---
        while (accumulator >= fixedDt)
        {
            simulation.update(fixedDt);
            accumulator -= fixedDt;
        }

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

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        
        renderMesh(rocket, t, r, rA, uniformModel, shinyMaterial, rocketTex);

        // --- Rocket Fire ---
        if (simulation.getFlightTime() < simulation.getBurnTime() && !simulation.getSimulationEnded())
        {
            float angleRad = glm::radians(rA);
            glm::vec3 rocketUp = glm::vec3(cos(angleRad - glm::half_pi<float>()), sin(angleRad - glm::half_pi<float>()), 0.0f);
            glm::vec3 fireOffset = -rocketUp * 0.85f;


            fireShader->useShader();
            fireShader->setMat4("projection", worldProjection);
            fireShader->setMat4("view", worldView);
            glUniform1f(glGetUniformLocation(fireShader->getShaderId(), "uTime"), glfwGetTime());

            renderMesh(rocketFire, t + fireOffset, r, rA, fireShader->getModelLocation(), shinyMaterial, plainTexture, glm::vec3(1, 0.5, 1));

            glDepthMask(GL_TRUE);
        }
        // --- Arrows ---
        lineShader->useShader();
        lineShader->setMat4("projection", worldProjection);
        lineShader->setMat4("view", worldView);

        colorLoc = glGetUniformLocation(lineShader->getShaderId(), "lineColor");
         // --- Vel Arrow ---
        if (!simulation.getSimulationEnded()) {
            rA = glm::degrees(atan2(vel.y, vel.x));
            velArrow->setArrowPos(t);
            velArrow->setArrowAngle(rA);
            velArrow->setArrowLength(glm::clamp(glm::length(vel) * 0.2f, 0.0f, 5.0f));
            glUniform4f(colorLoc, velocityColor.r, velocityColor.g, velocityColor.b, velocityColor.a);
            velArrow->render(lineModel);
             // -- Acc Arrow ---

            float accMag = glm::length(acc);

            if (accMag > 0.0001f)
            {
                float rA = glm::degrees(atan2(acc.y, acc.x));

                accArrow->setArrowPos(t);
                accArrow->setArrowAngle(rA);
                accArrow->setArrowLength(accMag * 0.15f);

                glUniform4f(colorLoc, accelColor.r, accelColor.g, accelColor.b, accelColor.a);
                accArrow->render(lineModel);
            }
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

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLint btnColorLoc = glGetUniformLocation(uiShader->getShaderId(), "btnColor");
        GLuint uiModelLoc = glGetUniformLocation(uiShader->getShaderId(), "model");

        uiShader->useShader();

        uiShader->setMat4("projection", uiProjection);
        uiShader->setMat4("view", uiView);

        restartBtn->render(uiModelLoc, btnColorLoc);

        for (auto& field : configFields)
        {
            uiShader->useShader();
            uiShader->setMat4("projection", uiProjection);
            uiShader->setMat4("view", uiView);

            field.inputField->render(uiModelLoc, btnColorLoc);

            float left = field.inputField->getPos().x
                - (field.inputField->getWidth() * field.inputField->getScale().x) / 2;

            textRenderer->renderText(
                field.label,
                left - 120.0f,
                field.inputField->getPos().y - 8.0f,
                0.6f,
                glm::vec3(1.0f, 1.0f, 1.0f)
            );
        }

        std::stringstream ss;

        textRenderer->renderText("Rocket Flight Simulator",
            w - 250.0f, h - 40.0f,
            0.8f,
            glm::vec3(1.0f, 1.0f, 1.0f));

        ss << std::fixed << std::setprecision(2) << simulation.getMaxHeight();
        textRenderer->renderText("Max Height: " + ss.str() + " m",
            w - 250.0f, h - 80.0f,
            0.7f,
            glm::vec3(1.0f, 1.0f, 1.0f));

        ss = std::stringstream();
        ss << std::fixed << std::setprecision(2) << simulation.getRange();
        textRenderer->renderText("Range: " + ss.str() + " m",
            w - 250.0f, h - 110.0f,
            0.7f,
            glm::vec3(1.0f, 1.0f, 1.0f));

        ss = std::stringstream();
        ss << std::fixed << std::setprecision(2) << simulation.getFlightTime();
        textRenderer->renderText("Flight Time: " + ss.str() + " s",
            w - 250.0f, h - 140.0f,
            0.7f,
            glm::vec3(1.0f, 1.0f, 1.0f));

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

    Shader* lineShader = new Shader();
    lineShader->createFromFiles("assets/shaders/line.vert", "assets/shaders/line.frag");
    shaderList.push_back(lineShader);

    Shader* btnShader = new Shader();
    btnShader->createFromFiles("assets/shaders/btn.vert", "assets/shaders/btn.frag");
    shaderList.push_back(btnShader);

    Shader* textShader = new Shader();
    textShader->createFromFiles("assets/shaders/text.vert", "assets/shaders/text.frag");
    shaderList.push_back(textShader);

    Shader* fireShader = new Shader();
    fireShader->createFromFiles("assets/shaders/fire.vert", "assets/shaders/fire.frag");
    shaderList.push_back(fireShader);
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

void Application::applyFieldValue(const ConfigField& field)
{
    float value = std::stof(field.inputField->getCurrentText());

    switch (field.key)
    {
    case ConfigKey::Gravity:
        simulation.setGravity(value);
        break;
    case ConfigKey::Angle:
        simulation.setAngle(value);
        break;
    case ConfigKey::Mass:
        simulation.setMass(value);
        break;
    case ConfigKey::Thrust:
        simulation.setThrust(value);
        break;
    case ConfigKey::BurnTime:
        simulation.setBurnTime(value);
        break;
    case ConfigKey::Restitution:
        simulation.setRestitution(value);
        break;
    default:
        break;
    }
}

std::string Application::getFieldValue(ConfigKey key)
{
    float value;

    switch (key)
    {
    case ConfigKey::Gravity:
        value = simulation.getGravity();
        break;
    case ConfigKey::Angle:
        value = simulation.getAngle();
        break;
    case ConfigKey::Mass:
        value = simulation.getMass();
        break;
    case ConfigKey::Thrust:
        value = simulation.getThrust();
        break;
    case ConfigKey::BurnTime:
        value = simulation.getBurnTime();
        break;
    case ConfigKey::Restitution:
        value = simulation.getRestitution();
        break;
    default:
        value = 0;
        break;
    }
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    return ss.str();
}
