#pragma once

#include <string>
#include <vector>

std::vector<uint8_t> dpapi_decrypt(const std::vector<uint8_t> &);
std::vector<uint8_t> base64_decode(const std::string &buffer);