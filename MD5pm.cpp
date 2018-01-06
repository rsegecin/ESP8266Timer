#include "MD5pm.h"

#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

void MD5pmClass::to_bytes(unsigned long int val, byte *bytes)
{
    bytes[0] = (unsigned char)val;
    bytes[1] = (unsigned char)(val >> 8);
    bytes[2] = (unsigned char)(val >> 16);
    bytes[3] = (unsigned char)(val >> 24);
}

unsigned long int MD5pmClass::to_int32(byte *bytes)
{
    return (unsigned long int) bytes[0] | ((unsigned long int) bytes[1] << 8) | ((unsigned long int) bytes[2] << 16) | ((unsigned long int) bytes[3] << 24);
}

void MD5pmClass::MakeMD5(byte *pData, size_t pDataLength, byte *pBuffer)
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

    for (new_len = pDataLength + 1; new_len % (512 / 8) != 448 / 8; new_len++);

    msg = (unsigned char *)malloc(new_len + 8);
    memcpy(msg, pData, pDataLength);
    msg[pDataLength] = 0x80; // append the "1" bit; most significant bit is "first"
    for (offset = pDataLength + 1; offset < new_len; offset++)
        msg[offset] = 0; // append "0" bits

                         // append the len in bits at the end of the lBuffer.
    to_bytes(pDataLength * 8, msg + new_len);
    // initial_len>>29 == initial_len*8>>32, but avoids overflow.
    to_bytes(pDataLength >> 29, msg + new_len + 4);

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
    to_bytes(h0, pBuffer);
    to_bytes(h1, pBuffer + 4);
    to_bytes(h2, pBuffer + 8);
    to_bytes(h3, pBuffer + 12);
}

MD5pmClass MD5pm;