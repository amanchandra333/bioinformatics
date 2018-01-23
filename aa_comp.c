//compile with -std=gnu11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define token_count 12					//Number of Columns

char **strsplit(const char* pdb_line, const char* delim) {
    char *s = strdup(pdb_line);      			//Create a copy of the string

    size_t allocated = 1, used =0;
    char **tokens = calloc(allocated, sizeof(char*));
    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);	//Split input string
            token != NULL;
            token = strtok_r(NULL, delim, &strtok_ctx)) {
        if (used == allocated) {			//Increase allocated space if needed
            allocated *= 2;
            tokens = realloc(tokens, allocated * sizeof(char*));
        }
        tokens[used++] = strdup(token);
    }

    if (used == 0) {					//Free Memory
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, used * sizeof(char*));
    }
    free(s);
    return tokens;
}
int main(void) {
    FILE * pdb;
    char *line = NULL;
    size_t len;
    ssize_t read;
    char **tokens;
    int molecules=0;
    int prev_num=0, num=0;
    int flag = 0;
    float *comp = calloc(5, sizeof(float));							// 0: positive, 1: negetive, 2: aromatic,
												// 3: neutral pole, 4: hydrophobic

    pdb = fopen("3fe0.pdb", "r");								//Check if file is valid
    if (pdb == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, pdb)) != -1) {						//Read one line at a time
        tokens = strsplit(line, " \t\n");
        if(strcmp(tokens[0],"ATOM") ==0){
            num = atoi(tokens[5]);
	    if(num==prev_num){
		prev_num=num;
	    }

	    else{
                if(!strcmp(tokens[3], "ARG") || !strcmp(tokens[3], "LYS"))
		    comp[0]++;
	        if(!strcmp(tokens[3], "ASP") || !strcmp(tokens[3], "GLU"))
		    comp[1]++;
	        if(!strcmp(tokens[3], "TYR") || !strcmp(tokens[3], "TRP") || !strcmp(tokens[3], "PHE"))
                    comp[2]++;
	         if(!strcmp(tokens[3], "SER") || !strcmp(tokens[3], "THR") || !strcmp(tokens[3], "HIS") || 
		    !strcmp(tokens[3], "ASN") || !strcmp(tokens[3], "GLN") || !strcmp(tokens[3], "MET") ||
		    !strcmp(tokens[3], "CYS"))
                    comp[3]++;
	         if(!strcmp(tokens[3], "ILE") || !strcmp(tokens[3], "LEU") || !strcmp(tokens[3], "VAL") || 
		    !strcmp(tokens[3], "PRO") || !strcmp(tokens[3], "ALA") || !strcmp(tokens[3], "GLY"))
                    comp[4]++;

	        prev_num=num;
	        molecules++;
	    }
        }
        if (tokens != NULL)
            free(tokens);
    }

    double *percent = calloc(5, sizeof(double));
    int p;
    for(p=0; p<5; p++)
	percent[p]=comp[p]*100/molecules;
    printf("Positive\t:\t%.2f %% \nNegetive\t:\t%.2f %%\nAromatic\t:\t%.2f %%\nNeutral Pole\t:\t%.2f %%\nHydrophobic\t:\t%.2f %%\n",
		percent[0], percent[1], percent[2], percent[3], percent[4]);

    free(comp); free(percent);
    if (line != NULL) free(line);
    return EXIT_SUCCESS;
}
