#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <cstdio>

#define X 0
#define Y 1

#define I 0
#define J 1

#define TEMP_FONTE 15
#define PROB_FRIO 0.1
#define PROB_CALOR 0.1
#define DURACAO_CALOR 3
#define DURACAO_FRIO 3

#define THETA_MAX 50
#define THETA_MIN -50

#define N 7
#define M 7


const char EMPTY = ' ';
const char JOANINHA = 'J';
const char CALOR = '+';
const char FRIO = '-';
const char LEFT_DEL = '(';
const char RIGHT_DEL = ')';
const char CALOR_E_FRIO = '$';

using namespace std;

typedef struct celhexa Hexa;

struct joaninha{
  int pos_i;
  int pos_j;
  bool move; //destermina se a joninha se move ou nao
  Hexa* dest; //caso a joaninha se mova, determina seu destino
};

typedef struct joaninha Joaninha;

struct fonte{
  int pos_i;
  int pos_j;
  double temperatura;
  int ciclos_ativa;
};

typedef struct fonte* FireAndIce;

struct celhexa{
  double euclidian[2];
  bool joaninha;
  bool frio;
  bool calor;
  double temperatura;
  Joaninha* bug; /*joaninha que esta na celula. NULL caso nao nada.*/
  double old_diff; /*diferenca absoluta entre a temp do hexagono e a temp do hexagono
		     em que a joaninha estava anteriormente.*/
  int i,j; /*localizacao na matriz*/
  int seed;
};


vector<Joaninha*> joanas;
vector<FireAndIce> gelo_e_fogo;
int seed_gb;

Hexa **init_grid(int altura, int largura){
  Hexa **matriz = (Hexa**) malloc(altura*sizeof(Hexa*));
  for(int i = 0; i < altura; i++)
    matriz[i] = (Hexa*) malloc(largura*sizeof(Hexa));

  // LADO = sqrt(3)/3.0
  // APOTEMA = 0.5;
  double dy = 0.5*sqrt(3.0);
      
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++){
      matriz[i][j].joaninha = matriz[i][j].frio = matriz[i][j].calor = false;
      matriz[i][j].temperatura = 0;
      matriz[i][j].i = i;
      matriz[i][j].j = j;
      if(i%2){
        matriz[i][j].euclidian[X] = j - 0.5;
        matriz[i][j].euclidian[Y] = dy*i;
      }
      else{
        matriz[i][j].euclidian[X] = j;
        matriz[i][j].euclidian[Y] = dy*i;
      }
      matriz[i][j].bug = NULL;
      matriz[i][j].old_diff = -1;
      matriz[i][j].seed = ((i+1) * seed_gb + j)%RAND_MAX;
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

void print_matrix2(Hexa** matrix, int altura, int largura){
  for(int i = 0; i < altura; i++ ){
    for(int j = 0; j < largura; j++){
      if(matrix[j][i].joaninha)
        printf("* ");
      else if(matrix[j][i].calor)
        printf("+ ");
      else if(matrix[j][i].frio)
        printf("- ");
      else
        printf(". ");
    }
    printf("\n");
  }
}


void coloca_joaninha(Hexa** matrix, int altura, int largura, int num_j){
  srand(seed_gb);
  for(int i  =  0; i < num_j; ){
    int pos_i = rand()%altura;
    int pos_j = rand()%largura;		
    if(!matrix[pos_i][pos_j].joaninha){
      Joaninha* ladybug;
      matrix[pos_i][pos_j].joaninha = true;
      ladybug = (Joaninha*) malloc(sizeof(Joaninha));
      ladybug->pos_i = pos_i;
      ladybug->pos_j = pos_j;
      ladybug->move = false;
      ladybug->dest = NULL;
      joanas.push_back(ladybug);
      matrix[pos_i][pos_j].bug = ladybug;
      i++;
    }
  }
}

void coloca_fonte(Hexa** matrix, int altura, int largura, double p, int t,double calor){
  for(int i = 0; i < altura; i++){
    for(int j = 0; j < largura; j++){
      Hexa *m = &matrix[i][j];
      srand(m->seed);
      m->seed = ( (i+1)*m->seed + j)%RAND_MAX;
      double prob = (1.0*rand())/RAND_MAX;
      if(p > prob){
	if(calor < 0 && !m->frio){
	  m->frio = true;
	  FireAndIce ned = (FireAndIce) malloc(sizeof(*ned));
	  ned->temperatura = calor;
	  ned->ciclos_ativa = t;
	  ned->pos_i = i; ned->pos_j = j;
	  gelo_e_fogo.push_back(ned);
	}
	else if(calor > 0 && !m->calor){
	  m->calor = true;
	  FireAndIce ned = (FireAndIce) malloc(sizeof(*ned));
	  ned->temperatura = calor;
	  ned->ciclos_ativa = t;
	  ned->pos_i = i; ned->pos_j = j;
	  gelo_e_fogo.push_back(ned);
	}
      }
    }
  }
}

double dist_euclidiana(Hexa** m, int i, int j, int k, int l){
  double x1 = m[i][j].euclidian[X];
  double y1 = m[i][j].euclidian[Y];
  double x2 = m[k][l].euclidian[X];
  double y2 = m[k][l].euclidian[Y];

  return fabs((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


double temp_hex(Hexa**m, int alt, int lar, int i, int j, double cte){
  double temp_h = 0;
  for(int k = 0; k < joanas.size(); k++){
    Joaninha* jojo = joanas[k];
    if(i != jojo->pos_i || j != jojo->pos_j)
      temp_h += cte/dist_euclidiana(m, i, j, jojo->pos_i, jojo->pos_j);
  }
  for(int k = 0; k < gelo_e_fogo.size(); k++){
    FireAndIce fonte = gelo_e_fogo[k];
    if(i != fonte->pos_i || j != fonte->pos_j)
      temp_h += (fonte->temperatura)/dist_euclidiana(m, i, j, fonte->pos_i, fonte->pos_j);
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


void move_joaninha(Hexa** m, int alt, int lar, int index, double cte){
  Joaninha* jojo = joanas[index];
  int i = jojo->pos_i, j = jojo->pos_j; 
  Hexa cel_j = m[i][j];
  Hexa* v[6];
  Hexa* dest = &cel_j;
  double temp_atual = temp_hex(m, alt, lar, i, j,cte);
        
  if (temp_atual <= THETA_MAX && temp_atual >= THETA_MIN) {
    return ;
  }
        
  for(int k = 0; k < 6; k++)
    v[k] = NULL;
      
  double diff = 1 << 20;

  if(i%2){ /*linha impar*/
    if(i >= 1 && j >= 1) v[1] = &m[i-1][j-1];
    if(i >= 1) v[2] = &m[i-1][j];
    if(j >= 1) v[3] = &m[i][j-1];
    if(j + 1 < lar ) v[4] = &m[i][j+1];
    if(i + 1 < alt && j >= 1) v[5] = &m[i+1][j-1];
    if(i + 1 < alt) v[0] = &m[i+1][j];
  }

  else{ /*linha par*/
    if(i >= 1 && j + 1 < lar) v[1] = &m[i-1][j+1];
    if(i >= 1) v[2] = &m[i-1][j];
    if(j >= 1) v[3] = &m[i][j-1];
    if(j + 1 < lar ) v[4] = &m[i][j+1];
    if(i + 1 < alt && j + 1 < lar) v[5] = &m[i+1][j+1];
    if(i + 1 < alt) v[0] = &m[i+1][j];
  }

  for(int k = 0; k < 6; k++){
    if(v[k] != NULL && !v[k]->joaninha){
      v[k]->temperatura = temp_hex(m,alt,lar,v[k]->i,v[k]->j,cte);
      if(v[k]->temperatura >= THETA_MIN && v[k]->temperatura <= THETA_MAX){
        if( fabs(temp_atual - v[k]->temperatura) < diff ){
          diff = fabs(temp_atual - v[k]->temperatura);
          dest = v[k];
        }
      }
      v[k]->temperatura = 0;
    }
  }
      
  if(dest->euclidian[0] != cel_j.euclidian[0] || dest->euclidian[1] != cel_j.euclidian[1]){
    if(dest->old_diff < fabs(dest->temperatura - temp_atual)){
      if(dest->bug == NULL){
        dest->bug = jojo;
        jojo->move = true;
        dest->old_diff = fabs(dest->temperatura - temp_atual);
        jojo->dest = dest;
      }
      else{
        dest->bug->move = false;
        dest->bug = jojo;
        jojo->move = true;
        dest->old_diff  = fabs(dest->temperatura - temp_atual);
        jojo->dest = dest;
      }
    }
  }

}

void remove_fontes_esgotadas(Hexa** m){
  for (int i = 0; i < gelo_e_fogo.size(); i++) {
    gelo_e_fogo[i]->ciclos_ativa--;
    if (!gelo_e_fogo[i]->ciclos_ativa) {
		if(gelo_e_fogo[i]->temperatura >= 0){
			m[gelo_e_fogo[i]->pos_i][gelo_e_fogo[i]->pos_j].calor = false;
		}
		if(gelo_e_fogo[i]->temperatura <0){
			m[gelo_e_fogo[i]->pos_i][gelo_e_fogo[i]->pos_j].frio = false;
		}
      gelo_e_fogo.erase(gelo_e_fogo.begin()+i);
    }
  }
}

void resolve_movimentos(Hexa** m, int alt, int lar, double cte){
  for(int i = 0; i < joanas.size(); i++)
    move_joaninha(m, alt, lar, i, cte);
      
  for(int i = 0; i < joanas.size(); i++){
    Joaninha *j = joanas[i];
    if(j->move){
      m[j->pos_i][j->pos_j].joaninha = false;
      m[j->pos_i][j->pos_j].bug = NULL;
      m[j->pos_i][j->pos_j].old_diff = -1;;
      j->pos_i = j->dest->i;
      j->pos_j = j->dest->j;
      j->move = false;
      m[j->pos_i][j->pos_j].joaninha = true;
      m[j->pos_i][j->pos_j].old_diff = -1;
    }
    else{
      j->dest = NULL;
    }
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
      else if (matriz[j][i].frio && !matriz[j][i].calor){
        screen[x + 1][y + 3] = LEFT_DEL;
        screen[x + 1][y + 4] = FRIO;
        screen[x + 1][y + 5] = RIGHT_DEL;
      }
      else if (matriz[j][i].calor && !matriz[j][i].frio){
        screen[x + 1][y + 3] = LEFT_DEL;
        screen[x + 1][y + 4] = CALOR;
        screen[x + 1][y + 5] = RIGHT_DEL;
      }
      else if(matriz[j][i].calor && matriz[j][i].frio){
	screen[x + 1][y + 3] = LEFT_DEL;
        screen[x + 1][y + 4] = CALOR_E_FRIO;
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
      cout << screen[i][j];
    }
    cout << endl;
  }
  cout << endl;
}



int main(int arg, char** argv){
  Hexa **matriz = init_grid(N, M);
      
  int j = atoi(argv[1]);
  int t = atoi(argv[2]);
  seed_gb = atoi(argv[3]);
      
  joanas.resize(j);
  joanas.clear(); 

  coloca_joaninha(matriz, N, M, j);
      
  for(int k = 0; k < t; k++){
      
    coloca_fonte(matriz, N, M, PROB_CALOR, DURACAO_CALOR, TEMP_FONTE);
    coloca_fonte(matriz, N, M, PROB_FRIO, DURACAO_FRIO, -TEMP_FONTE);
      
    init_Screen(N,M,matriz);
    resolve_movimentos(matriz, N, M, TEMP_FONTE);
    remove_fontes_esgotadas(matriz);
    sleep(1);
  }
  //init_Screen(N,M,matriz);
  /* for(int i = 0; i < N; i++)
     for(int j = 0; j < M; j++)
     cout << "Pos " << i << j << " Euclidiana: " << matriz[i][j].euclidian[X] << " , " << matriz[i][j].euclidian[Y] << endl;
  */
      
  return 0;
}
