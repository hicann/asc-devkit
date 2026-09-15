# Operator Project Compilation<a name="ZH-CN_TOPIC_0000001646118574"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T11:58:23.963Z pushedAt=2026-09-02T10:59:33.490Z -->

After the kernel side and host side implementations of the operator are developed, the operator project needs to be compiled to generate the custom operator installation package \*.run. The detailed compilation operations include:

<a id="source-release"></a>
-   Compile the Ascend C operator kernel side implementation files \*.cpp. Two modes are available: source release and binary release.
    -   **Source release**: The operator kernel-side implementation is not compiled, and the operator kernel source files \*.cpp are retained. This mode supports online compilation of operators and operator compilation through ATC model conversion.
    -   **Binary release**: The operator kernel-side implementation is compiled to generate the JSON file \*.json that describes operator-related information and the operator binary file \*.o. When the operator is called, if the operator binary needs to be directly called, use this compilation mode, for example, completing the single-operator call through [single-operator API call](../single_operator_api_call.md), single-operator call in the PyTorch framework, and operator call in dynamic networks.

-   Compile the Ascend C operator host-side implementation files \*.cpp and \*.h.
    -   Compile the prototype definition and shape derivation implementation into the operator prototype definition dynamic library **libcust\_opsproto\_\*.so**, and generate the operator prototype external interface **op\_proto.h**.
    -   Compile the tiling implementation into the tiling dynamic library **liboptiling.so** and others.
    -   Based on the operator prototype definition, automatically generate the single operator API call code and header file **aclnn\_\*.h**, and compile them into the single operator API call dynamic library **libcust\_opapi.so**.

The preceding compilation process is illustrated as follows:

**Figure 1**  Operator project compilation diagram<a name="fig11482161513267"></a>  
![](../../../../figures/operator_project_compilation_diagram.png "Operator project compilation diagram")

## Compilation Steps<a name="section12364725125716"></a>

<a id="complete-build-config"></a>
1.  Complete the project compilation configuration.
    -   Modify the **cacheVariables** configuration items in the **CMakePresets.json** file in the project directory. The content of the **CMakePresets.json** file is as follows. For the parameters to be configured, see [Table 1](#table2023245818513). Other parameters are automatically generated when the project is created.

        ```
        {
            "version": 1,
            "cmakeMinimumRequired": {
                "major": 3,
                "minor": 19,
                "patch": 0
            },
            "configurePresets": [
                {
                    "name": "default",
                    "displayName": "Default Config",
                    "description": "Default build using Unix Makefiles generator",
                    "generator": "Unix Makefiles",
                    "binaryDir": "${sourceDir}/build_out",
                    "cacheVariables": {
                        "CMAKE_BUILD_TYPE": {
                            "type": "STRING",
                            "value": "Release"
                        },
                        "ENABLE_SOURCE_PACKAGE": {
                            "type": "BOOL",
                            "value": "True"
                        },
                        "ENABLE_BINARY_PACKAGE": {
                            "type": "BOOL",
                            "value": "True"
                        },
                        "ASCEND_COMPUTE_UNIT": {
                            "type": "STRING",
                            "value": "ascendxxx"
                        },
                        "ENABLE_TEST": {
                            "type": "BOOL",
                            "value": "True"
                        },
                        "vendor_name": {
                            "type": "STRING",
                            "value": "customize"
                        },
                        "ASCEND_PYTHON_EXECUTABLE": {
                            "type": "STRING",
                            "value": "python3"
                        },
                        "CMAKE_INSTALL_PREFIX": {
                            "type": "PATH",
                            "value": "${sourceDir}/build_out"
                        },
                        "ENABLE_CROSS_COMPILE": {
                            "type": "BOOL",
                            "value": "False"
                        },
                        "CMAKE_CROSS_PLATFORM_COMPILER": {
                            "type": "PATH",
                            "value": "/usr/bin/aarch64-linux-gnu-g++"
                        },
                        "ASCEND_PACK_SHARED_LIBRARY": {
                            "type": "BOOL",
                            "value": "False"
                        }
                    }
                }
            ]
        }
        ```

        **Table 1**  Parameters to be configured by developers

        <a name="table2023245818513"></a>
        <table><thead align="left"><tr id="row1723219582515"><th class="cellrowborder" valign="top" width="28.63%" id="mcps1.2.4.1.1"><p id="p1223245811518"><a name="p1223245811518"></a><a name="p1223245811518"></a>Parameter</p>
        </th>
        <th class="cellrowborder" valign="top" width="41.47%" id="mcps1.2.4.1.2"><p id="p723235812517"><a name="p723235812517"></a><a name="p723235812517"></a>Description</p>
        </th>
        <th class="cellrowborder" valign="top" width="29.9%" id="mcps1.2.4.1.3"><p id="p7121154014917"><a name="p7121154014917"></a><a name="p7121154014917"></a>Default Value</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row1923211587510"><td class="cellrowborder" valign="top" width="28.63%" headers="mcps1.2.4.1.1 "><p id="p112322587513"><a name="p112322587513"></a><a name="p112322587513"></a>CMAKE_BUILD_TYPE</p>
        </td>
        <td class="cellrowborder" valign="top" width="41.47%" headers="mcps1.2.4.1.2 "><p id="p623215581058"><a name="p623215581058"></a><a name="p623215581058"></a>Compilation mode option, which can be configured as:</p>
        <a name="ul91941346191017"></a><a name="ul91941346191017"></a><ul id="ul91941346191017"><li>"Release", the Release version, which does not contain debugging information and is used to compile the final released version.</li><li>"Debug", the Debug version, which contains debugging information to facilitate development and debugging by developers.</li></ul>
        </td>
        <td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.3 "><p id="p4419936397"><a name="p4419936397"></a><a name="p4419936397"></a>"Release"</p>
        </td>
        </tr>
        <tr id="row1923216580514"><td class="cellrowborder" valign="top" width="28.63%" headers="mcps1.2.4.1.1 "><p id="p1023255817513"><a name="p1023255817513"></a><a name="p1023255817513"></a>ENABLE_SOURCE_PACKAGE</p>
        </td>
        <td class="cellrowborder" valign="top" width="41.47%" headers="mcps1.2.4.1.2 "><p id="p423215818512"><a name="p423215818512"></a><a name="p423215818512"></a>Whether to enable source compilation</p>
        </td>
        <td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.3 "><p id="p19420036290"><a name="p19420036290"></a><a name="p19420036290"></a>"True"</p>
        </td>
        </tr>
        <tr id="row122328581956"><td class="cellrowborder" valign="top" width="28.63%" headers="mcps1.2.4.1.1 "><p id="p202323589515"><a name="p202323589515"></a><a name="p202323589515"></a>ENABLE_BINARY_PACKAGE</p>
        </td>
        <td class="cellrowborder" valign="top" width="41.47%" headers="mcps1.2.4.1.2 "><p id="p122321658353"><a name="p122321658353"></a><a name="p122321658353"></a>Whether to enable binary compilation</p>
        </td>
        <td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.3 "><p id="p2042015361794"><a name="p2042015361794"></a><a name="p2042015361794"></a>"True"</p>
        </td>
        </tr>
        <tr id="row102322588518"><td class="cellrowborder" valign="top" width="28.63%" headers="mcps1.2.4.1.1 "><p id="p123295810510"><a name="p123295810510"></a><a name="p123295810510"></a>vendor_name</p>
        </td>
        <td class="cellrowborder" valign="top" width="41.47%" headers="mcps1.2.4.1.2 "><p id="p122334581150"><a name="p122334581150"></a><a name="p122334581150"></a>Name that identifies the vendor of the custom operator. It is recommended that developers specify the vendor name to avoid conflicts with operator packages provided by other vendors.</p>
        </td>
        <td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.3 "><p id="p942011361797"><a name="p942011361797"></a><a name="p942011361797"></a>"customize"</p>
        </td>
        </tr>
        <tr id="row557188202813"><td class="cellrowborder" valign="top" width="28.63%" headers="mcps1.2.4.1.1 "><p id="p157138192812"><a name="p157138192812"></a><a name="p157138192812"></a>ASCEND_PACK_SHARED_LIBRARY</p>
        </td>
        <td class="cellrowborder" valign="top" width="41.47%" headers="mcps1.2.4.1.2 "><p id="p1857212842815"><a name="p1857212842815"></a><a name="p1857212842815"></a>Whether to enable <a href="../operator_dynamic_and_static_library_compilation.md">dynamic library compilation</a>.</p>
        </td>
        <td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.3 "><p id="p10572178142818"><a name="p10572178142818"></a><a name="p10572178142818"></a>"False"</p>
        </td>
        </tr>
        </tbody>
        </table>

    -    Configure compilation-related environment variables (optional)

        **Table 2**  Environment variable description

        <a name="table132801862211"></a>
        <table><thead align="left"><tr id="row22792616214"><th class="cellrowborder" valign="top" width="29.54%" id="mcps1.2.3.1.1"><p id="p4279961210"><a name="p4279961210"></a><a name="p4279961210"></a>Environment Variable</p>
        </th>
        <th class="cellrowborder" valign="top" width="70.46%" id="mcps1.2.3.1.2"><p id="p02796619211"><a name="p02796619211"></a><a name="p02796619211"></a>Configuration Description</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row17280365220"><td class="cellrowborder" valign="top" width="29.54%" headers="mcps1.2.3.1.1 "><p id="p628011615219"><a name="p628011615219"></a><a name="p628011615219"></a>CMAKE_CXX_COMPILER_LAUNCHER</p>
        </td>
        <td class="cellrowborder" valign="top" width="70.46%" headers="mcps1.2.3.1.2 "><p id="p1809444104711"><a name="p1809444104711"></a><a name="p1809444104711"></a>The launcher program used to configure the C++ language compiler (such as g++) and the BiSheng compiler is ccache. After configuration, cache-based compilation is enabled, <span>accelerating repeated compilation and improving build efficiency</span>. ccache must be installed before using this feature.</p>
        <p id="p0280176125"><a name="p0280176125"></a><a name="p0280176125"></a>The configuration method is as follows. Set it in the corresponding CMakeLists.txt:</p>
        <pre class="screen" id="screen628016616212"><a name="screen628016616212"></a><a name="screen628016616212"></a>set(CMAKE_CXX_COMPILER_LAUNCHER &lt;launcher_program&gt;)</pre>
        <p id="p328026522"><a name="p328026522"></a><a name="p328026522"></a>Where &lt;launcher_program&gt; is the installation path of ccache. For example, if the installation path of ccache is /usr/bin/ccache, the example is as follows:</p>
        <pre class="screen" id="screen102801861225"><a name="screen102801861225"></a><a name="screen102801861225"></a>set(CMAKE_CXX_COMPILER_LAUNCHER /usr/bin/ccache)</pre>
        </td>
        </tr>
        </tbody>
        </table>

2.  Run the following command in the operator project directory to compile the operator project.

    **./build.sh**

    After successful compilation, a **build\_out** directory is created in the current directory, and the custom operator installation package **custom\_opp\_<target os\>\_<target architecture\>.run** is generated in the **build\_out** directory.

    If you need to save the compilation process logs, you can use the environment variable **ASCENDC\_BUILD\_LOG\_DIR** to control the storage path. After you set this option, if no error occurs during compilation, the suffix "\_success" is added to the corresponding log file name. If an error occurs during compilation, the corresponding error information is printed on the screen, and the specific path and file name of the log file are indicated. Meanwhile, the suffix "\_error" is added to the corresponding log file name.

    ```
    # If you want the compilation logs to be stored in /home/build_log/, set it as follows. Log storage is disabled by default.
    export ASCENDC_BUILD_LOG_DIR=/home/build_log/
    ```

## Operator Package Cross-Compilation<a name="section2036752518573"></a>

After the operator code is implemented, if the current platform architecture is consistent with the runtime environment, compile the operator by referring to the content in the previous section. If you need to implement cross-compilation of the operator package, refer to the following process.

1.  Download the cross-compilation tool. The following table uses the Ubuntu series operating system as an example to show sample commands for downloading the compilation tool. For other operating systems, replace them with the actual download commands.

    **Table 3**  Sample commands for downloading the cross-compilation tool on the Ubuntu series operating system

    <a name="table1110215319257"></a>
    <table><thead align="left"><tr id="row101037313253"><th class="cellrowborder" valign="top" width="19.21192119211921%" id="mcps1.2.4.1.1"><p id="p710312352513"><a name="p710312352513"></a><a name="p710312352513"></a>Current Platform Architecture</p>
    </th>
    <th class="cellrowborder" valign="top" width="26.192619261926193%" id="mcps1.2.4.1.2"><p id="p12103163142514"><a name="p12103163142514"></a><a name="p12103163142514"></a>Runtime Environment Platform Architecture</p>
    </th>
    <th class="cellrowborder" valign="top" width="54.595459545954604%" id="mcps1.2.4.1.3"><p id="p1103536256"><a name="p1103536256"></a><a name="p1103536256"></a>Compilation Tool Download Command</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row11035312257"><td class="cellrowborder" valign="top" width="19.21192119211921%" headers="mcps1.2.4.1.1 "><p id="p1745514366214"><a name="p1745514366214"></a><a name="p1745514366214"></a>x86_64</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.192619261926193%" headers="mcps1.2.4.1.2 "><p id="p510312362515"><a name="p510312362515"></a><a name="p510312362515"></a>aarch64</p>
    </td>
    <td class="cellrowborder" valign="top" width="54.595459545954604%" headers="mcps1.2.4.1.3 "><p id="p310317362511"><a name="p310317362511"></a><a name="p310317362511"></a>sudo apt-get install -y g++-aarch64-linux-gnu</p>
    </td>
    </tr>
    <tr id="row91031633258"><td class="cellrowborder" valign="top" width="19.21192119211921%" headers="mcps1.2.4.1.1 "><p id="p8103133172512"><a name="p8103133172512"></a><a name="p8103133172512"></a>aarch64</p>
    </td>
    <td class="cellrowborder" valign="top" width="26.192619261926193%" headers="mcps1.2.4.1.2 "><p id="p641954118469"><a name="p641954118469"></a><a name="p641954118469"></a>x86_64</p>
    </td>
    <td class="cellrowborder" valign="top" width="54.595459545954604%" headers="mcps1.2.4.1.3 "><p id="p710373132512"><a name="p710373132512"></a><a name="p710373132512"></a>sudo apt-get install g++-x86-64-linux-gnu</p>
    </td>
    </tr>
    </tbody>
    </table>

2.   Cross-compile the custom operator project to build and generate the custom operator package.
    1.  Set **ENABLE\_CROSS\_COMPILE** in **CMakePresets.json** to **True** to enable cross-compilation.

        ```
        "ENABLE_CROSS_COMPILE": {
            "type": "BOOL",
            "value": "True"
         }
        ```

    2.  Set **CMAKE\_CROSS\_PLATFORM\_COMPILER** in **CMakePresets.json** to the path of the installed cross-compilation tool.

        ```
        "CMAKE_CROSS_PLATFORM_COMPILER": {
            "type": "PATH",
            "value": "/usr/bin/aarch64-linux-gnu-g++"
        }
        ```

    3.  In the operator project directory, run the following command to cross-compile the operator project.

        **./build.sh**

        After successful compilation, a **build\_out** directory is created in the current directory, and the custom operator installation package **custom\_opp\_<target os\>_\_<target architecture\>.run** is generated in the **build\_out** directory.

## Supporting Custom Compilation Options<a name="section7321165972311"></a>

In the operator project, if developers want to add some custom compilation options to the operator kernel-side code, they can customize the compilation options by referring to the following content.

Modify **CMakeLists.txt** in the **op_kernel** directory of the operator project, and use **add_ops_compile_options** to add compilation options. The method is as follows:

```
add_ops_compile_options(OpType COMPUTE_UNIT soc_version1 soc_version2 ... OPTIONS option1 option2 ...)
```

The specific parameters are described as follows:

**Table 4**  Specific parameters

<a name="table151052168302"></a>
| Parameter | Optional/Required | Description |
|---|---|---|
| OpType (Operator Type) | Required | The first parameter should pass the operator type. If it needs to take effect for all operators in the operator project, configure it as ALL. |
| COMPUTE_UNIT | Optional | Identifies on which AI processor models the compilation option takes effect. Multiple models are separated by spaces. If not configured, it takes effect for all AI processor models. |
| OPTIONS | Required | Custom compilation options. Multiple compilation options are separated by spaces.<br><ul><li>Add the -D compilation option to define macros during compilation.<pre class="screen">OPTIONS -Dname=definition</pre></li><li>Add debugging compilation options such as -g -O0.</li><li>Supports passing BiSheng compiler compilation options: for example, --cce-auto-sync=off. Setting this option disables the automatic synchronization function. The custom operator project has enabled it by default, so developers usually do not need to set it manually. For details, see [*BiSheng Compiler User Guide*](https://www.hiascend.com/document/redirect/CannCommunityBiSheng).</li><li>Compilation options provided by the Ascend C framework. For details, refer to the detailed description below.</li></ul> |

>[!NOTE] Note   
>The method to obtain COMPUTE_UNIT is as follows:
>- For the following products: execute the **npu-smi info** command on the server where the AI processor is installed to query and obtain the **Name** information. The actual configuration value is **AscendName**. For example, if the **Name** value is **xxxyyy**, the actual configuration value is **Ascendxxxyy**.<br><br>
>    Atlas A2 training products/Atlas A2 inference products  
>    Atlas 200I/500 A2 inference products  
>    Atlas inference products  
>    Atlas training products<br><br> 
>- For <term>Atlas A3 training products</term>/<term>Atlas A3 inference products</term>, execute the **npu-smi info -t board -i id -c chip_id** command on the server where the AI processor is installed to query and obtain the **Chip Name** and **NPU Name** information. The actual configuration value is **Chip Name_NPU Name**. For example, if the **Chip Name** value is **Ascendxxx** and the **NPU Name** value is **1234**, the actual configuration value is **Ascendxxx_1234**. Where:<br>**id**: device ID. The NPU ID queried by the **npu-smi info -l** command is the device ID.<br> **chip_id**: chip ID. The Chip ID queried by the **npu-smi info -m** command is the chip ID.<br><br>
>- For Ascend 950PR/Ascend 950DT, execute the **npu-smi info -t board -i id** command on the server where the AI processor is installed to query and obtain the **Chip Name** and **NPU Name** information. The actual configuration value is **Chip Name_NPU Name**. For example, if the **Chip Name** value is **Ascendxxx** and the **NPU Name** value is **1234**, the actual configuration value is **Ascendxxx_1234**.<br> Where: **id** is the device ID. The NPU ID queried by the **npu-smi info -l** command is the device ID.

>[!NOTE] Note
>- Compilation options are configured based on "operator type + AI processor model series", that is, different "operator type + AI processor model series" can be configured with different compilation options.
>    ```
>    add_ops_compile_options(AddCustom COMPUTE_UNIT Ascendxxxyy ... OPTIONS -DNEW_MACRO1=xx)
>    add_ops_compile_options(AddCustom COMPUTE_UNIT Ascendxxxyy ... OPTIONS -DNEW_MACRO2=xx)
>    add_ops_compile_options(AddCustom COMPUTE_UNIT Ascendxxxyy ... OPTIONS -DNEW_MACRO3=xx)
>    ```
>- For the same operator type + AI processor model series, if compilation options are configured multiple times, the later configuration prevails.
>- If the compilation options that take effect for ALL and the compilation options that take effect for a single operator do not conflict, they take effect simultaneously. If they conflict, the compilation options of the single operator prevail.

The compilation options provided by the Ascend C framework are described as follows:

-   --tiling\_key: after this option is set, only the kernel code related to the specified [TilingKey](../host_tiling_implementation/basic_process.md) is compiled, which accelerates the compilation process. If no TilingKey is specified for compilation, all TilingKeys are compiled by default. When multiple TilingKeys are configured, no space is allowed between them. An example is as follows, where 1 and 2 are tiling\_key.

    ```
    --tiling_key=1,2
    ```

-   For compile macro switches, see [Built-in Compile Macro Switches](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation_basic_usage.md).
-   --op\_relocatable\_kernel\_binary: when this option is set to true, an additional relocatable binary file is compiled; when it is not configured or is set to false, this option does not take effect. This option is used in the scenario where a custom Tiling-offloaded operator enables SuperKernel. The binary file generated with this option configured allows the operator to directly reuse the binary file during SuperKernel compilation, reducing compilation time.
-   --kernel-template-input: used for [operator project template programming](../host_tiling_implementation/tiling_template_programming.md). After this option is set, only the Kernel code related to the specified template parameter combination is compiled, which accelerates the compilation process. If this option is not set, all template parameter combinations are compiled by default. The input parameter is a list of key-value pairs, and the whole list must be enclosed in double or single quotation marks. Different template parameters are separated by semicolons (;), and multiple values of the same template parameter are separated by commas (,). No space is allowed in the configuration. An example is as follows:

    ```
    --kernel-template-input="D_T_X=A1,A2;D_T_Y=B;D_T_Z=C"
    --kernel-template-input='D_T_X=A1,A2;D_T_Y=B;D_T_Z=C'
    ```

    When configuring a template parameter combination, the template parameter names must match the template parameter names defined at the kernel entry and on the host side. For the values of the template parameter combination, if a custom type exists, it must be replaced with its corresponding numeric value; if it is a natively supported data type, it must be consistent with the input parameter at the kernel entry. An example is as follows:

    ```
    // Template parameter definition on the host side
    #define ADD_TPL_FP16 10
    #define ADD_TPL_FP32 20
    
    ASCENDC_TPL_ARGS_DECL(AddCustomTemplateNativeDtype,
        ASCENDC_TPL_DATATYPE_DECL(D_T_X, C_DT_FLOAT, C_DT_FLOAT16, ASCENDC_TPL_INPUT(0)),
        ASCENDC_TPL_DTYPE_DECL(D_T_Y, ADD_TPL_FP16, ADD_TPL_FP32),
        ASCENDC_TPL_DATATYPE_DECL(D_T_Z, C_DT_FLOAT, C_DT_FLOAT16, ASCENDC_TPL_OUTPUT(0)),
        ASCENDC_TPL_UINT_DECL(TILE_NUM, ASCENDC_TPL_8_BW, ASCENDC_TPL_UI_MIX, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8),
        ASCENDC_TPL_BOOL_DECL(IS_SPLIT, 0, 1),
    );
    
    ASCENDC_TPL_SEL(
        ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT),
        ASCENDC_TPL_DTYPE_SEL(D_T_Y, ADD_TPL_FP32),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
        ASCENDC_TPL_DETERMINISTIC_SEL(true),
        ASCENDC_TPL_KERNEL_TYPE_SEL(ASCENDC_TPL_AIV_ONLY),
        ),
        ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT16),
        ASCENDC_TPL_DTYPE_SEL(D_T_Y, ADD_TPL_FP16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT16),
        ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1),
        ASCENDC_TPL_DETERMINISTIC_SEL(false),
        ASCENDC_TPL_KERNEL_TYPE_SEL(ASCENDC_TPL_AIV_ONLY),
        ),
    );
    #endif
    
    //Kernel entry
    if constexpr (std::is_same_v<D_T_X, float> && std::is_same_v<D_T_Z, float>) {
            KernelAdd<D_T_X, float, D_T_Z> op;
            op.Init(x, y, z, tiling_data.totalLength, TILE_NUM);
            op.Process1();
        } else if constexpr (std::is_same_v<D_T_X, half> && std::is_same_v<D_T_Z, half>){
            KernelAdd<D_T_X, half, D_T_Z> op;
            op.Init(x, y, z, tiling_data.totalLength, TILE_NUM);
    ```

    A compilation option example is as follows:

    ```
    --kernel-template-input="D_T_X=10;D_T_Y=half;D_T_Z=10" 
    --kernel-template-input="D_T_X=10,20;D_T_Y=half,float;D_T_Z=10,20"
    ```
-   -DFORCE_TILING_CONST_PROPAGATION: this option is used for constant optimization of Tiling data in static shape scenarios. For complex operators, this compilation option can improve the static shape execution performance of the operator. This compilation option is supported only by custom operator projects, and it takes effect only when the operator uses BEGIN_TILING_DATA_DEF to register the Tiling structure; it does not take effect when the operator uses standard C++ syntax to define the Tiling structure. A usage example is as follows:
    ```
    -DFORCE_TILING_CONST_PROPAGATION
    ```
    The principle of this compilation option is that, in the scenario where the operator uses BEGIN_TILING_DATA_DEF to register the Tiling structure, when the custom operator project defines the Tiling structure, it defines all its member variables as static constexpr constants, that is, it turns the Tiling structure into a static constant class. Because the constants are marked at the frontend, the compiler performs more thorough constant folding and optimization based on the Tiling data.

    This option imposes constraints on the operator implementation. The following scenarios are not supported:

    1. The operator code must not define non-const objects using the Tiling structure, nor assign values to members of a Tiling object. Otherwise, a compilation error occurs because static constants cannot be reassigned.

    2. The operator code must not contain reinterpret_cast<OtherType*>(tiling_data). Otherwise, unexpected results are obtained. This is because all members of the Tiling structure are constants, the object of this type has no entity, its size is always 1 byte, and reinterpret_cast on tiling_data produces undefined behavior.

    For the scenario where the operator uses standard C++ syntax to define the Tiling structure, this option does not take effect because when the operator defines the Tiling structure as static constexpr, the Tiling data cannot be obtained, and the members of the Tiling structure cannot be initialized at definition time.