#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
#include "functionC/generatMatrix.h"


//*********************************************************************************

int flag;

const char* bitFile = "./bitfile/poroto100k.bit";

const int configPin1 = 2; //Permet de piloter le type d'output sur le terminal
const int configPin2 = 3;
const int sessionPin1 = 4; //Permet de synchroniser l'état de la session de mesure
const int sessionPin2 = 5;



int main() {

    //*****************************************************************************
    // FIRMATA FRAMEWORK + FPGA INITIALISATION
    //*****************************************************************************
/*
    t_firmata     *firmata;
    firmata = firmata_new("/dev/ttyUSB1"); //init Firmata
    while(!firmata->isReady) firmata_pull(firmata); //Wait until device is up

    firmata_pinMode(firmata, configPin1, MODE_OUTPUT);
    firmata_pinMode(firmata, configPin2, MODE_OUTPUT);
    firmata_pinMode(firmata, sessionPin1, MODE_OUTPUT);
    firmata_pinMode(firmata, sessionPin2, MODE_OUTPUT);
*/

    int res = fpga_init(bitFile);
    printf("FPGA_init (0 = success): %x\n", res);
//*****************************************************************************



    int test_successful = 1;
    int i,j,k;
    int c;
    //srand(time(NULL));

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


	generateMatrixline(A_2,height_A* width_A_height_B,1,50);
	generateMatrixline(B_2,width_A_height_B*width_B,1,50);
    //printf("\nmatrix A:\n");
    //matrixDisplay(A_0,height_A, width_A_height_B);
	//matrixDisplay(B_0,width_A_height_B,width_B);
    //printf("\nmatrix B:\n");
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
	int **C_expected_1= initMatrix(height_A, width_B);
	if (C_expected_1 == NULL) {
        printf("malloc error for C_expected_1");
        return 0;
    }

	matrixLineToMatrix2D(A_2, A_1, height_A, width_A_height_B);
	matrixLineToMatrix2D(B_2, B_1, width_A_height_B, width_B);


    for ( i = 0; i < height_A; i++)
    {
        for( j = 0; j < width_B; j++)
        {
            C_2[i*width_B+j]   =  -1 ;
	    C_expected_1[i][j] = -1;
        }
    }

    int **c_expected_tab[500];
    for(i=0 ; i<500 ; i++){
   	c_expected_tab[i]=initMatrix(height_A, width_B);	
    }

    c= 2;
    printf("**** Sleep some time (%d s) to allow mesurement of CPU under no charge: \n ", c);
    sleep (c);
    printf("\n**********************************\n ");
    printf("**** Matrix Mult (%d by %d) x (%d by %d) => (%d by %d): \n",height_A,width_A_height_B,width_A_height_B,width_B, height_A,width_B);
    printf("**********************************\n\n\n\n ");
    printf("\n**********************************\n ");
    printf("**** CPU-only implem execution: \n");
    printf("**********************************\n ");

    //printf("\nmatrix C = A x B (expected):\n");
    

   // firmata_digitalWrite(firmata, sessionPin1, HIGH);

    ResetTimerStats();
    flag=1;
    i=0;
    int iter_cpu=0, iter_fpga=0;
    printf("nombre de thread dispo %d\n",omp_get_num_threads());
    for(i=0 ; i< 500 ; i++)
    { 
		#pragma omp target device (smp) copy_deps
    		#pragma omp task inout(A_1,A_2,B_1,B_2,C_expected_1,C_2,i,flag,iter_cpu,iter_fpga)
		{
			
			#pragma omp target device (smp) copy_deps
                	#pragma omp task  in(A_1,A_2,B_1,B_2) out(C_expected_1,C_2,i,flag,iter_cpu,iter_fpga) 
			{
			 int cpu = omp_get_thread_num();		 	
			if(flag)     
			{
				flag=0;
			//	printf("FPGA exec! iter %d sur thread num %d\n",iter_fpga,cpu);				
				MatrixMultiplication ((int**)A_2,(int**) B_2, height_A, width_A_height_B,
							   width_B, (int**)C_2 );
				iter_fpga++;
				flag = 1;
			}	
			else
			{
				C_expected_1 = calcMatrix( A_1,  B_1, height_A, width_B ,width_A_height_B);
				iter_cpu++;
			//	printf("CPU exec! iter %d sur thread num %d\n",iter_cpu,cpu);
			}
			}
		 }  	  
	 }
	 #pragma omp taskwait
	 printf("\n\tfin de traitement par ajout des taches\n");
         printf("\tflag_CPU = %d , flag_FPGA=%d\n\n",iter_cpu,iter_fpga);		
   
    //firmata_digitalWrite(firmata, sessionPin1, LOW);
	for(i=0 ; i< height_A%10 ; i++){
		for(j=0 ;j<width_B%10 ; j++)
		printf("%d dans c_expected_tab position [%d,%d]\n",c_expected_tab[17][i][j],i,j);
	}	
	matrix2DToMatrixLine(C_expected_0, C_expected_1, height_A, width_B);
	freeMatrix(A_1);
	freeMatrix(B_1);
	freeMatrix(C_expected_1);
	for(i=0;i<500;i++){
        	freeMatrix(c_expected_tab[i]);
	}
  

    //resultReady not verified
    printf("\nC_0(after computation) :\n");



    for (i = 0; i < (height_A*width_B%10); ++i) {
        printf("C_0 = %d et C_expected_0 = %d\n", C_2[i],C_expected_0[i]);
    }
    printf("....\n");
    if ( memcmp(C_2, C_expected_0, height_A*width_B*4) != 0 ) {
        printf( "Retrieved data C_0 is NOT CORRECT\n" );
        test_successful = 0;
    }
    else
        printf( "Retrieved data C_0 is CORRECT\n" );


    fpga_close();
    free (A_2);
    free (B_2);
    free (C_2);
    free (C_expected_0);

    /*
    printf("press any key to continue: ");
        scanf("%c",&c);
    */
    printf("\nEND\n");

    return test_successful ? 0 : 1;
}

