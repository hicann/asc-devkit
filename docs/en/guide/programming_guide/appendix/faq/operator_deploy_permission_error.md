# Permission Error Occurs When Deploying an Operator Package<a name="ZH-CN_TOPIC_0000002096022474"></a>

<!-- md-trans-meta sourceCommit=aebba88d99ed994d7d3e029834e76b3cfd40ec01 translatedAt=2026-11-09T10:57:21.867Z pushedAt=2026-11-19T06:32:19.924Z nl=2 -->

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

-   Method 1 (recommended): Use the `--install-path` parameter to specify an installation directory that is owned by the user and has controlled permissions (see [custom install path](../../advanced_programming/aclnn_operator_development/compilation_and_deployment/basic_process.md#custom-install-path)). The deploying user must have read, write, and execute permissions on the specified installation path. After installation, run the generated `set_env.bash` script, which configures the operator runtime search path.

    ```
    ./custom_opp_<target os>_<target architecture>.run --install-path=<path>
    source <path>/vendors/<vendor_name>/bin/set_env.bash
    ```

-   Method 2: If you need to deploy to the default installation path, contact the user who installed the CANN software package or the administrator to deploy the custom operator package centrally. If you must authorize other users to deploy, follow the principle of least privilege and grant only the necessary permissions to trusted users.

    **Security note:** Do not set the permissions of the shared vendors directory to 777, and do not recursively grant write permissions on that directory to all users. Write permissions on the parent directory may allow unauthorized users to add or rename operator directories and replace loading configurations, affecting operator loading for other users. When a running user only needs to use the operator package, do not grant write permissions on the deployment directory to that user.
