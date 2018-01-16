FILE* openFileRead(char* file);
void readFile(FILE* file, char** matrix1, char** matrix2, int* h_A,int* wA_hB, int* w_B);
void createMatrixLine(char* chain_matrix, int* matrix);
int testArguments(int argc);
int testFileCSV();
void createFileCSV();
/* fonction de nettoyage de fichier et reecriture propore avec demande d'arguments height,width des matrices, puis generation. dans un interval demandé*/
