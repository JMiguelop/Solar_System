#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




//int const NUM_MAX_TRIANGULOS = 12000;


//-----------------------------------------------------------ESTRUTURA DE DADOS
typedef struct Triangulo {
	//Pontos
	float ponto1[3];
	float ponto2[3];
	float ponto3[3];

	//Normais
	float normal1[3];
	float normal2[3];
	float normal3[3];
	
	//Texturas
	float text1[2];
	float text2[2];
	float text3[2];
} TRIANGULO;



//----------------------------------------------------------------FUNCOES

//Grava para ficheiro os varios triangulos que constituem o solido
void grava3d(TRIANGULO *triang, int nTriangulos, char nomeFicheiro[30]) {
	FILE *fp;
	int i;

	if ((fp = fopen(nomeFicheiro, "w")) == NULL) {
		perror("Impossivel gravar ficheiro!!!!!!\n");
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%d\n", nTriangulos);

	//A cada 3 linhas vai ser um triangulo novo no ficheiro !!!
	//Escreve os pontos:
	for (i = 0; i < nTriangulos; i++) {
		fprintf(fp, "%f %f %f\n", triang[i].ponto1[0], triang[i].ponto1[1], triang[i].ponto1[2]);
		fprintf(fp, "%f %f %f\n", triang[i].ponto2[0], triang[i].ponto2[1], triang[i].ponto2[2]);
		fprintf(fp, "%f %f %f\n", triang[i].ponto3[0], triang[i].ponto3[1], triang[i].ponto3[2]);
		fflush(fp);
	}
	
	//Escreve as normais:
	for (i = 0; i < nTriangulos; i++) {
		fprintf(fp, "%f %f %f\n", triang[i].normal1[0], triang[i].normal1[1], triang[i].normal1[2]);
		fprintf(fp, "%f %f %f\n", triang[i].normal2[0], triang[i].normal2[1], triang[i].normal2[2]);
		fprintf(fp, "%f %f %f\n", triang[i].normal3[0], triang[i].normal3[1], triang[i].normal3[2]);
		fflush(fp);
	}

	//Escreve as coordenadas de textura:
	for (i = 0; i < nTriangulos; i++) {
		fprintf(fp, "%f %f\n", triang[i].text1[0], triang[i].text1[1]);
		fprintf(fp, "%f %f\n", triang[i].text2[0], triang[i].text2[1]);
		fprintf(fp, "%f %f\n", triang[i].text3[0], triang[i].text3[1]);
		fflush(fp);
	}

	fclose(fp);
}


//ESFERA (raio, fatias, camadas, nome ficheiro onde vai ser gravado)
void esfera(int raio, int nfatias, int ncamadas, char nomeFicheiro[30]) {
	TRIANGULO *triangulos = (TRIANGULO *)malloc(sizeof(TRIANGULO) * 30000);
	int nTriangulos = 0;
	float alturaCamada = ((float)raio * 2) / ncamadas;
	float raioTemp = 0; //raio da camada a desenhar
	float raioTempPrev = 0; //raio da camada anterior
	float alfa = ((2 * M_PI) / (float)nfatias); //angulo alfa no plano y = 0
	float beta; // angulo beta no plano x = 0 ou z = 0, depende da camada em que esta, pi/2 > beta > -pi/2

	float normal[3];

	//TOPO 1 da esfera (metade dos triangulos, todos com o centro em comum, tem 0.2 da altura por camada)
	beta = asin(((float)raio - (alturaCamada * 0.2)) / raio);
	raioTemp = (float)raio * cos(beta);
	for (int fatia = 1; fatia <= nfatias; fatia++){

		//pontos

		triangulos[nTriangulos].ponto1[0] = 0.0f;
		triangulos[nTriangulos].ponto1[1] = (float)raio;
		triangulos[nTriangulos].ponto1[2] = 0.0f;
		triangulos[nTriangulos].text1[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text1[1] = 1; //topo da esfera e textura

		triangulos[nTriangulos].ponto2[0] = raioTemp * sin(alfa * fatia);
		triangulos[nTriangulos].ponto2[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].ponto2[2] = raioTemp * cos(alfa * fatia);
		triangulos[nTriangulos].text2[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text2[1] = 1 - ((1 * 0.2) / (float)ncamadas);//0 representa a camada 0

		triangulos[nTriangulos].ponto3[0] = raioTemp * sin(alfa * (fatia + 1));
		triangulos[nTriangulos].ponto3[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].ponto3[2] = raioTemp * cos(alfa * (fatia + 1));
		triangulos[nTriangulos].text3[0] = (fatia + 1) / (float)nfatias;
		triangulos[nTriangulos].text3[1] = 1 - ((1 * 0.2) / (float)ncamadas);//0 representa a camada 0

		//normais

		triangulos[nTriangulos].normal1[0] = 0;
		triangulos[nTriangulos].normal1[1] = (float)raio;
		triangulos[nTriangulos].normal1[2] = 0;

		triangulos[nTriangulos].normal2[0] = sin(alfa * fatia);
		triangulos[nTriangulos].normal2[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].normal2[2] = cos(alfa * fatia);

		triangulos[nTriangulos].normal3[0] = sin(alfa * (fatia + 1));
		triangulos[nTriangulos].normal3[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].normal3[2] = cos(alfa * (fatia + 1));

		nTriangulos++;

	}

	//TOPO 2 da esfera 
	raioTempPrev = raioTemp;
	beta = asin(((float)raio - (alturaCamada * 1)) / raio);
	raioTemp = (float)raio * cos(beta);
	for (int fatia = 1; fatia <= nfatias; fatia++){

		//2 triangulos por fatia
		triangulos[nTriangulos].ponto1[0] = raioTempPrev * sin(alfa * fatia);
		triangulos[nTriangulos].ponto1[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].ponto1[2] = raioTempPrev * cos(alfa * fatia);
		triangulos[nTriangulos].text1[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text1[1] = 1 - ((1 - 1) / (float)ncamadas); //1 representa a camada 1

		triangulos[nTriangulos].ponto2[0] = raioTemp * sin(alfa * fatia);
		triangulos[nTriangulos].ponto2[1] = (float)raio - (alturaCamada * 1);
		triangulos[nTriangulos].ponto2[2] = raioTemp * cos(alfa * fatia);
		triangulos[nTriangulos].text2[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text2[1] = 1 - ((1) / (float)ncamadas);//1 representa a camada 1

		triangulos[nTriangulos].ponto3[0] = raioTemp * sin(alfa * (fatia + 1));
		triangulos[nTriangulos].ponto3[1] = (float)raio - (alturaCamada * 1);
		triangulos[nTriangulos].ponto3[2] = raioTemp * cos(alfa * (fatia + 1));
		triangulos[nTriangulos].text3[0] = (fatia + 1) / (float)nfatias;
		triangulos[nTriangulos].text3[1] = 1 - ((1) / (float)ncamadas);//1 representa a camada 1


		triangulos[nTriangulos].normal1[0] = sin(alfa * fatia);
		triangulos[nTriangulos].normal1[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].normal1[2] = cos(alfa * fatia);

		triangulos[nTriangulos].normal2[0] = sin(alfa * fatia);
		triangulos[nTriangulos].normal2[1] = (float)raio - (alturaCamada * 1);
		triangulos[nTriangulos].normal2[2] = cos(alfa * fatia);

		triangulos[nTriangulos].normal3[0] = sin(alfa * (fatia + 1));
		triangulos[nTriangulos].normal3[1] = (float)raio - (alturaCamada * 1);
		triangulos[nTriangulos].normal3[2] = cos(alfa * (fatia + 1));

		nTriangulos++;

		triangulos[nTriangulos].ponto1[0] = raioTempPrev * sin(alfa * fatia);
		triangulos[nTriangulos].ponto1[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].ponto1[2] = raioTempPrev * cos(alfa * fatia);
		triangulos[nTriangulos].text1[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text1[1] = 1 - ((1 - 1) / (float)ncamadas); //1 representa a camada 1


		triangulos[nTriangulos].ponto2[0] = raioTemp * sin(alfa * (fatia + 1));
		triangulos[nTriangulos].ponto2[1] = (float)raio - (alturaCamada * 1);
		triangulos[nTriangulos].ponto2[2] = raioTemp * cos(alfa * (fatia + 1));
		triangulos[nTriangulos].text2[0] = (fatia + 1) / (float)nfatias;
		triangulos[nTriangulos].text2[1] = 1 - ((1) / (float)ncamadas);//1 representa a camada 1

		triangulos[nTriangulos].ponto3[0] = raioTempPrev * sin(alfa * (fatia + 1));
		triangulos[nTriangulos].ponto3[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].ponto3[2] = raioTempPrev * cos(alfa * (fatia + 1));
		triangulos[nTriangulos].text3[0] = (fatia + 1) / (float)nfatias;
		triangulos[nTriangulos].text3[1] = 1 - ((1 - 1) / (float)ncamadas);//1 representa a camada 1


		triangulos[nTriangulos].normal1[0] = sin(alfa * fatia);
		triangulos[nTriangulos].normal1[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].normal1[2] = cos(alfa * fatia);

		triangulos[nTriangulos].normal2[0] = sin(alfa * (fatia + 1));
		triangulos[nTriangulos].normal2[1] = (float)raio - (alturaCamada * 1);
		triangulos[nTriangulos].normal2[2] = cos(alfa * (fatia + 1));

		triangulos[nTriangulos].normal3[0] = sin(alfa * (fatia + 1));
		triangulos[nTriangulos].normal3[1] = (float)raio - (alturaCamada * 0.2);
		triangulos[nTriangulos].normal3[2] = cos(alfa * (fatia + 1));

		nTriangulos++;
	}

	for (int camada = 2; camada < ncamadas; camada++){

		beta = asin(((float)raio - (alturaCamada * camada)) / raio);
		raioTempPrev = raioTemp;
		raioTemp = raio * cos(beta);

		for (int fatia = 1; fatia <= nfatias; fatia++){

			//2 triangulos por fatia
			//Triangulo de baixo na fatia
			//Vertice cima
			triangulos[nTriangulos].ponto1[0] = raioTempPrev * sin(alfa * fatia);
			triangulos[nTriangulos].ponto1[1] = (float)raio - (alturaCamada * (camada - 1));
			triangulos[nTriangulos].ponto1[2] = raioTempPrev * cos(alfa * fatia);
			triangulos[nTriangulos].text1[0] = (fatia) / (float)nfatias;
			triangulos[nTriangulos].text1[1] = 1 - ((camada - 1) / (float)ncamadas);


			//Vertice Baixo esquerda
			triangulos[nTriangulos].ponto2[0] = raioTemp * sin(alfa * fatia);
			triangulos[nTriangulos].ponto2[1] = (float)raio - (alturaCamada * camada);
			triangulos[nTriangulos].ponto2[2] = raioTemp * cos(alfa * fatia);
			triangulos[nTriangulos].text2[0] = (fatia) / (float)nfatias;
			triangulos[nTriangulos].text2[1] = 1 - ((camada) / (float)ncamadas);


			//Vertice Baixo Direita
			triangulos[nTriangulos].ponto3[0] = raioTemp * sin(alfa * (fatia + 1));
			triangulos[nTriangulos].ponto3[1] = (float)raio - (alturaCamada * camada);
			triangulos[nTriangulos].ponto3[2] = raioTemp * cos(alfa * (fatia + 1));
			triangulos[nTriangulos].text3[0] = (fatia + 1) / (float)nfatias;
			triangulos[nTriangulos].text3[1] = 1 - ((camada) / (float)ncamadas);


			triangulos[nTriangulos].normal1[0] = sin(alfa * fatia);
			triangulos[nTriangulos].normal1[1] = (float)raio - (alturaCamada * (camada - 1));
			triangulos[nTriangulos].normal1[2] = cos(alfa * fatia);

			triangulos[nTriangulos].normal2[0] = sin(alfa * fatia);
			triangulos[nTriangulos].normal2[1] = (float)raio - (alturaCamada * camada);
			triangulos[nTriangulos].normal2[2] = cos(alfa * fatia);

			triangulos[nTriangulos].normal3[0] = sin(alfa * (fatia + 1));
			triangulos[nTriangulos].normal3[1] = (float)raio - (alturaCamada * camada);
			triangulos[nTriangulos].normal3[2] = cos(alfa * (fatia + 1));

			nTriangulos++;
			//Triangulo de cima na fatia
			//Vertice cima esquerda
			triangulos[nTriangulos].ponto1[0] = raioTempPrev * sin(alfa * fatia);
			triangulos[nTriangulos].ponto1[1] = (float)raio - (alturaCamada * (camada - 1));
			triangulos[nTriangulos].ponto1[2] = raioTempPrev * cos(alfa * fatia);
			triangulos[nTriangulos].text1[0] = (fatia) / (float)nfatias;
			triangulos[nTriangulos].text1[1] = 1 - ((camada - 1) / (float)ncamadas);


			//Vertice baixo
			triangulos[nTriangulos].ponto2[0] = raioTemp * sin(alfa * (fatia + 1));
			triangulos[nTriangulos].ponto2[1] = (float)raio - (alturaCamada * camada);
			triangulos[nTriangulos].ponto2[2] = raioTemp * cos(alfa * (fatia + 1));
			triangulos[nTriangulos].text2[0] = (fatia + 1) / (float)nfatias;
			triangulos[nTriangulos].text2[1] = 1 - ((camada) / (float)ncamadas);

			//Vertice cima direita
			triangulos[nTriangulos].ponto3[0] = raioTempPrev * sin(alfa * (fatia + 1));
			triangulos[nTriangulos].ponto3[1] = (float)raio - (alturaCamada * (camada - 1));
			triangulos[nTriangulos].ponto3[2] = raioTempPrev * cos(alfa * (fatia + 1));
			triangulos[nTriangulos].text3[0] = (fatia + 1) / (float)nfatias;
			triangulos[nTriangulos].text3[1] = 1 - ((camada - 1) / (float)ncamadas);


			triangulos[nTriangulos].normal1[0] = sin(alfa * fatia);
			triangulos[nTriangulos].normal1[1] = (float)raio - (alturaCamada * (camada - 1));
			triangulos[nTriangulos].normal1[2] = cos(alfa * fatia);

			triangulos[nTriangulos].normal2[0] = sin(alfa * (fatia + 1));
			triangulos[nTriangulos].normal2[1] = (float)raio - (alturaCamada * camada);
			triangulos[nTriangulos].normal2[2] = cos(alfa * (fatia + 1));

			triangulos[nTriangulos].normal3[0] = sin(alfa * (fatia + 1));
			triangulos[nTriangulos].normal3[1] = (float)raio - (alturaCamada * (camada - 1));
			triangulos[nTriangulos].normal3[2] = cos(alfa * (fatia + 1));

			nTriangulos++;
		}

	}

	//FUNDO 1 da esfera 
	raioTempPrev = raioTemp;
	beta = asin(((float)raio - (alturaCamada * (ncamadas - 0.2))) / raio);
	raioTemp = (float)raio * cos(beta);
	for (int fatia = 1; fatia <= nfatias; fatia++){

		//2 triangulos por fatia
		triangulos[nTriangulos].ponto3[0] = raioTemp * sin(alfa * (fatia + 1));
		triangulos[nTriangulos].ponto3[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].ponto3[2] = raioTemp * cos(alfa * (fatia + 1));
		triangulos[nTriangulos].text3[0] = (fatia + 1) / (float)nfatias;
		triangulos[nTriangulos].text3[1] = 1 - ((ncamadas - 0.2) / ((float)ncamadas));

		triangulos[nTriangulos].ponto2[0] = raioTemp * sin(alfa * fatia);
		triangulos[nTriangulos].ponto2[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].ponto2[2] = raioTemp * cos(alfa * fatia);
		triangulos[nTriangulos].text2[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text2[1] = 1 - ((ncamadas - 0.2) / ((float)ncamadas));

		triangulos[nTriangulos].ponto1[0] = raioTempPrev * sin(alfa * fatia);
		triangulos[nTriangulos].ponto1[1] = (float)raio - (alturaCamada * (ncamadas - 1));
		triangulos[nTriangulos].ponto1[2] = raioTempPrev * cos(alfa * fatia);
		triangulos[nTriangulos].text1[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text1[1] = 1 - ((ncamadas - 1) / ((float)ncamadas));

		triangulos[nTriangulos].normal3[0] = sin(alfa * (fatia + 1));
		triangulos[nTriangulos].normal3[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].normal3[2] = cos(alfa * (fatia + 1));

		triangulos[nTriangulos].normal2[0] = sin(alfa * fatia);
		triangulos[nTriangulos].normal2[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].normal2[2] = cos(alfa * fatia);

		triangulos[nTriangulos].normal1[0] = sin(alfa * fatia);
		triangulos[nTriangulos].normal1[1] = (float)raio - (alturaCamada * (ncamadas - 1));
		triangulos[nTriangulos].normal1[2] = cos(alfa * fatia);

		nTriangulos++;

		triangulos[nTriangulos].ponto3[0] = raioTemp * sin(alfa * (fatia + 1));
		triangulos[nTriangulos].ponto3[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].ponto3[2] = raioTemp * cos(alfa * (fatia + 1));
		triangulos[nTriangulos].text3[0] = (fatia + 1) / (float)nfatias;
		triangulos[nTriangulos].text3[1] = 1 - ((ncamadas - 0.2) / ((float)ncamadas));

		triangulos[nTriangulos].ponto2[0] = raioTempPrev * sin(alfa * fatia);
		triangulos[nTriangulos].ponto2[1] = (float)raio - (alturaCamada *(ncamadas - 1));
		triangulos[nTriangulos].ponto2[2] = raioTempPrev * cos(alfa * fatia);
		triangulos[nTriangulos].text2[0] = (fatia + 1) / (float)nfatias;
		triangulos[nTriangulos].text2[1] = 1 - ((ncamadas - 1) / ((float)ncamadas));

		triangulos[nTriangulos].ponto1[0] = raioTempPrev * sin(alfa * (fatia + 1));
		triangulos[nTriangulos].ponto1[1] = (float)raio - (alturaCamada * (ncamadas - 1));
		triangulos[nTriangulos].ponto1[2] = raioTempPrev * cos(alfa * (fatia + 1));
		triangulos[nTriangulos].text1[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text1[1] = 1 - ((ncamadas - 1) / ((float)ncamadas));



		triangulos[nTriangulos].normal3[0] = sin(alfa * (fatia + 1));
		triangulos[nTriangulos].normal3[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].normal3[2] = cos(alfa * (fatia + 1));

		triangulos[nTriangulos].normal2[0] = sin(alfa * fatia);
		triangulos[nTriangulos].normal2[1] = (float)raio - (alturaCamada *(ncamadas - 1));
		triangulos[nTriangulos].normal2[2] = cos(alfa * fatia);

		triangulos[nTriangulos].normal1[0] = sin(alfa * (fatia + 1));
		triangulos[nTriangulos].normal1[1] = (float)raio - (alturaCamada * (ncamadas - 1));
		triangulos[nTriangulos].normal1[2] = cos(alfa * (fatia + 1));

		nTriangulos++;
	}

	////FUNDO 2 da esfera
	raioTempPrev = raioTemp;
	beta = asin(((float)raio - (alturaCamada * (ncamadas - 0.2))) / raio);
	raioTemp = (float)raio * cos(beta);

	for (int fatia = 1; fatia <= nfatias; fatia++){

		triangulos[nTriangulos].ponto1[0] = raioTemp * sin(alfa * (fatia + 1));
		triangulos[nTriangulos].ponto1[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].ponto1[2] = raioTemp * cos(alfa * (fatia + 1));
		triangulos[nTriangulos].text1[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text1[1] = 0.2 / (float)ncamadas;

		triangulos[nTriangulos].ponto2[0] = raioTemp * sin(alfa * fatia);
		triangulos[nTriangulos].ponto2[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].ponto2[2] = raioTemp * cos(alfa * fatia);
		triangulos[nTriangulos].text2[0] = (fatia) / (float)nfatias;
		triangulos[nTriangulos].text2[1] = 0.2 / (float)ncamadas;

		triangulos[nTriangulos].ponto3[0] = 0.0f;
		triangulos[nTriangulos].ponto3[1] = -(float)raio;
		triangulos[nTriangulos].ponto3[2] = 0.0f;
		triangulos[nTriangulos].text3[0] = (fatia + 1) / (float)nfatias;
		triangulos[nTriangulos].text3[1] = 0;


		triangulos[nTriangulos].normal1[0] = sin(alfa * (fatia + 1));
		triangulos[nTriangulos].normal1[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].normal1[2] = cos(alfa * (fatia + 1));

		triangulos[nTriangulos].normal2[0] = sin(alfa * fatia);
		triangulos[nTriangulos].normal2[1] = (float)raio - (alturaCamada * (ncamadas - 0.2));
		triangulos[nTriangulos].normal2[2] = cos(alfa * fatia);

		triangulos[nTriangulos].normal3[0] = 0.0f;
		triangulos[nTriangulos].normal3[1] = -(float)raio;
		triangulos[nTriangulos].normal3[2] = 0.0f;

		nTriangulos++;
	}

	grava3d(triangulos, nTriangulos, nomeFicheiro);
}


//CONE (raio da base, altura, fatias, camadas, nome ficheiro onde vai ser gravado)
void cone(float raioBase, float altura, int nFatias, int nCamadas, char nomeFicheiro[30])
{
	float alt0 = 0;
	float alt1, raio0 = raioBase, raio1;
	//TRIANGULO triangulos[NUM_MAX_TRIANGULOS];
	TRIANGULO *triangulos = (TRIANGULO *)malloc(sizeof(TRIANGULO) * 30000);
	int nTriangulos = 0;
	

	//Desenho da base do cone
	for (float i = 0; i <= nFatias; i++) {
		triangulos[nTriangulos].ponto1[0] = 0;
		triangulos[nTriangulos].ponto1[1] = 0;
		triangulos[nTriangulos].ponto1[2] = 0;
		triangulos[nTriangulos].ponto2[0] = raio0 * sin(((2 * M_PI) / nFatias)*i);
		triangulos[nTriangulos].ponto2[1] = alt0;
		triangulos[nTriangulos].ponto2[2] = raio0 * cos(((2 * M_PI) / nFatias)*i);
		triangulos[nTriangulos].ponto3[0] = raio0 * sin(((2 * M_PI) / nFatias)*(i + 1));
		triangulos[nTriangulos].ponto3[1] = alt0;
		triangulos[nTriangulos].ponto3[2] = raio0 * cos(((2 * M_PI) / nFatias)*(i + 1));
		nTriangulos++;
	}

	//desenho das camadas do cone
	for (int cam = 0; cam <= nCamadas; cam++)
	{
		alt1 = altura / nCamadas*cam;
		raio1 = raioBase - raioBase / nCamadas*(cam);
		//camadasl(raio0, raio1, alt0, alt1, nFatias);
		for (float i = 1; i <= nFatias; i++) {
			triangulos[nTriangulos].ponto1[0] = raio0 * sin(((2 * M_PI) / nFatias)*i);
			triangulos[nTriangulos].ponto1[1] = alt0;
			triangulos[nTriangulos].ponto1[2] = raio0* cos(((2 * M_PI) / nFatias)*i);
			triangulos[nTriangulos].ponto2[0] = raio1 * sin(((2 * M_PI) / nFatias)*i);
			triangulos[nTriangulos].ponto2[1] = alt1;
			triangulos[nTriangulos].ponto2[2] = raio1 * cos(((2 * M_PI) / nFatias)*i);
			triangulos[nTriangulos].ponto3[0] = raio1 * sin(((2 * M_PI) / nFatias)*(i+1));
			triangulos[nTriangulos].ponto3[1] = alt1;
			triangulos[nTriangulos].ponto3[2] = raio1 * cos(((2 * M_PI) / nFatias)*(i+1));
			nTriangulos++;
			
			triangulos[nTriangulos].ponto1[0] = raio0 * sin(((2 * M_PI) / nFatias)*i);
			triangulos[nTriangulos].ponto1[1] = alt0;
			triangulos[nTriangulos].ponto1[2] = raio0* cos(((2 * M_PI) / nFatias)*i);
			triangulos[nTriangulos].ponto2[0] = raio1 * sin(((2 * M_PI) / nFatias)*(i + 1));
			triangulos[nTriangulos].ponto2[1] = alt1;
			triangulos[nTriangulos].ponto2[2] = raio1 * cos(((2 * M_PI) / nFatias)*(i + 1));
			triangulos[nTriangulos].ponto3[0] = raio0 * sin(((2 * M_PI) / nFatias)*(i + 1));
			triangulos[nTriangulos].ponto3[1] = alt0;
			triangulos[nTriangulos].ponto3[2] = raio0 * cos(((2 * M_PI) / nFatias)*(i + 1));
			nTriangulos++;
		}

		raio0 = raio1; alt0 = alt1;
	}

	grava3d(triangulos, nTriangulos, nomeFicheiro);
}

void paralelepipedo(float altura, float comprimento, float profundidade, char nomeFicheiro[30]) {
	//TRIANGULO triangulos[NUM_MAX_TRIANGULOS];
	TRIANGULO *triangulos = (TRIANGULO *)malloc(sizeof(TRIANGULO) * 30000);
	float meiaAltura = (float)(altura / 2);
	float meioComprimento = (float)(comprimento / 2);
	float meiaProfundidade = (float)(profundidade / 2);
	int nTriangulos = 0;

	//FRENTE
	//Pontos
	triangulos[0].ponto1[0] = meioComprimento;
	triangulos[0].ponto1[1] = (meiaAltura * (-1));
	triangulos[0].ponto1[2] = meiaProfundidade;

	triangulos[0].ponto2[0] = (meioComprimento * (-1));
	triangulos[0].ponto2[1] = meiaAltura;
	triangulos[0].ponto2[2] = meiaProfundidade;

	triangulos[0].ponto3[0] = (meioComprimento * (-1));
	triangulos[0].ponto3[1] = (meiaAltura * (-1));
	triangulos[0].ponto3[2] = meiaProfundidade;

	//Normais
	triangulos[0].normal1[0] = 0.0f;
	triangulos[0].normal1[1] = 0.0f;
	triangulos[0].normal1[2] = 1.0f;

	triangulos[0].normal2[0] = 0.0f;
	triangulos[0].normal2[1] = 0.0f;
	triangulos[0].normal2[2] = 1.0f;

	triangulos[0].normal3[0] = 0.0f;
	triangulos[0].normal3[1] = 0.0f;
	triangulos[0].normal3[2] = 1.0f;

	//Texturas
	triangulos[0].text1[0] = 0.5;
	triangulos[0].text1[1] = 0.0;

	triangulos[0].text2[0] = 0.25;
	triangulos[0].text2[1] = 0.3;

	triangulos[0].text3[0] = 0.25;
	triangulos[0].text3[1] = 0.0;
	nTriangulos++;

	//Pontos
	triangulos[1].ponto1[0] = meioComprimento;
	triangulos[1].ponto1[1] = (meiaAltura * (-1));
	triangulos[1].ponto1[2] = meiaProfundidade;

	triangulos[1].ponto2[0] = meioComprimento;
	triangulos[1].ponto2[1] = meiaAltura;
	triangulos[1].ponto2[2] = meiaProfundidade;
	
	triangulos[1].ponto3[0] = (meioComprimento *(-1));
	triangulos[1].ponto3[1] = meiaAltura;
	triangulos[1].ponto3[2] = meiaProfundidade;

	//Normais
	triangulos[1].normal1[0] = 0.0f;
	triangulos[1].normal1[1] = 0.0f;
	triangulos[1].normal1[2] = 1.0f;

	triangulos[1].normal2[0] = 0.0f;
	triangulos[1].normal2[1] = 0.0f;
	triangulos[1].normal2[2] = 1.0f;

	triangulos[1].normal3[0] = 0.0f;
	triangulos[1].normal3[1] = 0.0f;
	triangulos[1].normal3[2] = 1.0f;

	//Texturas
	triangulos[1].text1[0] = 0.5;
	triangulos[1].text1[1] = 0.0;

	triangulos[1].text2[0] = 0.5;
	triangulos[1].text2[1] = 0.3;

	triangulos[1].text3[0] = 0.25;
	triangulos[1].text3[1] = 0.3;
	nTriangulos++;

	//CIMA
	//Pontos
	triangulos[2].ponto1[0] = meioComprimento;
	triangulos[2].ponto1[1] = meiaAltura;
	triangulos[2].ponto1[2] = meiaProfundidade;
	
	triangulos[2].ponto2[0] = meioComprimento;
	triangulos[2].ponto2[1] = meiaAltura;
	triangulos[2].ponto2[2] = (meiaProfundidade * (-1));
	
	triangulos[2].ponto3[0] = (meioComprimento * (-1));
	triangulos[2].ponto3[1] = meiaAltura;
	triangulos[2].ponto3[2] = meiaProfundidade;

	//Normais
	triangulos[2].normal1[0] = 0.0f;
	triangulos[2].normal1[1] = 1.0f;
	triangulos[2].normal1[2] = 0.0f;

	triangulos[2].normal2[0] = 0.0f;
	triangulos[2].normal2[1] = 1.0f;
	triangulos[2].normal2[2] = 0.0f;

	triangulos[2].normal3[0] = 0.0f;
	triangulos[2].normal3[1] = 1.0f;
	triangulos[2].normal3[2] = 0.0f;

	//Texturas
	triangulos[2].text1[0] = 1.0;
	triangulos[2].text1[1] = 0.3;

	triangulos[2].text2[0] = 1.0;
	triangulos[2].text2[1] = 0.6;

	triangulos[2].text3[0] = 0.75;
	triangulos[2].text3[1] = 0.3;
	nTriangulos++;

	//Pontos
	triangulos[3].ponto1[0] = meioComprimento;
	triangulos[3].ponto1[1] = meiaAltura;
	triangulos[3].ponto1[2] = (meiaProfundidade * (-1));
	
	triangulos[3].ponto2[0] = (meioComprimento * (-1));
	triangulos[3].ponto2[1] = meiaAltura;
	triangulos[3].ponto2[2] = (meiaProfundidade * (-1));
	
	triangulos[3].ponto3[0] = (meioComprimento * (-1));
	triangulos[3].ponto3[1] = meiaAltura;
	triangulos[3].ponto3[2] = meiaProfundidade;

	//Normais
	triangulos[3].normal1[0] = 0.0f;
	triangulos[3].normal1[1] = 1.0f;
	triangulos[3].normal1[2] = 0.0f;

	triangulos[3].normal2[0] = 0.0f;
	triangulos[3].normal2[1] = 1.0f;
	triangulos[3].normal2[2] = 0.0f;

	triangulos[3].normal3[0] = 0.0f;
	triangulos[3].normal3[1] = 1.0f;
	triangulos[3].normal3[2] = 0.0f;

	//Texturas
	triangulos[3].text1[0] = 1.0;
	triangulos[3].text1[1] = 0.6;

	triangulos[3].text2[0] = 0.75;
	triangulos[3].text2[1] = 0.6;

	triangulos[3].text3[0] = 0.75;
	triangulos[3].text3[1] = 0.3;
	nTriangulos++;

	//TRAS (Pontos gerados no sentido dos ponteiros do relogio)
	//Pontos
	triangulos[4].ponto1[0] = meioComprimento;
	triangulos[4].ponto1[1] = (meiaAltura * (-1));
	triangulos[4].ponto1[2] = (meiaProfundidade * (-1));
	
	triangulos[4].ponto2[0] = (meioComprimento * (-1));
	triangulos[4].ponto2[1] = (meiaAltura * (-1));
	triangulos[4].ponto2[2] = (meiaProfundidade * (-1));
	
	triangulos[4].ponto3[0] = (meioComprimento * (-1));
	triangulos[4].ponto3[1] = meiaAltura;
	triangulos[4].ponto3[2] = (meiaProfundidade * (-1));

	//Normais
	triangulos[4].normal1[0] = 0.0f;
	triangulos[4].normal1[1] = 0.0f;
	triangulos[4].normal1[2] = -1.0f;

	triangulos[4].normal2[0] = 0.0f;
	triangulos[4].normal2[1] = 0.0f;
	triangulos[4].normal2[2] = -1.0f;

	triangulos[4].normal3[0] = 0.0f;
	triangulos[4].normal3[1] = 0.0f;
	triangulos[4].normal3[2] = -1.0f;

	//Texturas
	triangulos[4].text1[0] = 0.5;
	triangulos[4].text1[1] = 0.6;

	triangulos[4].text2[0] = 0.25;
	triangulos[4].text2[1] = 0.6;

	triangulos[4].text3[0] = 0.25;
	triangulos[4].text3[1] = 1.0;
	nTriangulos++;

	//Pontos
	triangulos[5].ponto1[0] = meioComprimento;
	triangulos[5].ponto1[1] = (meiaAltura * (-1));
	triangulos[5].ponto1[2] = (meiaProfundidade * (-1));
	
	triangulos[5].ponto2[0] = (meioComprimento * (-1));
	triangulos[5].ponto2[1] = meiaAltura;
	triangulos[5].ponto2[2] = (meiaProfundidade * (-1));
	
	triangulos[5].ponto3[0] = meioComprimento;
	triangulos[5].ponto3[1] = meiaAltura;
	triangulos[5].ponto3[2] = (meiaProfundidade * (-1));

	//Normais
	triangulos[5].normal1[0] = 0.0f;
	triangulos[5].normal1[1] = 0.0f;
	triangulos[5].normal1[2] = -1.0f;

	triangulos[5].normal2[0] = 0.0f;
	triangulos[5].normal2[1] = 0.0f;
	triangulos[5].normal2[2] = -1.0f;

	triangulos[5].normal3[0] = 0.0f;
	triangulos[5].normal3[1] = 0.0f;
	triangulos[5].normal3[2] = -1.0f;

	//Texturas
	triangulos[5].text1[0] = 0.5;
	triangulos[5].text1[1] = 0.6;

	triangulos[5].text2[0] = 0.25;
	triangulos[5].text2[1] = 1.0;

	triangulos[5].text3[0] = 0.5;
	triangulos[5].text3[1] = 1.0;
	nTriangulos++;

	//BAIXO (Pontos gerados no sentido dos ponteiros do relogio)
	//Pontos
	triangulos[6].ponto1[0] = meioComprimento;
	triangulos[6].ponto1[1] = (meiaAltura * (-1));
	triangulos[6].ponto1[2] = meiaProfundidade;
	
	triangulos[6].ponto2[0] = (meioComprimento * (-1));
	triangulos[6].ponto2[1] = (meiaAltura * (-1));
	triangulos[6].ponto2[2] = meiaProfundidade;
	
	triangulos[6].ponto3[0] = meioComprimento;
	triangulos[6].ponto3[1] = (meiaAltura * (-1));
	triangulos[6].ponto3[2] = (meiaProfundidade * (-1));

	//Normais
	triangulos[6].normal1[0] = 0.0f;
	triangulos[6].normal1[1] = -1.0f;
	triangulos[6].normal1[2] = 0.0f;

	triangulos[6].normal2[0] = 0.0f;
	triangulos[6].normal2[1] = -1.0f;
	triangulos[6].normal2[2] = 0.0f;

	triangulos[6].normal3[0] = 0.0f;
	triangulos[6].normal3[1] = -1.0f;
	triangulos[6].normal3[2] = 0.0f;

	//Texturas
	triangulos[6].text1[0] = 0.5;
	triangulos[6].text1[1] = 0.3;

	triangulos[6].text2[0] = 0.25;
	triangulos[6].text2[1] = 0.3;

	triangulos[6].text3[0] = 0.5;
	triangulos[6].text3[1] = 0.6;
	nTriangulos++;

	//Pontos
	triangulos[7].ponto1[0] = meioComprimento;
	triangulos[7].ponto1[1] = (meiaAltura * (-1));
	triangulos[7].ponto1[2] = (meiaProfundidade * (-1));
	
	triangulos[7].ponto2[0] = (meioComprimento * (-1));
	triangulos[7].ponto2[1] = (meiaAltura * (-1));
	triangulos[7].ponto2[2] = meiaProfundidade;
	
	triangulos[7].ponto3[0] = (meioComprimento * (-1));
	triangulos[7].ponto3[1] = (meiaAltura * (-1));
	triangulos[7].ponto3[2] = (meiaProfundidade * (-1));

	//Normais
	triangulos[7].normal1[0] = 0.0f;
	triangulos[7].normal1[1] = -1.0f;
	triangulos[7].normal1[2] = 0.0f;

	triangulos[7].normal2[0] = 0.0f;
	triangulos[7].normal2[1] = -1.0f;
	triangulos[7].normal2[2] = 0.0f;

	triangulos[7].normal3[0] = 0.0f;
	triangulos[7].normal3[1] = -1.0f;
	triangulos[7].normal3[2] = 0.0f;

	//Texturas
	triangulos[7].text1[0] = 0.5;
	triangulos[7].text1[1] = 0.6;
	
	triangulos[7].text2[0] = 0.25;
	triangulos[7].text2[1] = 0.3;

	triangulos[7].text3[0] = 0.25;
	triangulos[7].text3[1] = 0.6;
	nTriangulos++;

	//ESQUERDA (Pontos gerados no sentido contrario aos ponteiros do relogio)
	//Pontos
	triangulos[8].ponto1[0] = (meioComprimento * (-1));
	triangulos[8].ponto1[1] = (meiaAltura * (-1));
	triangulos[8].ponto1[2] = meiaProfundidade;
	
	triangulos[8].ponto2[0] = (meioComprimento * (-1));
	triangulos[8].ponto2[1] = meiaAltura;
	triangulos[8].ponto2[2] = meiaProfundidade;
	
	triangulos[8].ponto3[0] = (meioComprimento * (-1));
	triangulos[8].ponto3[1] = meiaAltura;
	triangulos[8].ponto3[2] = (meiaProfundidade * (-1));

	//Normais
	triangulos[8].normal1[0] = -1.0f;
	triangulos[8].normal1[1] = 0.0f;
	triangulos[8].normal1[2] = 0.0f;

	triangulos[8].normal2[0] = -1.0f;
	triangulos[8].normal2[1] = 0.0f;
	triangulos[8].normal2[2] = 0.0f;

	triangulos[8].normal3[0] = -1.0f;
	triangulos[8].normal3[1] = 0.0f;
	triangulos[8].normal3[2] = 0.0f;

	//Texturas
	triangulos[8].text1[0] = 0.0;
	triangulos[8].text1[1] = 0.3;

	triangulos[8].text2[0] = 0.0;
	triangulos[8].text2[1] = 0.6;

	triangulos[8].text3[0] = 0.25;
	triangulos[8].text3[1] = 0.6;
	nTriangulos++;

	//Pontos
	triangulos[9].ponto1[0] = (meioComprimento * (-1));
	triangulos[9].ponto1[1] = (meiaAltura * (-1));
	triangulos[9].ponto1[2] = meiaProfundidade;
	
	triangulos[9].ponto2[0] = (meioComprimento * (-1));
	triangulos[9].ponto2[1] = meiaAltura;
	triangulos[9].ponto2[2] = (meiaProfundidade * (-1));
	
	triangulos[9].ponto3[0] = (meioComprimento * (-1));
	triangulos[9].ponto3[1] = (meiaAltura * (-1));
	triangulos[9].ponto3[2] = (meiaProfundidade * (-1));

	//Normais
	triangulos[9].normal1[0] = -1.0f;
	triangulos[9].normal1[1] = 0.0f;
	triangulos[9].normal1[2] = 0.0f;

	triangulos[9].normal2[0] = -1.0f;
	triangulos[9].normal2[1] = 0.0f;
	triangulos[9].normal2[2] = 0.0f;

	triangulos[9].normal3[0] = -1.0f;
	triangulos[9].normal3[1] = 0.0f;
	triangulos[9].normal3[2] = 0.0f;

	//Texturas
	triangulos[9].text1[0] = 0.0;
	triangulos[9].text1[1] = 0.3;

	triangulos[9].text2[0] = 0.25;
	triangulos[9].text2[1] = 0.6;

	triangulos[9].text3[0] = 0.25;
	triangulos[9].text3[1] = 0.3;
	nTriangulos++;

	//DIREITA (Pontos gerados no sentido dos ponteiros do relogio)
	//Pontos
	triangulos[10].ponto1[0] = meioComprimento;
	triangulos[10].ponto1[1] = (meiaAltura * (-1));
	triangulos[10].ponto1[2] = (meiaProfundidade * (-1));
	
	triangulos[10].ponto2[0] = meioComprimento;
	triangulos[10].ponto2[1] = meiaAltura;
	triangulos[10].ponto2[2] = (meiaProfundidade * (-1));
	
	triangulos[10].ponto3[0] = meioComprimento;
	triangulos[10].ponto3[1] = (meiaAltura * (-1));
	triangulos[10].ponto3[2] = meiaProfundidade;

	//Normais
	triangulos[10].normal1[0] = 1.0f;
	triangulos[10].normal1[1] = 0.0f;
	triangulos[10].normal1[2] = 0.0f;

	triangulos[10].normal2[0] = 1.0f;
	triangulos[10].normal2[1] = 0.0f;
	triangulos[10].normal2[2] = 0.0f;

	triangulos[10].normal3[0] = 1.0f;
	triangulos[10].normal3[1] = 0.0f;
	triangulos[10].normal3[2] = 0.0f;

	//Texturas
	triangulos[10].text1[0] = 0.5;
	triangulos[10].text1[1] = 0.3;

	triangulos[10].text2[0] = 0.5;
	triangulos[10].text2[1] = 0.6;

	triangulos[10].text3[0] = 0.75;
	triangulos[10].text3[1] = 0.3;
	nTriangulos++;

	//Pontos
	triangulos[11].ponto1[0] = meioComprimento;
	triangulos[11].ponto1[1] = (meiaAltura * (-1));
	triangulos[11].ponto1[2] = meiaProfundidade;
	
	triangulos[11].ponto2[0] = meioComprimento;
	triangulos[11].ponto2[1] = meiaAltura;
	triangulos[11].ponto2[2] = (meiaProfundidade * (-1));
	
	triangulos[11].ponto3[0] = meioComprimento;
	triangulos[11].ponto3[1] = meiaAltura;
	triangulos[11].ponto3[2] = meiaProfundidade;

	//Normais
	triangulos[11].normal1[0] = 1.0f;
	triangulos[11].normal1[1] = 0.0f;
	triangulos[11].normal1[2] = 0.0f;

	triangulos[11].normal2[0] = 1.0f;
	triangulos[11].normal2[1] = 0.0f;
	triangulos[11].normal2[2] = 0.0f;

	triangulos[11].normal3[0] = 1.0f;
	triangulos[11].normal3[1] = 0.0f;
	triangulos[11].normal3[2] = 0.0f;

	//Texturas
	triangulos[11].text1[0] = 0.75;
	triangulos[11].text1[1] = 0.3;

	triangulos[11].text2[0] = 0.5;
	triangulos[11].text2[1] = 0.6;

	triangulos[11].text3[0] = 0.75;
	triangulos[11].text3[1] = 0.6;
	nTriangulos++;

	grava3d(triangulos, nTriangulos, nomeFicheiro);
}


void plano(float largura, float comprimento, char nomeFicheiro[30]) {
	//TRIANGULO triangulos[NUM_MAX_TRIANGULOS];
	TRIANGULO *triangulos = (TRIANGULO *)malloc(sizeof(TRIANGULO) * 30000);
	float meiaLargura = (float)(largura / 2);
	float meioComprimento = (float)(comprimento / 2);
	int nTriangulos = 0;

	//Pontos gerados no sentido contrario ao dos ponteiros do relogio
	//Pontos:
	triangulos[0].ponto1[0] = meioComprimento;
	triangulos[0].ponto1[1] = 0.0;
	triangulos[0].ponto1[2] = meiaLargura;

	triangulos[0].ponto2[0] = meioComprimento;
	triangulos[0].ponto2[1] = 0.0;
	triangulos[0].ponto2[2] = (meiaLargura * (-1));

	triangulos[0].ponto3[0] = (meioComprimento * (-1));
	triangulos[0].ponto3[1] = 0.0;
	triangulos[0].ponto3[2] = meiaLargura;

	//Normais:
	triangulos[0].normal1[0] = 0.0f;
	triangulos[0].normal1[1] = 1.0f;
	triangulos[0].normal1[2] = 0.0f;

	triangulos[0].normal2[0] = 0.0f;
	triangulos[0].normal2[1] = 1.0f;
	triangulos[0].normal2[2] = 0.0f;

	triangulos[0].normal3[0] = 0.0f;
	triangulos[0].normal3[1] = 1.0f;
	triangulos[0].normal3[2] = 0.0f;

	//Texturas:
	triangulos[0].text1[0] = 1.0;
	triangulos[0].text1[1] = 0.0;

	triangulos[0].text2[0] = 1.0;
	triangulos[0].text2[1] = 1.0;

	triangulos[0].text3[0] = 0.0;
	triangulos[0].text3[1] = 0.0;
	nTriangulos++;

	//Pontos:
	triangulos[1].ponto1[0] = meioComprimento;
	triangulos[1].ponto1[1] = 0.0;
	triangulos[1].ponto1[2] = (meiaLargura * (-1));

	triangulos[1].ponto2[0] = (meioComprimento * (-1));
	triangulos[1].ponto2[1] = 0.0;
	triangulos[1].ponto2[2] = (meiaLargura * (-1));

	triangulos[1].ponto3[0] = (meioComprimento * (-1));
	triangulos[1].ponto3[1] = 0.0;
	triangulos[1].ponto3[2] = meiaLargura;

	//Normais:
	triangulos[1].normal1[0] = 0.0f;
	triangulos[1].normal1[1] = 1.0f;
	triangulos[1].normal1[2] = 0.0f;

	triangulos[1].normal2[0] = 0.0f;
	triangulos[1].normal2[1] = 1.0f;
	triangulos[1].normal2[2] = 0.0f;

	triangulos[1].normal3[0] = 0.0f;
	triangulos[1].normal3[1] = 1.0f;
	triangulos[1].normal3[2] = 0.0f;

	//Texturas:
	triangulos[1].text1[0] = 1.0;
	triangulos[1].text1[1] = 1.0;

	triangulos[1].text2[0] = 0.0;
	triangulos[1].text2[1] = 1.0;

	triangulos[1].text3[0] = 0.0;
	triangulos[1].text3[1] = 0.0;
	nTriangulos++;

	grava3d(triangulos, nTriangulos, nomeFicheiro);
}


int main(int argc, char **argv) {
	//ESFERAS
	esfera(1.0, 30, 30, "f4_esfera1.3d");
	esfera(1.0, 10, 10, "f4_esfera2.3d");
	esfera(1.5, 50, 40, "f4_esfera3.3d");

	//CONES
	//cone(1.0, 2.0, 10, 5, "cone1.3d");
	//cone(1.0, 2.0, 40, 40, "cone2.3d");
	//cone(1.5, 1.5, 50, 60, "cone3.3d");

	//PARALELEPIPEDOS
	paralelepipedo(1.0, 2.0, 1.0, "f4_para1.3d");
	paralelepipedo(2.0, 4.0, 2.0, "f4_para2.3d");
	paralelepipedo(1.5, 3.5, 3.5, "f4_para3.3d");

	//PLANOS
	plano(1.0, 2.0, "f4_plano1.3d");
	plano(2.0, 4.0, "f4_plano2.3d");
	plano(1.5, 3.5, "f4_plano3.3d");
}