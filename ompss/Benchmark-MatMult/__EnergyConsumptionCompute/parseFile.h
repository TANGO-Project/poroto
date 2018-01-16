typedef struct calcul_val{
	int nb_elements;
	float *tabelements;
	float moyenne;
	float ecart_type;
	
}calcul_val;

FILE* openFileRead(char* file);
void readFile(FILE* file, calcul_val* struct_init, calcul_val* struct_cpu, calcul_val* struct_fpga);
void moy_ect(calcul_val* struct_val);
