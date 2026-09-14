# Security Statement

## User Running Recommendations

From a security perspective, it is not recommended to use root or other administrator-type accounts to execute any commands. Follow the principle of least privilege.

## File Permission Control

- Recommend users set the running system umask value to 0027 or higher on hosts (including host machines) and containers, ensuring new folder default maximum permission is 750 and new file default maximum permission is 640.
- Recommend users implement permission control and other security measures for sensitive content such as personal privacy data, business assets, source files, and various files saved during code development. For example, permission control for this project installation directory, public data file permission control. Refer to [A-File (Folder) Permission Control Recommended Maximum Values for Various Scenarios](SECURITY_en.md#a-file-folder-permission-control-recommended-maximum-values-for-various-scenarios) for recommended permissions.
- Users need to implement permission control during installation and usage. Refer to [A-File (Folder) Permission Control Recommended Maximum Values for Various Scenarios](SECURITY_en.md#a-file-folder-permission-control-recommended-maximum-values-for-various-scenarios) for file permission reference settings.

## Build Security Statement

When compiling and installing this project from source code, you need to compile it yourself. The compilation process generates intermediate files. Recommend implementing permission control for intermediate files after compilation completion to ensure file security.

## Runtime Security Statement

- When runtime exceptions occur, the process exits and prints error information. Recommend locating specific error causes based on error prompts.

## Public Network Address Statement
Public network address statements contained in this project code are shown below:

| Type | Open Source Code Address | File Name | Public IP Address/Public URL Address/Domain Name/Email Address/Compressed File Address | Usage Description |
| :------------: |:------------------------------------------------------------------------------------------:|:----------------------------------------------------------| :---------------------------------------------------------- |:-----------------------------------------|
| Dependency | Not applicable | cmake/third_party/gtest.cmake | https://gitcode.com/cann-src-third-party/googletest/releases/download/v1.14.0/googletest-1.14.0.tar.gz | Download googletest source code from gitcode, used as compilation dependency |
---

## Vulnerability Mechanism Statement
[Vulnerability Management](https://gitcode.com/cann/community/blob/master/security/security.md)

## Appendix

### A-File (Folder) Permission Control Recommended Maximum Values for Various Scenarios

| Type | Linux Permission Reference Maximum Value |
| -------------- | ---------------  |
| User home directory | 750 (rwxr-x---) |
| Program files (including script files, library files, and so on) | 550 (r-xr-x---) |
| Program file directory | 550 (r-xr-x---) |
| Configuration files | 640 (rw-r-----) |
| Configuration file directory | 750 (rwxr-x---) |
| Log files (completed recording or archived) | 440 (r--r-----) |
| Log files (currently recording) | 640 (rw-r-----) |
| Log file directory | 750 (rwxr-x---) |
| Debug files | 640 (rw-r-----) |
| Debug file directory | 750 (rwxr-x---) |
| Temporary file directory | 750 (rwxr-x---) |
| Maintenance upgrade file directory | 770 (rwxrwx---) |
| Business data files | 640 (rw-r-----) |
| Business data file directory | 750 (rwxr-x---) |
| Key component, private key, certificate, ciphertext file directory | 700 (rwx------) |
| Key component, private key, certificate, encrypted ciphertext | 600 (rw-------) |
| Encryption/decryption interfaces, encryption/decryption scripts | 500 (r-x------)
