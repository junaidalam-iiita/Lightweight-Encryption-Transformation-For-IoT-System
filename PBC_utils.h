#include <pbc/pbc.h>
#include <pbc/pbc_test.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

pairing_t global_params; // PBC Pairing Parameters - initialized in main()

/* Error Handling */
void handleErrors(const char *errpoint) {
    fprintf(stderr, "Error at: %s\n", errpoint);
    ERR_print_errors_fp(stderr);
    exit(1);
}

/* PBC Initialization */

void myPBC_Initialize(){
/* The  'pbc_demo_pairing_init()' function was designed to take the PBC params file-path from the command line argument. Thus by-default, it expects the 2nd and its 3rd arguments as
'argc' and 'argv'. It then checks whether 'argc' is greater than 1 or not (i.e., at-least one command line argument is given) and
then takes the 'argv[1]' as the params-file-path.
Now, since we are passing the params-file-path hard-coded from the main() function by calling this function - we are mimicking the 'argv' with an array called 'arr' where,
we place the  params-file-path as it's 2nd item. */
	FILE * parampath_file;
	char ** arr = NULL;
	char param_path[1000];
    	int size = 0;  // Number of elements in the array
   	int capacity = 0;  // Capacity of the array
   	
   	/* Read the params file path from 'parampath.txt' */
	parampath_file = fopen("parampath.txt", "r");
	if(parampath_file == NULL){
	   printf("Error: Couldn't open 'parampath.txt' file!\n");
	   exit(1);
	}
	fgets(param_path, 1000, parampath_file);
	size_t len = strlen(param_path);
        if (len > 0 && param_path[len - 1] == '\n') {
            param_path[len - 1] = '\0';
        }
	
	// Function to add a string to the array
	    void addString(char *newString) {
            
			if (size == capacity) {
			    // Increase the capacity of the array
			    capacity = (capacity == 0) ? 1 : capacity * 2;
			    arr = realloc(arr, capacity * sizeof(char *));
			}
			// Allocate memory for the new string and copy it to the array
			arr[size] = malloc(strlen(newString) + 1);
			strcpy(arr[size], newString);
			size++;
		}

    	// Add the given param_path as the 2nd item in the array
    	addString(" ");
    	addString(param_path);
    	
    	//Initialize the PBC Libraray Global Parameters as per the specified PBC-params-file
    	pbc_demo_pairing_init(global_params, 2, arr);  // Note: the 2nd argument we manually setting to '2' as it mimicks 'argc' which should be greaer than '1'
    	fclose(parampath_file);
}

// void myPBC_Initialize(char *arg1) {   
//     FILE *param_file = fopen(arg1, "r");
    
//     // Initialize PBC parameters using command line
//     char *arr[2] = {" ", arg1};
//     pbc_demo_pairing_init(global_params, 2, arr);
    
//     fclose(param_file);
// }

/* Save Element to File */
void save_element_to_file(element_t X, FILE *fptr) {
    size_t element_size = element_length_in_bytes(X);
    unsigned char *element_bin = malloc(element_size);
    element_to_bytes(element_bin, X);
    
    if (fwrite(element_bin, 1, element_size, fptr) != element_size) {
        handleErrors("Saving Element");
    }
    free(element_bin);
}

/* Read Element from File */
void read_element_from_file(element_t X, FILE *fptr) {
    size_t element_size = element_length_in_bytes(X);
    unsigned char *element_bin = malloc(element_size);

    if (fread(element_bin, 1, element_size, fptr) != element_size) {
        handleErrors("Reading Element");
    }
    element_from_bytes(X, element_bin);
    free(element_bin);
}
