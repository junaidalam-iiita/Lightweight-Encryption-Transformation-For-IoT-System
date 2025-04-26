#include "LCM_IBBE_header.h"

typedef struct {
    char data[ELEMENT_SIZE];  // Each element is an array of 64 bytes
} Element;

int main(int argc, char **argv) {
    if (argc < 5) {
        printf("Usage: %s <priv_key_file> <params_file> <ID>\n", argv[0]);
        return 1;
    }

    myPBC_Initialize();

    FILE *privt_key_file = fopen(argv[1], "rb");
    FILE *ibparams_file = fopen(argv[2], "rb");
    char *ID = argv[3];

    if (!privt_key_file || !ibparams_file) {
        perror("Error opening files");
        return 1;
    }
    
    SETUPVALS setupvals;
    element_t SK_ID;
    
    element_init_G1(SK_ID, global_params);
    element_init_G1(setupvals.ibbeparams.g, global_params);
    element_init_G1(setupvals.ibbeparams.g1, global_params);
    element_init_G1(setupvals.ibbeparams.h, global_params);
    element_init_G1(setupvals.ibbeparams.u, global_params);
    element_init_GT(setupvals.ibbeparams.v, global_params);
    element_init_G1(setupvals.ibbeparams.u_alpha, global_params);
    
    read_element_from_file(SK_ID, privt_key_file);
    read_element_from_file(setupvals.ibbeparams.g, ibparams_file);
    read_element_from_file(setupvals.ibbeparams.g1, ibparams_file);
    read_element_from_file(setupvals.ibbeparams.h, ibparams_file);
    read_element_from_file(setupvals.ibbeparams.u, ibparams_file);
    read_element_from_file(setupvals.ibbeparams.v, ibparams_file);
    read_element_from_file(setupvals.ibbeparams.u_alpha, ibparams_file);
    
    //main algo
    /*FILE * NSkey_file = fopen(argv[4], "rb");
    if (NSkey_file == NULL) {
        perror("Error opening files");
        return 1;
    }
   
    
    const char * filename = argv[5];
    FILE *nameFile = fopen(filename, "r");
    if (nameFile == NULL) {
        perror("Error opening file");
        return 1;
    }

    int neighbours_count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), nameFile)) {
        printf("%s", line);  // Print each line
        neighbours_count++;
    }
    
    
    printf("Count = %d\n\n", neighbours_count);  
    
    unsigned char *buffer;
    size_t i, j;

    // Dynamically allocate memory for n blocks of 64 bytes
    buffer = (unsigned char *)malloc(neighbours_count * ELEMENT_SIZE);
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    // Read the binary file in chunks of 64 bytes and store them in the buffer
    for (i = 0; i < neighbours_count; i++) {
        size_t bytesRead = fread(buffer + i * ELEMENT_SIZE, 1, ELEMENT_SIZE, NSkey_file);
        if (bytesRead < ELEMENT_SIZE) {
            if (feof(NSkey_file)) {
                printf("End of file reached after reading %zu blocks.\n", i);
                break;
            } else if (ferror(NSkey_file)) {
                perror("Error reading file");
                break;
            }
        }
    }*/
    
    autherization(SK_ID, setupvals.ibbeparams.u, setupvals.ibbeparams.h, ID, argv[5], argv[4]);

  

    // Print the buffer data in hexadecimal format
    /*printf("Buffer contents:\n");
    for (i = 0; i < neighbours_count; i++) {
        printf("Block %zu: ", i + 1);
        for (j = 0; j < ELEMENT_SIZE; j++) {
            printf("%02x ", buffer[i * ELEMENT_SIZE + j]);
        }
        printf("\n");
    }*/

    // Free the allocated memory
    //free(buffer);
    
    fclose(privt_key_file); 
    fclose(ibparams_file); 
    //fclose(nameFile);
    //fclose(NSkey_file);
    pairing_clear(global_params);
    return 0;
}
