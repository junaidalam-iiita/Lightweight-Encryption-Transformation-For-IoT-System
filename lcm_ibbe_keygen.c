#include "LCM_IBBE_header.h"

int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Error: Invalid command. Usage: ./prog pairing_params msk_file ibbeparams_file ID\n");
        exit(EXIT_FAILURE);
    }

    // Initialize PBC and variables
    myPBC_Initialize();

    FILE *msk_file = fopen(argv[1], "rb");
    FILE *ibbeparams_file = fopen(argv[2], "rb");
    FILE *priv_key_file = fopen("private_key.bin", "wb");

    if (!msk_file || !ibbeparams_file || !priv_key_file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char *ID = argv[3];
    element_t alpha, SK_ID, g;
    element_init_Zr(alpha, global_params);
    element_init_G1(g, global_params);
    element_init_G1(SK_ID, global_params);

    // Read MSK and public parameter g
    read_element_from_file(alpha, msk_file);
    read_element_from_file(g, ibbeparams_file);
    
    element_printf("setup_vals.alpha = %B\n", alpha);
    element_printf("setup_vals.ibbeparams.g = %B\n", g);

    // Generate private key
    lcm_ibbe_keygen(SK_ID, alpha, ID, g);
    save_element_to_file(SK_ID, priv_key_file);
    element_printf("SK_ID = %B\n", SK_ID);

    // Close files and clear elements
    fclose(msk_file); 
    fclose(priv_key_file);
    fclose(ibbeparams_file);
    element_clear(alpha); 
    element_clear(SK_ID); 
    element_clear(g);
    pairing_clear(global_params);

    return 0;
}
