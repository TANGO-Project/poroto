#include <stdio.h>		/*basic library*/
#include <stdlib.h>		/*basic library*/
#include <string.h> 	/*for read buffer and processing */
#include <dirent.h>		/*for read and parse directory*/
#ifndef WIN32
	#include <sys/types.h>
#endif

#include "r_w_file.h"
#include "generatMatrix.h"
#define N 10000

/*
 * The document format is ".csv".
 * The first line is dimension of matrices with :
 *		the first element is matrix 1 height
 *		the second element is matrix 1 width , and matrix 2 height
 *		the third element is matrix 2 width. 
 * The second line is matrix 1
 * The third line is matrix 2
*/
/*just for hide function fopen*/
FILE* openFileRead(char* file){
	return fopen(file,"r");
}

void readFile(FILE* file, char* *matrix1, char* *matrix2, int* h_A,int* wA_hB, int* w_B){
	char* delimiter = ";";
	int i= 0,p=0,m=0;
	int test2;
	char* chaine = (char*)malloc(sizeof(char)*N);
	char* chaine2 = NULL;
	char* pointeur = NULL;
	while (fgets(chaine,N,file)!=NULL){
			m=0;
			if(i==0){
				chaine2 = strdup(chaine);
				pointeur = strtok(chaine2,delimiter);
				*h_A = atoi(pointeur);
				while(pointeur != NULL){
					pointeur = strtok(NULL,delimiter);
					if(pointeur != NULL){
						if(p==0){*wA_hB = atoi(pointeur);}
						if(p==1){*w_B = atoi(pointeur);}
					}
					p++;
				}
			}
			if(i==1){
				*matrix1 = strdup(chaine);
			}
			if(i==2){
				*matrix2 = strdup(chaine);
			}
			i++;				
	}	
}


void createMatrixLine(char* chain_matrix,int* matrix){
	char* ptr_chain=NULL, *chain_cp=(char*)malloc(sizeof(char)*strlen(chain_matrix));
	int i=1;
	char* delimiter = ";";
	chain_cp = strdup(chain_matrix);
	ptr_chain = strtok(chain_cp, delimiter);
	matrix[0] = atoi(ptr_chain);
	while(ptr_chain != NULL){
		ptr_chain = strtok(NULL,delimiter);
		if(ptr_chain != NULL){
			matrix[i]=atoi(ptr_chain);
		}
		i++;
	}

}

/*
 * if function testArguments return 0 => not arguments
 * else file is detected.
*/
int testArguments(int argc){
	return (argc == 1) ? 0 : 1 ;
}

int testFileCSV(){
	DIR* rep= NULL;
	char* delimiter = ".";
	char * pointeur = NULL ;
	char* namecp = NULL;
	rep = opendir("./");
	if(rep == NULL)
		exit(-1);
	struct dirent* fileread = NULL;
	while((fileread = readdir(rep)) != NULL)
		if(strcmp(fileread->d_name,".") != 0 && strcmp(fileread->d_name,"..") != 0){
			if(strchr(fileread->d_name,'.') != NULL){			
				namecp = strdup(fileread->d_name);
				pointeur = strtok(namecp,delimiter);
				pointeur = strtok(NULL,delimiter);
				if(strcmp(pointeur, "csv") == 0)
					return 1;
			}		
		}
		
	if(closedir(rep)==-1){
		puts("erreur de fermeture");
		exit(-1);
	}
	return 0;
}


void createFileCSV(){
	FILE* file=NULL;
	int a,b,c,i;
	file = fopen("source.csv", "w+");
	if(file == NULL){
		perror("");	
	}
	printf("Please enter the values of matrices\nhaving the form : \n"
	"height 1 space width 1 height 2 space width 2(ex 2 4 2) :");
	scanf("%d %d %d",&a,&b,&c);
	char* chaine = (char*)malloc(sizeof(char)*5);
	sprintf(chaine,"%d;%d;%d\n",a,b,c);
	fputs(chaine,file);
	int *matrix = NULL;
	matrix = (int*) malloc (sizeof(int)* a*b);
	generateMatrixline(matrix, a*b,50);
	for(i=0 ; i<a*b ; i++){
		sprintf(chaine,"%d",matrix[i]);
		fputs(chaine,file);		
		if(i < a*b -1){
			fputc(';',file);
		}	
	}
	fputs("\n",file);
	free(matrix);
	matrix = (int*) malloc (sizeof(int)* c*b);
	generateMatrixline(matrix, c*b,50);
	for(i=0 ; i<c*b ; i++){
		sprintf(chaine,"%d",matrix[i]);
		fputs(chaine,file);		
		if(i < c*b -1){
			fputc(';',file);
		}	
	}
	fputs("\n",file);
	free(chaine);
	free(matrix);
	fclose(file);
}




FILE* benchmarkingMatrixFile(const char* name){
    FILE* file=NULL;
    file = fopen(name, "a");
    if(file == NULL){
        perror("");
    }
    return file;
}
