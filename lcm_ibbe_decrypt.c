#include "LCM_IBBE_header.h"

int main(int argc, char **argv) {
    if (argc < 5) {
        printf("Usage: %s <cipher_file> <priv_key_file> <params_file> <enc_file>\n", argv[0]);
        return 1;
    }

    myPBC_Initialize();

    FILE *cipher_file = fopen(argv[1], "rb");
    FILE *privt_key_file = fopen(argv[2], "rb");
    FILE *ibparams_file = fopen(argv[3], "rb");
    FILE *encrypted_file = fopen(argv[4], "rb");

    if (!cipher_file || !privt_key_file || !ibparams_file || !encrypted_file) {
        perror("Error opening files");
        return 1;
    }

    SETUPVALS setupvals;
    element_t SK_ID, C0, C1;

    element_init_G1(SK_ID, global_params);
    element_init_GT(C0, global_params);
    element_init_G1(C1, global_params);

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
    read_element_from_file(C0, cipher_file);
    read_element_from_file(C1, cipher_file);

    lcm_ibbe_decrypt(SK_ID, C0, C1, encrypted_file, &setupvals.ibbeparams);

    fclose(cipher_file);
    fclose(privt_key_file); 
    fclose(ibparams_file); 
    fclose(encrypted_file);

    element_clear(SK_ID); 
    element_clear(C0); 
    element_clear(C1);
    element_clear(setupvals.ibbeparams.g); 
    element_clear(setupvals.ibbeparams.g1);
    element_clear(setupvals.ibbeparams.h); 
    element_clear(setupvals.ibbeparams.u);
    element_clear(setupvals.ibbeparams.v); 
    element_clear(setupvals.ibbeparams.u_alpha);

    pairing_clear(global_params);
    return 0;
}
