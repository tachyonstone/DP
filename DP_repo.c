#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RAND_SEED 201677
#define N_DATA 200
#define SIGMA 0.7 // 加わるノイズの標準偏差
#define P00 0.99 // 状態遷移確率の定義．P01 は 0→1 の遷移確率
#define P01 0.01
#define P10 0.03
#define P11 0.97

/////my_define
#define C -log((sqrt(2*M_PI)) * SIGMA)

/* ここではグローバル変数を使う．コードを短くし，分かりやすくするため． */
int x[N_DATA]; // もともとの信号． 0 か 1
int xmap[N_DATA]; // 推定値． 0 か 1
double y[N_DATA]; // 観測データ
int xhat[N_DATA][2]; // xhat[2][b] = argmax_a { ( f[2][a] + h(a,b) } 教科書 p.218 参照
double f[N_DATA][2];

////////////my_define
double p; //p(x)
double p_comp_1;
double p_comp_2;
double p_comp_3;
double p_comp_4;
double p_2;  //max p
double p_3;
int j=0;

double p_00=0;
double p_01=0;
double p_10=0;
double p_11=0;

double nrnd();

double max_value(double a, double b, double c, double d, int *xmap, int i, double p_00, double p_01, double p_10, double p_11, double *p_3){

  double max_1 = a;
  double max_2 = c;

  xmap[i] = 0;
  *p_3 = p_00;

  if(a < b){
	max_1 = b;
	*p_3 = p_01;
  }
  if(c < d){
	max_2 = d;
	xmap[i] = 1;
	*p_3 = p_11;
  }
  if(max_1 < max_2){
	xmap[i] = 1;
	if(max_2 == c){
	  *p_3 = p_10;
	}else{
	  *p_3 = p_11;
	}
	return max_2;
  }else{
	if(max_1 == a){
	  *p_3 = p_00;
	}else{
	  *p_3 = p_01;
	}
	xmap[i] = 0;
	return max_1;
  }

}


double max_value2(double a, double b, int *xmap, int i, double p_a, double p_b, double *p_3){

  double max = a;

  xmap[i] = 0;
  *p_3 = p_a;

  if(a < b){
	max=b;
	xmap[i] = 1;
	*p_3 = p_b;
	return max;
  }

}

void generate_x(){

  int i;

  /*
	if ( drand48() < 0.5 ){
	x[0]=0;
	}
	else{
	x[0]=1;
	}

	for (i=1; i<N_DATA; i++){
	x[i]=x[i-1];
	}
  */

  /*const_ver (temp)*/
  for (i=0; i<20; i++){
	x[i]=1;
  }
  for (i=20; i<80; i++){
	x[i]=0;
  }
  for (i=80; i<120; i++){
	x[i]=1;
  }
  for (i=120; i<140; i++){
	x[i]=0;
  }
  for (i=140; i<N_DATA; i++){
	x[i]=1;
  }

}

void generate_y(){

  int i;

  for (i=0; i<N_DATA; i++){
	y[i]=(double)x[i] + nrnd();
  }

}

void compute_xmap(){

  int i=0;

  p_2=0;
  p_3=0.5;


  p_00 = log(P00) + p_3;
  p_01 = log(P01) + p_3;
  p_10 = log(P10) + p_3;
  p_11 = log(P11) + p_3;

  p_comp_1 = (p_00+(C + (-(pow((y[0]-0),2)/(2*pow(SIGMA,2)))) ) + ( C + (-(pow((y[1]-0),2)/(2*pow(SIGMA,2)))) )) +p_2;
  p_comp_2 = (p_01+(C + (-(pow((y[0]-0),2)/(2*pow(SIGMA,2)))) ) + ( C + (-(pow((y[1]-1),2)/(2*pow(SIGMA,2)))) )) +p_2;
  p_comp_3 = (p_10+(C + (-(pow((y[0]-1),2)/(2*pow(SIGMA,2)))) ) + ( C + (-(pow((y[1]-0),2)/(2*pow(SIGMA,2)))) )) +p_2;
  p_comp_4 = (p_11+(C + (-(pow((y[0]-1),2)/(2*pow(SIGMA,2)))) ) + ( C + (-(pow((y[1]-1),2)/(2*pow(SIGMA,2)))) )) +p_2;

  p_2 = max_value(p_comp_1, p_comp_2, p_comp_3, p_comp_4, xmap, i, p_00,p_01,p_10,p_11,&p_3);

  printf("p_ :test0 :%f %f %f %f %f\n",p_comp_1,p_comp_2,p_comp_3,p_comp_4,p_2);///test

  for(i=1; i<N_DATA; i++){

	if(xmap[i-1]==0){
	  p_00 = log(P00) + p_3;

	  p_comp_1 = (p_00+(C + (-(pow((y[i]-0),2)/(2*pow(SIGMA,2)))) ) + ( C + (-(pow((y[i+1]-0),2)/(2*pow(SIGMA,2)))) )) +p_2;

	  p_01 = log(P01) + p_3;

	  p_comp_2 = (p_01+(C + (-(pow((y[i]-0),2)/(2*pow(SIGMA,2)))) ) + ( C + (-(pow((y[i+1]-1),2)/(2*pow(SIGMA,2)))) )) +p_2;

	  p_2 = max_value2(p_comp_1, p_comp_2, xmap, i, p_00,p_01,&p_3);


	}else if(xmap[i-1]==1){

	  p_10 = log(P10) + p_3;

	  p_comp_3 = (p_10+(C + (-(pow((y[i]-1),2)/(2*pow(SIGMA,2)))) ) + ( C + (-(pow((y[i+1]-0),2)/(2*pow(SIGMA,2)))) )) +p_2;

	  p_11 = log(P11) + p_3;

	  p_comp_4 = (p_11+(C + (-(pow((y[i]-1),2)/(2*pow(SIGMA,2)))) ) + ( C + (-(pow((y[i+1]-1),2)/(2*pow(SIGMA,2)))) )) +p_2;

 p_2 = max_value2(p_comp_3, p_comp_4, xmap, i, p_10,p_11,&p_3);

	}else{
	  printf("error\n");
	}



	printf("p_ :test :%f %f %f %f %f\n",p_comp_1,p_comp_2,p_comp_3,p_comp_4,p_2);///test
	p_comp_1=0;
	p_comp_2=0;
	p_comp_3=0;
	p_comp_4=0;


  }

}

void show_resuls(){

  int i=0;

  for(i=0; i<N_DATA; i++){
	printf("%d\t%d\t%.8lf\t%d\n",i, x[i],y[i],xmap[i]+3);
  }

  printf("p_2 : %f\n", p_2);

  printf("p : %f\n", p);

  printf("p_3 : %f\n", p_3);

}

/* 標準正規分布にしたがう擬似乱数の生成 */
double nrnd(){

  static int sw=0;
  static double r1,r2,s;

  if(sw==0){
	sw=1;
	do{
	  r1=2.0*drand48()-1.0;
	  r2=2.0*drand48()-1.0;
	  s=r1*r1+r2*r2;
	}while(s>1.0 || s==0.0);

	s=sqrt(-2.0*log(s)/s);

	return(r1*s);
  }
  else {
	sw=0;
	return(r2*s);
  }

}

int main(int argc , char * argv []){

  srand48(RAND_SEED); /* 擬似乱数の種を設定 */

  /* 問題を作る（200 個のデータ生成） */
  generate_x ();
  generate_y ();

  /* 復元する */
  compute_xmap ();

  /* 結果を表示する */
  show_resuls();

  return 0;

}
