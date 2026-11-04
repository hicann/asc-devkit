# Permission Error Occurs When Deploying an Operator Package<a name="ZH-CN_TOPIC_0000002096022474"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-10-23T15:01:08.900Z pushedAt=2026-11-03T07:05:13.452Z nl=2 -->

## Symptoms<a name="section151611254194612"></a>

When deploying a custom operator package, the following error message is displayed:

```
[WARNING] The directory /usr/local/Ascend/cann/opp does not have sufficient permissions. Please check and modify the folder permissions (e.g., using chmod), or use the --install-path option to specify an installation path and source the generated set_env.bash.
...
[ERROR] create /usr/local/Ascend/cann/opp/vendors/customize/framework failed
```

## Root Cause<a name="section417961104715"></a>

The current operating user does not have write permission on the **vendors** directory under the deployment path.

The directory permission of the default installation path `${INSTALL_DIR}/opp/vendors` for custom operator packages depends on the user who installs the CANN package and the installation configuration: if the CANN package is installed by the root user, the permission of `${INSTALL_DIR}/opp/vendors` is 755; if the CANN package is installed by a non-root user with the `--install for all` parameter, the permission of this directory is 755; if the CANN package is installed by a non-root user without the `--install for all` parameter, the permission of this directory is 750.

For example, after the root user installs the CANN package, a user in the HwHiAiUser group deploys a custom operator package in the corresponding directory. Because other users do not have write permission, the preceding error message is displayed, indicating that the custom operator package fails to be deployed due to insufficient permissions.

## Handling Procedure<a name="section12149153520416"></a>

-   Method 1: Use the `--install-path` parameter to specify the installation directory (see [custom install path](../../advanced_programming/aclnn_operator_development/compilation_and_deployment/basic_process.md#custom-install-path)). The running user must have read and write permissions on the specified installation path. After installation, run the generated `set_env.bash` script, which configures the operator runtime search path.

    ```
    ./custom_opp_<target os>_<target architecture>.run --install-path=<path>
    source <path>/vendors/<vendor_name>/bin/set_env.bash
    ```

-   Method 2: Contact the user who installed the CANN software package to change the permissions of the **vendors** directory under the default installation path, for example, change them to 777:

    ```
    chmod 777 /usr/local/Ascend/cann/opp/vendors/
    ```
