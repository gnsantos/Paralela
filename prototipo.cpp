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

#define TEMP_FONTE 10
#define PROB_FRIO 0.3
#define PROB_CALOR 0.5

#define THETA_MAX 7
#define THETA_MIN 3

#define N 3
#define M 3

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

vector<Joaninha*> joanas;

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
      joanas[i] = (Joaninha*) malloc(sizeof(Joaninha));
      joanas[i]->pos_i = pos_i;
      joanas[i]->pos_j = pos_j;
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

void atualiza_temps(Hexa** m, int alt, int lar, double cte){
  for(int i = 0; i < alt; i++)
    for(int j = 0; j < lar; j++)
      temp_hex(m, alt, lar, i, j, cte);
}

void print_temps(Hexa** m, int altura, int largura){
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++)
      printf("%+.2le ", m[i][j].temperatura);
    printf("\n");
  }
}


void move_joaninha(Hexa** m, int alt, int lar, int index){
  Joaninha jojo = joanas[index];
  int i = jojo->pos_i, j = jojo->pos_j; 
  Hexa cel_j = m[i][j], v[6];
  Hexa dest = cel_j;
  double temp_atual = cel_j.temperatura;
  
  double diff = -1;

  if(pos_i%2){ /*linha impar*/
    if(i > 1 && j > 1) v[1] = m[i-1][j-1];
    if(i > 1) v[2] = m[i-1][j];
    if(j > 1) v[3] = m[i][j-1];
    if(j + 1 < lar ) v[4] = m[i][j+1];
    if(i + 1 < alt && j > 1) v[5] = m[i+1][j-1];
    if(i + 1 < alt) v[0] = m[i+1][j];
  }

  else{ /*linha par*/
    if(i > 1 && j + 1 < lar) v[1] = m[i-1][j+1];
    if(i > 1) v[2] = m[i-1][j];
    if(j > 1) v[3] = m[i][j-1];
    if(j + 1 < lar ) v[4] = m[i][j+1];
    if(i + 1 < alt && j + 1 < lar) v[5] = m[i+1][j+1];
    if(i + 1 < alt) v[0] = m[i+1][j];
  }

  for(int k = 0; k < 6; k++){
    if(v[k] != NULL && v[k].temperatura >= THETA_MIN && v[k].temperatura <= THETA_MAX){
      if( fabs(temp_atual - v[k].temperatura) > diff ){
	diff = fabs(temp_atual - v[k].temperatura);
	dest = v[k];
      }
    }
  }
  
  if(dest != cel_j)
    dest.list.push_back(jojo); /*coloca a joaninha na lista daqueles que querem se mover para o hexagono dest*/

}

void resolve_movimentos(Hexa** m, int alt, int lar){
  for(int i = 0; i < joanas.size(); i++)
    move_joaninha(m, alt, lar, i);
  
  double diff = -1;
  Joaninha escolhida = NULL;

  for(int i = 0; i < alt; i++){ //itera em todos os hexagonos
    for(int j = 0; j < lar; j++){

      int x,y;
      for(int k = 0; k < m[i][j].list.size(); k++){ //itera na lista de quem quer ir para o hexagono
	x = m[i][j].list[k]->pos_i;
	y = m[i][j].list[k]->pos_j;
	if(fabs(m[i][j].temperatura - m[x][y].temperatura) > diff){
	  diff = fabs(m[i][j].temperatura - m[x][y].temperatura);
	  escolhida = m[i][j].list[k];
	}
      }

      if(escolhida != NULL){
	x = escolhida->pos_i;
	y = escolhida->pos_j;
      
	if(!m[i][j].joaninha){
	  m[i][j].joaninha = true;
	  m[x][y].joaninha = false;
	}
      }

      diff = -1;
    }
  }

}

int main(int arg, char** argv){
  Hexa **matriz = init_grid(N, M);
  
  int j = atoi(argv[1]);

  joanas.resize(j);
  joanas.clear(); 

  coloca_joaninha(matriz, N, M, j);

  coloca_fonte(matriz, N, M, PROB_CALOR, 2, TEMP_FONTE);
  coloca_fonte(matriz, N, M, PROB_FRIO, 2, -TEMP_FONTE);
  
  
  /*matriz[0][0].joaninha = false; matriz[0][0].calor = false; matriz[0][0].frio = true;
  matriz[0][1].joaninha = false; matriz[0][1].calor = true; matriz[0][1].frio = false;
  matriz[0][2].joaninha = false; matriz[0][2].calor = false; matriz[0][2].frio = false;
  matriz[1][0].joaninha = false; matriz[1][0].calor = true; matriz[1][0].frio = false;
  matriz[1][1].joaninha = true; matriz[1][1].calor = false; matriz[1][1].frio = false;
  matriz[1][2].joaninha = false; matriz[1][2].calor = false; matriz[1][2].frio = true;
  matriz[2][0].joaninha = false; matriz[2][0].calor = false; matriz[2][0].frio = true;
  matriz[2][1].joaninha = false; matriz[2][1].calor = true; matriz[2][1].frio = false;
  matriz[2][2].joaninha = false; matriz[2][2].calor = false; matriz[2][2].frio = false;
  */

  print_matrix(matriz, N, M);

  atualiza_temps(matriz, N, M, TEMP_FONTE);
 
  print_temps(matriz, N, M);
  
  /* for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      cout << "Pos " << i << j << " Euclidiana: " << matriz[i][j].euclidian[X] << " , " << matriz[i][j].euclidian[Y] << endl;
  */
  
  return 0;
}
