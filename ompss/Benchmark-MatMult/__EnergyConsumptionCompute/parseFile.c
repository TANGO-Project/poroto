#include <stdio.h>		/*basic library*/
#include <stdlib.h>		/*basic library*/
#include <string.h> 	/*for read buffer and processing */
#include <dirent.h>		/*for read and parse directory*/
#include <math.h>
#ifndef WIN32
	#include <sys/types.h>
#endif
#include "parseFile.h"

FILE* openFileRead(char* file){
	return fopen(file,"r");
}

void readFile(FILE* file, calcul_val* struct_init, calcul_val* struct_cpu, calcul_val* struct_fpga){
	char* delimiter = ";";
	int cpu , fpga;
	printf("Verify File with number for cpu and fpga\n");
	printf("number for cpu : ");
	scanf("%d",&cpu);
	printf("number for fpga : ");
	scanf("%d",&fpga);
	int i,p=0, tabentry, cpt_init = 0, cpt_cpu = 0, cpt_fpga = 0;
	float temp_val  , tabinit[1000], tabcpu[1000], tabfpga[1000];
	int test2;
	char* chaine = (char*)malloc(sizeof(char)*500);
	char* chaine2 = NULL;
	char* pointeur = NULL;
	
	while (fgets(chaine, 500 ,file)!=NULL){
		p=0;
		chaine2 = strdup(chaine);
		pointeur = strtok(chaine2,delimiter);
		temp_val = atof(pointeur);
		while(pointeur != NULL){
			pointeur = strtok(NULL,delimiter);
			if(pointeur != NULL || pointeur!="" ){
				if(p == 0){
					tabentry = atoi(pointeur);
				}
				p++;
			}
			
		}
		if(tabentry == 0)tabinit[cpt_init++] = temp_val;
		else if(tabentry == cpu)tabcpu[cpt_cpu++] = temp_val;
		else if(tabentry == fpga)tabfpga[cpt_fpga++] = temp_val;
		else tabfpga[cpt_fpga++] = temp_val;
			
	}
	
	struct_init->nb_elements= cpt_init;
	struct_cpu->nb_elements=cpt_cpu;
	struct_fpga->nb_elements=cpt_fpga;
	struct_init->tabelements = (float*)calloc(cpt_init, sizeof(float));
	struct_cpu->tabelements = (float*)calloc(cpt_cpu, sizeof(float));
	struct_fpga->tabelements = (float*)calloc(cpt_fpga, sizeof(float));
	
	int lenght_max;
	lenght_max = (cpt_init>cpt_cpu) ? cpt_init : cpt_cpu;
	lenght_max = (lenght_max > cpt_fpga) ? lenght_max : cpt_fpga;
	for(i = 0 ; i<lenght_max ; i++){
		if(i<cpt_init){
			struct_init->tabelements[i] = tabinit[i];
		}
		if(i<cpt_cpu){
			struct_cpu->tabelements[i] = tabcpu[i];
		}
		if(i<cpt_fpga){
			struct_fpga->tabelements[i] = tabfpga[i];
		}
	} 
	moy_ect(struct_init);
	moy_ect(struct_cpu);
	moy_ect(struct_fpga);
		
}

void moy_ect(calcul_val* struct_val){
	int i; float somme = 0, sommecarre = 0 ;
	for(i = 0 ; i < struct_val->nb_elements ; i++){
		somme += struct_val->tabelements[i];
		
	}
	struct_val->moyenne = somme/(float) struct_val->nb_elements;
	for(i = 0 ; i < struct_val->nb_elements ; i++){
		sommecarre = (struct_val->tabelements[i]-struct_val->moyenne) * (struct_val->tabelements[i]-struct_val->moyenne);
		
	}
	
	
	
	
	struct_val->ecart_type = sqrt(sommecarre / (float) struct_val->nb_elements);
}
