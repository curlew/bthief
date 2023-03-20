#ifndef CRYPT_HXX_INCLUDED
#define CRYPT_HXX_INCLUDED

#include <string>
#include <vector>
#include <windows.h>
#include <bcrypt.h>
#include <wil/resource.h>

std::vector<uint8_t> b64_decode(const std::string &);
std::vector<uint8_t> dpapi_decrypt(const std::vector<uint8_t> &);
wil::unique_bcrypt_key import_key_data(const wil::unique_bcrypt_algorithm &,
                                       const std::vector<uint8_t> &);

#endif // ifndef CRYPT_HXX_INCLUDED
