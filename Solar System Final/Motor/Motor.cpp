#pragma comment(lib,"glew32.lib")
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <math.h>
#include <stdio.h>
#include "tinyxml/tinyxml.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/il.h>




//static int const NUM_MAX_PONTOS = 100000;
static int const NUM_MAX_PATHCES = 50;
static int const NUM_MAX_CONTROL_POINTS = NUM_MAX_PATHCES * 16;


//----------------------------------------------------------- ESTRUTURAS DE DADOS
typedef struct Transformacoes {
	float tt, ta, tx, ty, tz; //tt -> avanço da translacao (1/tempo lido em milissegundos); ta -> tempo passado (intervalo 0 a 1)
	float rt, ra, rx, ry, rz;
	float ex, ey, ez;
} TRANSFOR;

typedef struct Solido {
	float *vertexB = (float *)malloc(sizeof(float) * 100000); //Array de vertices do solido
	//float vertexB[NUM_MAX_PONTOS]; Array de vertices
	int nVertex = 0; //Contador onde o array de vertices vai
	int totalVertices = 0; //Numero total de VERTICES do array

	float *normalB = (float *)malloc(sizeof(float) * 100000); //Array de normais do solido
	//float normalB[NUM_MAX_PONTOS]; Array das normais do solido
	int nNormal = 0; //Contador onde o array de normais vai

	float *texturaB = (float *)malloc(sizeof(float) * 100000); //Array de coordenadas de textura do solido
	//float texturaB[NUM_MAX_PONTOS]; Array das coordenadas de textura do solido
	int nTextura = 0; //Contador onde o array de coordenadas de textura vai
	int flagTextura = 0; //Flag que permite saber se o solido tem ou nao uma textura a ser aplicada. 0 -> nao, 1 -> sim!!!
	char nomeTextura[50]; //Nome da textura a aplicar ao solido

	//Pontos por onde vai passar o solido
	float **matrizPontos;
	int nPontos = 0;
	
	TRANSFOR transf[50]; //Limite de 50 transformacoes por solido
	int nTransformacoes; //Numero de transformacoes do solido

	//Materiais para o solido (cor e luz do mesmo)
	float corSolido[3];
	GLfloat difusa[4];
	GLfloat especular[4];
	GLfloat ambiente[4];
	GLfloat emissiva[4];
	GLfloat shininess;
	
	//Flags de controlo para saber que tipo de materiais aplicar ao solido (podia-se ter feito isto de forma melhor)
	int flagCorSolido = 0;
	int flagDifusa = 0;
	int flagEspecular = 0;
	int flagAmbiente = 0;
	int flagEmissiva = 0;
	int flagShininess = 0;
} SOLIDO;

typedef struct Luz {
	GLfloat posicao[4];
	GLfloat ambiente[4];
	GLfloat difusa[4];
	GLfloat especular[4];
	GLfloat spotDir[3];
	GLfloat spotCutof;
	GLfloat spotExponent;
} LUZES;



//----------------------------------------------------------- VARIAVEIS GLOBAIS
SOLIDO solidos[50]; //Limitaçao de até 50 solidos
int nSolidos = 0; //Contador de escrita na variavel global "solidos"

LUZES luzes[20]; //Limitacao de ate 20 luzes
int nLuzes = 0; //Contador de escrita na variavel global "luzes"
int flagLuz = 0; //Flag que indica se existem luzes a ser aplicadas ou nao (0 - nao, 1 - sim)

GLuint *buffers; //Variável para os VBOs. Vai ser o mesmo tamanho que o numero de solidos.
GLuint *normais; //Variavel para as NORMAIS de cada solido. Vai ter o mesmo tamanho que o numero de solidos.
GLuint *texturas; //Variavel para as TEXTURAS de cada solido. Vai ter o mesmo tamanho que o numero de solidos.
GLuint *texturasID; //Necessario para o glGenTextures!!! No maximo tem-se tantas quanto o numero de solidos.

int nTexturas = 0; //Variavel global que contem o numero total de texturas a desenhar. Necessario para o glGenTextures!!!

TRANSFOR transFormacoes[50]; //Limitaçao de até 50 transformaçoes por cada ficheiro XML. Comporta-se como uma especie de stack.
int indiceTransFormacoes = 0; //Indice onde o array "transFormacoes" está a ser escrito.

//Variaveis para o tempo
int tempo;
int tempoTotal = 0;

//Variaveis para a camara:
float alpha = 0.0f, beta = 0.0f, raio = 5.0f;
float camX = 0, camY, camZ = 5;
int startX, startY, tracking = 0;

//Variaveis para o teapot
int p_index[NUM_MAX_PATHCES][16];
float p_cpoints[NUM_MAX_CONTROL_POINTS][3];
int np, ncp;



//---------------------------------------------------------------- FUNCOES
//Funcao auxiliar de verificacao para a linha de comandos para imprimir os pontos
void imprime() {
	for (int i = 0; i < nSolidos; i++) {
		for (int j = 0; j < solidos[i].nVertex; j=j+3) {
			printf("%f %f %f\n", solidos[i].vertexB[j], solidos[i].vertexB[j + 1], solidos[i].vertexB[j + 2]);
		}

		//printf("PROXIMO SOLIDO: \n");
	}
}

//Funcao auxiliar de verificacao para a linha de comandos para imprimir as transformacoes
void imprimeListaTrans() {
	for (int i = 0; i <= indiceTransFormacoes; i++) {
		printf("Translacao indice: %d -> %f %f %f\n", indiceTransFormacoes, transFormacoes[i].tx, transFormacoes[i].ty, transFormacoes[i].tz);
		printf("Rotacao indice: %d -> %f %f %f %f\n", indiceTransFormacoes, transFormacoes[i].ra, transFormacoes[i].rx, transFormacoes[i].ry, transFormacoes[i].rz);
		printf("Escala indice: %d -> %f %f %f\n", indiceTransFormacoes, transFormacoes[i].ex, transFormacoes[i].ey, transFormacoes[i].ez);
	}
	printf("\nProxima Transformacao\n");
}

//Funcao auxiliar de verificacao para a linha de comandos das transformacoes de cada solido
void imprimeListaTransfSolidos() {
	printf("NUMERO DE SOLIDOS: %d\n\n", nSolidos);

	for (int i = 0; i < nSolidos; i++) {
		for (int j = 0; j <= solidos[i].nTransformacoes; j++) {
			printf("Translacao: %f %f %f %f %f\n", solidos[i].transf[j].tt, solidos[i].transf[j].ta, solidos[i].transf[j].tx, solidos[i].transf[j].ty, solidos[i].transf[j].tz);
			printf("Rotacao: %f %f %f %f %f\n", solidos[i].transf[j].rt, solidos[i].transf[j].ra, solidos[i].transf[j].rx, solidos[i].transf[j].ry, solidos[i].transf[j].rz);
			printf("Escala: %f %f %f\n", solidos[i].transf[j].ex, solidos[i].transf[j].ey, solidos[i].transf[j].ez);
		}

		printf("PROXIMO SOLIDO: \n\n");
	}
}

void imprimePontos() {
	printf("PONTOS:\n");
	for (int i = 0; i < nSolidos; i++) {
		for (int j = 0; j < solidos[i].nPontos; j++) {
			printf("%f %f %f\n", solidos[i].matrizPontos[j][0], solidos[i].matrizPontos[j][1], solidos[i].matrizPontos[j][2]);
		}

		printf("PROXIMO SOLIDO: \n\n");
	}
}

//Funcao auxiliar de verificacao para a linha de comandos para imprimir as normais
void imprimeNormais() {
	printf("A IMPRIMIR NORMAIS:\n");
	for (int i = 0; i < nSolidos; i++) {
		for (int j = 0; j < solidos[i].nNormal; j = j + 3) {
			printf("%f %f %f\n", solidos[i].normalB[j], solidos[i].normalB[j + 1], solidos[i].normalB[j + 2]);
		}

		printf("PROXIMO SOLIDO: \n");
	}
}

void imprimeCoresSolidos() {
	printf("A IMPRIMIR CORES SOLIDOS:\n");
	for (int i = 0; i < nSolidos; i++) {
		printf("%f %f %f -> flag = %d\n", solidos[i].corSolido[0], solidos[i].corSolido[1], solidos[i].corSolido[2], solidos[i].flagCorSolido);
		printf("%f %f %f %f -> flag = %d\n", solidos[i].difusa[0], solidos[i].difusa[1], solidos[i].difusa[2], solidos[i].difusa[3], solidos[i].flagDifusa);
		printf("%f %f %f %f -> flag = %d\n", solidos[i].especular[0], solidos[i].especular[1], solidos[i].especular[2], solidos[i].especular[3], solidos[i].flagEspecular);
		printf("%f %f %f %f -> flag = %d\n", solidos[i].ambiente[0], solidos[i].ambiente[1], solidos[i].ambiente[2], solidos[i].ambiente[3], solidos[i].flagAmbiente);
		printf("%f %f %f %f -> flag = %d\n", solidos[i].emissiva[0], solidos[i].emissiva[1], solidos[i].emissiva[2], solidos[i].emissiva[3], solidos[i].flagEmissiva);
	}

	printf("PROXIMO SOLIDO: \n");
}

void imprimeTexturas() {
	printf("A IMPRIMIR TEXTURAS:\n");
	for (int i = 0; i < nSolidos; i++) {
		printf("Nome textura: %s. Flag: %d\n", solidos[i].nomeTextura, solidos[i].flagTextura);
		for (int j = 0; j < solidos[i].nTextura; j = j + 2) {
			printf("%f %f\n", solidos[i].texturaB[j], solidos[i].texturaB[j + 1]);
		}

		printf("PROXIMO SOLIDO: \n");
	}
}


//Funcao para a camara
void esfericasParaCartesianas() {
	camX = raio * cos(beta) * sin(alpha);
	camY = raio * sin(beta);
	camZ = raio * cos(beta) * cos(alpha);
}


//Funcao que inicia a zero as varias transformacoes para um determinado indice
void initTransformacoes() {
	transFormacoes[indiceTransFormacoes].tt = (float)0;
	transFormacoes[indiceTransFormacoes].ta = (float)0;
	transFormacoes[indiceTransFormacoes].tx = (float)0;
	transFormacoes[indiceTransFormacoes].ty = (float)0;
	transFormacoes[indiceTransFormacoes].tz = (float)0;

	transFormacoes[indiceTransFormacoes].rt = (float)0; //NOVA PARA A 3 FASE !!!
	transFormacoes[indiceTransFormacoes].ra = (float)0;
	transFormacoes[indiceTransFormacoes].rx = (float)0;
	transFormacoes[indiceTransFormacoes].ry = (float)0;
	transFormacoes[indiceTransFormacoes].rz = (float)0;

	transFormacoes[indiceTransFormacoes].ex = (float)0;
	transFormacoes[indiceTransFormacoes].ey = (float)0;
	transFormacoes[indiceTransFormacoes].ez = (float)0;
}


void initMemoria() {
	solidos[nSolidos].matrizPontos = (float **)malloc((solidos[nSolidos].nPontos + 1) * sizeof(float*)); //ALOCA LINHA
	solidos[nSolidos].matrizPontos[solidos[nSolidos].nPontos] = (float*)malloc(3 * sizeof(float)); //ALOCA COLUNA
}


//Funcao que coloca as transformacoes em cada solido
void transformacoesSolido(int indiceSolido) {
	for (int i = 0; i <= indiceTransFormacoes; i++) {
		solidos[indiceSolido].transf[i].tt = transFormacoes[i].tt;
		solidos[indiceSolido].transf[i].ta = transFormacoes[i].ta;
		solidos[indiceSolido].transf[i].tx = transFormacoes[i].tx;
		solidos[indiceSolido].transf[i].ty = transFormacoes[i].ty;
		solidos[indiceSolido].transf[i].tz = transFormacoes[i].tz;

		solidos[indiceSolido].transf[i].rt = transFormacoes[i].rt; //NOVA PARA A 3 FASE !!!
		solidos[indiceSolido].transf[i].ra = transFormacoes[i].ra;
		solidos[indiceSolido].transf[i].rx = transFormacoes[i].rx;
		solidos[indiceSolido].transf[i].ry = transFormacoes[i].ry;
		solidos[indiceSolido].transf[i].rz = transFormacoes[i].rz;

		solidos[indiceSolido].transf[i].ex = transFormacoes[i].ex;
		solidos[indiceSolido].transf[i].ey = transFormacoes[i].ey;
		solidos[indiceSolido].transf[i].ez = transFormacoes[i].ez;
	}

	solidos[indiceSolido].nTransformacoes = indiceTransFormacoes; //Tambem podia ser igual ao "i"
}


void initLuzes(int posicao) {
	luzes[posicao].posicao[0] = (float)0;
	luzes[posicao].posicao[1] = (float)0;
	luzes[posicao].posicao[2] = (float)0;
	luzes[posicao].posicao[3] = (float)0;
	luzes[posicao].ambiente[0] = (float)0;
	luzes[posicao].ambiente[1] = (float)0;
	luzes[posicao].ambiente[2] = (float)0;
	luzes[posicao].ambiente[3] = (float)0;
	luzes[posicao].difusa[0] = (float)1;
	luzes[posicao].difusa[1] = (float)1;
	luzes[posicao].difusa[2] = (float)1;
	luzes[posicao].difusa[3] = (float)1;
	luzes[posicao].especular[0] = (float)0.75;
	luzes[posicao].especular[1] = (float)0.75;
	luzes[posicao].especular[2] = (float)0.75;
	luzes[posicao].especular[3] = (float)1;
	luzes[posicao].spotDir[0] = (float)0;
	luzes[posicao].spotDir[1] = (float)0;
	luzes[posicao].spotDir[2] = (float)1;
	luzes[posicao].spotCutof = (float)180;
	luzes[posicao].spotExponent = (float)0;
}


//Funcao que desenha o teapot
void desenharTeaPot(){
	float cpoly[4][4][3];
	int divisions = 4;

	glRotatef(0.0, 0., 1., 0.);
	glRotatef(-90., 1., 0., 0.);

	for (int n = 0; n < np; n++)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{

				for (int k = 0; k < 3; k++) cpoly[i][j][k] = p_cpoints[p_index[n][4 * i + j]][k];
			}
		}

		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0., 1., 12, 4, &cpoly[0][0][0]);
		glMapGrid2f(divisions, 0., 1., divisions, 0., 1.);
		glEvalMesh2(GL_LINE, 0, divisions, 0, divisions);

	}
}


//Funcao para carregar pontos teapot do patch
void initTeaPotPoints(char* path){
	FILE *fd;
	int i, j;


	if ((fd = fopen(path, "r")) == NULL) {
		perror("Impossivel ler ficheiro .patch!!!!!!\n");
		exit(EXIT_FAILURE);
	}

	fscanf(fd, "%d\n", &np);
	printf("Numero Patches: %d\n", np);

	//read control points indexes
	for (i = 0; i < np; i++) {
		for (j = 0; j < 15; j++){
			fscanf(fd, "%d, ", &p_index[i][j]);
		}
		fscanf(fd, "%d", &p_index[i][15]);
		fscanf(fd, "\n");
	}

	fscanf(fd, "%d\n", &ncp);
	printf("Numero Control Points: %d\n", ncp);

	//read control points 
	for (i = 0; i < ncp; i++) {

		fscanf(fd, "%f, ", &p_cpoints[i][0]);
		fscanf(fd, "%f, ", &p_cpoints[i][1]);
		fscanf(fd, "%f\n", &p_cpoints[i][2]);
	}

	fclose(fd);

	glEnable(GL_MAP2_VERTEX_3);
}


//Funcao que abre um ficheiro .3d e le os pontos contidos nele
void abrir3d(char *nomeFicheiro3d) {
	FILE *fd;
	int numeroTriangulos;
	int i;
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	//TRIANGULO triang;
	int count = solidos[nSolidos].nVertex;
	int countN = solidos[nSolidos].nNormal;
	int countT = solidos[nSolidos].nTextura;


	if ((fd = fopen(nomeFicheiro3d, "r")) == NULL) {
		perror("Impossivel ler ficheiro .3d!!!!!!\n");
		exit(EXIT_FAILURE);
	}

	fscanf(fd, "%d\n", &numeroTriangulos);
	printf("Numero Triangulos: %d\n", numeroTriangulos);

	//Pontos
	if (!feof(fd)) {
		for (i = 0; i < numeroTriangulos; i++) {
			fscanf(fd, "%f %f %f\n", &x1, &y1, &z1);
			fscanf(fd, "%f %f %f\n", &x2, &y2, &z2);
			fscanf(fd, "%f %f %f\n", &x3, &y3, &z3);

			//Constroi o array de vertices para cada solido
			solidos[nSolidos].vertexB[count] = x1;
			count++;
			solidos[nSolidos].vertexB[count] = y1;
			count++;
			solidos[nSolidos].vertexB[count] = z1;
			count++;
			solidos[nSolidos].totalVertices++;

			solidos[nSolidos].vertexB[count] = x2;
			count++;
			solidos[nSolidos].vertexB[count] = y2;
			count++;
			solidos[nSolidos].vertexB[count] = z2;
			count++;
			solidos[nSolidos].totalVertices++;

			solidos[nSolidos].vertexB[count] = x3;
			count++;
			solidos[nSolidos].vertexB[count] = y3;
			count++;
			solidos[nSolidos].vertexB[count] = z3;
			count++;
			solidos[nSolidos].totalVertices++;
		}

		solidos[nSolidos].nVertex += count; //Atualiza o contador do array de vertices
	}

	//Normais
	if (!feof(fd)) {
		for (i = 0; i < numeroTriangulos; i++) {
			fscanf(fd, "%f %f %f\n", &x1, &y1, &z1);
			fscanf(fd, "%f %f %f\n", &x2, &y2, &z2);
			fscanf(fd, "%f %f %f\n", &x3, &y3, &z3);


			solidos[nSolidos].normalB[countN] = x1;
			countN++;
			solidos[nSolidos].normalB[countN] = y1;
			countN++;
			solidos[nSolidos].normalB[countN] = z1;
			countN++;

			solidos[nSolidos].normalB[countN] = x2;
			countN++;
			solidos[nSolidos].normalB[countN] = y2;
			countN++;
			solidos[nSolidos].normalB[countN] = z2;
			countN++;

			solidos[nSolidos].normalB[countN] = x3;
			countN++;
			solidos[nSolidos].normalB[countN] = y3;
			countN++;
			solidos[nSolidos].normalB[countN] = z3;
			countN++;
		}

		solidos[nSolidos].nNormal += countN; //Atualiza o contador do array de normais
	}

	//Texturas
	if (!feof(fd)) {
		for (i = 0; i < numeroTriangulos; i++) {
			fscanf(fd, "%f %f\n", &x1, &y1);
			fscanf(fd, "%f %f\n", &x2, &y2);
			fscanf(fd, "%f %f\n", &x3, &y3);


			solidos[nSolidos].texturaB[countT] = x1;
			countT++;
			solidos[nSolidos].texturaB[countT] = y1;
			countT++;

			solidos[nSolidos].texturaB[countT] = x2;
			countT++;
			solidos[nSolidos].texturaB[countT] = y2;
			countT++;

			solidos[nSolidos].texturaB[countT] = x3;
			countT++;
			solidos[nSolidos].texturaB[countT] = y3;
			countT++;
		}

		solidos[nSolidos].nTextura += countT; //Atualiza o contador do array de texturas
	}

	//solidos[nSolidos].nVertex += count; //Atualiza o contador do array de vertices
	//solidos[nSolidos].nNormal += countN; //Atualiza o contador do array de normais
	//solidos[nSolidos].nTextura += countT; //Atualiza o contador do array de texturas

	transformacoesSolido(nSolidos); //Associa as transformacoes a este solido

	nSolidos++;
}


//------------------------------------------------------ FUNCOES DE XML
void abrirXML3(TiXmlElement *pElement, char op) {
	TiXmlAttribute *pAttrib = pElement->FirstAttribute();
	double transVals; //Variavel para guardar os valores das transformacoes e dos pontos das translacoes

	switch (op) {
		case 'l': {
			char tipoLuz[20];

			while (pAttrib) {
				if (strcmp(pAttrib->Name(), "tipo") == 0) {
					strncpy(tipoLuz, pAttrib->Value(), sizeof(pAttrib->Value())*4);
					if (strcmp(tipoLuz, "DIR") == 0) luzes[nLuzes].posicao[3] = (float)0;
					if (strcmp(tipoLuz, "POINT") == 0) luzes[nLuzes].posicao[3] = (float)1;
					if (strcmp(tipoLuz, "SPOTL") == 0) luzes[nLuzes].posicao[3] = (float)-1;
				}
				else if (pAttrib->QueryDoubleValue(&transVals) == TIXML_SUCCESS) {
					if (strcmp(pAttrib->Name(), "posX") == 0) luzes[nLuzes].posicao[0] = (float)transVals;
					if (strcmp(pAttrib->Name(), "posY") == 0) luzes[nLuzes].posicao[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "posZ") == 0) luzes[nLuzes].posicao[2] = (float)transVals;

					if (strcmp(pAttrib->Name(), "ambR") == 0) luzes[nLuzes].ambiente[0] = (float)transVals;
					if (strcmp(pAttrib->Name(), "ambG") == 0) luzes[nLuzes].ambiente[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "ambB") == 0) luzes[nLuzes].ambiente[2] = (float)transVals;

					if (strcmp(pAttrib->Name(), "diffR") == 0) luzes[nLuzes].difusa[0] = (float)transVals;
					if (strcmp(pAttrib->Name(), "diffG") == 0) luzes[nLuzes].difusa[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "diffB") == 0) luzes[nLuzes].difusa[2] = (float)transVals;

					if (strcmp(pAttrib->Name(), "specR") == 0) luzes[nLuzes].especular[0] = (float)transVals;
					if (strcmp(pAttrib->Name(), "specG") == 0) luzes[nLuzes].especular[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "specB") == 0) luzes[nLuzes].especular[2] = (float)transVals;

					if (strcmp(pAttrib->Name(), "spotDX") == 0) luzes[nLuzes].spotDir[0] = (float)transVals;
					if (strcmp(pAttrib->Name(), "spotDY") == 0) luzes[nLuzes].spotDir[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "spotDZ") == 0) luzes[nLuzes].spotDir[2] = (float)transVals;

					if (strcmp(pAttrib->Name(), "spotC") == 0) luzes[nLuzes].spotCutof = (float)transVals;

					if (strcmp(pAttrib->Name(), "spotE") == 0) luzes[nLuzes].spotExponent = (float)transVals;
				}

				pAttrib = pAttrib->Next();
			}

			break;
		}

		case 't': {
			
			//printf("TOU NO TRANSFORMACOES com INDICE: %d\n", indiceTransFormacoes);

			while (pAttrib) {
				if (pAttrib->QueryDoubleValue(&transVals) == TIXML_SUCCESS) {
					if (strcmp(pAttrib->Name(), "tempo") == 0) transFormacoes[indiceTransFormacoes].tt = 1.0f / ((float)transVals * 1000.0f);
					if (strcmp(pAttrib->Name(), "X") == 0) transFormacoes[indiceTransFormacoes].tx = (float) transVals;
					if (strcmp(pAttrib->Name(), "Y") == 0) transFormacoes[indiceTransFormacoes].ty = (float) transVals;
					if (strcmp(pAttrib->Name(), "Z") == 0) transFormacoes[indiceTransFormacoes].tz = (float) transVals;
				}

				pAttrib = pAttrib->Next();
			}

			//imprimeListaTrans();

			break;
		}

		case 'r': {
			//printf("ROTACAO com INDICE: %d\n", indiceTransFormacoes);

			while (pAttrib) {
				if (pAttrib->QueryDoubleValue(&transVals) == TIXML_SUCCESS) {


					if (strcmp(pAttrib->Name(), "tempo") == 0) transFormacoes[indiceTransFormacoes].rt = 360.0f / ((float)transVals * 1000.0f);


					if (strcmp(pAttrib->Name(), "angulo") == 0) transFormacoes[indiceTransFormacoes].ra = (float) transVals;
					if (strcmp(pAttrib->Name(), "eixoX") == 0) transFormacoes[indiceTransFormacoes].rx = (float) transVals;
					if (strcmp(pAttrib->Name(), "eixoY") == 0) transFormacoes[indiceTransFormacoes].ry = (float) transVals;
					if (strcmp(pAttrib->Name(), "eixoZ") == 0) transFormacoes[indiceTransFormacoes].rz = (float) transVals;
				}

				pAttrib = pAttrib->Next();
			}

			//imprimeListaTrans();

			break;
		}

		case 'e': {
			while (pAttrib) {
				if (pAttrib->QueryDoubleValue(&transVals) == TIXML_SUCCESS) {
					if (strcmp(pAttrib->Name(), "X") == 0) transFormacoes[indiceTransFormacoes].ex = (float) transVals;
					if (strcmp(pAttrib->Name(), "Y") == 0) transFormacoes[indiceTransFormacoes].ey = (float) transVals;
					if (strcmp(pAttrib->Name(), "Z") == 0) transFormacoes[indiceTransFormacoes].ez = (float) transVals;
				}

				pAttrib = pAttrib->Next();
			}

			//imprimeListaTrans();

			break;
		}

		case 'p': {
			while (pAttrib) {
				if (pAttrib->QueryDoubleValue(&transVals) == TIXML_SUCCESS) {
					if (strcmp(pAttrib->Name(), "X") == 0) solidos[nSolidos].matrizPontos[solidos[nSolidos].nPontos][0] = (float)transVals;
					if (strcmp(pAttrib->Name(), "Y") == 0) solidos[nSolidos].matrizPontos[solidos[nSolidos].nPontos][1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "Z") == 0) solidos[nSolidos].matrizPontos[solidos[nSolidos].nPontos][2] = (float)transVals;
				}

				pAttrib = pAttrib->Next();
			}

			break;
		}

		case 'm': {
			char nomeFicheiro[40];

			while (pAttrib) {
				if (strcmp(pAttrib->Name(), "ficheiro") == 0) {
					strncpy(nomeFicheiro, pAttrib->Value(), sizeof(pAttrib->Value()) * 4);
					printf("Nome Ficheiro .3d: %s\n", nomeFicheiro);
					abrir3d(nomeFicheiro);
					//pAttrib = pAttrib->Next();
				}
				else if (strcmp(pAttrib->Name(), "patch") == 0) {
					strncpy(nomeFicheiro, pAttrib->Value(), sizeof(pAttrib->Value()) * 4);
					printf("Nome Ficheiro .3d: %s\n", nomeFicheiro);
					initTeaPotPoints(nomeFicheiro);
					//pAttrib = pAttrib->Next();
				}
				else if (strcmp(pAttrib->Name(), "textura") == 0) {
					strcpy(solidos[nSolidos - 1].nomeTextura, pAttrib->Value());
					solidos[nSolidos - 1].flagTextura = 1;
					nTexturas++;
				}
				else if (pAttrib->QueryDoubleValue(&transVals) == TIXML_SUCCESS) {
					if (strcmp(pAttrib->Name(), "corR") == 0) {
						solidos[nSolidos - 1].corSolido[0] = (float)transVals;
						solidos[nSolidos - 1].flagCorSolido = 1; //Mete a flag de cor a 1 para que seja implementada a cor
					}
					if (strcmp(pAttrib->Name(), "corG") == 0) solidos[nSolidos - 1].corSolido[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "corB") == 0) solidos[nSolidos - 1].corSolido[2] = (float)transVals;


					if (strcmp(pAttrib->Name(), "diffR") == 0) {
						solidos[nSolidos - 1].difusa[0] = (float)transVals;
						solidos[nSolidos - 1].flagDifusa = 1; //Mete a flag de difusa a 1 para que seja implementada difusa
						solidos[nSolidos - 1].difusa[3] = (float)1;
					}
					//printf("A IMPRIMIR A CENA\n");
					//printf("%d\n", nSolidos);
					//printf("%f\n", solidos[nSolidos-1].difusa[0]);
					if (strcmp(pAttrib->Name(), "diffG") == 0) solidos[nSolidos - 1].difusa[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "diffB") == 0) solidos[nSolidos - 1].difusa[2] = (float)transVals;


					if (strcmp(pAttrib->Name(), "specR") == 0) {
						solidos[nSolidos - 1].especular[0] = (float)transVals;
						solidos[nSolidos - 1].flagEspecular = 1; //Mete a flag de especular a 1 para que seja implementada especular
						solidos[nSolidos - 1].especular[3] = (float)1;
					}
					if (strcmp(pAttrib->Name(), "specG") == 0) solidos[nSolidos - 1].especular[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "specB") == 0) solidos[nSolidos - 1].especular[2] = (float)transVals;


					if (strcmp(pAttrib->Name(), "ambR") == 0) {
						solidos[nSolidos - 1].ambiente[0] = (float)transVals;
						solidos[nSolidos - 1].flagAmbiente = 1; //Mete a flag de ambiente a 1 para que seja implementada ambiente
						solidos[nSolidos - 1].ambiente[3] = (float)1;
					}
					if (strcmp(pAttrib->Name(), "ambG") == 0) solidos[nSolidos - 1].ambiente[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "ambB") == 0) solidos[nSolidos - 1].ambiente[2] = (float)transVals;


					if (strcmp(pAttrib->Name(), "emiR") == 0) {
						solidos[nSolidos - 1].emissiva[0] = (float)transVals;
						solidos[nSolidos - 1].flagEmissiva = 1; //Meta a flag de emissiva a 1 para que seja implementada emissiva
						solidos[nSolidos - 1].emissiva[3] = (float)1;
					}
					if (strcmp(pAttrib->Name(), "emiG") == 0) solidos[nSolidos - 1].emissiva[1] = (float)transVals;
					if (strcmp(pAttrib->Name(), "emiB") == 0) solidos[nSolidos - 1].emissiva[2] = (float)transVals;


					if (strcmp(pAttrib->Name(), "shininess") == 0) {
						solidos[nSolidos - 1].flagShininess = 1; //Mete a flag de shininess a 1 para que seja implementada shininess
						solidos[nSolidos - 1].shininess = (float)transVals;
					}
				}

				pAttrib = pAttrib->Next();
			}
			
			break;
		}

		default: 
			break;
	}
}


void abrirXML2(TiXmlNode* pParent, unsigned int indent = 0) {
	TiXmlNode *pChild;
	TiXmlAttribute *pAttrib;
	int t;
	
	if (pParent) {
		//printf("TOU AQUI 3\n");
		t = pParent->Type();
		
		switch (t) {
			case TiXmlNode::TINYXML_ELEMENT:
				if (strcmp("grupo", pParent->Value()) == 0) {
					initMemoria(); //Aloca memorias (meter o que falta com memoria dinamica nesta funçao !!!)
					indiceTransFormacoes = indent - 2; //Indent possui o "indice" onde determinado parent está. -2 permite obter esse indice a partir de 0.
					initTransformacoes(); //Inicializa a zero os valores da várias transformacoes para o novo indice.
				}
				if (strcmp("luz", pParent->Value()) == 0) {
					if(flagLuz == 0) flagLuz = 1; //Coloca flag de luz a 1 para indicar que vai existir luzes a desenhar
					initLuzes(nLuzes); //Inicia os valores das luzes para a luz na posicao "nLuzes"
					abrirXML3(pParent->ToElement(), 'l'); //Le os valores das luzes do XML
					nLuzes++; //Atualiza o contador de luzes
				}
				if (strcmp("translacao", pParent->Value()) == 0) abrirXML3(pParent->ToElement(), 't');
				if (strcmp("rotacao", pParent->Value()) == 0) abrirXML3(pParent->ToElement(), 'r');
				if (strcmp("escala", pParent->Value()) == 0) abrirXML3(pParent->ToElement(), 'e');
				if (strcmp("modelo", pParent->Value()) == 0) abrirXML3(pParent->ToElement(), 'm');
				if (strcmp("ponto", pParent->Value()) == 0) {
					abrirXML3(pParent->ToElement(), 'p');
					//Atualiza o numero de pontos
					solidos[nSolidos].nPontos++;
					//Realoca memoria para os pontos
					solidos[nSolidos].matrizPontos = (float**)realloc(solidos[nSolidos].matrizPontos, (solidos[nSolidos].nPontos + 1) * sizeof(float*));
					solidos[nSolidos].matrizPontos[solidos[nSolidos].nPontos] = (float *)malloc(3 * sizeof(float));
				}
				
				break;
			
			default:
				//printf("NAO E ELEMENT\n");
				break;
		}

		for (pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) abrirXML2(pChild, indent + 1);
	}
}


void abrirXML(char *nomeFicheiro) {
	bool flag;

	TiXmlDocument ficheiro(nomeFicheiro); //"ficheiro" fica com toda a informaçao !!!
	if (flag = ficheiro.LoadFile()) {
		abrirXML2(&ficheiro);
	}
	else printf("Nao foi possivel abrir ficheiro XML\n");
}
//----------------------------------------------------- FIM FUNCOES XML




//----------------------------------------------------- FUNCOES DE GLUT
void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


//Funçao feita para testar se os VBOs estao a funcionar bem. Apagar depois !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void desenhar() {
	for (int i = 0; i < nSolidos; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, solidos[i].totalVertices);
	}
}


//Variaveis globais com os valores considerados default para as cores dos solidos
GLfloat defaultDifusa[4] = {0.8, 0.8, 0.8, 1.0};
GLfloat defaultAmbiente[4] = {0.2, 0.2, 0.2, 1.0};
GLfloat defaultEspecular[4] = {0.75, 0.75, 0.75, 1.0};
GLfloat defaultEmissiva[4] = {0.0, 0.0, 0.0, 1.0};
GLfloat defaultShininess = 128.0;

void colocaMaterialSolido(int posSolido) {
	//printf("CORES !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	//printf("Solido: %d Flag: %d NumSolidos: %d\n", posSolido, solidos[posSolido].flagCorSolido, nSolidos);
	if (solidos[posSolido].flagCorSolido == 1) glColor3f(solidos[posSolido].corSolido[0], solidos[posSolido].corSolido[1], solidos[posSolido].corSolido[2]);
	else glColor3f(1.0, 1.0, 1.0);
	if (solidos[posSolido].flagDifusa == 1) glMaterialfv(GL_FRONT, GL_DIFFUSE, solidos[posSolido].difusa);
	else glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultDifusa);
	if (solidos[posSolido].flagEspecular == 1) glMaterialfv(GL_FRONT, GL_SPECULAR, solidos[posSolido].especular);
	else glMaterialfv(GL_FRONT, GL_SPECULAR, defaultEspecular);
	if (solidos[posSolido].flagAmbiente == 1) glMaterialfv(GL_FRONT, GL_AMBIENT, solidos[posSolido].ambiente);
	else glMaterialfv(GL_FRONT, GL_AMBIENT, defaultAmbiente);
	if (solidos[posSolido].flagEmissiva == 1) glMaterialfv(GL_FRONT, GL_EMISSION, solidos[posSolido].emissiva);
	else glMaterialfv(GL_FRONT, GL_EMISSION, defaultEmissiva);
	if (solidos[posSolido].flagShininess == 1) glMaterialf(GL_FRONT, GL_SHININESS, solidos[posSolido].shininess);
	else {
		glMaterialf(GL_FRONT, GL_SHININESS, defaultShininess);
		//printf("SHININESS: %f\n", defaultShininess);
	}
}


void colocaLuzes() {
	for (int i = 0; i < nLuzes; i++) {
		glLightfv(GL_LIGHT0 + i, GL_POSITION, luzes[i].posicao); //Posicao
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, luzes[i].ambiente); //Cor
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, luzes[i].difusa); //Cor
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, luzes[i].especular); //Cor
		if (luzes[i].posicao[3] == (float)-1) { //Caso seja uma spotlight tem de se passar mais valores para serem desenhados !!!!!!!!!!!!!!!!!!
			glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, luzes[i].spotDir);
			glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, luzes[i].spotCutof);
			glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, luzes[i].spotExponent);
		}
	}
}


void getCatmullRomPoint(float t, int *indices, float *res, int posSolido) {
	res[0] = res[1] = res[2] = 0.0;
	
	//Catmull-rom matrix
	float m[4][4] = {	{ -0.5f, 1.5f, -1.5f, 0.5f },
						{ 1.0f, -2.5f, 2.0f, -0.5f },
						{ -0.5f, 0.0f, 0.5f, 0.0f },
						{ 0.0f, 1.0f, 0.0f, 0.0f }	};

	float sum = 0;
	float mT[4]; //Matriz T
	//Matriz auxiliar que guarda a multiplicaçao da matriz M (catmull-rom matrix) por os quatro pontos (4)
	//da curva que estao a ser utilizados (que sao passados na variavel indices) !!!
	float mXpCurva[4][3];
	//Constroi a matriz T:
	mT[0] = t*t*t;
	mT[1] = t*t;
	mT[2] = t;
	mT[3] = 1;

	//Calculo de M*P para mXpCurva
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 4; k++) {
				sum += m[i][k] * solidos[posSolido].matrizPontos[indices[k]][j];
			}
			mXpCurva[i][j] = sum; //Guarda os valores na matriz mXpCurva
			sum = 0;
		}
	}

	//Calculo de mXpCurva*T (T é mT)
	for (int j = 0; j < 3; j++) {
		for (int k = 0; k < 4; k++) {
			sum += mT[k] * mXpCurva[k][j];
		}
		res[j] = sum; //O res é o array que contem todos os pontinhos entre dois pontos principais !!!
		sum = 0;
	}
}


//Given global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, float *res, int posSolido) {
	float t = gt * (solidos[posSolido].nPontos); //This is the real global t
	int index = floor(t); //Which segment
	t = t - index; //Where within the segment

	//Indices store the points
	if (solidos[posSolido].nPontos == (float)0) solidos[posSolido].nPontos = (float)1;
	if (solidos[posSolido].nPontos == (float)0) printf("E ZERO !!!!!!!!!!!!!!!!!!!!!!!!! \n");
	int indices[4];
	indices[0] = (index + (solidos[posSolido].nPontos) - 1) % (solidos[posSolido].nPontos);
	indices[1] = (indices[0] + 1) % (solidos[posSolido].nPontos);
	indices[2] = (indices[1] + 1) % (solidos[posSolido].nPontos);
	indices[3] = (indices[2] + 1) % (solidos[posSolido].nPontos);

	getCatmullRomPoint(t, indices, res, posSolido);
}


//Funcao que desenha os triangulos/solidos. ORDEM IMPLEMENTADA: ROTACAO -> TRANSLACAO -> ESCALA
void desenharSolidos() {
	float res[3];
	int auxText = 0;

	//glPushMatrix();
	glEnable(GL_NORMALIZE);
	if (flagLuz == 1) colocaLuzes();

	for (int i = 0; i < nSolidos; i++) { //Percorre o array de solidos
		glPushMatrix();

		for (int k = 0; k <= solidos[i].nTransformacoes; k++) { //Percorre o array de transformacoes de cada solido
			//Translacoes
			if (solidos[i].transf[k].tt != (float)0 || solidos[i].transf[k].ta != 0 || solidos[i].transf[k].tx != (float)0 || solidos[i].transf[k].ty != (float)0 || solidos[i].transf[k].tz != (float)0) {
				if (solidos[i].transf[k].tt != 0) {
					//imprimePontos();
					if (solidos[i].transf[k].ta > 1) solidos[i].transf[k].ta -= 1.0f;
					getGlobalCatmullRomPoint(solidos[i].transf[k].ta, res, i);
					glTranslatef(res[0], res[1], res[2]);
					solidos[i].transf[k].ta += tempo * solidos[i].transf[k].tt;
					
				}
				else glTranslatef(solidos[i].transf[k].tx, solidos[i].transf[k].ty, solidos[i].transf[k].tz);
			}


			//Rotacoes
			if (solidos[i].transf[k].rt != (float)0 || solidos[i].transf[k].ra != (float)0 || solidos[i].transf[k].rx != (float)0 || solidos[i].transf[k].ry != (float)0 || solidos[i].transf[k].rz != (float)0) {
				if (solidos[i].transf[k].rt != (float)0) {
					if (solidos[i].transf[k].ra > 360.0f) solidos[i].transf[k].ra -= 360.0f;
					glRotatef(solidos[i].transf[k].ra, solidos[i].transf[k].rx, solidos[i].transf[k].ry, solidos[i].transf[k].rz);
					solidos[i].transf[k].ra += tempo * solidos[i].transf[k].rt;
				}
				else glRotatef(solidos[i].transf[k].ra, solidos[i].transf[k].rx, solidos[i].transf[k].ry, solidos[i].transf[k].rz);
			}

			
			//Escalas
			if (solidos[i].transf[k].ex != (float)0 || solidos[i].transf[k].ey != (float)0 || solidos[i].transf[k].ez != (float)0) {
				if (solidos[i].transf[k].ex == (float)0) solidos[i].transf[k].ex = (float)1;
				if (solidos[i].transf[k].ey == (float)0) solidos[i].transf[k].ey = (float)1;
				if (solidos[i].transf[k].ez == (float)0) solidos[i].transf[k].ez = (float)1;
				glScalef(solidos[i].transf[k].ex, solidos[i].transf[k].ey, solidos[i].transf[k].ez);
			}
		}

		//Desenha o solido que esta no indice (i) actual
		colocaMaterialSolido(i);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, normais[i]);
		glNormalPointer(GL_FLOAT, 0, 0);
		//Aplica texturas caso tenha textura para ser aplicada
		if (solidos[i].flagTextura == 1) {
			glBindBuffer(GL_ARRAY_BUFFER, texturas[i]);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
			glBindTexture(GL_TEXTURE_2D, texturasID[auxText]);
			auxText++;
		}
		glDrawArrays(GL_TRIANGLES, 0, solidos[i].totalVertices);
		if (solidos[i].flagTextura == 1) glBindTexture(GL_TEXTURE_2D, 0);

		//Faz o pop matrix
		glPopMatrix();
	}
}


void renderScene(void) {
	//Tempo
	int tempoAc = glutGet(GLUT_ELAPSED_TIME);

	//Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	//Por instruções de desenho aqui
	//if (flagLuz == 1) colocaLuzes();
	desenharSolidos();
	//desenhar();

	//End of frame
	glutSwapBuffers();
	
	//Atualiza tempos
	tempo = (tempoAc - tempoTotal);
	tempoTotal = tempoAc;
}


void iniciarVBOs() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (nTexturas > 0) glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	buffers = (GLuint *)malloc(sizeof(GLuint) * (nSolidos)); //Aloca memoria para o array de pontos (mesmo tamanho que o array de solidos quando ja preenchido)
	normais = (GLuint *)malloc(sizeof(GLuint) * (nSolidos)); //Aloca memoria para o array de normais (mesmo tamanho que o array de solidos quando ja preenchido)
	texturas = (GLuint *)malloc(sizeof(GLuint) * (nSolidos)); //Aloca memoria para o array texturas (mesmo tamanho que o array de solidos quando ja preencgido)

	glGenBuffers(nSolidos, buffers);
	glGenBuffers(nSolidos, normais);
	glGenBuffers(nSolidos, texturas);
	
	for (int i = 0; i < nSolidos; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * solidos[i].nVertex, solidos[i].vertexB, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normais[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * solidos[i].nNormal, solidos[i].normalB, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, texturas[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * solidos[i].nTextura, solidos[i].texturaB, GL_STATIC_DRAW);
	}
}


void preparaTexturas() {
	unsigned int ima, imaW, imaH;
	unsigned char *imaData;
	int posicaoTextura = 0; //Variavel para saber a posicao de textura a aplicar

	texturasID = (GLuint *)malloc(sizeof(GLuint) * (nSolidos)); //Aloca memoria para o array de ID de texturas (no maximo tem o mesmo tamanho que o numero de solidos)

	glGenTextures(nTexturas, texturasID);

	for (int i = 0; i < nSolidos; i++) { //Percorre todos os solidos para verificar quais possuem texturas a desenhar
		if (solidos[i].flagTextura == 1) {
			ilGenImages(1, &ima);
			ilBindImage(ima);
			ilLoadImage((ILstring) solidos[i].nomeTextura);
			imaW = ilGetInteger(IL_IMAGE_WIDTH);
			imaH = ilGetInteger(IL_IMAGE_HEIGHT);
			ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
			imaData = ilGetData();

			glBindTexture(GL_TEXTURE_2D, texturasID[posicaoTextura]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imaW, imaH, 0, GL_RGBA, GL_UNSIGNED_BYTE, imaData);

			posicaoTextura++; //Atualiza o contador para "apontar" para a proxima textura a ser desenhada
		}
	}
}


//Funcao de processamento de carateres normais
/*void caracteresNormais(unsigned char tecla, int x, int y) {
	switch (tecla) {
		case 'd':
			alpha -= 0.1;
			break;
		case 'a':
			alpha += 0.1;
			break;
		case 's':
			beta -= 0.1f;
			if (beta < -1.5f) beta = -1.5f;
			break;
		case 'w':
			beta += 0.1f;
			if (beta > 1.5f) beta = 1.5f;
			break;
		case 'z': raio -= 0.1f;
			if (raio < 0.1f)
				raio = 0.1f;
			break;
		case 'x': raio += 0.1f;
			break;
		default:
			break;
	}

	esfericasParaCartesianas();
	glutPostRedisplay();
}*/


void processMouseButtons(int button, int state, int xx, int yy) {
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			raio -= yy - startY;
			if (raio < 3)
				raio = 3.0;
		}
		tracking = 0;
	}
}


void processMouseMotion(int xx, int yy) {
	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {
		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = raio;
	}
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = raio - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camY = rAux *							     sin(betaAux * 3.14 / 180.0);
}


//Função de processamento do menu
void processaMenu(int opcao) {
	switch (opcao) {
	case GL_FILL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case GL_LINE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case GL_POINT:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 0:
		if (!glIsEnabled(GL_LIGHTING)) glEnable(GL_LIGHTING);
		break;
	case 1:
		if (glIsEnabled(GL_LIGHTING)) glDisable(GL_LIGHTING);
		break;
	}
	glutPostRedisplay();
}


void menus() {
	int menuId;

	menuId = glutCreateMenu(processaMenu);

	//Adicionar entradas ao menu
	glutAddMenuEntry("GL_FILL", GL_FILL);
	glutAddMenuEntry("GL_LINE", GL_LINE);
	glutAddMenuEntry("GL_POINT", GL_POINT);
	glutAddMenuEntry("LIGHTS_ON", 0);
	glutAddMenuEntry("LIGHTS_OFF", 1);

	//Associar o menu ao botao direito do rato
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
	glutPostRedisplay();
}



int main(int argc, char **argv) {
	if (argc == 2) {
		printf("A abrir ficheiro: %s\n", argv[1]);

		abrirXML(argv[1]);


		imprime();
		imprimeNormais();
		imprimeCoresSolidos();
		imprimeTexturas();
		imprimeListaTransfSolidos();
		//imprimePontos();


		//Inicialização
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
		glutInitWindowPosition(100, 100);
		glutInitWindowSize(800, 800);
		glutCreateWindow("FASE 4 - CG");


		//Registo de funções 
		glutDisplayFunc(renderScene);
		glutIdleFunc(renderScene);
		glutReshapeFunc(changeSize);


		//Registo da funções do teclado e rato
		//glutKeyboardFunc(caracteresNormais);
		glutMouseFunc(processMouseButtons);
		glutMotionFunc(processMouseMotion);


		//Criação do menu
		menus();


		//Iniciar o glew
		glewInit();

		//Iniciar o DevIL
		ilInit();
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);


		//Ligar as luzes
		if (flagLuz == 1) {
			//Ligar o quadro:
			glEnable(GL_LIGHTING);
			//Ligar as varias luzes:
			for (int i = 0; i < nLuzes; i++) {
				glEnable(GL_LIGHT0 + i);
			}
		}


		//Alguns settings para OpenGL
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glShadeModel(GL_SMOOTH);

		//Permitir texturas
		glEnable(GL_TEXTURE_2D);


		//Iniciar VBOs
		iniciarVBOs();


		//Preparar texturas
		preparaTexturas();


		//Entrar no ciclo do GLUT 
		glutMainLoop();


		return 1;
	}

	return 0;
}