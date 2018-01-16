#include <stdio.h>		/*basic library*/
#include <stdlib.h>		/*basic library*/
#include <string.h> 	/*for read buffer and processing */

#include "parseFile.h"

int main(int arg, char* args[]){
	calcul_val *struct_init=NULL, *struct_cpu=NULL, *struct_fpga=NULL;
	if(arg < 2){
		printf("./parsefile <Name of File>\n");
		exit(-1);
	}
	if(NULL == (struct_init= (calcul_val*)malloc(sizeof(calcul_val)))){
		printf("erreur malloc struct_init\n");
		exit(-2);
	}
	if(NULL == (struct_cpu= (calcul_val*)malloc(sizeof(calcul_val)))){
		printf("erreur malloc struct_cpu\n");
		exit(-2);
	}
	if(NULL == (struct_fpga= (calcul_val*)malloc(sizeof(calcul_val)))){
		printf("erreur malloc struct_fpga\n");
		exit(-2);
	}
	
	FILE* read = openFileRead(args[1]);
	readFile(read, struct_init, struct_cpu, struct_fpga);
	printf("nb_val_init = %d , moy_init = %f , ecart_type_init = %f\n",struct_init->nb_elements,struct_init->moyenne,struct_init->ecart_type);
	printf("nb_val_cpu = %d , moy_cpu = %f , ecart_type_cpu = %f\n",struct_cpu->nb_elements,struct_cpu->moyenne,struct_cpu->ecart_type);
	printf("nb_val_fpga = %d , moy_fpga = %f , ecart_type_fpga = %f\n",struct_fpga->nb_elements,struct_fpga->moyenne,struct_fpga->ecart_type);
	
	
	return 0;
}
