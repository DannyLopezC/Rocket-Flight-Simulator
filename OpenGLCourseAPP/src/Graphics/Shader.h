#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#include "Graphics/Lightning/DirectionalLight.h"
#include "Graphics/Lightning/PointLight.h"
#include "Graphics/Lightning/SpotLight.h"
#include "Core/CommonValues.h"
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader();

	void createFromString(const char* vertexCode, const char* fragmentCode);
	void createFromFiles(const char* vertexLocation, const char* fragmentLocation);

	std::string ReadFile(const char* location);

	GLuint getProjectionLocation();
	GLuint getModelLocation();
	GLuint getViewLocation();
	GLuint getEyePositionLocation();
	GLuint getAmbientIntensityLocation();
	GLuint getAmbientColorLocation();
	GLuint getDiffuseIntensityLocation();
	GLuint getDiffuseDirectionLocation();
	GLuint getSpecularIntensityLocation();
	GLuint getShininessLocation();

	void setDirectionalLight(DirectionalLight * dLight);
	void setPointLights(PointLight* pLight, unsigned int lightCount);
	void setSpotLights(SpotLight* sLight, unsigned int lightCount);

	void useShader();
	void setMat4(const std::string& name, const glm::mat4& value);
	void setVec3(const std::string& name, const glm::vec3& value);
	void setFloat(const std::string& name, float value);

	GLuint getShaderId() const { return shaderId; }

	~Shader();

private:
	int pointLightCount;
	int spotLightCount;

	GLuint shaderId, uniformProjection, uniformModel, uniformView, uniformEyePosition,
		uniformSpecularIntensity, uniformShininess;

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	} uniformDirectionalLight;

	GLuint uniformPointLightCount;

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLight[MAX_POINT_LIGHTS];

	GLuint uniformSpotLightCount;

	struct {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		GLuint uniformDirection;
		GLuint uniformEdge;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

	void compileShader(const char* vertexCode, const char* fragmentCode);
	void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);

	void clearShader();

	const char *vertexCodeSample;
};

