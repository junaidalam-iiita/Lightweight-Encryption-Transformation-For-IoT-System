#include "LCM_IBBE_header.h"
int main(int argc, char **argv) {
    // Variable Declarations
    FILE *msk_file = fopen("MSK.bin", "wb");
    FILE *ibbeparams_file = fopen("ibbeparams.bin", "wb");
    if (!msk_file || !ibbeparams_file) {
        perror("Error creating output file");
        exit(EXIT_FAILURE);
    }

    int m = 5; // Max. number of receiver entities
    myPBC_Initialize(); // Initialize PBC and pairing
    SETUPVALS setup_vals = lcm_ibbe_setup(global_params, m); // Setup phase


    //  Printing the generated params
    
    element_printf("setup_vals.alpha = %B\n", setup_vals.alpha);
    element_printf("setup_vals.ibbeparams.g = %B\n", setup_vals.ibbeparams.g);
    element_printf("setup_vals.ibbeparams.g1 = %B\n", setup_vals.ibbeparams.g1);
    element_printf("setup_vals.ibbeparams.h = %B\n", setup_vals.ibbeparams.h);
    element_printf("setup_vals.ibbeparams.u = %B\n", setup_vals.ibbeparams.u);
    element_printf("setup_vals.ibbeparams.v = %B\n", setup_vals.ibbeparams.v);
    element_printf("setup_vals.ibbeparams.u_alpha = %B\n", setup_vals.ibbeparams.u_alpha);
    for (int i = 0; i < m; i++) 
        element_printf("setup_vals.ibbeparams.h_powers[%d] = %B\n", i, setup_vals.ibbeparams.h_powers[i]);
    // Save Master Secret Key and public parameters to files
    save_element_to_file(setup_vals.alpha, msk_file);
    save_element_to_file(setup_vals.ibbeparams.g, ibbeparams_file);
    save_element_to_file(setup_vals.ibbeparams.g1, ibbeparams_file);
    save_element_to_file(setup_vals.ibbeparams.h, ibbeparams_file);
    save_element_to_file(setup_vals.ibbeparams.u, ibbeparams_file);
    save_element_to_file(setup_vals.ibbeparams.v, ibbeparams_file);
    save_element_to_file(setup_vals.ibbeparams.u_alpha, ibbeparams_file); // Save u^alpha
    for (int i = 0; i < m; i++) 
        save_element_to_file(setup_vals.ibbeparams.h_powers[i], ibbeparams_file);

    // Clear memory and close files
    fclose(msk_file); 
    fclose(ibbeparams_file);
    element_clear(setup_vals.alpha);
    element_clear(setup_vals.ibbeparams.g);
    element_clear(setup_vals.ibbeparams.g1);
    element_clear(setup_vals.ibbeparams.u);
    element_clear(setup_vals.ibbeparams.h);
    element_clear(setup_vals.ibbeparams.u_alpha); // Clear u_alpha
    element_clear(setup_vals.ibbeparams.v);
    
    for (int i = 0; i < m; i++) 
        element_clear(setup_vals.ibbeparams.h_powers[i]);

    for (int i = 0; i < m - 1; i++) 
        element_clear(setup_vals.alpha_powers[i]);

    free(setup_vals.ibbeparams.h_powers);
    free(setup_vals.alpha_powers);
    pairing_clear(global_params);

    return 0;
}
