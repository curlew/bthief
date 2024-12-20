<div align="center">

# bthief

**A web browser credential thief for Windows.**

[![GitHub Workflow status](https://img.shields.io/github/actions/workflow/status/curlew/bthief/cmake.yml?logo=github&style=flat-square)](https://github.com/curlew/bthief/actions/workflows/cmake.yml)
[![Codacy grade](https://img.shields.io/codacy/grade/3b387ef9c9b842439e788913ad792943?logo=codacy&style=flat-square)](https://app.codacy.com/gh/curlew/bthief/dashboard)
[![Latest tag](https://img.shields.io/github/v/tag/curlew/bthief?color=informational&logo=github&style=flat-square)](https://github.com/curlew/bthief/tags)
[![License](https://img.shields.io/github/license/curlew/bthief?color=informational&style=flat-square)](https://github.com/curlew/bthief/blob/main/LICENSE)

</div>

## Overview

bthief is a web browser credential harvester that supports Chrome, Chromium, Opera, Brave, Edge, and Firefox.
The Chrome Beta and Canary versions are also supported.
It retrieves usernames and passwords, how many times each login has been used, and when it was created, last used, and last modified.

## Building

**Requirements**: Visual C++ Build Tools and CMake must be installed.

[Dependencies](#dependencies) will be gathered automatically by CMake.

```
cmake -B build
cmake --build build --config Release
```

## Usage

Only installed browsers are shown.
JSON output can be enabled with the `-j` flag.

```
PS C:\Users\User\bthief> .\build\Release\bthief.exe
Chrome:
  - https://login.blockchain.com
    - Username: [someone@example.com]
    - Password: [Password!]
    - Times used: 2
    - Created: 2022-02-21 17:26:10 UTC
    - Last used: 2022-02-21 17:26:07 UTC
    - Password last modified: 2022-02-21 17:26:10 UTC

Edge:
  - https://www.dropbox.com/login
    - Username: [username]
    - Password: [password321]
    - Times used: 4
    - Created: 2023-07-25 03:51:40 UTC
    - Last used: 2023-07-25 03:51:36 UTC
    - Password last modified: 2023-07-25 03:51:40 UTC
  - http://10.0.0.2
    - Username: [38476682]
    - Password: [123456]
    - Times used: 0
    - Created: 2023-04-12 20:16:49 UTC
    - Last used: never
    - Password last modified: 2023-04-12 20:16:49 UTC

Brave:
  - https://accounts.google.com/v3/signin/challenge/pwd
    - Username: [person@example.com]
    - Password: [password123]
    - Times used: 6
    - Created: 2024-01-28 23:43:18 UTC
    - Last used: 2024-01-28 23:43:13 UTC
    - Password last modified: 2024-01-28 23:43:18 UTC
```

<details open>
<summary>Example output (JSON)</summary>

```json
{
    "Chrome Beta": [
        {
            "url": "https://www.netflix.com",
            "username": "user123@example.com",
            "password": "Pa$$w0rd!",
            "times_used": 1,
            "date_created": 1690257099,
            "date_last_used": 1690257095,
            "date_password_modified": 1690257099
        },
        {
            "url": "https://stackoverflow.com",
            "username": "someone@example.com",
            "password": "password123!",
            "times_used": 3,
            "date_created": 1681330609,
            "date_last_used": 1681330600,
            "date_password_modified": 1681330609
        },
    ],
    "Opera": [
        {
            "url": "https://www.dropbox.com/login",
            "username": "user@gmail.com",
            "password": "MyPassword1!",
            "times_used": 0,
            "date_created": 1733288709,
            "date_last_used": 0,
            "date_password_modified": 1733288709
        }
    ],
    "Firefox": [
        {
            "url": "https://accounts.firefox.com",
            "username": "user123@example.com",
            "password": "Password321!",
            "times_used": 5,
            "date_created": 1602721685,
            "date_last_used": 1705689206,
            "date_password_modified": 1602721685
        }
    ]
}
```

</details>

## Disclaimer

bthief is intended for educational purposes only. Do not use it illegally.

## Acknowledgements

### Contributors

- [@zedek1](https://github.com/zedek1)

### Dependencies

- [Windows Implementation Libraries](https://github.com/microsoft/wil)
- [nlohmann/json](https://github.com/nlohmann/json)
- [SQLite](https://www.sqlite.org/index.html)
