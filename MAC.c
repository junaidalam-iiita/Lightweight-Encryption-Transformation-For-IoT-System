#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/hmac.h>

#define KEY "your-secret-key"    // Define your secret key
#define KEY_LEN strlen(KEY)
#define DATA "your-data-message" // The data to be authenticated
#define DATA_LEN strlen(DATA)

int main() {
    unsigned char *result;
    unsigned int len = 0;

    // Calculate HMAC
    result = HMAC(EVP_sha256(), KEY, KEY_LEN, (unsigned char *)DATA, DATA_LEN, NULL, &len);
    
    // Check for result
    if (result == NULL) {
        fprintf(stderr, "Error calculating HMAC\n");
        return 1;
    }

    // Print the HMAC in hex format
    printf("HMAC: ");
    for (unsigned int i = 0; i < len; i++) {
        printf("%02x", result[i]);
    }
    printf("\n");

    return 0;
}

