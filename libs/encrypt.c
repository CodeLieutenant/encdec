#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include <sodium/crypto_pwhash.h>

#include "../include/common.h"
#include "../include/encrypt.h"
#include "../include/log.h"

#define ERROR_SODIUM_INIT (-1)

#define CRYPTO_INIT_PUSH crypto_secretstream_xchacha20poly1305_init_push
#define CRYPTO_INIT_PULL crypto_secretstream_xchacha20poly1305_init_pull
#define CRYPTO_PUSH crypto_secretstream_xchacha20poly1305_push
#define CRYPTO_PULL crypto_secretstream_xchacha20poly1305_pull
#define CRYPTO_TAG_FINAL crypto_secretstream_xchacha20poly1305_TAG_FINAL
#define CRYPTO_HEADER_BYTES crypto_secretstream_xchacha20poly1305_HEADERBYTES

typedef crypto_secretstream_xchacha20poly1305_state state;
typedef int32_t(*read_callback)(state* st,
	FILE* out_file,
	byte* buffer,
	byte* out,
	const uint64_t read,
	const int32_t tag,
	const int32_t is_eof);

static inline int32_t
open_files(
	FILE** in_file,
	FILE** out_file,
	const char* const input_file_name,
	const char* const output_file_name)
{
#ifdef _MSC_VER
	if (ENCDEC_SUCCESS != fopen_s(in_file, input_file_name, "rb+"))
	{
		log_debug("Error while opening: %s", input_file_name);
		return ERROR_FILE_OPEN;
	}

	if (ENCDEC_SUCCESS != fopen_s(out_file, output_file_name, "wb+"))
	{
		log_debug("Error while opening: %s", output_file_name);
		return ERROR_FILE_OPEN;
	}

#else
	* in_file = fopen(input_file_name, "rb+");
	if (!*in_file) {
		log_debug("Error while opening: %s", input_file_name);
		return ERROR_FILE_OPEN;
	}

	*out_file = fopen(output_file_name, "wb+");
	if (!*out_file) {
		log_debug("Error while opening: %s", output_file_name);
		return ERROR_FILE_OPEN;
	}

#endif
	return ENCDEC_SUCCESS;

}


static inline int32_t
sodium_start(
	byte* const key,
	byte* const salt,
	const char* const passphrase)
{
	int32_t status = sodium_init();

	if (ERROR_SODIUM_INIT == status) {
		log_debug("Error while setting up libsodium");
		return status;
	}

	status = password_derivation(key, salt, passphrase);

	if (ENCDEC_SUCCESS != status) {
		log_debug("Error while generating key, %s");
		return ERROR_PASSWORD_DERIVATION_FAILED;
	}

	return ENCDEC_SUCCESS;
}

static inline int32_t
encrypt(
	state* st,
	FILE* out_file,
	byte* buffer,
	byte* out,
	const uint64_t read,
	const int32_t tag,
	const int32_t is_eof)
{
	unsigned long long out_len;
	if (ENCDEC_SUCCESS != CRYPTO_PUSH(st, out, &out_len, buffer, read, NULL, 0, tag)) {
		return ERROR_XCHACHA20_ENCRYPTION;
	}
	fwrite(out, sizeof(byte), (size_t)out_len, out_file);
	fflush(out_file);
	return ENCDEC_SUCCESS;
}

static inline int32_t
decrypt(
	state* st,
	FILE* out_file,
	byte* buffer,
	byte* out,
	const uint64_t read,
	const int32_t tag,
	const int32_t is_eof)
{
	int32_t status = ENCDEC_SUCCESS;
	uint8_t t;
	unsigned long long out_len;
	status = CRYPTO_PULL(st, out, &out_len, &t, buffer, read, NULL, 0);
	if (0 != status) {
		log_debug("Decryption failed, stream cipher failed, %d: INVALID HEADER",
			status);
		return ERROR_XCHACHA20_DECRYPTION;
	}

	if (t == CRYPTO_TAG_FINAL && !is_eof) {
		log_debug("Decryption failed, end of file not reached");
		return ERROR_PREMATURE_ENDING;
	}

	fwrite(out, sizeof(byte), (size_t)out_len, out_file);
	fflush(out_file);
	return status;
}


int32_t
encrypt_file_password(
	const char* const file,
	const char* const out_file,
	const char* const passphrase)
{
	state st;
	FILE* in_file = NULL, * out_file_p = NULL;
	int32_t is_eof;
	byte
		salt[FILE_SALT_BYTES_LENGTH],
		buffer[READ_BUFFER_SIZE],
		enc[ENCRYPTED_LENGTH_BUFFER],
		header[CRYPTO_HEADER_BYTES],
		key[DERIVED_PASSPHRASE_LENGTH];

	generate_salt(salt);

	int32_t status = open_files(&in_file, &out_file_p, file, out_file);

	if (ENCDEC_SUCCESS != status)
	{
		return status;
	}

	status = sodium_start(key, salt, passphrase);

	if (ENCDEC_SUCCESS != status) {
		return status;
	}

	status = CRYPTO_INIT_PUSH(&st, header, key);

	if (ENCDEC_SUCCESS != status) {
		log_debug("Error while encrypting");
		return ERROR_XCHACHA20_INVALID_HEADER;
	}

	// Metadata
	fwrite(salt, sizeof(byte), FILE_SALT_BYTES_LENGTH, out_file_p);
	fwrite(header, sizeof(byte), sizeof header, out_file_p);
	fflush(out_file_p);


	do {
		const size_t read = fread(buffer, sizeof(byte), READ_BUFFER_SIZE, in_file);
		is_eof = feof(in_file);
		const int32_t tag = is_eof ? CRYPTO_TAG_FINAL : 0;
		status = encrypt(&st, out_file_p, buffer, enc, read, tag, is_eof);
		if (ENCDEC_SUCCESS != status) {
			return status;
		}
	} while (!is_eof);

	if (ENCDEC_SUCCESS != status) {
		return ERROR_XCHACHA20_ENCRYPTION;
	}

	fflush(out_file_p);
	fclose(in_file);
	fclose(out_file_p);
	return ENCDEC_SUCCESS;
}

int32_t
decrypt_file_password(const char* const file,
	const char* const out_file,
	const char* const passphrase,
	const output o)
{
	FILE* in_file, * out_file_p;
	int32_t is_eof;
	state st;
	byte
		salt[FILE_SALT_BYTES_LENGTH],
		buffer[ENCRYPTED_LENGTH_BUFFER],
		dec[READ_BUFFER_SIZE],
		header[CRYPTO_HEADER_BYTES],
		key[DERIVED_PASSPHRASE_LENGTH];
#define HEX_PRINT_LENGTH (READ_BUFFER_SIZE * 2 + 1)

	char* print = calloc(HEX_PRINT_LENGTH, sizeof(char));

	if (!print) {
		return ERROR_MEMORY_ALLOCATION;
	}

	int32_t status = open_files(&in_file, &out_file_p, file, out_file);

	if (ENCDEC_SUCCESS != status)
	{
		return status;
	}

	fread(salt, sizeof(byte), sizeof salt, in_file);
	fread(header, sizeof(byte), sizeof header, in_file);

	status = sodium_start(key, salt, passphrase);

	if (ENCDEC_SUCCESS != status) {
		return status;
	}

	status = CRYPTO_INIT_PULL(&st, header, key);

	if (0 != status) {
		log_debug("Error while initializing xchacha state");
		return ERROR_XCHACHA20_INVALID_HEADER;
	}


	do {
		const size_t read = fread(&buffer, sizeof(byte), ENCRYPTED_LENGTH_BUFFER, in_file);
		is_eof = feof(in_file);
		const int32_t tag = is_eof ? CRYPTO_TAG_FINAL : 0;
		status = decrypt(&st, out_file_p, buffer, dec, read, tag, is_eof);
		if (ENCDEC_SUCCESS != status) {
			return status;
		}

		if (o & HEX) {
			memset(print, 0, sizeof HEX_PRINT_LENGTH);
			bin2hex(print, HEX_PRINT_LENGTH, dec, read);
			hexdump(stdout, print);
		}

		if (o & ASCII) {
			printf("%s", (const char*)dec);
		}

	} while (!is_eof);

	if (0 != status) {
		log_debug("Error while decrypting");
		return ERROR_XCHACHA20_INVALID_HEADER;
	}

	free(print);
#undef HEX_PRINT_LENGTH
	return status;
}