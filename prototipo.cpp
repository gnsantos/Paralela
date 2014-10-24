#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>

#define X 0
#define Y 1

#define I 0
#define J 1

using namespace std;

struct joaninha{
	int pos[2];
};

typedef struct joaninha Joaninha;

struct celhexa{
	double euclidian[2];
	bool joaninha;
	bool frio;
	bool calor;
	vector<Joaninha*> list;
	double temperatura;
	int turnos_fonte_ativa;
};

typedef struct celhexa Hexa;

Hexa **init_grid(int altura, int largura){
	Hexa **matriz = (Hexa**) malloc(altura*sizeof(Hexa*));
	double A = sqrt(3)/2;
	for(int i = 0; i < altura; i++)
		matriz[i] = (Hexa*) malloc(largura*sizeof(Hexa));
  
	for(int i = 0; i < altura; i++){
		for(int j = 0; j < largura; j++){
			matriz[i][j].joaninha = matriz[i][j].frio = matriz[i][j].calor = false;
			matriz[i][j].temperatura = 0;
			matriz[i][j].turnos_fonte_ativa = 0;
			if(i%2){
				matriz[i][j].euclidian[X] = 2*A*j - A;
				matriz[i][j].euclidian[Y] = 1.5*i;
			}
			else{
				matriz[i][j].euclidian[X] = 2*A*j;
				matriz[i][j].euclidian[Y] = 1.5*i;
			}
		}
	}
	return matriz;
}

void print_matrix(Hexa** matrix, int altura, int largura){
	for(int i = 0; i < altura; i++ ){
		for(int j = 0; j < largura; j++){
			if(matrix[i][j].joaninha)
				printf("* ");
			else if(matrix[i][j].calor)
				printf("+ ");
			else if(matrix[i][j].frio)
				printf("- ");
			else
				printf(". ");
		}
		printf("\n");
	}
}

void coloca_joaninha(Hexa** matrix, int altura, int largura, int num_j){
	srand(time(NULL));
	for(int i  =  0; i < num_j; ){
		int pos_i = rand()%altura;
		int pos_j = rand()%largura;		
		if(!matrix[pos_i][pos_j].joaninha){
			matrix[pos_i][pos_j].joaninha = true;
			i++;
		}
	}
}

void coloca_fonte(Hexa** matrix, int altura, int largura, double p, int t,double calor){
	srand(time(NULL));
	for(int i = 0; i < altura; i++){
		for(int j = 0; j < largura; j++){
			double prob = (1.0*rand())/RAND_MAX;
			if(p > prob  && !(matrix[i][j].joaninha || matrix[i][j].calor || matrix[i][j].frio)){
				if(calor < 0)
					matrix[i][j].frio = true;
				else
					matrix[i][j].calor = true;
				matrix[i][j].turnos_fonte_ativa = t;
			}
		}
	}
}

double dist_euclidiana(Hexa** m, int i, int j, int k, int l){
	int x1 = m[i][j].euclidian[X];
	int y1 = m[i][j].euclidian[Y];
  	int x2 = m[k][l].euclidian[X];
	int y2 = m[k][l].euclidian[Y];

	return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}

double temp_hex(Hexa** m, int altura, int largura, int i, int j, double cte){
	Hexa h = m[i][j];
	double temp_h = 0;
	for(int k = 0; k < altura; k++){
		for(int l = 0; l < largura; l++){
			if(k != i && l != j ){
				double d = dist_euclidiana(m, i, j, k, l);
				if(m[k][l].joaninha || m[k][l].calor){
					temp_h += cte/d;
				}
				else if(m[k][k].frio)
					temp_h -= cte/d;
			}
		}
	}
	m[i][j].temperatura = temp_h;
	return temp_h;
}

void print_temps(Hexa** m, int altura, int largura){
	for(int i = 0; i < altura; i++){
		for(int j = 0; j < largura; j++)
			printf("%.2lf ", m[i][j].temperatura);
		printf("\n");
	}
}

int main(int arg, char** argv){
	Hexa **matriz = init_grid(5,5);
	coloca_joaninha(matriz, 5, 5, 10);
	coloca_fonte(matriz, 5, 5, 0.3, 2, 10);
	coloca_fonte(matriz, 5, 5, 0.5, 2, -10);
	print_matrix(matriz, 5,5);
	for(int i; i < 5; i ++)
		for(int j; j < 5; j++)
			temp_hex(matriz, 5, 5, i, j, 10);
	print_temps(matriz, 5, 5);
	return 0;
}
