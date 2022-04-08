#pragma once

#include <string>
#include <vector>
#include <windows.h> // needed before bcrypt
#include <bcrypt.h>
#include <wil/resource.h>

wil::unique_bcrypt_key bcrypt_import_key_blob(const wil::unique_bcrypt_algorithm &,
                                              const std::vector<uint8_t> &);
std::vector<uint8_t> dpapi_decrypt(const std::vector<uint8_t> &);
std::vector<uint8_t> base64_decode(const std::string &buffer);