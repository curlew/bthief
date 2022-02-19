#include "crypto.hxx"

#include <windows.h>
#include <dpapi.h>

namespace {
static const int b64index[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  62, 63, 62, 62, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,
    0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18,
    19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63, 0,  26, 27, 28, 29, 30, 31, 32, 33,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
}

std::vector<uint8_t> dpapi_decrypt(const std::vector<uint8_t> &c) {
    DATA_BLOB ciphertext_blob, plaintext_blob;
    ciphertext_blob.cbData = c.size();
    ciphertext_blob.pbData = new BYTE[ciphertext_blob.cbData];

    std::memcpy(ciphertext_blob.pbData, c.data(), ciphertext_blob.cbData);

    if (!CryptUnprotectData(&ciphertext_blob, NULL, NULL, NULL, NULL, 0, &plaintext_blob)) {
        delete[] ciphertext_blob.pbData;
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> ret(plaintext_blob.pbData, plaintext_blob.pbData + plaintext_blob.cbData);

    delete[] ciphertext_blob.pbData;

    return ret;
}

// https://stackoverflow.com/a/41094722
std::vector<uint8_t> base64_decode(const std::string &buffer) {
    unsigned char *p = (unsigned char *)&buffer[0];
    size_t len = buffer.size();
    int pad = len > 0 && (len % 4 || p[len - 1] == '=');
    const size_t L = ((len + 3) / 4 - pad) * 4;
    std::vector<uint8_t> ret(L / 4 * 3 + pad, '\0');

    for (size_t i = 0, j = 0; i < L; i += 4) {
        int n = b64index[p[i]] << 18 | b64index[p[i + 1]] << 12 | b64index[p[i + 2]] << 6 | b64index[p[i + 3]];
        ret[j++] = n >> 16;
        ret[j++] = n >> 8 & 0xFF;
        ret[j++] = n & 0xFF;
    }
    if (pad) {
        int n = b64index[p[L]] << 18 | b64index[p[L + 1]] << 12;
        ret[ret.size() - 1] = n >> 16;

        if (len > L + 2 && p[L + 2] != '=') {
            n |= b64index[p[L + 2]] << 6;
            ret.push_back(n >> 8 & 0xFF);
        }
    }
    return ret;
}