// #include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SOIL/SOIL.h"

GLuint
	VaoId,
	VboId,
	EboId,
	ProgramId,
	myMatrixLocation,
	viewLocation,
	projLocation,
	matrRotlLocation,
	codColLocation;
GLuint
	texture;

GLfloat
	winWidth = 1100,
	winHeight = 800;

glm::mat4
	myMatrix,
	matrRot;

int codCol;

float PI = 3.141592;

float obsX = 0.0, obsY = 0.0, obsZ = 800.f;
float refX = 0.0f, refY = 0.0f;
float vX = 0.0;

glm::mat4 view;
float width = 800, height = 600, xMin = -800.f, xMax = 800, yMin = -600, yMax = 600, zNear = 0, zFar = 1000, fov = 45;
glm::mat4 projection;

void LoadTexture(const char *texturePath)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char *image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CreateShaders(void)
{
	ProgramId = LoadShaders("example.vert", "example.frag");
	glUseProgram(ProgramId);
}

void CreateVBO(void)
{

	GLfloat Vertices[] = {
		-50.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		50.0f, -50.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		50.0f, 50.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-50.0f, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f};

	GLuint Indices[] = {
		0, 1, 2,
		0, 2, 3};

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(4 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void Initialize(void)
{
	glClearColor(0.49f, 0.98f, 0.91f, 1.0f);

	CreateVBO();

	LoadTexture("pinguin.png");

	CreateShaders();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	matrRot = glm::rotate(glm::mat4(1.0f), PI / 8, glm::vec3(0.0, 0.0, 1.0));

	glm::vec3 obs = glm::vec3(obsX, obsY, obsZ);

	refX = obsX;
	refY = obsY;
	glm::vec3 refPoint = glm::vec3(refX, refY, -1.0f);

	glm::vec3 vert = glm::vec3(vX, 1.0f, 0.0f);

	view = glm::lookAt(obs, refPoint, vert);

	projection = glm::perspective(fov, GLfloat(width) / GLfloat(height), zNear, zFar);

	myMatrix = glm::mat4(1.0f);

	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
	glFlush();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Flappy Penguin");

	glewInit();

	Initialize();
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);

	glutCloseFunc(Cleanup);

	glutMainLoop();

	return 0;
}
