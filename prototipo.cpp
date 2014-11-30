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
#define PROB_FRIO 0.3
#define PROB_CALOR 0.1

#define THETA_MAX 27
#define THETA_MIN 18

#define N 100
#define M 100


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

vector<Joaninha*> joanas;
int seed;

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
  srand(seed);
  for(int i  =  0; i < num_j; ){
    int pos_i = rand()%altura;
    int pos_j = rand()%largura;		
    if(!matrix[pos_i][pos_j].joaninha){
      Joaninha* ladybug;
      matrix[pos_i][pos_j].joaninha = true;
      ladybug = (Joaninha*) malloc(sizeof(Joaninha));
      ladybug->pos_i = pos_i;
      ladybug->pos_j = pos_j;
      joanas.push_back(ladybug);
      i++;
    }
  }
}

void coloca_fonte(Hexa** matrix, int altura, int largura, double p, int t,double calor){
  srand(seed);
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

/*double dist_gubi(int i,int j,int k, int l){
  if(i == k){
    return abs(l-j);
  }
  if(j == l){
    return abs(i-k);
  }
  else{ //nao-adjacente
    
  }
  }*/

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
  Joaninha* jojo = joanas[index];
  int i = jojo->pos_i, j = jojo->pos_j; 
  Hexa cel_j = m[i][j];
  Hexa* v[6];
  Hexa* dest = &cel_j;
  double temp_atual = cel_j.temperatura;

  for(int k = 0; k < 6; k++)
    v[k] = NULL;
  
  double diff = -1;

  if(i%2){ /*linha impar*/
    if(i > 1 && j > 1) v[1] = &m[i-1][j-1];
    if(i > 1) v[2] = &m[i-1][j];
    if(j > 1) v[3] = &m[i][j-1];
    if(j + 1 < lar ) v[4] = &m[i][j+1];
    if(i + 1 < alt && j > 1) v[5] = &m[i+1][j-1];
    if(i + 1 < alt) v[0] = &m[i+1][j];
  }

  else{ /*linha par*/
    if(i > 1 && j + 1 < lar) v[1] = &m[i-1][j+1];
    if(i > 1) v[2] = &m[i-1][j];
    if(j > 1) v[3] = &m[i][j-1];
    if(j + 1 < lar ) v[4] = &m[i][j+1];
    if(i + 1 < alt && j + 1 < lar) v[5] = &m[i+1][j+1];
    if(i + 1 < alt) v[0] = &m[i+1][j];
  }

  for(int k = 0; k < 6; k++){
    if(v[k] != NULL && v[k]->temperatura >= THETA_MIN && v[k]->temperatura <= THETA_MAX && !v[k]->joaninha){
      if( fabs(temp_atual - v[k]->temperatura) > diff ){
	diff = fabs(temp_atual - v[k]->temperatura);
	dest = v[k];
      }
    }
  }
  
  if(dest->euclidian[0] != cel_j.euclidian[0] || dest->euclidian[1] != cel_j.euclidian[1]){
    Joaninha* hue = jojo;
    //    printf("Joaninha na pos [%d][%d] quer se mover para cel de temp %lf.\n", i, j, dest->temperatura); 
    dest->list.push_back(hue); /*coloca a joaninha na lista daqueles que querem se mover para o hexagono dest*/
  }

}

void resolve_movimentos(Hexa** m, int alt, int lar){
  for(int i = 0; i < joanas.size(); i++)
    move_joaninha(m, alt, lar, i);
  
  double diff = -1;

  for(int i = 0; i < alt; i++){ //itera em todos os hexagonos
    for(int j = 0; j < lar; j++){
      int x,y;
      Joaninha* escolhida = NULL;
      for(int k = 0; k < m[i][j].list.size(); k++){ //itera na lista de quem quer ir para o hexagono
	//	printf("celula [%d][%d] tem list nao vazia.\n", i,j);
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
	  escolhida->pos_i = i;
	  escolhida->pos_j = j;
	}
      }

      diff = -1;
      m[i][j].list.clear();
      escolhida = NULL;
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
  seed = atoi(argv[3]);
  
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
  for(int k = 0; k < t; k++){
    
    //print_matrix(matriz, N, M);
  
    //    cout << endl;

    //print_matrix2(matriz, N, M); 
    
    atualiza_temps(matriz, N, M, TEMP_FONTE);
    
    // print_temps(matriz, N, M);
    
    //init_Screen(N,M,matriz);
    //cout << "num: " << joanas.size() << endl;
    resolve_movimentos(matriz, N, M);
    //sleep(1);
  }
  
  /* for(int i = 0; i < N; i++)
     for(int j = 0; j < M; j++)
     cout << "Pos " << i << j << " Euclidiana: " << matriz[i][j].euclidian[X] << " , " << matriz[i][j].euclidian[Y] << endl;
  */
  
  return 0;
}
