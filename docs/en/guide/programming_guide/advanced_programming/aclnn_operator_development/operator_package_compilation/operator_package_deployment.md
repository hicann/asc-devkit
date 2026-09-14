# Operator Package Deployment<a name="ZH-CN_TOPIC_0000001694478261"></a>

<!-- md-trans-meta sourceCommit=51bdbee063cf4e08caf1930420490ac5d334ab6c translatedAt=2026-08-26T11:57:24.234Z pushedAt=2026-09-02T10:26:00.228Z -->

Operator package deployment refers to installing a custom operator package. The compilation result of an operator project is automatically deployed to the operator package installation directory.

## Operator Package Deployment<a name="section588111410113"></a>

1.  Install and deploy the custom operator package.

    In the directory where the custom operator package resides, run the following command to install the custom operator package.

    ./custom\_opp\_<target os\>_\_<target architecture\>.run --install-path=<path\>

    **--install-path** is an optional parameter used to specify the installation directory of the custom operator package. An absolute path is supported, and the running user must have read and write permissions on the specified installation path.

    In the following description, **_<vendor\_name\>_** is the value of the "vendor\_name" field in the **CMakePresets.json** configuration file when the operator project is compiled. The default value is "customize".

    -   In the default installation scenario, the **--install-path** parameter is not configured. After successful installation, the compiled custom operator files are deployed to the ***$\{INSTALL\_DIR\}*/opp/vendors/_<vendor\_name\>_** directory. Replace *$\{INSTALL\_DIR\}* with the storage path of the CANN software after installation. For example, when installed by the root user, the default storage path is **/usr/local/Ascend/cann**.

        >[!NOTE]
        >The directory permissions of the default installation path **$\{INSTALL\_DIR\}/opp/vendors** for the custom operator package are related to the installation user and installation configuration of the CANN software package. If the custom operator package installation fails due to insufficient permissions, you can use the **--install-path** parameter and configure the **ASCEND\_CUSTOM\_OPP\_PATH** environment variable to specify the installation directory (see [Specified Directory Installation](#li1652971821912)), or contact the installation user of the CANN software package to modify the permissions of the **vendors** directory. For detailed cases, see [the error indicating unable to open config.ini during operator call](../../../appendix/faq/unable_to_open_config_ini_during_operator_call.md) and [the error indicating insufficient permission for operator package deployment](../../../appendix/faq/insufficient_permission_for_operator_package_deployment.md).

    -   <a name="li1652971821912"></a>In the specified directory installation scenario, the **--install-path** parameter is configured. After successful installation, the compiled custom operator files are deployed to the `<path>/vendors/<vendor_name>` directory, and a **set\_env.bash** file is added to the `<path>/vendors/<vendor_name>/bin` directory to write the environment variables related to the current custom operator package.

        >[!NOTE]
        >- If the installation directory of the operator package is specified by configuring the **--install-path** parameter during deployment, run the `source <path>/vendors/<vendor_name>/bin/set_env.bash` command before using the custom operator. The **set\_env.bash** script appends the installation path of the custom operator package to the **ASCEND\_CUSTOM\_OPP\_PATH** environment variable so that the custom operator takes effect in the current environment.  
        >- The architecture of the operator package must be consistent with the system architecture where the deployment is performed, and the deployment environment must meet the glibc version dependency of the operator package. Otherwise, an error is reported. You can use **--force** to force the installation.

After the command is executed successfully, the related files in the custom operator package are deployed to the current environment.

2.  Taking the default installation scenario as an example, you can view the directory structure after deployment, as shown below:

    ```
    ├── opp    //Operator library directory
    │   ├── vendors     //Directory where custom operators are located
    │       ├── config.ini
    │       └── vendor_name1   // Stores the custom operators deployed by the corresponding vendor. This name is the vendor_name configured when compiling the custom operator installation package. If it is not configured, the default value is customize.
    │           ├── framework     //Custom operator plugin library
    │           ├── op_api
    │           │   ├── include
    │           │   │  └── aclnn_xx.h      //Operator call API declaration file
    │           │   └── lib
    │           │       └── libcust_opapi.so
    │           ├── op_impl
    │           │   └── ai_core
    │           │       └── tbe
    │           │           ├── config
    │           │           ├── vendor_name1_impl    //Custom operator implementation code file
    │           │           │   └── dynamic
    │           │           │       ├── xx.cpp
    │           │           │       └── xx.py
    │           │           ├── kernel     //Custom operator binary file
    │           │           │   └── ${soc_version}     //AI processor type
    │           │           │   └── config
    │           │           └── op_tiling
    │           │               ├── lib
    │           │               └── liboptiling.so 
    │           └── op_proto     //Directory where the custom operator prototype library is located
    │                ├── inc
    │                │   └── op_proto.h
    │                └── lib
    │       ├── vendor_name2   // Stores the custom operators deployed by vendor_name2
    ```

3.  Configure the priority of custom operators.

    When multiple operator packages coexist, if custom operators with the same OpType exist in different operator package directories, the operator in the operator package directory with a higher priority takes effect. The following describes how to configure the operator package priority:

    -   Default installation scenario

        When custom operators from multiple vendors exist in the "opp/vendors" directory, you can configure the priority of custom operator packages by configuring the "config.ini" file in the "opp/vendors" directory.

        The following is an example of the "config.ini" file configuration:

        ```
        load_priority=vendor_name1,vendor_name2,vendor_name3
        ```

        -   "load\_priority": keyword of the priority configuration sequence. It cannot be modified.
        -   "vendor_name1,vendor_name2,vendor_name3": priority sequence of custom operator vendors, arranged in descending order of priority.

    -   Specified directory installation scenario

In the specified directory installation scenario, if multiple custom operator packages need to take effect simultaneously, run the **set\_env.bash** script in the installation path of each operator package separately. Each script execution appends the installation path of the current operator package to the front of the **ASCEND\_CUSTOM\_OPP\_PATH** environment variable. Therefore, the priority can be determined by the script execution order: the later the script is executed, the higher the priority of the operator package.

For example, if you first run `source  <path>/vendor_name1/bin/set_env.bash` and then run `source  <path>/vendor_name2/bin/set_env.bash`, the **vendor\_name2** operator package has a higher priority than **vendor\_name1**. An example of **ASCEND\_CUSTOM\_OPP\_PATH** is as follows:

        ```
        ASCEND_CUSTOM_OPP_PATH=<path>/vendor_name2:<path>/vendor_name1:
        ```

    -   The operator package installed in the specified directory installation scenario has a higher priority than the operator package installed in the default manner.
