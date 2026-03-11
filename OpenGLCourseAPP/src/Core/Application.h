#pragma once
#include <vector>
#include "Core/Button.h"
#include "Core/Input.h"
#include "Core/Window.h"

#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Graphics/Model.h"
#include "Graphics/Lightning/DirectionalLight.h"
#include "Graphics/Lightning/PointLight.h"
#include "Graphics/Lightning/SpotLight.h"
#include "Graphics/PrimitiveFactory.h"
#include "Graphics/Trail.h"
#include "Graphics/Arrow.h"

#include "Simulation/Simulation.h"


class Application
{
public:
    Application();
    ~Application();

    void Init();
    void Run();

private:
    void createShaders();
    void renderMesh(Mesh* mesh, glm::vec3 translate, glm::vec3 rotate, float rotationAngle,
        GLuint uniformModel, Material mat, Texture tex, glm::vec3 scale = { 1.0f, 1.0f, 1.0f });

    void renderLine(Mesh* mesh, glm::vec3 translate, glm::vec3 rotate, float rotationAngle,
        GLuint uniformModel, glm::vec3 scale = { 1.0f, 1.0f, 1.0f });

    Window mainWindow;
    std::vector<Shader*> shaderList;
    Camera camera;

    Texture plainTexture;
    Texture resetTexture;
    Material shinyMaterial;
    Material dullMaterial;

    Mesh* floor;
    Mesh* rocket;
    Mesh* maxHeightLine;
    
    DirectionalLight mainLight;

    GLfloat deltaTime;
    GLfloat lastTime;
    float accumulator = 0.0f;
    const float fixedDt = 1.0f / 120.0f;

    Simulation simulation;

    Trail* trail;
    Arrow* velArrow;
    Arrow* accArrow;

    const glm::vec4 velocityColor = { 0.2f, 1.0f, 0.2f, 1.0f };
    const glm::vec4 accelColor = { 1.0f, 0.2f, 0.2f, 1.0f };

    float trajectoryTimer = 0.0f;

    Button* restartBtn;

    Input input;
};
