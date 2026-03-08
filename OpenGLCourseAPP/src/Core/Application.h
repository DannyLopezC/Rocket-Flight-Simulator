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

#include "Simulation/Ball.h"
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

    Window mainWindow;
    std::vector<Shader*> shaderList;
    Camera camera;

    Texture plainTexture;
    Texture resetTexture;
    Material shinyMaterial;
    Material dullMaterial;

    Mesh* floor;
    Mesh* circle;
    Mesh* wall;

    DirectionalLight mainLight;

    GLfloat deltaTime;
    GLfloat lastTime;

    Simulation simulation;

    Trail* trail;
    float trajectoryTimer = 0.0f;

    Button* restartBtn;

    Input input;
};
