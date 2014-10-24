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

#define N 5
#define M 6

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
	double x1 = m[i][j].euclidian[X];
	double y1 = m[i][j].euclidian[Y];
  	double x2 = m[k][l].euclidian[X];
	double y2 = m[k][l].euclidian[Y];

	return abs((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

double temp_hex(Hexa** m, int altura, int largura, int i, int j, double cte){
	double temp_h = 0;
	for(int k = 0; k < altura; k++){
		for(int l = 0; l < largura; l++){
			if(!(k == i && l == j)){
				double d = dist_euclidiana(m, i, j, k, l);
				if(m[k][l].joaninha || m[k][l].calor){
					temp_h += cte/d;
				}
				else if(m[k][l].frio)
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
			printf("%+.2le ", m[i][j].temperatura);
		printf("\n");
	}
}

int main(int arg, char** argv){
	Hexa **matriz = init_grid(N, M);
	coloca_joaninha(matriz, N, M, 10);
	coloca_fonte(matriz, N, M, 0.3, 2, 10);
	coloca_fonte(matriz, N, M, 0.5, 2, -10);
	print_matrix(matriz, N, M);
	for(int i = 0; i < N; i++)
		for(int j = 0; j < M; j++)
			temp_hex(matriz, N, M, i, j, 10);
	print_temps(matriz, N, M);
	return 0;
}
