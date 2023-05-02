<div align="center">

# bthief

**A Chrome and Firefox credential thief for Windows.**

[![GitHub Workflow status](https://img.shields.io/github/actions/workflow/status/curlew/bthief/cmake.yml?logo=github&style=flat-square)](https://github.com/curlew/bthief/actions/workflows/cmake.yml)
[![Codacy grade](https://img.shields.io/codacy/grade/3b387ef9c9b842439e788913ad792943?logo=codacy&style=flat-square)](https://app.codacy.com/gh/curlew/bthief/dashboard)
[![Latest tag](https://img.shields.io/github/v/tag/curlew/bthief?color=informational&logo=github&style=flat-square)](https://github.com/curlew/bthief/tags)
[![License](https://img.shields.io/github/license/curlew/bthief?color=informational&style=flat-square)](https://github.com/curlew/bthief/blob/main/LICENSE)

</div>

## Building
**Requirements**: Visual C++ Build Tools and CMake must be installed.

```
cmake -B build
cmake --build build --config Release
```

## Usage
```
PS C:\Users\User\bthief> .\build\Release\bthief.exe
##############################  Google Chrome  ##############################

  - https://login.blockchain.com
    - Username: [someone@example.com]
    - Password: [Password123]
    - Created: 2022-02-21 17:26:10 UTC
    - Last used: 2022-02-21 17:26:07 UTC
    - Password last modified: 2022-02-21 17:26:10 UTC

  - https://www.dropbox.com/login
    - Username: [someone@example.com]
    - Password: [Passw0rd!]
    - Created: 2022-03-28 21:08:12 UTC
    - Last used: 2022-03-28 21:07:58 UTC
    - Password last modified: 2022-03-28 21:08:12 UTC

##############################  Firefox  ##############################

  - https://stackoverflow.com
    - Username: [someone@example.com]
    - Password: [qwerty123]
    - Created: 2023-03-21 19:33:54 UTC
    - Last used: 2023-03-21 19:33:54 UTC
    - Password last modified: 2023-03-21 19:33:54 UTC
```

## Acknowledgements

### Contributors
- [@zedek1](https://github.com/zedek1)

### Dependencies
(These are handled automatically by CMake)
- [Windows Implementation Libraries](https://github.com/microsoft/wil)
- [nlohmann/json](https://github.com/nlohmann/json)
- [SQLite](https://www.sqlite.org/index.html)
