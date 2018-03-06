//compile with -std=gnu11 -lm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

struct pose {
   float x;
   float y;
   float z;
   bool atom;
   bool check;
   char chain;
   //int res;
   //char* atom_name;
   //char* res_name;
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
    FILE *asa, *pdb;
    char *line_asa = NULL, *line_pdb = NULL;
    size_t len_asa, len_pdb;
    ssize_t read_asa, read_pdb;
    char **tokens_asa, **tokens_pdb;
    int token_count_asa, token_count_pdb;
    int i,j, atom_num;
    float x,y,z,x1,y1,z1,dist;
    size_t allocated_a = 1, used_a=1;
    struct pose *atoms = calloc(allocated_a, sizeof(struct pose));

    for(i=0;i<CHAINS;i++){
        snprintf(buffer, sizeof(char) * 15, "pops/%c.pdb", i+65);
        asa[i] = fopen(buffer, "r");                              //Check if file is valid
        if (asa[i] == NULL)
            exit(EXIT_FAILURE);
    }

    while ((read_asa = getline(&line_asa, &len_asa, asa)) != -1) {						//Read one line at a time
        tokens_asa = strsplit(line_asa, " \t\n", &token_count_asa);
        if(token_count_asa==7){
            atom_num = atoi(tokens_asa[0]);
            if (atom_num >= allocated_a) {
                used_a = atom_num+1000;
                atoms = realloc(atoms, used_a * sizeof(struct pose));
            }
            atoms[atom_num].atom = true;
        }
        if (tokens_asa != NULL)
            free(tokens_asa);
    }

    while ((read_pdb = getline(&line_pdb, &len_pdb, pdb)) != -1) {               //Find interface atom coordinates
        tokens_pdb = strsplit(line_pdb, " \t\n", &token_count_pdb);
        if(!strcmp(tokens_pdb[0],"ATOM")){
            atom_num = atoi(tokens_pdb[1]);
            if(atoms[atom_num].atom){
                atoms[atom_num].x= atof(tokens_pdb[6]);
                atoms[atom_num].y= atof(tokens_pdb[7]);
                atoms[atom_num].z= atof(tokens_pdb[8]);
                atoms[atom_num].chain= atoi(tokens_pdb[4]);
        		// atoms[atom_num].res= atof(tokens[5]);
        		// atoms[atom_num].atom_name= strdup(tokens[2]);
        		// atoms[atom_num].res_name= strdup(tokens[3]);
            }
        }
        if (tokens_pdb != NULL)
            free(tokens_pdb);
    }

    for(i=0; i<used_a; i++){
        for(j=0; j<used_a; j++){
            if(atoms[i].atom && atoms[j].atom && atoms[i].chain!=atoms[j].chain){
                x = atoms[i].x; y= atoms[i].y; z=atoms[i].z;
                x1 = atoms[j].x; y1= atoms[j].y; z1=atoms[j].z;
                dist = sqrt(pow(x-x1,2) + pow(y-y1,2) + pow(z-z1,2));
                if(dist<=6.0){
                    atoms[i].check = true;
                    atoms[j].check = true;
                }
            }
        }
    }

    for(i=0; i<used_a; i++){
        if(atoms[i].check){
            
        }
    }

    return EXIT_SUCCESS;
}
