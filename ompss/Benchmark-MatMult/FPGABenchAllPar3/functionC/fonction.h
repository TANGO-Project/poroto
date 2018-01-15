
int* initMatrixFpga(int height, int width);
int** initMatrix(int height, int width);
int** initMatrixBlock(int height);
void freeMatrix(int** matrix);
int sizeTest(int size);
int matrixNbBlock(int height, int width);
void matrixDisplay(int **tab, int height, int width);
void matrixIncrease(int **oldTab, int **newTab, int newHeight, int newWidth);
void rebuildMatrix2DOfBlock(int** matrixOld, int **matrix_block, int height, int width);
void rebuildMatrixBlockOf2D(int** matrix2D, int **matrix_block, int height, int width);
void matrixLineToMatrix2D(int matrixOld[], int** newMatrix, int height, int width);
void matrix2DToMatrixLine(int* matrixOld, int** newMatrix, int height,int width);
void multMatrix2of2(int* matrix1, int* matrix2, int* matrix_res);
void addMatrix2of2(int* matrix1, int* matrix2);
int ** calcMatrixBlock(int **matrix1, int** matrix2, int num_Block_HA, int num_Block_WB , int num_Block_WAHB);
int ** calcMatrix(int **matrix1, int** matrix2, int height_A, int width_B , int width_A_height_B);
