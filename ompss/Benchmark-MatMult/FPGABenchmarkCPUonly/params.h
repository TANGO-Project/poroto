//#define HEIGHTA		10
//#define WIDTHA		10000
#define HEIGHTB		HEIGHTA
//#define WIDTHB		10
#define HEIGHTC		HEIGHTA
#define WIDTHC		WIDTHB


#define HEIGHTA		316
#define WIDTHA		316
#define WIDTHB		316


// Test related params
#define DEBUG           0
#define PROFILING       1
#define SAMPLES         1
// 30
   // Tolerance for comparing FP results
   // due to different rounding in GPU ops.
#define FP_TOLERANCE    1e-3f


// Use case related params
#define         N               1024
#define         T               100
#define		DT		0.01f

// define number of iterations of the execution of the function on CPU and on FPGA
#define CPU_ITER	100 /*100 200*/
#define FPGA_ITER	20 /*10  200*/ 
