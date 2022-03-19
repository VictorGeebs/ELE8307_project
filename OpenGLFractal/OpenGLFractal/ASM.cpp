
/*
    Exemple illustratif d'une fonction qui retourne la somme des valeurs d'un tableau
    C'est la version purement logicielle
*/
int exemple_ASM(unsigned int * table, int length) {
    int result = 0;
    for (int i = 0; i < length; i++) {
        result += table[i];
    }
    return result;
}

/**************************************************************************************
/ Instruction specilalisee, utiliser la forme :
/ unsigned int fct(unsigned int n, unsigned int datab, unsigned int datab)
**************************************************************************************/ 

/*
    Premiere version ASM pas optimisee
    ATTENTION comme cela reste du C/C++, toutes les operations se font de maniere 
	sequentielles, ce qui ne sera pas le cas en VHDL
*/
unsigned int example_custom_1(int n, unsigned int dataa, unsigned int datab) {    // pas optimise
	int start = 1;		// Not really used, just for simulation purpose in C/C++
	int valid = 0;		// Not really used, just for simulation purpose in C/C++
	static unsigned int result;		//All registers must be declared static to keep their value from call to call
    static unsigned int* table; 	//idem
    static unsigned int* MAR; 		//idem
	static int length; 				//idem
    static int i;					//idem

    INIT:   if (start == 0) { valid=0; goto INIT; }
            else { valid = 0; table = (unsigned int*) dataa; length = datab; result = 0;  i = 0;  goto S1; }

    S1:     if (i < length) { MAR = table + i; goto S2; }
            else { valid = 1; return result; }

    S2:     if (1) { result = result + *MAR; i++; goto S1; }

}

/*
    C'est une version un peu plus optimisee de l'ASM
    ATTENTION comme cela reste du C/C++, toutes les operations se font de maniere 
	sequentielles, ce qui ne sera pas le cas en VHDL
 */
unsigned int example_custom_2(unsigned int n, unsigned int dataa, unsigned int datab) {    // pas optimise
	int start = 1;		// Not really used, just for simulation purpose in C/C++
	int valid = 0;		// Not really used, just for simulation purpose in C/C++
	static unsigned int result;		//All registers must be declared static to keep their value from call to call
    static unsigned int* table;  	//idem
    static unsigned int* MAR;  		//idem
	static static int length;  		//idem
    int i;                      	//idem

    INIT:   if (start == 0) {valid=0; goto INIT; }
            else { valid = 0; (unsigned int*) dataa; length = datab; result = 0;  i = 0; MAR = table; goto S1; }

    S1:     if (i < length) { result = result + *MAR; MAR = table + i + 1; i++; goto S1; }
            else { valid = 1; return result; }

 }
 
 /**************************************************************************************
/ Peripherique Avalon, utiliser la forme :
/ unsigned int fct(unsigned address, unsigned int writedata, unsigned read, unsigned write)
**************************************************************************************/ 

/*
    Premiere version ASM pas optimisee
    ATTENTION comme cela reste du C/C++, toutes les operations se font de maniere 
	sequentielles, ce qui ne sera pas le cas en VHDL
*/
unsigned int example_avalon(unsigned address, unsigned int writedata, unsigned read, unsigned write) {    // pas optimise
	int waitrequest = 1;			// Not really used, just for simulation purpose in C/C++
	static unsigned int result, readdata;		//All registers must be declared static to keep their value from call to call
    static unsigned int* table; 	         	//idem
    static unsigned int* MAR;               	//idem
	static int length;                      	//idem
    static int i;                              	//idem

    INIT:   if (write == 1) && (address == 0) {waitrequest = 0; table = (unsigned int*) writedata; return 0; }
            else if (write = 1) && {address == 4) {waitrequest = 1; length = writedata; result = 0;  i = 0; MAR = table; goto S1; }
            else if (read = 1) && {address == 0) {waitrequest = 0; readdata = result; return readdata; }
			else {waitrequest = 1; goto INIT; }

    S1:     if (i < length) { result = result + *MAR; MAR = table + i + 1; i++; goto S1; }
            else { waitrequest = 0; readdata = result; return readdata; }

}

 
 

