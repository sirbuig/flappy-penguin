// standard libraries
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <cmath>

// GL
#include <GL/glew.h>
#include <GL/freeglut.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// shaders + texturing
#include "SOIL/SOIL.h"
#include "loadShaders.h"

// audio
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

// ------------------------------------------------
// DECLARATION

GLuint
VaoId, VboId1, VboId2, EboId,
ProgramId,
myMatrixLocation;

GLuint
texturePenguin,
textureBackground,
textureGlacier;

GLfloat
winWidth = 1100, winHeight = 800;

glm::mat4 myMatrix, resizeMatrix;

float width = 800, height = 800, xMin = -800.f, xMax = 800, yMin = -800, yMax = 800;

float
penguinY = 0.0f,
fallSpeed = 5.0f,
jumpHeight = 10.0f,
penguinWidth = 150.0f,
penguinHeight = 200.0f,
// obstacleXPos = xMax,
obstacleSpeed = 5.6f;

bool jumping = false;
int jumpFrames = 10, jumpCounter = 0;

ma_engine engine;

struct Glacier {
	GLuint vao, vbo, colorBuffer, ebo;
	float obstacleXPos, y;
	//std::vector<GLfloat> vertices;
	//std::vector<GLfloat> colors;
	//std::vector<GLuint> indices;
};

std::vector<Glacier> glaciers(6);

// ------------------------------------------------
// HELPER FUNCTIONS

void InitAudio() {
	if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
		std::cerr << "Eroare initializare sunet!" << std::endl;
	}
}

void PlayJumpSound() {
	if (ma_engine_play_sound(&engine, "fein_sound.wav", NULL) != MA_SUCCESS) {
		std::cerr << "Eroare redare sunet!" << std::endl;
	}
}

void KeyboardFunction(unsigned char key, int x, int y) {
	if (key == 32 && !jumping) {
		jumping = true;
		jumpCounter = 0;
		PlayJumpSound();
		glutPostRedisplay();
	}
}

void LoadTexture(const char* texturePath, GLuint &texture) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CreateShaders(void) {
	ProgramId = LoadShaders("example.vert", "example.frag");
	glUseProgram(ProgramId);
}

void InitGlacier(Glacier& glacier, const std::vector<GLfloat>& vertices, const std::vector<GLfloat>& colors, const std::vector<GLuint>& indices) {
	glGenVertexArrays(1, &glacier.vao);
	glBindVertexArray(glacier.vao);

	glGenBuffers(1, &glacier.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, glacier.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &glacier.colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, glacier.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &glacier.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glacier.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindTexture(GL_TEXTURE_2D, textureGlacier);
}

std::vector<GLfloat> customizeVertices(float yParam, int i) {
	std::vector<GLfloat> vertices = {
		-125.0f, yParam, 0.0f, 1.0f,
		125.0f, yParam, 0.0f, 1.0f,
		125.0f, GLfloat(pow(-1, i)) * 850.0f, 0.0f, 1.0f,
		-125.0f, GLfloat(pow(-1, i)) * 850.0f, 0.0f, 1.0f
	};

	return vertices;
}

void UpdateObstaclePosition() {
	for (Glacier& glacier : glaciers) {
		glacier.obstacleXPos -= obstacleSpeed;

		if (glacier.obstacleXPos < xMin - 100.0f) {
			glacier.obstacleXPos = xMax + 250.0f;
		}
	}
}

void jumpingPenguin() {
	if (jumping) {
		if (jumpCounter < jumpFrames) {
			penguinY += jumpHeight;
			jumpCounter++;

			if (penguinY > winHeight) {
				glutLeaveMainLoop();
			}
		}
		else {
			jumping = false;
		}
	}
	else {
		if (penguinY > -winHeight)
			penguinY -= fallSpeed;
		else {
			glutLeaveMainLoop();
		}
	}
}

bool topCollision(float penguinY) {
	bool result = false;

	for (size_t i = 0; i < glaciers.size(); i+=2) {
		//std::cerr << i << " " << glaciers[i].y << " " << result << std::endl;
		result = result || ((penguinY >= glaciers[i].y) && ((glaciers[i].obstacleXPos) >= -400.0f) && ((glaciers[i].obstacleXPos) <= -250.0f));
	}

	return result;
}

bool bottomCollision(float penguinY) {
	bool result = false;

	for (size_t i = 1; i < glaciers.size(); i+=2) {
		result = result || ((penguinY <= glaciers[i].y) && ((glaciers[i].obstacleXPos) >= -400.0f) && ((glaciers[i].obstacleXPos) <= -250.0f));
	}

	return result;
}

bool checkCollision() {
	return topCollision(penguinY + 100.0f) || bottomCollision(penguinY - 100.0f);
}

// ------------------------------------------------
// VAO/VBO CREATION
float yTop = 250.0f, yBottom = -250.0f;

void CreateGlaciers(void) {
	std::vector<GLfloat> colors = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f
	};

	std::vector<GLuint> indices = {
		0, 1, 2, 2, 3, 0
	};

	for (size_t i = 0; i < glaciers.size(); i++) {
		std::vector<GLfloat> vertices;
		if (i % 2 == 0) {
			glaciers[i].obstacleXPos = xMax + i * 700.0f;
			glaciers[i+1].obstacleXPos = xMax + i * 700.0f;

			vertices = customizeVertices(yTop, i);
			InitGlacier(glaciers[i], vertices, colors, indices);

			glaciers[i].y = yTop;

			yTop += 100.0f;
			yBottom += 100.0f;
		}
		else {
			glaciers[i].y = yBottom;
			vertices = customizeVertices(yBottom, i);
			InitGlacier(glaciers[i], vertices, colors, indices);
		}
	}
}

void CreateVBO(void) {
	// penguin
	GLfloat Vertices[] = {
		  -400.0f, -100.0f, 0.0f, 1.0f,	 0.49f, 0.98f, 0.91f,	0.0f, 0.0f,
		  -250.0f, -100.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	1.0f, 0.0f,
		  -250.0f, 100.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	1.0f, 1.0f,
		  -400.0f, 100.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	0.0f, 1.0f

	};

	// background
	GLfloat Vertices2[] = {

		  -800.0f, -800.0f, 0.0f, 1.0f,	 0.49f, 0.98f, 0.91f,	0.0f, 0.0f,
		   800.0f, -800.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	1.0f, 0.0f,
		   800.0f,  800.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	1.0f, 1.0f,
		  -800.0f,  800.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	0.0f, 1.0f
	};

	GLuint Indices[] = {
	  0, 1, 2,
	  0, 2, 3,

	  4, 5, 6,
	  4, 6, 7
	};

	glGenVertexArrays(1, &VaoId);                                                   
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VboId1);													
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);										
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);


	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

	glGenBuffers(1, &VboId2);													
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);										
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2), Vertices2, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);


	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));


	glGenBuffers(1, &EboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

// ------------------------------------------------
// DESTROYERS
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void DestroyVBO(void) {
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	for (Glacier& glacier : glaciers) {
		glDeleteBuffers(1, &glacier.vbo);
		glDeleteBuffers(1, &glacier.colorBuffer);
		glDeleteBuffers(1, &glacier.ebo);
	}
	glDeleteBuffers(1, &VboId1);
	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &EboId);

	glBindVertexArray(0);
	for (Glacier& glacier : glaciers) {
		glDeleteBuffers(1, &glacier.vao);
	}
	glDeleteVertexArrays(1, &VaoId);
}

void DestroyAudio() {
	ma_engine_uninit(&engine);
}

void Cleanup(void) {
	DestroyAudio();
	DestroyShaders();
	DestroyVBO();
}

// ------------------------------------------------
// INITIALIZATION
// RENDERING

void Initialize(void) {
	glClearColor(0.49f, 0.98f, 0.91f, 1.0f);

	CreateGlaciers();
	CreateVBO();
	InitAudio();

	LoadTexture("ghetar2.png", textureGlacier);
	LoadTexture("background.jpeg", textureBackground);
	LoadTexture("pinguin.png", texturePenguin);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	CreateShaders();

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
}


void RenderFunction(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	jumpingPenguin();

	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);

	// ===========
	// BACKGROUND
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, VboId2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureBackground);

	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// ===========
	// OBSTACLES
	UpdateObstaclePosition();
	if (checkCollision()) {
		glutLeaveMainLoop();
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureGlacier);
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	for (int i = 0; i < glaciers.size(); i++) {
		myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(glaciers[i].obstacleXPos, 0.0f, 0.0f));
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

		glBindTexture(GL_TEXTURE_2D, textureGlacier);
		//glBindVertexArray(glaciers[i].vao);
		glBindBuffer(GL_ARRAY_BUFFER, glaciers[i].vbo);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	// ===========
	// PENGUIN
	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, penguinY, 0.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, VboId1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturePenguin);

	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
	glFlush();
}

// ------------------------------------------------
// MAIN
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Flappy Penguin");

	glewInit();

	Initialize();
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);
	glutKeyboardFunc(KeyboardFunction);

	glutCloseFunc(Cleanup);

	glutMainLoop();

	return 0;
}