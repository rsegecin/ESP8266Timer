#include "MD5.h"

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

void MD5Class::byte_to_hex_string(char * hexStrParam, unsigned int hexStrLength, unsigned char * byteArrayParam)
{
	unsigned char num;

	for (int i = 0, u = 0; i < (hexStrLength - 1) / 2; i++, u++)
	{
		num = byteArrayParam[i] >> 4;
		hexStrParam[u] = (char)pgm_read_byte(HEX_VALUES + num);
		num = byteArrayParam[i] & 0xf;
		hexStrParam[++u] = (char)pgm_read_byte(HEX_VALUES + num);
	}
}

void MD5Class::to_bytes(unsigned long int val, unsigned char *bytes)
{
	bytes[0] = (unsigned char)val;
	bytes[1] = (unsigned char)(val >> 8);
	bytes[2] = (unsigned char)(val >> 16);
	bytes[3] = (unsigned char)(val >> 24);
}

unsigned long int MD5Class::to_int32(unsigned char *bytes)
{
	return (unsigned long int) bytes[0] | ((unsigned long int) bytes[1] << 8) | ((unsigned long int) bytes[2] << 16) | ((unsigned long int) bytes[3] << 24);
}

void MD5Class::md5(unsigned char *initial_msg, size_t initial_len, unsigned char *digest)
{
	// These vars will contain the hash
	unsigned long int h0, h1, h2, h3;

	uint32_t num32;
	uint8_t num8;

	// Message (to prepare)
	unsigned char *msg = NULL;

	size_t new_len, offset;
	unsigned long int w[16];
	unsigned long int a, b, c, d, i, f, g, temp;

	// Initialize variables - simple count in nibbles:
	h0 = 0x67452301;
	h1 = 0xefcdab89;
	h2 = 0x98badcfe;
	h3 = 0x10325476;

	//Pre-processing:
	//append "1" bit to message    
	//append "0" bits until message length in bits ≡ 448 (mod 512)
	//append length mod (2^64) to message

	for (new_len = initial_len + 1; new_len % (512 / 8) != 448 / 8; new_len++);

	msg = (unsigned char *)malloc(new_len + 8);
	memcpy(msg, initial_msg, initial_len);
	msg[initial_len] = 0x80; // append the "1" bit; most significant bit is "first"
	for (offset = initial_len + 1; offset < new_len; offset++)
		msg[offset] = 0; // append "0" bits

						 // append the len in bits at the end of the lBuffer.
	to_bytes(initial_len * 8, msg + new_len);
	// initial_len>>29 == initial_len*8>>32, but avoids overflow.
	to_bytes(initial_len >> 29, msg + new_len + 4);

	// Process the message in successive 512-bit chunks:
	//for each 512-bit chunk of message:
	for (offset = 0; offset<new_len; offset += (512 / 8))
	{

		// break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
		for (i = 0; i < 16; i++)
			w[i] = to_int32(msg + offset + i * 4);

		// Initialize hash value for this chunk:
		a = h0;
		b = h1;
		c = h2;
		d = h3;

		// Main loop:
		for (i = 0; i<64; i++)
		{

			if (i < 16)
			{
				f = (b & c) | ((~b) & d);
				g = i;
			}
			else if (i < 32)
			{
				f = (d & b) | ((~d) & c);
				g = (5 * i + 1) % 16;
			}
			else if (i < 48)
			{
				f = b ^ c ^ d;
				g = (3 * i + 5) % 16;
			}
			else
			{
				f = c ^ (b | (~d));
				g = (7 * i) % 16;
			}

			temp = d;
			d = c;
			c = b;

			num32 = pgm_read_dword(MD5_K + i);
			num8 = pgm_read_byte(MD5_R + i);

			b = b + LEFTROTATE((a + f + num32 + w[g]), num8);
			a = temp;

		}

		// Add this chunk's hash to result so far:
		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;
	}

	// cleanup
	free(msg);

	//var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
	to_bytes(h0, digest);
	to_bytes(h1, digest + 4);
	to_bytes(h2, digest + 8);
	to_bytes(h3, digest + 12);
}

/// hash must have at least size of 33
void MD5Class::MakeHash(char *hash, char *initial_msg)
{
	unsigned char hashInBytes[16];

	md5((unsigned char *)initial_msg, strlen((char *)initial_msg), hashInBytes);

	memset(hash, 0x00, 33);

	byte_to_hex_string(hash, 33, hashInBytes);
}

MD5Class MD5;