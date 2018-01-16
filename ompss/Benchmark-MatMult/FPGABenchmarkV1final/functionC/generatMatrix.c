#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generatMatrix.h"
#include "random.h"



void generateMatrixline(int* matrix, int nb_element, int interval_min,int interval_max){ 
	int i;	
	initRandom();
	for (i=0 ; i<nb_element ; i++){
		matrix[i] = 1;	
	}
}

void generatMatrix(int** matrix,int height, int width){
	int i,j;
	initRandom();
	for(i=0 ; i<height ; i++){
		for(j=0 ; j<width ; j++){
			matrix[i][j] = myRandomMinMax(1,50);
		}	
	}

}

int createMatrixFile(){
	char reply;
	printf("Do you want creat a new matrix file ? (y or n) : ");
	scanf("%c",&reply);
	while(reply != 'y' && reply!='Y' && reply != 'n' && reply != 'N'){
		reply = ' ';		
		printf("Answer 'y' or 'n' please :");
		scanf("%c",&reply);	
	}
	return (reply == 'y' || reply == 'Y') ? 1 : 0;	
}
