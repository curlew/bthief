<div align="center">

# bthief

**A web browser credential thief for Windows.**

[![GitHub Workflow status](https://img.shields.io/github/actions/workflow/status/curlew/bthief/cmake.yml?logo=github&style=flat-square)](https://github.com/curlew/bthief/actions/workflows/cmake.yml)
[![Codacy grade](https://img.shields.io/codacy/grade/3b387ef9c9b842439e788913ad792943?logo=codacy&style=flat-square)](https://app.codacy.com/gh/curlew/bthief/dashboard)
[![Latest tag](https://img.shields.io/github/v/tag/curlew/bthief?color=informational&logo=github&style=flat-square)](https://github.com/curlew/bthief/tags)
[![License](https://img.shields.io/github/license/curlew/bthief?color=informational&style=flat-square)](https://github.com/curlew/bthief/blob/main/LICENSE)

</div>

## Overview

bthief is a web browser credential harvester for Windows, written in C++. It supports Chrome, Chromium, Opera, Brave, Edge, and Firefox.

## Building
**Requirements**: Visual C++ Build Tools and CMake must be installed.

```
cmake -B build
cmake --build build --config Release
```

## Usage
```
PS C:\Users\User\bthief> .\build\Release\bthief.exe
[Chrome] FOUND:
  - https://login.blockchain.com
    - Username: [someone@example.com]
    - Password: [Password!]
    - Created: 2022-02-21 17:26:10 UTC
    - Last used: 2022-02-21 17:26:07 UTC
    - Password last modified: 2022-02-21 17:26:10 UTC
[Chrome Beta] NOT FOUND
[Chrome Canary] NOT FOUND
[Chromium] NOT FOUND
[Edge] FOUND:
  - https://www.dropbox.com/login
    - Username: [username]
    - Password: [password321]
    - Created: 2023-07-25 03:51:40 UTC
    - Last used: 2023-07-25 03:51:36 UTC
    - Password last modified: 2023-07-25 03:51:40 UTC
  - http://10.0.0.2
    - Username: [38476682]
    - Password: [123456]
    - Created: 2023-04-12 20:16:49 UTC
    - Last used: never
    - Password last modified: 2023-04-12 20:16:49 UTC
[Brave] FOUND:
  - https://site.com/
    - Username: [username]
    - Password: [qwerty123]
    - Created: 2024-01-28 23:38:26 UTC
    - Last used: never
    - Password last modified: 2024-01-28 23:38:26 UTC
  - https://accounts.google.com/v3/signin/challenge/pwd
    - Username: [person@example.com]
    - Password: [password123]
    - Created: 2024-01-28 23:43:18 UTC
    - Last used: 2024-01-28 23:43:13 UTC
    - Password last modified: 2024-01-28 23:43:18 UTC
[Opera] NOT FOUND
[Firefox] NOT FOUND
```

## Disclaimer
bthief is intended for educational purposes only. Do not use it illegally.

## Acknowledgements

### Contributors
- [@zedek1](https://github.com/zedek1)

### Dependencies
(These are handled automatically by CMake)
- [Windows Implementation Libraries](https://github.com/microsoft/wil)
- [nlohmann/json](https://github.com/nlohmann/json)
- [SQLite](https://www.sqlite.org/index.html)
