# Why Does a Permission Error Occur When Deploying an Operator Package?<a name="ZH-CN_TOPIC_0000002096022474"></a>

<!-- md-trans-meta sourceCommit=d7f8cd8648f37bc3a4868cb030f666b2f7fd6d4d translatedAt=2026-08-26T11:22:16.942Z pushedAt=2026-08-31T10:52:09.501Z -->

## Symptoms<a name="section151611254194612"></a>

When deploying a custom operator package, the following error message is displayed:

```
[WARNING] The directory /usr/local/Ascend/cann/opp does not have sufficient permissions. Please check and modify the folder permissions (e.g., using chmod), or use the --install-path option to specify an installation path and change the environment variable ASCEND_CUSTOM_OPP_PATH to the specified path.
...
[ERROR] create /usr/local/Ascend/cann/opp/vendors/customize/framework failed
```

## Root Cause<a name="section417961104715"></a>

The current operating user does not have write permission on the **vendors** directory under the deployment path.

The directory permission of the default installation path **$\{INSTALL\_DIR\}/opp/vendors** for custom operator packages depends on the user who installs the CANN package and the installation configuration: if the CANN package is installed by the root user, the permission of **$\{INSTALL\_DIR\}/opp/vendors** is 755; if the CANN package is installed by a non-root user with the **--install for all** parameter, the permission of this directory is 755; if the CANN package is installed by a non-root user without the **--install for all** parameter, the permission of this directory is 750.

For example, after the root user installs the CANN package, a user in the HwHiAiUser group deploys a custom operator package in the corresponding directory. Because other users do not have write permission, the preceding error message is displayed, indicating that the custom operator package fails to be deployed due to insufficient permissions.

## Procedure<a name="section12149153520416"></a>

-   Method 1: Use the **--install-path** parameter and configure the environment variable *ASCEND\_CUSTOM\_OPP\_PATH* to specify the installation directory (see [Specified Directory Installation](../../advanced_programming/aclnn_operator_development/operator_package_compilation/operator_package_deployment.md#li1652971821912)). The running user must have read and write permissions on the specified installation path.

    ```
    ./custom_opp_<target os>_<target architecture>.run --install-path=<path>
    source <path>/vendors/<vendor_name>/bin/set_env.bash
    ```

-   Method 2: Contact the user who installed the CANN software package to change the permissions of the **vendors** directory under the default installation path, for example, change them to 777:

    ```
    chmod 777 /usr/local/Ascend/cann/opp/vendors/
    ```

