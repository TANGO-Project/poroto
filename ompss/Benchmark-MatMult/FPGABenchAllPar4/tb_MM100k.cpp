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
#define SEARCH 1000
#define NB_LOOP 10

//*********************************************************************************

int flag;

const char* bitFile = "./bitfile/poroto100k.bit";
const char* name_file= "../__ExecStats/benchCPU_FPGA_4Par.csv";


const int configPin1 = 2; //Permet de piloter le type d'output sur le terminal
const int configPin2 = 3;
const int sessionPin1 = 4; //Permet de synchroniser l'état de la session de mesure
const int sessionPin2 = 5;


int main() {
    struct timeval time_full_top, time_full_end , time_loop_start, time_loop_end;

       
 //*****************************************************************************

    int test_successful = 1;
    int i,j,k;
    int c;
    double total_time = 0;

    //*****************************************************************************
    // OPEN FILE  : Storage in a file of benchmark data.
    // INIT FPGA DEVICE + FIRMATA FRAMEWORK
    //*****************************************************************************
    
     t_firmata     *firmata;
    firmata = firmata_new("/dev/ttyUSB1"); //init Firmata
    while(!firmata->isReady) firmata_pull(firmata); //Wait until device is up

    firmata_pinMode(firmata, configPin1, MODE_OUTPUT);
    firmata_pinMode(firmata, configPin2, MODE_OUTPUT);
    firmata_pinMode(firmata, sessionPin1, MODE_OUTPUT);
    firmata_pinMode(firmata, sessionPin2, MODE_OUTPUT);
    
    int res = fpga_init(bitFile);
    if(res) exit(-1);
    printf("FPGA init (success = 0 ) : %d\n",res);
    FILE* file=benchmarkingMatrixFile(name_file);
    fputs("Test of Matrice multiplication on CPU & FPGA\n",file);
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
    double total_cpu=0, total_fpga=0;
/*with OmpSs, the loop benchmarking brings about a task scheduling*/
//for(c=0 ; c < NB_LOOP ; c++)
//{ 
    /*The time of execution of variables declaration and to open file are insignificant */
    gettimeofday(&time_loop_start,NULL);
    

    int* A_2 = initMatrixFpga(height_A, width_A_height_B);
    if (A_2 == NULL) {
        printf("malloc error for A_0");
        return 0;
    }
    int* B_2 = initMatrixFpga(width_A_height_B, width_B);
    if (B_2 == NULL) {
        printf("malloc error for B_0");
        return 0;
    }
    int* C_2 = initMatrixFpga(height_A, width_B);
    if (C_2 == NULL) {
        printf("malloc error for C_0");
        return 0;
    }
    int* C_expected_0 = initMatrixFpga(height_A, width_B);
    if (C_expected_0 == NULL) {
        printf("malloc error for C_expected_0");
        return 0;
    }


    generateMatrixline(A_2,height_A* width_A_height_B,10);
    generateMatrixline(B_2,width_A_height_B*width_B,10);
 
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

    matrixLineToMatrix2D(A_2, A_1, height_A, width_A_height_B);
    matrixLineToMatrix2D(B_2, B_1, width_A_height_B, width_B);
    
    int **c_expected_tab[SEARCH];
    for(i=0 ; i<SEARCH ; i++){
   	c_expected_tab[i]=initMatrix(height_A, width_B);	
    }


     c= 2;
    printf("**** Sleep some time (%d s) to allow mesurement of CPU under no charge: \n ", c);
    sleep (c);
    
    printf("\n**********************************\n ");
    printf("**** Matrix Mult (%d by %d) x (%d by %d) => (%d by %d): \n",height_A,width_A_height_B,width_A_height_B,width_B, height_A,width_B);
    fprintf(file,"**** Matrix Mult (%d by %d) x (%d by %d) => (%d by %d) for the loop %d : \n",height_A,width_A_height_B,width_A_height_B,width_B, height_A,width_B,c);
    printf("**********************************\n\n\n\n ");
    printf("\n**********************************\n ");
    printf("**** CPU-FPGA implem execution: \n");
    printf("**********************************\n ");
    
    
    

    int iter_cpu=0,iter_fpga=0,position;
    struct timeval time_top[SEARCH], time_end[SEARCH];
    double time_cpu = 0, time_fpga=0 , total_time_loop = 0;
    
    printf("nombre de thread dispo %d\n",omp_get_num_threads());
    
    for(i=0 ; i< SEARCH ; i++)
    { 
	position = i;
	#pragma omp target device (smp) copy_deps
        #pragma omp task in(A_1,B_1,A_2,B_2) inout(c_expected_tab[100],iter_cpu,time_top,time_end,time_cpu,iter_fpga,time_fpga)
	{
            	if(position%5 ==0)
            	{
                	flag = 1;
            	}
            	if(!flag)
	   	{
			
                        #pragma omp task in(A_1,B_1) inout(c_expected_tab[100],iter_cpu,time_top,time_end,time_cpu) no_copy_deps
                        {
				firmata_digitalWrite(firmata, sessionPin1, HIGH);
				gettimeofday(&time_top[position],NULL);
				c_expected_tab[position] = calcMatrix( A_1,  B_1, height_A, width_B ,width_A_height_B);
				iter_cpu++;
                		gettimeofday(&time_end[position],NULL);
                		time_cpu+= ((time_end[position].tv_sec - time_top[position].tv_sec) * 1000000 + time_end[position].tv_usec) - time_top[position].tv_usec;
				firmata_digitalWrite(firmata, sessionPin1, LOW);
            		}
			
		}
	         			
		if(flag)
        	{
	    		
			flag=0;
            		gettimeofday(&time_top[position],NULL);
			
			#pragma omp task in(A_2,B_2) inout(C_2,iter_fpga) copy_deps
	    		{
				firmata_digitalWrite(firmata, sessionPin2, HIGH);
				MatrixMultiplication ((int**)A_2,(int**) B_2, height_A, width_A_height_B,width_B, (int**)C_2 );
            			iter_fpga++;
				firmata_digitalWrite(firmata, sessionPin2, LOW);
            		}
			#pragma omp taskwait
			
			gettimeofday(&time_end[position],NULL);
            		time_fpga+= ((time_end[position].tv_sec - time_top[position].tv_sec) * 1000000 + time_end[position].tv_usec) - time_top[position].tv_usec;
			
        	}
	}
    }
    #pragma omp taskwait 	          	
    
    
    
    printf("\n\tfin de traitement par ajout des taches\n");
    printf("\titer_CPU = %d ; iter_FPGA = %d\n\n",SEARCH-iter_fpga, iter_fpga);
    printf("temps total CPU = %f\ntemps total FPGA = %f\n",time_cpu/1000000,time_fpga/1000000);
    total_cpu += time_cpu/1000000;
    total_fpga += time_fpga/1000000;
    freeMatrix(A_1);
    freeMatrix(B_1);
    free (A_2);
    free (B_2);
    free (C_2);
    free (C_expected_0);

    for(i=0;i<SEARCH;i++)
    {
        freeMatrix(c_expected_tab[i]);
    }
    gettimeofday(&time_loop_end,NULL);
    total_time_loop = ((time_loop_end.tv_sec - time_loop_start.tv_sec) * 1000000 + time_loop_end.tv_usec) - time_loop_start.tv_usec;
    total_time += total_time_loop ;
    printf("temps total loop = %f , temps total cpu = %f \n temps total FPGA = %f temps total d'execution = %f\n",total_time_loop/1000000 , time_cpu/1000000 , time_fpga/1000000, total_time/1000000);
    fprintf(file, "temps CPU = %f temps FPGA = %f\n",time_cpu/1000000 , time_fpga/1000000);
    fprintf(file,"temps total loop= %f  \n\n",total_time_loop/1000000);
    fprintf(file,"temps cumul cpu = %f ; temps cumul fpga = %f \n",total_cpu,total_fpga);
//}
    fprintf(file,"\ntemps cumul moy cpu = %f ; temps cumul moy fpga = %f \n",total_cpu/iter_cpu, total_fpga/iter_fpga);
    fprintf(file,"temps total = %f for %d loop \n temps moyen = %f \n\n",total_time/1000000, NB_LOOP, (total_time/1000000)/10);
    fclose(file);   
    printf("\nEND\n");
   
    fpga_close();
    sleep(5);
    return test_successful ? 0 : 1;
}
