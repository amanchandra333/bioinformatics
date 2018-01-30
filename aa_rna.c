//compile with -std=gnu11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

struct pose {
   float x;
   float y;
   float z;
   int atom;
   int res;
   char* atom_name;
   char* res_name;
};

char **strsplit(const char* pdb_line, const char* delim, int * token_count) {
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
    * token_count = used;
    free(s);
    return tokens;
}

int main(void) {
    FILE * pdb;
    FILE * pdb2;
    FILE * temp;
    FILE * temp2;
    char *line = NULL;
    char * line2 = NULL;
    size_t len, len2;
    ssize_t read, read2;
    char **tokens;
    int token_count;
    double dist;

    pdb2 = fopen("1asy.pdb", "r");								//Check if file is valid
    if (pdb2 == NULL)
        exit(EXIT_FAILURE);
    pdb = fopen("1asy.pdb", "r");								//Check if file is valid
    if (pdb == NULL)
        exit(EXIT_FAILURE);

    temp = fopen ("interact.pdb","w");
    fclose (temp);
    temp2 = fopen ("pymol_interact.pdb","w");
    fclose (temp2);

    size_t allocated_a = 1, allocated_r=1, used_a =0, used_r=0;
    struct pose *aa = calloc(allocated_a, sizeof(struct pose));
    struct pose *rna = calloc(allocated_r, sizeof(struct pose));

    while ((read = getline(&line, &len, pdb)) != -1) {						                        //Read one line at a time
    	tokens = strsplit(line, " \t\n", &token_count);
    	if(!strcmp(tokens[0],"ATOM")){
            if(!strcmp(tokens[4],"A")){
                if (used_a == allocated_a) {			                                            //Increase allocated space if needed
                    allocated_a *= 2;
                    aa= realloc(aa, allocated_a * sizeof(struct pose));
                }
                aa[used_a].x= atof(tokens[6]);
                aa[used_a].y= atof(tokens[7]);
                aa[used_a].z= atof(tokens[8]);
        		aa[used_a].res= atof(tokens[5]);
        		aa[used_a].atom_name= strdup(tokens[2]);
        		aa[used_a].res_name= strdup(tokens[3]);
                aa[used_a++].atom = atoi(tokens[1]);
            }
            if(!strcmp(tokens[4],"R")){
                if (used_r == allocated_r) {			                                            //Increase allocated space if needed
                    allocated_r *= 2;
                    rna = realloc(rna, allocated_r * sizeof(struct pose));
                }
                rna[used_r].x= atof(tokens[6]);
                rna[used_r].y= atof(tokens[7]);
                rna[used_r].z= atof(tokens[8]);
		        rna[used_r].res= atof(tokens[5]);
                rna[used_r].atom_name= strdup(tokens[2]);
                rna[used_r].res_name= strdup(tokens[3]);
                rna[used_r++].atom = atoi(tokens[1]);
            }
        }
        if (tokens != NULL)
            free(tokens);
    }
    aa= realloc(aa, used_a * sizeof(struct pose));
    rna= realloc(rna, used_r * sizeof(struct pose));
    if (line != NULL) free(line);
    fclose(pdb);

    bool *check = calloc(used_a+used_r, sizeof(bool));
    int i, j;
    for(i=0; i<used_a; i++){
        for(j=0; j<used_r; j++){
            dist = sqrt(pow(aa[i].x-rna[j].x,2) + pow(aa[i].y-rna[j].y,2) + pow(aa[i].z-rna[j].z,2));
            if(dist <= 4.5){
                check[aa[i].atom] = true;
                check[rna[j].atom] = true;
        		temp = fopen ("interact.pdb","a");
        		//fprintf(temp, "%d\n", aa[i].res);				 //Print only residue numbers
        		fprintf (temp, "%s,\t%d,\t%s,\t%d\t-\t%s,\t%d,\t%s,\t%d\t%f\n",  //Write to file
                                aa[i].atom_name, aa[i].atom, aa[i].res_name, aa[i].res,
        			rna[j].atom_name, rna[j].atom, rna[j].res_name, rna[j].res, dist);
        		fclose (temp);
            }
        }
    }

    while ((read2 = getline(&line2, &len2, pdb2)) != -1) {						//Read one line at a time
        tokens = strsplit(line2, " \t\n", &token_count);
        if(!strcmp(tokens[0],"ATOM") && check[atoi(tokens[1])]){
            temp2 = fopen ("pymol_interact.pdb","a");
            if(token_count==12)
                fprintf (temp2, "%s  %5s  %-3s %3s %s%4s    %8s%8s%8s  %4s %5s           %s\n",	//Write to file
                        tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6],
                        tokens[7], tokens[8], tokens[9], tokens[10], tokens[11]);
            else
                fprintf (temp2, "%s  %5s  %-3s %3s %s%4s    %8s%8s%8s  %10s           %s\n",	//Write to file
                        tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6],
                        tokens[7], tokens[8], tokens[9], tokens[10]);
            fclose (temp2);
        }
        if (tokens != NULL)
            free(tokens);
    }
    if (line2 != NULL) free(line2);

    return EXIT_SUCCESS;
}
