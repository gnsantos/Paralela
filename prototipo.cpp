#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

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
  bool cold;
  bool hot;
  vector<Joaninha*> list;
  double temperatura;
};

typedef struct celhexa Hexa;

Hexa **init_grid(int altura, int largura){
  Hexa **matriz = (Hexa**) malloc(altura*sizeof(Hexa*));
  double A = sqrt(3)/2;
  for(int i = 0; i < altura; i++)
    matriz[i] = (Hexa*) malloc(largura*sizeof(Hexa));
  
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++){
      matriz[i][j].joaninha = matriz[i][j].cold = matriz[i][j].hot = false;
      matriz[i][j].temperatura = 0;
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

int main(int arg, char** argv){
  Hexa **matriz = init_grid(5,5);
  for(int i = 0; i < 5; i++)
    for(int j = 0; j < 5; j++)
      cout << "Pos " << i << j << " Euclidiana: " << matriz[i][j].euclidian[X] << " , " << matriz[i][j].euclidian[Y] << endl;
  return 0;
}
