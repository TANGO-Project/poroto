#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "omp.h"
#include "fonction.h"


int* initMatrixFpga(int height, int width){
	int *newMatrix;
	newMatrix = (int*)calloc(height*width,sizeof(*newMatrix));
	if (newMatrix == NULL) {
        return NULL;
    }
	return newMatrix;
}

int** initMatrix(int height, int width){
	int **newMatrix,i,j;
	newMatrix = (int**)calloc(height,sizeof(int*));
	if (newMatrix == NULL) {
        return NULL;
    }
	int *tabinMatrix = (int*)calloc(height*width,sizeof(int));
	if (tabinMatrix == NULL) {
        return NULL;
    }
	for(i=0 ; i < height; i++){
		newMatrix[i] = &tabinMatrix[i*width];
	}
	return newMatrix;
}

int** initMatrixBlock(int nb_block){
	int **newMatrix,i,j;
	newMatrix = (int**)calloc(nb_block,sizeof(int*));
	int *tabinMatrix = (int*)calloc(nb_block*4,sizeof(int));
	for(i=0 ; i < nb_block; i++){
		newMatrix[i] = &tabinMatrix[i*4];
	}
	return newMatrix;
}

void freeMatrix(int** matrix){
	free(matrix[0]);
	free(matrix);
}
/*
 *return  1 if size is not good
 *return  0 if size does not need to change   
*/
int sizeTest(int size){	
	return (size%2 == 0) ? 1 : 0;
}

int matrixNbBlock(int height, int width){
    return (height/2)*(width/2);
}

void matrixDisplay(int **tab, int height, int width){
	int i,j;
	for(i=0 ; i< height ; i++){
		for(j=0; j<width ; j++){
			if(tab[i][j]>=0 && tab[i][j]<=9){
				printf("  %d ",tab[i][j]);
			}
			else{
				printf(" %d ", tab[i][j]);
			}
			if(j != width-1){printf(",");}
		}
		printf("\n");
	}
}

void  matrixIncrease(int **oldTab, int **newTab, int newHeight, int newWidth){
	int i,j;
	//printf("newHeight = %d && newWidth==%d\n",newHeight,newWidth);
	for(i=0 ; i<newHeight-1 ; i++){
		for(j=0 ; j<newWidth-1 ; j++){
			newTab[i][j] = oldTab[i][j];
		}
	}
	
}

/*
 *	rebuild matrix in block
 *	all matrix blocks have a size of 2*2
 *  return rebuild matrix
*/
void rebuildMatrix2DOfBlock(int** matrixOld, int **matrix_block, int height, int width){

	int i,j,k,m,n=0,p=0,l=0;	
	int nb_block, size_block, limit_Block;
	nb_block = (width/2)*(height/2);
	size_block = 4;
	limit_Block = (width/2) -1;

	for(i=0 ; i<nb_block-limit_Block ; i+=(width/2)){
		for(j=0;j<2 ; j++){
			for(m=0 ; m<size_block ; m+=2){
				n=0;
				while(n<(width/2)){
					matrix_block[i+n][j+m]= matrixOld[l+(m/2)][(n*2)+j];
					n++;
				}
			}
		}		
		l+=2;	
	}	
}

void rebuildMatrixBlockOf2D(int** matrix2D, int **matrix_block, int height, int width){

	int i,j,k,m,n=0,p=0,l=0;	
	int nb_block, size_block, limit_Block;
	nb_block = (width/2)*(height/2);
	size_block = 4;
	limit_Block = (width/2) -1;

	for(i=0 ; i<nb_block-limit_Block ; i+=(width/2)){
		for(j=0;j<2 ; j++){
			for(m=0 ; m<size_block ; m+=2){
				n=0;
				while(n<(width/2)){
					matrix2D[l+(m/2)][(n*2)+j]=matrix_block[i+n][j+m];
					n++;
				}
			}
		}		
		l+=2;	
	}	
}

void matrixLineToMatrix2D(int *matrixOld, int** newMatrix, int height, int width){
	unsigned i,j,k=0;
	for(i=0 ; i<width*(height-1)+1 ;i+=width){
		for(j=0 ; j<width ; j++){
			newMatrix[k][j]=matrixOld[i+j];
		}
		k++;	
	}
}


void matrix2DToMatrixLine(int *matrixOld, int** newMatrix, int height, int width){
	unsigned i,j,k=0;
	for(i=0 ; i<width*(height-1)+1 ;i+=width){
		for(j=0 ; j<width ; j++){
			matrixOld[i+j] = newMatrix[k][j];
		}
		k++;	
	}
}


void multMatrix2of2(int* matrix1, int* matrix2, int* matrix_res){

	matrix_res[0] = matrix1[0]*matrix2[0]+matrix1[1]*matrix2[2];
	matrix_res[1] = matrix1[0]*matrix2[1]+matrix1[1]*matrix2[3];
	matrix_res[2] = matrix1[2]*matrix2[0]+matrix1[3]*matrix2[2];
	matrix_res[3] = matrix1[2]*matrix2[1]+matrix1[3]*matrix2[3];
}


void addMatrix2of2(int* matrix1, int* matrix2){
	matrix1[0]=matrix1[0]+matrix2[0];
	matrix1[1]=matrix1[1]+matrix2[1];
	matrix1[2]=matrix1[2]+matrix2[2];
	matrix1[3]=matrix1[3]+matrix2[3];
	
}


int ** calcMatrixBlock(int **matrix1, int** matrix2, int num_Block_HA, int num_Block_WB , int num_Block_WAHB){
	struct timeval start;
	struct timeval stop;
	unsigned long elapsed=0L;
	int i,j,k,l=0,m;
	int num_Bloc_Res = num_Block_HA*num_Block_WB;
	int ** matrix_res;
	int * matrix_tmp;
	matrix_res = initMatrixBlock(num_Bloc_Res);
	matrix_tmp = (int*)calloc(4,sizeof(int));
	gettimeofday(&start,NULL);
	for(i=0 ; i<num_Bloc_Res ; i+=num_Block_WB){
		m=0;
		for(j=0 ; j<num_Block_WAHB ; j++){
			for(k=0 ; k<num_Block_WB ; k++)
			{
				multMatrix2of2(matrix1[j+l],matrix2[m],matrix_tmp);
				addMatrix2of2(matrix_res[i+k], matrix_tmp );
				m++;
			}
		}
		l+=num_Block_WAHB;
		
	}
	gettimeofday(&stop,NULL);
	elapsed = 1000000 * (stop.tv_sec - start.tv_sec);
  	elapsed += stop.tv_usec - start.tv_usec;
	free(matrix_tmp);
	// time in usecs
  	printf("time: ");
  	printf ("%lu millisecondes;\n", elapsed/1000);
	return matrix_res;	
}


int ** calcMatrix(int **matrix1, int** matrix2, int height_A, int width_B , int width_A_height_B){
	struct timeval start;
	struct timeval stop;
	unsigned long elapsed=0L;
	int i,j,k;
	int ** matrix_res;
	int cur_sum=0;
	matrix_res = initMatrix(height_A,width_B);
	//gettimeofday(&start,NULL);
	for(i=0 ; i<height_A ; i++){
		for(j=0 ; j<width_B ; j++){
			cur_sum = 0;
			for(k=0 ; k<width_A_height_B ; ++k){
				cur_sum += matrix1[i][k]*matrix2[k][j];
			}
			matrix_res[i][j]= cur_sum;
		}
	}
	//gettimeofday(&stop,NULL);
	//elapsed = 1000000 * (stop.tv_sec - start.tv_sec);
  	//elapsed += stop.tv_usec - start.tv_usec;
	// time in usecs
  	//printf("time: ");
  	//printf ("%lu millisecondes;\n", elapsed/1000);
	return matrix_res;	
}

