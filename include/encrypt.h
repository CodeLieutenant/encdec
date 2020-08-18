#include <stdint.h>
#include <common.h>
/*
 * High resolution timing API
 */
#ifndef ENCDEC_ENCRYPT

#ifdef __cplusplus
extern "C"
{
#endif
#define ENCDEC_ENCRYPT


	int32_t encrypt_file_password(const char* const file,
		const char* const out_file,
		const char* const passphrase);

	int32_t decrypt_file_password(const char* const file,
		const char* const out_file,
		const char* const passphrase,
		output o);

#ifdef __cplusplus
}
#endif
#endif