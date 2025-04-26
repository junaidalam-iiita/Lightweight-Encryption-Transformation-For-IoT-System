#include "PBC_utils.h"

#define IBE_MSG_STR_LEN EVP_MAX_KEY_LENGTH 

#define MAX_LINE_LENGTH 1024
#define ELEMENT_SIZE 64  // Define the size of each element

// Data-structures for IBBE
typedef struct {
    element_t h, u, v, g, g1, u_alpha;   
    element_t *h_powers;     
} IBBEPARAMS;

typedef struct {
    element_t alpha; 
    element_t *alpha_powers;          
    IBBEPARAMS ibbeparams;
} SETUPVALS;

// Hash functions
void H1(element_t Q, char *str) {
    element_from_hash(Q, str, strlen(str));
}

void H2(unsigned char *outstr, int outstr_len, unsigned char *instr, int instr_len) {
    int i, len = instr_len + 1, k = 0; char c = 'A';
    unsigned char *buffer = malloc(SHA256_DIGEST_LENGTH), *input_to_SHA = malloc(len);

    while (k < outstr_len) {
        memcpy(input_to_SHA, instr, instr_len); 

        input_to_SHA[len-1] = c++;

        SHA256(input_to_SHA, len, buffer);
        
        for (i = 0; i < SHA256_DIGEST_LENGTH && k < outstr_len; i++) 
            outstr[k++] = buffer[i];
    }

    free(buffer); 
    free(input_to_SHA);
}

void H3(unsigned char *ssnkey, element_t X) {
    int len = element_length_in_bytes(X);
    unsigned char *hashed = pbc_malloc(len); 

    element_to_bytes(hashed, X);

    H2(ssnkey, IBE_MSG_STR_LEN, hashed, len); 
    
    pbc_free(hashed);
}

// AES-256 Encryption/Decryption
void MyAES_256_ECB_Encr(FILE *input_file, unsigned char *key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char buffer[1024], ciphertext[1024 + EVP_MAX_BLOCK_LENGTH]; 
    int bytes_read, len;
    FILE *output_file = fopen("encrypted_file.bin", "wb");

    EVP_EncryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, key, NULL);

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), input_file)) > 0) {
        EVP_EncryptUpdate(ctx, ciphertext, &len, buffer, bytes_read);
        fwrite(ciphertext, 1, len, output_file);
    }

    EVP_EncryptFinal_ex(ctx, ciphertext, &len);
    fwrite(ciphertext, 1, len, output_file);
    EVP_CIPHER_CTX_free(ctx); 
    fclose(output_file);
}

void MyAES_256_ECB_Decr(FILE *cipher_file, unsigned char *key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    unsigned char buffer[1024], plaintext[1024 + EVP_MAX_BLOCK_LENGTH]; 
    int bytes_read, len;
    FILE *output_file = fopen("output.jpg", "wb");

    EVP_DecryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, key, NULL);

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), cipher_file)) > 0) {
        EVP_DecryptUpdate(ctx, plaintext, &len, buffer, bytes_read);
        fwrite(plaintext, 1, len, output_file);
    }

    EVP_DecryptFinal_ex(ctx, plaintext, &len);
    fwrite(plaintext, 1, len, output_file);
    EVP_CIPHER_CTX_free(ctx); 
    fclose(output_file);
}

// Setup, Keygen, Encrypt, Decrypt
SETUPVALS lcm_ibbe_setup(pairing_t pairing, int m) {
    SETUPVALS setup_vals; IBBEPARAMS *params = &setup_vals.ibbeparams; int i;
    element_init_G1(params->g, pairing); 
    element_init_G1(params->h, pairing);
    element_init_G1(params->u, pairing); 
    element_init_G1(params->g1, pairing);
    element_init_G1(params->u_alpha, pairing); 
    element_init_GT(params->v, pairing);
    element_init_Zr(setup_vals.alpha, pairing);

    params->h_powers = malloc(m * sizeof(element_t)); 

    setup_vals.alpha_powers = malloc((m - 1) * sizeof(element_t));
    for (i = 0; i < m; i++) 
        element_init_G1(params->h_powers[i], pairing);
    
    for (i = 0; i < m - 1; i++) 
        element_init_Zr(setup_vals.alpha_powers[i], pairing);

    element_random(params->g); 
    element_random(params->h); 
    element_random(params->u);
    element_random(setup_vals.alpha);
    element_pow_zn(params->g1, params->g, setup_vals.alpha); 
    
    element_pairing(params->v, params->g, params->h);
    element_pow_zn(params->u_alpha, params->u, setup_vals.alpha);
    
    element_mul(setup_vals.alpha_powers[0], setup_vals.alpha, setup_vals.alpha);
    for (i = 1; i < m-1; i++) 
        element_mul(setup_vals.alpha_powers[i], setup_vals.alpha_powers[i-1], setup_vals.alpha);

    element_pow_zn(params->h_powers[0], params->h, setup_vals.alpha);

    for (i = 1; i < m; i++) 
        element_pow_zn(params->h_powers[i], params->h, setup_vals.alpha_powers[i-1]);
    
    
    return setup_vals;
}

void lcm_ibbe_keygen(element_t SK_ID, element_t alpha, char *ID, element_t g) {
    element_t hash_val, denom, inv_denom;

    element_init_Zr(hash_val, global_params); 
    element_init_Zr(denom, global_params);
    element_init_Zr(inv_denom, global_params); 
    
    H1(hash_val, ID);

    element_add(denom, alpha, hash_val); 
    element_invert(inv_denom, denom);
    element_pow_zn(SK_ID, g, inv_denom); 


    element_clear(hash_val); 
    element_clear(denom); 
    element_clear(inv_denom);
}

void lcm_ibbe_encrypt(char *msg_file, char *ID, IBBEPARAMS ibbeparams) {
    element_t s, C0, C1, C2, temp, u_H0_ID, h_H0_ID, hash_val, k_element;
    
    element_init_GT(C0, global_params);
    element_init_G1(C1, global_params); 
    element_init_G1(C2, global_params);
    element_init_Zr(s, global_params);
    element_init_GT(temp, global_params);
    element_init_G1(u_H0_ID, global_params);
    element_init_G1(h_H0_ID, global_params);
    element_init_Zr(hash_val, global_params);
    element_init_GT(k_element, global_params);
    
    unsigned char* ssnkey = malloc(IBE_MSG_STR_LEN);    
   
    element_random(k_element);
    element_random(s);
    
    H3(ssnkey, k_element);
    
    H1(hash_val, ID);
    
    element_pow_zn(temp, ibbeparams.v, s);
    element_mul(C0, k_element, temp);
    
    element_pow_zn(h_H0_ID, ibbeparams.h, hash_val);
    element_mul(C1, ibbeparams.h_powers[0], h_H0_ID);
    element_pow_zn(C1, C1, s);
    
    element_pow_zn(u_H0_ID, ibbeparams.u, hash_val); 
    element_mul(C2, ibbeparams.u_alpha, u_H0_ID);
    element_pow_zn(C2, C2, s);
    
    FILE *msg_fp = fopen(msg_file, "rb"), *cipher_file = fopen("encrypted_key.bin", "wb");
    
    save_element_to_file(C0, cipher_file);
    save_element_to_file(C1, cipher_file);
    save_element_to_file(C2, cipher_file);
    
    MyAES_256_ECB_Encr(msg_fp, ssnkey);
    
    fclose(msg_fp);
    fclose(cipher_file);
    
    element_clear(temp);
    element_clear(u_H0_ID);
    element_clear(h_H0_ID);
    element_clear(hash_val);
    element_clear(k_element);
    element_clear(s);
    element_clear(C0);
    element_clear(C1);
    element_clear(C2);
    free(ssnkey);
}

void lcm_ibbe_decrypt(element_t SK_ID, element_t C0, element_t C1, FILE *encrypted_file, IBBEPARAMS *ibbeparams) {
    element_t k, temp, temp1;
    
    unsigned char* ssnkey = malloc(IBE_MSG_STR_LEN);
    
    element_init_GT(k, global_params);
    element_init_GT(temp, global_params);
    element_init_GT(temp1, global_params);
    
    element_pairing(temp, SK_ID, C1);
    element_invert(temp1, temp);
    element_mul(k, C0, temp1);
    
    H3(ssnkey, k);
    
    MyAES_256_ECB_Decr(encrypted_file, ssnkey);
    
    element_clear(k);
    element_clear(temp);
    element_clear(temp1);
    free(ssnkey);
}

void autherization(element_t SK_ID, element_t u, element_t h, char * ID, char * filename, char * NSname){

    element_printf("SK_ID = %B\n", SK_ID);
    element_printf("u = %B\n", u);
    element_printf("h = %B\n", h);
    
    printf("ID : %s\n", ID);
    
    FILE * NSkey_file = fopen(NSname, "rb");
    if (NSkey_file == NULL) {
        perror("Error opening files");
        return;
    }
   
    
    //const char * filename = argv[5];
    FILE *nameFile = fopen(filename, "r");
    if (nameFile == NULL) {
        perror("Error opening file");
        return;
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
        return;
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
    }
    
    for (i = 0; i < neighbours_count; i++) {
        printf("Block %zu: ", i + 1);
        for (j = 0; j < ELEMENT_SIZE; j++) {
            printf("%02x ", buffer[i * ELEMENT_SIZE + j]);
        }
        printf("\n");
    }
	
}
