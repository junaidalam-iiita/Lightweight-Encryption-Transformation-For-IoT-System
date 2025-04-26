# Makefile for LCM_IBBE

# Output executables
all: setup keygen encrypt decrypt

# Compilation rules
setup:
	@echo "Compiling the setup algorithm..."
	gcc -o setup lcm_ibbe_setup.c -lgmp -lpbc -lcrypto

keygen:
	@echo "Compiling the keygen algorithm..."
	gcc -o keygen lcm_ibbe_keygen.c -lgmp -lpbc -lcrypto

encrypt:
	@echo "Compiling the encryption algorithm..."
	gcc -o encrypt lcm_ibbe_encrypt.c -lgmp -lpbc -lcrypto

decrypt:
	@echo "Compiling the decryption algorithm..."
	gcc -o decrypt lcm_ibbe_decrypt.c -lgmp -lpbc -lcrypto
	
runAll: runSetup runKeyGen runEncrypt runDecrypt

runSetup:
	./setup 
	
runKeyGen:
	./keygen MSK.bin ibbeparams.bin mcl2023005@iiita.ac.in
	
runEncrypt:
	./encrypt input.jpg ibbeparams.bin mcl2023005@iiita.ac.in
	
runDecrypt:
	./decrypt encrypted_key.bin private_key.bin ibbeparams.bin encrypted_file.bin

# Clean up build artifacts and output files
clean:
	@echo "Removing all executable and output files..."
	rm -f setup keygen encrypt decrypt MSK.bin ibbeparams.bin private_key.bin encrypted_file.bin output.jpeg encrypted_key.bin

