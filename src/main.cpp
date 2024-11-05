//	Biblioteci
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna;
#include <GL/freeglut.h>    //	Include functii pentru:
							//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse,
							//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii,
							//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "SOIL/SOIL.h"			//	Biblioteca pentru texturare;

#define MINIAUDIO_IMPLEMENTATION  //pt audio
#include "miniaudio.h"
#include <iostream>
//  Identificatorii obiectelor de tip OpenGL;
GLuint
VaoId,
VboId1,
VboId2,
EboId,
ProgramId,
myMatrixLocation,
viewLocation,
projLocation,
matrRotlLocation,
codColLocation,
VaoTGlacier, VboTGlacier, ColorTGlacier, EboTGlacier,
VaoBGlacier, VboBGlacier, ColorBGlacier, EboBGlacier;
GLuint
texturePenguin,
textureBackground,
textureTGlacier, textureBGlacier;
//	Dimensiunile ferestrei de afisare;
GLfloat
winWidth = 1100, winHeight = 800;
//	Variabile catre matricile de transformare;
glm::mat4
myMatrix, matrRot, resizeMatrix;

//	Elemente pentru matricea de proiectie;
float width = 800, height = 800, xMin = -800.f, xMax = 800, yMin = -800, yMax = 800;

float penguinY = -40.0f; // Pozitia intiala
float fallSpeed = 0.f;
float jumpHeight = 10.0f;

bool jumping = false;
int jumpFrames = 10;
int jumpCounter = 0;

float obstacleXPos = xMax;
float obstacleSpeed = 0.2f;

// Initializare miniaudio
ma_engine engine;

// Initializare sunet
void InitAudio() {
	if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
		std::cerr << "Eroare" << std::endl;
	}
}

void PlayJumpSound() {
	if (ma_engine_play_sound(&engine, "fein_sound.wav", NULL) != MA_SUCCESS) {
		std::cerr << "Eroare" << std::endl;
	}
}

void KeyboardFunction(unsigned char key, int x, int y) {
	if (key == 32 && !jumping) { // Codul ASCII pentru Space
		jumping = true;
		jumpCounter = 0;
		PlayJumpSound(); // Redare sunet la saritura
		glutPostRedisplay();
	}
}

//	Functia de incarcare a texturilor in program;
void LoadPenguinTexture(const char* texturePath)
{
	// Generarea unui obiect textura si legarea acestuia;
	glGenTextures(1, &texturePenguin);
	glBindTexture(GL_TEXTURE_2D, texturePenguin);

	//	Desfasurarea imaginii pe orizontala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Modul in care structura de texeli este aplicata pe cea de pixeli;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Incarcarea texturii si transferul datelor in obiectul textura;
	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Eliberarea resurselor
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void LoadBackgroundTexture(const char* texturePath)
{
	// Generarea unui obiect textura si legarea acestuia;
	glGenTextures(1, &textureBackground);
	glBindTexture(GL_TEXTURE_2D, textureBackground);

	//	Desfasurarea imaginii pe orizontala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Modul in care structura de texeli este aplicata pe cea de pixeli;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Incarcarea texturii si transferul datelor in obiectul textura;
	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Eliberarea resurselor
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void LoadTGlacierTexture(const char* texturePath)
{
	// Generarea unui obiect textura si legarea acestuia;
	glGenTextures(1, &textureTGlacier);
	glBindTexture(GL_TEXTURE_2D, textureTGlacier);

	//	Desfasurarea imaginii pe orizontala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Modul in care structura de texeli este aplicata pe cea de pixeli;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Incarcarea texturii si transferul datelor in obiectul textura;
	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Eliberarea resurselor
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void LoadBGlacierTexture(const char* texturePath)
{
	// Generarea unui obiect textura si legarea acestuia;
	glGenTextures(1, &textureBGlacier);
	glBindTexture(GL_TEXTURE_2D, textureBGlacier);

	//	Desfasurarea imaginii pe orizontala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Modul in care structura de texeli este aplicata pe cea de pixeli;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Incarcarea texturii si transferul datelor in obiectul textura;
	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Eliberarea resurselor
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
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

void UpdateObstaclePosition() {
	obstacleXPos -= obstacleSpeed;

	if (obstacleXPos < xMin - 100.0f) {
		obstacleXPos = xMax + 100.0f;
	}
}

//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{

	//	Atributele varfurilor -  COORDONATE, CULORI, COORDONATE DE TEXTURARE;
	GLfloat Vertices[] = {
		//	Coordonate;					Culori;				Coordonate de texturare;
		  -400.0f, -400.0f, 0.0f, 1.0f,	 0.49f, 0.98f, 0.91f,	0.0f, 0.0f,	// Stanga jos;
		  -250.0f, -400.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	1.0f, 0.0f, // Dreapta jos;
		  -250.0f, -250.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	1.0f, 1.0f,	// Dreapta sus;
		  -400.0f, -250.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	0.0f, 1.0f  // Stanga sus;

	};

	GLfloat Vertices2[] = {

		  -800.0f, -800.0f, 0.0f, 1.0f,	 0.49f, 0.98f, 0.91f,	0.0f, 0.0f,	// Stanga jos;
		   800.0f, -800.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	1.0f, 0.0f, // Dreapta jos;
		   800.0f,  800.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	1.0f, 1.0f,	// Dreapta sus;
		  -800.0f,  800.0f, 0.0f, 1.0f,   0.49f, 0.98f, 0.91f,	0.0f, 1.0f  // Stanga sus;
	};

	GLuint Indices[] = {
	  0, 1, 2,
	  0, 2, 3,

	  4, 5, 6,
	  4, 6, 7,

	};

	//  Transmiterea datelor prin buffere;

	//  Se creeaza / se leaga un VAO (Vertex Array Object);
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer pentru VARFURI - COORDONATE, CULORI si TEXTURARE;
	glGenBuffers(1, &VboId1);													//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);										//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	//  Se asociaza atributul (2 =  texturare) pentru shader;
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

	glGenBuffers(1, &VboId2);													//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);										//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2), Vertices2, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	//  Se asociaza atributul (2 =  texturare) pentru shader;
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);									//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

}

//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori, Textura), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId1);
	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &EboId);

	glDeleteBuffers(1, &VboTGlacier);
	glDeleteBuffers(1, &ColorTGlacier);
	glDeleteBuffers(1, &EboTGlacier);
	glDeleteBuffers(1, &VboBGlacier);
	glDeleteBuffers(1, &ColorBGlacier);
	glDeleteBuffers(1, &EboBGlacier);

	//  Eliberaea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);

	glDeleteVertexArrays(1, &VaoTGlacier);
	glDeleteVertexArrays(1, &VaoBGlacier);
}

void DestroyAudio() {
	ma_engine_uninit(&engine);
}


//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
	DestroyAudio();
	DestroyShaders();
	DestroyVBO();
}

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	glClearColor(0.49f, 0.98f, 0.91f, 1.0f);		//  Culoarea de fond a ecranului;

	//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateVBO();
	CreateTGlacier();
	CreateBGlacier();
	InitAudio();  // Inițializare audio
	//	Incarcarea texturii;
	LoadPenguinTexture("pinguin.png");
	LoadBackgroundTexture("background.jpeg");
	LoadTGlacierTexture("iceberg.png");
	LoadBGlacierTexture("ghetar.png");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//  Initializarea shaderelor;
	CreateShaders();

	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;

	if (jumping) {
		if (jumpCounter < jumpFrames) {
			penguinY += jumpHeight;
			jumpCounter++;
			// Dacs a atins partea de sus a ferestrei
			if (penguinY > winHeight / 1.5) {
				glutLeaveMainLoop();
				return;
			}
		}
		else {
			jumping = false;
		}
	}
	else {
		if (penguinY > -winHeight / 2) {
			penguinY -= fallSpeed;  // Scade pozitia
		}
		else {
			// Dacs a atins partea de jos a ferestrei
			glutLeaveMainLoop(); // inchide programul
			return;
		}
	}

	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);


	// --------------------- BACKGROUND -----------------
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// Coordonate Bkground
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	//Texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureBackground);
	//	Transmiterea variabilei uniforme pentru texturare spre shaderul de fragmente;
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTGlacier);
	//	Transmiterea variabilei uniforme pentru texturare spre shaderul de fragmente;
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);


	// GHETARI
	UpdateObstaclePosition();
	// SUS
	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(obstacleXPos, 0.0f, 0.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	//
	glBindVertexArray(VaoTGlacier);
	glBindBuffer(GL_ARRAY_BUFFER, VboTGlacier);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,0, (GLvoid*)0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// JOS
	glBindVertexArray(VaoBGlacier);
	glBindBuffer(GL_ARRAY_BUFFER, VboBGlacier);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,0, (GLvoid*)0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureBGlacier);
	//	Transmiterea variabilei uniforme pentru texturare spre shaderul de fragmente;
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// --------------------- PENGU -----------------
	// Seteaza matricea de transformare, cu pozitia verticala a pinguinului
	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, penguinY, 0.0f));
	//myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, penguinY, 0.0f));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	// Coordonate Pengu
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	// Coordonate Texturi
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

	// TEXTURARE
	//
	// Activarea / legarea texturii active
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturePenguin);
	//	Transmiterea variabilei uniforme pentru texturare spre shaderul de fragmente;
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glutSwapBuffers();	//	Inlocuieste imaginea deseneata in fereastra cu cea randata;
	glutPostRedisplay();
	glFlush();			//  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);					//	Se folosesc 2 buffere (unul pentru afisare si unul pentru randare => animatii cursive) si culori RGB;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(0, 0);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Flappy Penguin");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	//	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
	//  Trebuie initializat inainte de desenare;

	glewInit();

	Initialize();							//  Setarea parametrilor necesari pentru fereastra de vizualizare;
	glutDisplayFunc(RenderFunction);		//  Desenarea scenei in fereastra;
	glutIdleFunc(RenderFunction);			//	Asigura rularea continua a randarii;
	glutKeyboardFunc(KeyboardFunction);

	glutCloseFunc(Cleanup);					//  Eliberarea resurselor alocate de program;

	//  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
	//  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

	glutMainLoop();

	return 0;
}