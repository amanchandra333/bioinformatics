//compile with -std=gnu11 -lm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#define CHAINS 8

struct pose {
   float x;
   float y;
   float z;
   char* chain;
   int atom;
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
    FILE** pdb = malloc(sizeof(FILE*) * (CHAINS));
    char **line = calloc((CHAINS), sizeof(char*));
    size_t len;
    ssize_t *read = malloc(sizeof(size_t) * (CHAINS));
    char **tokens;
    int token_count;
    int i,j, atom_num;
    float x,y,z,x1,y1,z1,dist;
    char buffer[15], chain[2];
    size_t allocated = 1, used =0;
    struct pose *atoms = calloc(allocated, sizeof(struct pose));    

    for(i=0;i<CHAINS;i++){
        snprintf(buffer, sizeof(char) * 15, "pops/%c.pdb", i+65);
        pdb[i] = fopen(buffer, "r");                //Check if file is valid
        if (pdb[i] == NULL)
            exit(EXIT_FAILURE);
    }

    for(i=0; i<CHAINS; i++){
        while ((read[i] = getline(&line[i], &len, pdb[i])) != -1){
            tokens = strsplit(line[i], " \t\n", &token_count);
            if(!strcmp(tokens[0],"ATOM")){
                if (used == allocated) {      //Increase allocated space if needed
                    allocated *= 2;
                    atoms = realloc(atoms, allocated * sizeof(struct pose));
                }
                if(!strcmp(tokens[3], "GLN") || !strcmp(tokens[3], "ASN") ||
                    !strcmp(tokens[3], "HIS") || !strcmp(tokens[3], "SER") ||
                    !strcmp(tokens[3], "THR") || !strcmp(tokens[3], "TYR") ||
                    !strcmp(tokens[3], "CYS") || !strcmp(tokens[3], "TRP")){
                    atoms[used].atom=atoi(tokens[1]);
                    atoms[used].x= atof(tokens[6]);
                    atoms[used].y= atof(tokens[7]);
                    atoms[used].z= atof(tokens[8]);
                    atoms[used].chain= strdup(tokens[4]);
                    atoms[used].atom_name= strdup(tokens[2]);
                    atoms[used++].res_name= strdup(tokens[3]);
                }
            }
        }
    }
    atoms = realloc(atoms, used * sizeof(struct pose));
    printf("%zu\n", used);
    int k=0;
    char *map[][2] = {{"d","f"}};

    for(i=0; i<used; i++){
        for(j=0; j<used; j++){
            if(strcmp(atoms[i].chain, atoms[j].chain)){
            //     if((!strcmp(atoms[i].atom_name, "NH1") || !strcmp(atoms[i].atom_name, "NH2") ||
            //         !strcmp(atoms[i].atom_name, "NE") || !strcmp(atoms[i].atom_name, "NZ")) &&
            //         (!strcmp(atoms[j].atom_name, "OD1") || !strcmp(atoms[j].atom_name, "OD2") || 
            //         !strcmp(atoms[j].atom_name, "OE1") || !strcmp(atoms[j].atom_name, "OE2"))){
            //         x = atoms[i].x; y= atoms[i].y; z=atoms[i].z;
            //         x1 = atoms[j].x; y1= atoms[j].y; z1=atoms[j].z;
            //         dist = sqrt(pow(x-x1,2) + pow(y-y1,2) + pow(z-z1,2));
            //         if(dist<=4.0){
            //             k++;
            //             printf("%s%d   %s%d\n", atoms[i].chain, atoms[i].atom, atoms[j].chain, atoms[j].atom);
            //         }
            //     }
            }
        }
    }

printf("%d\n", k);
    return EXIT_SUCCESS;
}
