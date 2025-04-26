#include "LCM_IBBE_header.h"

int main(int argc, char **argv) {
    if (argc < 4) {
        printf("Usage: %s <message_file> <params_file> <ID>\n", argv[0]);
        return 1;
    }

    myPBC_Initialize();  // Initialize PBC

    FILE *msg_file = fopen(argv[1], "rb"), *params_file = fopen(argv[2], "rb");
    if (!msg_file || !params_file) {
        perror("Error opening input files");
        return 1;
    }

    char *ID = argv[3];
    int m = 5;  // Adjust as required
    IBBEPARAMS ibbeparams;

    element_init_G1(ibbeparams.g, global_params);
    element_init_G1(ibbeparams.g1, global_params);
    element_init_G1(ibbeparams.h, global_params);
    element_init_G1(ibbeparams.u, global_params);
    element_init_GT(ibbeparams.v, global_params);
    element_init_G1(ibbeparams.u_alpha, global_params);

    // Read IBBE params
    read_element_from_file(ibbeparams.g, params_file);
    read_element_from_file(ibbeparams.g1, params_file);
    read_element_from_file(ibbeparams.h, params_file);
    read_element_from_file(ibbeparams.u, params_file);
    read_element_from_file(ibbeparams.v, params_file);
    read_element_from_file(ibbeparams.u_alpha, params_file);

    // Allocate and read h_powers
    ibbeparams.h_powers = malloc(m * sizeof(element_t));

    for (int i = 0; i < m; i++) {
        element_init_G1(ibbeparams.h_powers[i], global_params);
        read_element_from_file(ibbeparams.h_powers[i], params_file);
    }

    // Perform encryption
    lcm_ibbe_encrypt(argv[1], ID, ibbeparams);

    // Cleanup
    fclose(msg_file); 
    fclose(params_file);

    for (int i = 0; i < m; i++) 
        element_clear(ibbeparams.h_powers[i]);

    free(ibbeparams.h_powers);
    element_clear(ibbeparams.g); 
    element_clear(ibbeparams.g1);
    element_clear(ibbeparams.h);
    element_clear(ibbeparams.u); 
    element_clear(ibbeparams.v); 
    element_clear(ibbeparams.u_alpha);
    pairing_clear(global_params);

    return 0;
}
