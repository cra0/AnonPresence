![Logo](./resources/img/Logo.png?raw=true "AnonPresence Logo")
# AnonPresence
Microsoft Teams presence report blocker.

## Introduction

Microsoft Teams peroticially sends back telemetry and presence data on your activity status while using the native client on Windows. The reported data can indicate how active you are on your pc. 
This tool hooks SSL_Write inside the third party library [BoringSSL](https://boringssl.googlesource.com/boringssl/) which is utilized inside [Electron](https://github.com/electron/electron).
Effectively spoofing your status to be always active.


## Usage

- Copy the built dummy `VERSION.DLL` into `C:\Users\%USERNAME%\AppData\Local\Microsoft\Teams\current\`
- Copy the original DLL either included in the [Release](https://github.com/cra0/AnonPresence/releases) or acquire a copy from your System32 'C:\Windows\System32\' directory.
- Rename the original `VERSION.dll` to `VERSION_orig.dll` and place it in the same directory `(C:\Users\%USERNAME%\AppData\Local\Microsoft\Teams\current\)`.

Once you run Teams there will be a log generated located at `C:\Users\cihan\AppData\Local\Microsoft\Teams\current\log\AnonPresence.log`

You may need to repeat these steps if Teams decides to repair it's install folder after an update.

