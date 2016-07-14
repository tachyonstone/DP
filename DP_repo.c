#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RAND_SEED 2014720
#define N_DATA 200
#define SIGMA 0.7 // 加わるノイズの標準偏差
#define P00 0.99 // 状態遷移確率の定義．P01 は 0→1 の遷移確率
#define P01 0.01
#define P10 0.03
#define P11 0.97

/* ここではグローバル変数を使う．コードを短くし，分かりやすくするため． */
int x[N_DATA]; // もともとの信号． 0 か 1
int xmap[N_DATA]; // 推定値． 0 か 1
double y[N_DATA]; // 観測データ
int xhat[N_DATA][2]; // xhat[2][b] = argmax_a { ( f[2][a] + h(a,b) } 教科書 p.218 参照
double f[N_DATA][2];

double nrnd();

void generate_x(){

  int i;
  double r;

  if ( drand48() < 0.5 ){
	x[0]=0;
  }
  else{
	x[0]=1;
  }

  for (i=1; i<N_DATA; i++){

	r = drand48();

	if(x[i-1] == 0){
	  if(r<0.99){
		x[i] = 0;
	  }else{
		x[i] = 1;
	  }
	}else{
	  if(r<0.97){
		x[i] = 1;
	  }else{
		x[i] = 0;
	  }
	}

  }

}

void generate_y(){

  int i;

  for (i=0; i<N_DATA; i++){
	y[i]=(double)x[i] + SIGMA*nrnd();
  }

}

void compute_xmap(){

  int i=0;

  double p_comp_1;
  double p_comp_2;
  double p_comp_3;
  double p_comp_4;

  double p_00 = log(P00);
  double p_01 = log(P01);
  double p_10 = log(P10);
  double p_11 = log(P11);

  int n = 199;
  int a;

  p_comp_1 = p_00-(pow((y[0]-0),2)/(2*pow(SIGMA,2))) + 0.5;
  p_comp_3 = p_10 -(pow((y[0]-1),2)/(2*pow(SIGMA,2))) + 0.5;

  if(p_comp_1 > p_comp_3){
	xhat[i][0] = 0;
	f[i][0] = p_comp_1;
  }else{
	xhat[i][0] = 1;
	f[i][0] = p_comp_3;
  }

  p_comp_2 = p_01 -(pow((y[0]-0),2)/(2*pow(SIGMA,2))) + 0.5;
  p_comp_4 = p_11-(pow((y[0]-1),2)/(2*pow(SIGMA,2))) + 0.5;

  if(p_comp_2 > p_comp_4){
	xhat[i][1] = 0;
	f[i][1] = p_comp_2;
  }else{
	xhat[i][1] = 1;
	f[i][1] = p_comp_4;
  }


  printf("p_ :test0 :%f %f %f %f\n",p_comp_1,p_comp_2,p_comp_3,p_comp_4);///test
  printf("xhat{{%d, %d}}\n", xhat[0][0], xhat[0][1]);


  for(i=1; i<N_DATA; i++){

 	p_comp_1=0;
	p_comp_2=0;
	p_comp_3=0;
	p_comp_4=0;

	//a-->0
    p_comp_1 = p_00 -(pow((y[i]-0),2)/(2*pow(SIGMA,2)))  +f[i-1][0];
    p_comp_3 = p_10 -(pow((y[i]-1),2)/(2*pow(SIGMA,2)))  +f[i-1][1];

	if(p_comp_1 > p_comp_3){
	  xhat[i][0] = 0;
	  f[i][0] = p_comp_1;
	}else{
	  xhat[i][0] = 1;
	  f[i][0] = p_comp_3;
	}

	//a-->1
	p_comp_2 = p_01 -(pow((y[i]-0),2)/(2*pow(SIGMA,2)))  + f[i-1][0];
	p_comp_4 = p_11 -(pow((y[i]-1),2)/(2*pow(SIGMA,2))) + f[i-1][1];

	if(p_comp_2 > p_comp_4){
	  xhat[i][1] = 0;
	  f[i][1] = p_comp_2;
	}else{
	  xhat[i][1] = 1;
	  f[i][1] = p_comp_4;
	}

	printf("p_ :test1 :%f %f %f %f\n",p_comp_1,p_comp_2,p_comp_3,p_comp_4);///test
	printf("xhat{{%d, %d}}\n", xhat[i][0], xhat[i][1]);

  }

  printf("f[199][0]=%f\n", f[199][0]);
  printf("f[199][1] =%f\n", f[199][1]);

  if(f[n][0] > f[n][1]){
	a = 0;
  }else{
	a = 1;
  }

  xmap[n] = xhat[n][a];

  for(i=n-1; i>0; i--){
	xmap[i] = xhat[i][a];
	a=xhat[i][a];

  }

}

void show_resuls(){

  int i=0;

  for(i=0; i<N_DATA; i++){
	printf("%d\t%d\t%.8lf\t%d\n",i, x[i],y[i],xmap[i]+3);
  }


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

  /* 擬似乱数の種を設定 */
  //srand48(RAND_SEED);
  srand48((int)time(NULL));

  /* 問題を作る（200 個のデータ生成） */
  generate_x ();
  generate_y ();

  /* 復元する */
  compute_xmap ();

  /* 結果を表示する */
  show_resuls();

  return 0;

}
