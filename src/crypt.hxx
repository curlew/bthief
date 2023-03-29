/**
 * @file crypt.hxx
 * @brief Common cryptographic functions.
 */

#ifndef BTHIEF_CRYPT_HXX
#define BTHIEF_CRYPT_HXX

#include <string>
#include <vector>
#include <windows.h>
#include <bcrypt.h>
#include <wil/resource.h>

/// Decode Base64.
std::vector<uint8_t> b64_decode(const std::string &);

/// Decrypt data using the Data Protection API.
std::vector<uint8_t> dpapi_decrypt(const std::vector<uint8_t> &);

/// Import a symmetric key from a key blob.
wil::unique_bcrypt_key import_key_data(const wil::unique_bcrypt_algorithm &,
                                       const std::vector<uint8_t> &);

#endif // ifndef BTHIEF_CRYPT_HXX
