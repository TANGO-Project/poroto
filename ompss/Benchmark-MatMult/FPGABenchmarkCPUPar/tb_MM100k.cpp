#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include "omp.h"
extern "C" {
#include <firmata.h>
}
#include <c_wrapper.h>
#include <fpga.h>
extern "C" {
#include "timer.h"
}
#include "params.h"
#include "functionC/fonction.h"
#include "functionC/r_w_file.h"
#include "functionC/generatMatrix.h"
#define SEARCH 100
#define NB_LOOP 10

//*********************************************************************************

int flag;

const char* bitFile = "./bitfile/poroto100k.bit";
const char* name_file= "./noloopCPU_2PAR.csv";
int main() {
    struct timeval time_full_top, time_full_end , time_loop_start, time_loop_end;
       
 //*****************************************************************************

    int test_successful = 1;
    int i,j,k;
    int c;
    double total_time = 0;

    //*****************************************************************************
    // OPEN FILE : Storage in a file of benchmark data.
    //*****************************************************************************
    FILE* file=benchmarkingMatrixFile(name_file);
    fputs("Test of Matrice multiplication only on CPU Parallel implementation\n",file);
    //*****************************************************************************
    // APPLICATION: SIMULATION  PATTERN
    //*****************************************************************************
    uint32_t rst;
    uint32_t height_A;
    uint32_t width_A_height_B;
    uint32_t width_B;
    uint32_t resultReady;
     

	//*****************************************************************************
    // test of size : works in multiples of 2 
    //*****************************************************************************
    height_A = sizeTest(HEIGHTA) ? HEIGHTA : HEIGHTA+1;
    width_A_height_B = sizeTest(WIDTHA) ? WIDTHA : WIDTHA+1;
    width_B = sizeTest(WIDTHB) ? WIDTHB : WIDTHB+1;
//for(c=0 ; c < NB_LOOP ; c++)
{ 
    /*The time of execution of variables declaration and to open file are insignificant */
    gettimeofday(&time_loop_start,NULL);
    printf("Loop nb %d\n",c); 
    int **A_1= initMatrix(height_A, width_A_height_B);
    if (A_1 == NULL) {
        printf("malloc error for A_1");
        return 0;
    }
    int **B_1= initMatrix(width_A_height_B, width_B);
    if (B_1 == NULL) {
        printf("malloc error for B_1");
        return 0;
    }

    generatMatrix(A_1, height_A, width_A_height_B);
    generatMatrix(B_1, width_A_height_B, width_B);
    
    int **c_expected_tab[SEARCH];
    for(i=0 ; i<SEARCH ; i++){
   	c_expected_tab[i]=initMatrix(height_A, width_B);	
    }

    printf("\n**********************************\n ");
    printf("**** Matrix Mult (%d by %d) x (%d by %d) => (%d by %d): \n",height_A,width_A_height_B,width_A_height_B,width_B, height_A,width_B);
    fprintf(file,"**** Matrix Mult (%d by %d) x (%d by %d) => (%d by %d) for the loop %d : \n",height_A,width_A_height_B,width_A_height_B,width_B, height_A,width_B,c);
    printf("**********************************\n\n\n\n ");
    printf("\n**********************************\n ");
    printf("**** CPU-only implem execution: \n");
    printf("**********************************\n ");
    
    i=0;
    
    int iter_cpu=0, position;
    struct timeval time_top[SEARCH], time_end[SEARCH];
    double time_cpu = 0 , total_time_loop = 0;
    printf("nombre de thread dispo %d\n",omp_get_num_threads());
    for(i=0 ; i< SEARCH ; i++)
    {

	position = i;
	#pragma omp target device (smp) 
	#pragma omp task in(A_1,B_1) out(c_expected_tab,time_top,time_end,time_cpu)
	{
		#pragma omp target device(smp)
		#pragma omp task in(A_1,B_1) out(c_expected_tab,time_top,time_end,time_cpu)
		{
	    	gettimeofday(&time_top[position],NULL);
	    	c_expected_tab[position] = calcMatrix( A_1,  B_1, height_A, width_B ,width_A_height_B);
	    	gettimeofday(&time_end[position],NULL);
	    	time_cpu+= ((time_end[position].tv_sec - time_top[position].tv_sec) * 1000000 + time_end[position].tv_usec) - time_top[position].tv_usec;
		}
		#pragma omp taskwait
	}		
    }
    #pragma omp taskwait  
    printf("\n\tfin de traitement par ajout des taches\n");
    printf("\tflag_CPU = %d \n\n",SEARCH);
    printf("temps total CPU = %f\n",time_cpu/1000000);
   
    freeMatrix(A_1);
    freeMatrix(B_1);
    for(i=0;i<SEARCH;i++)
    {
        freeMatrix(c_expected_tab[i]);
    }
    gettimeofday(&time_loop_end,NULL);
    total_time_loop = ((time_loop_end.tv_sec - time_loop_start.tv_sec) * 1000000 + time_loop_end.tv_usec) - time_loop_start.tv_usec;
    total_time += total_time_loop ;
    printf("temps total loop = %f , temps totalcpu = %f \n temps total d'execution = %f\n",total_time_loop/1000000, time_cpu/1000000,total_time/1000000);
    fprintf(file,"temps total loop= %f , temps totalcpu = %f \n\n",total_time_loop/1000000, time_cpu/1000000);
}
    fprintf(file,"temps total = %f for %d loop \n temps moyen = %f \n\n",total_time/1000000, NB_LOOP, (total_time/1000000)/10);
    fclose(file);   
    printf("\nEND\n");

    return test_successful ? 0 : 1;
}
