
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <inttypes.h>
#include "fpga.h"
extern "C" {
#include "timer.h"
}
void MatrixMultiplication(int **A, int **B, int height_A, int width_A_height_B, int width_B, int **C)
{
	pFpgaSpace[0x0] = 1; //reset block
	fpga_write_vector(0, (height_A*width_A_height_B)*4, A);
	fpga_write_vector(1, (width_A_height_B*width_B)*4, B);
	pFpgaSpace[0x1] = (uint32_t)height_A;
	pFpgaSpace[0x2] = (uint32_t)width_A_height_B;
	pFpgaSpace[0x3] = (uint32_t)width_B;
	//StartTimer();
	while (pFpgaSpace[0x2000] == 0) ; //Wait for resultReady
	//GetTimer();
	fpga_read_vector(2, (height_A*width_B)*4, C);
}
