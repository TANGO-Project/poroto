FILE* openFileRead(char* file);
void readFile(FILE* file, char** matrix1, char** matrix2, int* h_A,int* wA_hB, int* w_B);
void createMatrixLine(char* chain_matrix, int* matrix);
int testArguments(int argc);
int testFileCSV();
void createFileCSV();
FILE* benchmarkingMatrixFile(const char* name);
