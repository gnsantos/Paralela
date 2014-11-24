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

const char EMPTY = ' ';
const char JOANINHA = 'J';
const char CALOR = '+';
const char FRIO = '-';
const char LEFT_DEL = '(';
const char RIGHT_DEL = ')';

using namespace std;

struct joaninha{
	int pos_i;
	int pos_j;
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

void init_Screen(int mapHeight, int mapWidth, Hexa** matriz){
	// Cuidado, coordenadas x y são invertidas para printar
        int m = mapWidth*4+2;
        int n = mapHeight*7 + 3;
        
        //reen = new char[m][n];
		char** screen = new char*[m];
		for (int i = 0; i < m; i++){
			screen[i] = new char[n];
		}
		
        int i = 0;
        int j = 0;
        int x = 0;
        int y = 0;
        char a;
        
        // Inicializa a matriz
		// Trocar por std::fill quando possível
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                screen[i][j] = EMPTY;
            }
        }
        for (i = 0; i < m; i++) {
            if (i%4 == 0) {
                j = 0;
                while (j < n-3) {
                    screen[i][j] = '\\';
                    j += 8;
                    screen[i][j] = '/';
                    j += 6;
                }
                screen[i][j] = '\\';
            }
            if (i%4 == 1) {
                j = 1;
                while (j < n-2) {
                    screen[i][j] = '\\';
					for (int k = 1; k < 6; k++){
						screen[i][j+k] = '_';
					}
                    j += 6;
                    screen[i][j] = '/';
                    j += 8;
                }
                screen[i][j] = '\\';
            }
            if (i%4 == 2) {
                j = 1;
                while (j < n-2) {
                    screen[i][j] = '/';
                    j += 6;
                    screen[i][j] = '\\';
                    j += 8;
                }
                screen[i][j] = '/';
            }
            if (i%4 == 3) {
                j = 0;
                while (j < n-3) {
                    screen[i][j] = '/';
                    j += 8;
                    screen[i][j] = '\\';
					for (int k = 1; k < 6; k++){
						screen[i][j+k] = '_';
					}
                    j += 6;
                }
                screen[i][j] = '/';
            }
        }


		// Printa joaninhas/fontes
		
		for (j = 0; j < mapHeight; j++){
			for (i = 0; i < mapWidth; i++){
				y = j*7;
				x = i*4;
				if (j%2 == 0) x += 2;
				if (matriz[j][i].joaninha){
					screen[x + 1][y + 3] = LEFT_DEL;
					screen[x + 1][y + 4] = JOANINHA;
					screen[x + 1][y + 5] = RIGHT_DEL;
				}
				else if (matriz[j][i].frio){
					screen[x + 1][y + 3] = LEFT_DEL;
					screen[x + 1][y + 4] = FRIO;
					screen[x + 1][y + 5] = RIGHT_DEL;
				}
				else if (matriz[j][i].calor){
					screen[x + 1][y + 3] = LEFT_DEL;
					screen[x + 1][y + 4] = CALOR;
					screen[x + 1][y + 5] = RIGHT_DEL;
				}
				// else{
				// 	screen[x + 1][y + 3] = LEFT_DEL;
				// 	screen[x + 1][y + 4] = EMPTY;
				// 	screen[x + 1][y + 5] = RIGHT_DEL;
				// }
				
			}
		}
		// Remove pontas
        screen[0][0] = EMPTY;
        screen[1][1] = EMPTY;
        screen[m-1][n-2] = EMPTY;
        screen[m-2][n-3] = EMPTY;
		
		// Printa resultado
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
				std::cout << screen[i][j];
            }
			std::cout << std::endl;
        }
		std::cout <<std::endl;
}

int main(){
	Hexa **matriz = init_grid(N, M);
	coloca_joaninha(matriz, N, M, 10);
	coloca_fonte(matriz, N, M, 0.3, 2, 10);
	coloca_fonte(matriz, N, M, 0.5, 2, -10);
	print_matrix(matriz, N, M);	
	init_Screen(N,M, matriz);
	return 0;
}
