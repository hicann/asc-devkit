# Creating an Operator Project<a name="ZH-CN_TOPIC_0000001618617245"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T11:48:25.615Z pushedAt=2026-09-03T01:26:42.371Z -->

The CANN development kit provides msOpGen, a tool for generating custom operator projects. Based on the operator prototype definition, msOpGen outputs an operator project, including the **operator host-side code implementation file**, the **operator kernel-side implementation file**, and the **project build configuration file**.

>[!NOTE] Note
>Before using msOpGen to create an operator project, install the driver, firmware, and CANN software package by referring to [Environment Setup](../../../getting_started/environment_setup.md) to set up the development and running environments.

To create an operator development project using msOpGen, perform the following steps:

1.  Write the prototype definition JSON file of the operator, which is used to generate the operator development project.

    For example, the JSON file of the AddCustom operator is named **add\_custom.json**, and its content is as follows:

    ```
    [
        {
            "op": "AddCustom",
            "input_desc": [
                {
                    "name": "x",
                    "param_type": "required",
                    "format": [
                        "ND",
                        "ND",
                        "ND"
                    ],
                    "type": [
                        "float16",
                        "float",
                        "int32"
                    ]
                },
                {
                    "name": "y",
                    "param_type": "required",
                    "format": [
                        "ND",
                        "ND",
                        "ND"
                    ],
                    "type": [
                        "float16",
                        "float",
                        "int32"
                    ]
                }
            ],
            "output_desc": [
                {
                    "name": "z",
                    "param_type": "required",
                    "format": [
                        "ND",
                        "ND",
                        "ND"
                    ],
                    "type": [
                        "float16",
                        "float",
                        "int32"
                    ]
                }
            ]
        }
    ]
    ```

    For example, the JSON file of the ReduceMaxCustom operator (which contains attributes) is named **reduce\_max\_custom.json**, and its content is as follows:

    ```
    [
        {
            "op": "ReduceMaxCustom",
             "input_desc": [
                {
                    "name": "x",
                    "param_type": "required",
                    "format": ["ND"],
                    "type": ["float16"]
                }
            ],
            "output_desc": [
                {
                    "name": "y",
                    "param_type": "required",
                    "format": ["ND"],
                    "type": ["float16"]
                },
                {
                    "name": "idx",
                    "param_type": "required",
                    "format": ["ND"],
                    "type": ["int32"]
                }
            ],
            "attr": [                                                                   
                {
                    "name": "reduceDim",
                    "param_type": "required",
                    "type": "int"
                },
                {
                    "name": "isKeepDim",
                    "param_type": "optional",
                    "type": "int",
                    "default_value": 1
                }
            ]
        }
    ]
    ```

    >[!NOTE] Note
    >The operator type field **op** in the prototype definition JSON file must use the **UpperCamelCase** naming convention, that is, uppercase characters are used to distinguish different semantics.

2.  Use msOpGen to generate an operator development project. Taking the generation of the AddCustom operator project as an example, only the key parameters are explained below. For details about the parameters, see [*msOpGen User Guide*](https://gitcode.com/Ascend/msopgen/blob/26.1.0/docs/en/user_guide/msopgen_user_guide.md).

    ```
    ${INSTALL_DIR}/python/site-packages/bin/msopgen gen -i $HOME/sample/add_custom.json -c ai_core-<soc_version> -lan cpp -out $HOME/sample/AddCustom
    ```

    -   *$\{INSTALL\_DIR\}* is the storage path of the CANN software after installation. Replace it with the actual path in your environment.
    -   **-i**: specifies the path of the operator prototype definition file **_add\_custom_.json**. Modify it based on the actual situation.
    -   **-c**: **ai\_core-_<soc\_version\>_** indicates that the operator runs on the AI Core, where **_<soc\_version\>_** is the model of the AI processor.

        >[!NOTE] Note
        >- For the following products, run the **npu-smi info** command on the server where the AI processor is installed to query and obtain the Name information. The actual configuration value is AscendName. For example, if the Name value is xxxyy, the actual configuration value is Ascendxxxyy.<br><br>
        >    Atlas A2 training products/Atlas A2 inference products  
        >    Atlas 200I/500 A2 inference products  
        >    Atlas inference products  
        >    Atlas training products<br><br> 
        >- For <term>Atlas A3 training products</term>/<term>Atlas A3 inference products</term>, run the **npu-smi info -t board -i id -c chip_id** command on the server where the AI processor is installed to query and obtain the Chip Name and NPU Name information. The actual configuration value is Chip Name_NPU Name. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234. Where:<br>id: device ID. The NPU ID obtained through the **npu-smi info -l** command is the device ID.<br> chip_id: chip ID. The chip ID obtained through the **npu-smi info -m** command is the chip ID.<br><br>
        >- For Ascend 950PR/Ascend 950DT, run the **npu-smi info -t board -i id** command on the server where the AI processor is installed to query and obtain the Chip Name and NPU Name information. The actual configuration value is Chip Name_NPU Name. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234.<br> Where: id is the device ID. The NPU ID obtained through the **npu-smi info -l** command is the device ID.

        >[!NOTE] Note
        >The basic functions (operator development, compilation, and deployment based on the project) of operator projects created based on AI processor models of the same series are universal.

    -   -lan: the parameter **cpp** indicates that the operator is developed based on the Ascend C programming framework using the C/C++ programming language.
    -   -out: path for storing the generated files. It can be configured as an absolute path or a relative path, and the user executing the tool must have read and write permissions on the path. If it is not configured, the files are generated in the current path where the command is executed by default.

3.  After the command is executed, an operator project directory is generated in the directory specified by **-out** or in the default path. The project contains template files for operator implementation, build scripts, and so on. Taking the AddCustom operator as an example, the directory structure is as follows:

    ```
    AddCustom
    ├── build.sh         // Compilation entry script
    ├── cmake            // Directory for storing scripts and common compilation files required for operator project compilation
    ├── CMakeLists.txt    // CMakeLists.txt of the operator project
    ├── CMakePresets.json    // Compilation configuration items
    ├── framework        // Directory for operator plugin implementation files during AI framework adaptation
    ├── op_host                      // Host-side implementation files
    │   ├── add_custom.cpp         // Files for operator prototype registration, shape inference, information library, and tiling implementation
    │   ├── CMakeLists.txt
    ├── op_kernel                   // Kernel-side implementation files
    │   ├── add_custom.cpp        // Operator code implementation file
    │   ├── add_custom_tiling.h    // Operator tiling definition file
    │   ├── CMakeLists.txt
    └── scripts                     // Directory for scripts related to custom operator project packaging
    ```

    >[!NOTE] Note
    >- The bold files in the directory structure above are the files that need to be modified during subsequent operator development. Other files do not need to be modified.
    >- All files that the kernel-side implementation depends on must be placed in the **op\_kernel** directory. Failure to comply with this constraint will cause online compilation to fail in the [source release](operator_package_compilation/operator_project_compilation.md#source-release) scenario. This is because only the files in the **op\_kernel** directory are packaged and released during subsequent operator source code release.

The **op\_kernel** and **op\_host** directories in the project directory contain the core implementation files of the operator. The **op\_kernel** directory stores the [kernel-side operator implementation](kernel_side_operator_implementation.md). The **op\_host** directory stores the host-side code implementation, including the [operator prototype definition](operator_prototype_definition.md) and [host tiling implementation](host_tiling_implementation/basic_process.md). The core implementation methods of the kernel-side operator implementation and host-side tiling implementation have been introduced in the [SIMD operator implementation](../../../operator_practice/simd_operator_impl/overview.md) chapter. This chapter focuses on the programming mode and API usage after integration into the CANN framework. The **CMakePresets.json** file in the project directory is used by developers to complete the project compilation configuration, after which compilation and deployment can be performed.

