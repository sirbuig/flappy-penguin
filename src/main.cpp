#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

GLuint
    VaoTGlacier, VboTGlacier, ColorTGlacier, EboTGlacier,
    VaoBGlacier, VboBGlacier, ColorBGlacier, EboBGlacier,
    ProgramId,
    myMatrixLocation;

GLfloat
    winWidth = 800, winHeight = 600;

glm::mat4
    myMatrix,
    resizeMatrix;

float xMin = -400.f, xMax = 400.f, yMin = -300.f, yMax = 300.f;
float obstacleXPos = xMax;
float obstacleSpeed = 0.2f;

void CreateShaders(void) {
    ProgramId = LoadShaders("example.vert", "example.frag");
    glUseProgram(ProgramId);
}

void CreateTGlacier(void) {
    // coordinates for Top Glacier
    static const GLfloat VerticesTG[] = {
        -50.0f, 100.0f, 0.0f, 1.0f,
        50.0f, 100.0f, 0.0f, 1.0f,
        50.0f, 350.0f, 0.0f, 1.0f,
        -50.0f, 350.0f, 0.0f, 1.0f
    };

    // colors
    static const GLfloat ColorsTG[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f
    };

    // order
    static const GLuint IndicesTG[] = {
        0, 1, 2, 2, 3, 0
    };

    glGenVertexArrays(1, &VaoTGlacier);
    glBindVertexArray(VaoTGlacier);

    glGenBuffers(1, &VboTGlacier);
    glBindBuffer(GL_ARRAY_BUFFER, VboTGlacier);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesTG), VerticesTG, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &ColorTGlacier);
    glBindBuffer(GL_ARRAY_BUFFER, ColorTGlacier);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ColorsTG), ColorsTG, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &EboTGlacier);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboTGlacier);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndicesTG), IndicesTG, GL_STATIC_DRAW);
}

void CreateBGlacier(void) {
    // coordinates for Bottom Glacier
    static const GLfloat VerticesBG[] = {
        -50.0f, -100.0f, 0.0f, 1.0f,
        50.0f, -100.0f, 0.0f, 1.0f,
        50.0f, -350.0f, 0.0f, 1.0f,
        -50.0f, -350.0f, 0.0f, 1.0f
    };

    // colors
    static const GLfloat ColorsBG[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f
    };

    // order
    static const GLuint IndicesBG[] = {
        0, 1, 2, 2, 3, 0
    };

    glGenVertexArrays(1, &VaoBGlacier);
    glBindVertexArray(VaoBGlacier);

    glGenBuffers(1, &VboBGlacier);
    glBindBuffer(GL_ARRAY_BUFFER, VboBGlacier);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesBG), VerticesBG, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &ColorBGlacier);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBGlacier);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ColorsBG), ColorsBG, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &EboBGlacier);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboBGlacier);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndicesBG), IndicesBG, GL_STATIC_DRAW);
}

void DestroyShaders(void) {
    glDeleteProgram(ProgramId);
}

void DestroyVBO(void) {
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VboTGlacier);
    glDeleteBuffers(1, &ColorTGlacier);
    glDeleteBuffers(1, &EboTGlacier);
    glDeleteBuffers(1, &VboBGlacier);
    glDeleteBuffers(1, &ColorBGlacier);
    glDeleteBuffers(1, &EboBGlacier);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoTGlacier);
    glDeleteVertexArrays(1, &VaoBGlacier);
}

void Cleanup(void) {
    DestroyShaders();
    DestroyVBO();
}

void Initialize(void) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    CreateTGlacier();
    CreateBGlacier();
    CreateShaders();

    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
    resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}

void UpdateObstaclePosition() {
    obstacleXPos -= obstacleSpeed;

    if (obstacleXPos < xMin - 100.0f) {
        obstacleXPos = xMax + 100.0f;
    }
}

void RenderFunction(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    UpdateObstaclePosition();

    myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(obstacleXPos, 0.0f, 0.0f));
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

    glBindVertexArray(VaoTGlacier);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(VaoBGlacier);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glFlush();
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Ghetar speis");

    glewInit();

    Initialize();
    glutDisplayFunc(RenderFunction);
    glutCloseFunc(Cleanup);

    glutMainLoop();

    return 0;
}
