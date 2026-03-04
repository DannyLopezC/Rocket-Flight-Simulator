#pragma once
#include <vector>
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

#include "Simulation/Ball.h"

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
    Material shinyMaterial;
    Material dullMaterial;

    Model road;
    Model car;

    PrimitiveFactory primitiveFactory;
    Mesh* floor;
    Mesh* circle;

    DirectionalLight mainLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];

    GLfloat deltaTime;
    GLfloat lastTime;

    Ball ball;
    float gravity = 9.8f;
    float restitution = 0.8f;
    float floorY = -7.5f;
};
