# show\_kernel\_debug\_data Tool

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T11:11:10.898Z pushedAt=2026-09-01T02:13:44.560Z -->

In Ascend C operator program code, you can use the AscendC::[DumpTensor](../../../api/SIMD-API/basic_api/debug_interface/oonboard_print/DumpTensor.md), AscendC::[printf](../../../api/SIMD-API/basic_api/debug_interface/oonboard_print/printf.md), AscendC::[PrintTimeStamp](../../../api/SIMD-API/basic_api/debug_interface/oonboard_print/PrintTimeStamp.md), and [ascendc\_assert](../../../api/SIMD-API/basic_api/debug_interface/exception_detection/ascendc_assert.md) APIs to print related debug information, and enable the dump configuration through the aclInit API or by directly configuring the acl.json file to export the debug information of the Ascend C operator kernel. This tool provides offline parsing of the debug information, helping you obtain and parse the debug information, that is, parse the exported bin files into a readable format. For an example of using this tool, see the [show\_kernel\_debug\_data sample](https://gitcode.com/cann/asc-tools/tree/9.1.0/examples/01_show_kernel_debug_data).

>[!NOTE] Note
>show\_kernel\_debug\_data supports concurrent call by multiple users. However, users must specify different output paths; otherwise, the output content may be overwritten.

## Supported Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- Atlas inference products: Supported
- Atlas training products: Not supported

## Installing the Tool

1.  Install the tool.

    The tool is released with the CANN software package (see [Environment Setup](../../getting_started/environment_setup.md) to complete the CANN installation). By default, it is located at "\$\{INSTALL\_DIR\}/tools/show\_kernel\_debug\_data", where \$\{INSTALL\_DIR\} must be replaced with the storage path of the CANN software after installation. For example, when installed by the root user, the default storage path is /usr/local/Ascend/cann.

2.  Set the environment variables.
    -   When the Ascend-cann-toolkit package is installed by the root user

        ```
        source /usr/local/Ascend/cann/set_env.sh
        ```

    -   When the Ascend-cann-toolkit package is installed by a non-root user

        ```
        source ${HOME}/Ascend/cann/set_env.sh
        ```

3.  Check whether the tool is installed successfully.

    Run the following command. If the --help or -h information is displayed normally, the tool environment is normal and the functions can be used properly.

    ```
    show_kernel_debug_data -h
    ```

## Usage

-   **Command line mode**

    ```
    show_kernel_debug_data <bin_file_path> [<output_path>]
    ```

    | Parameter | Optional/Required | Description |
    | --- | --- | --- |
    | <bin_file_path> | Required | Path of the bin file written to disk for kernel-side debug information, or the directory containing the bin file, for example, "/input/dump_workspace.bin". |
    | <output_path> | Optional | Path for saving the parsing result, for example, "/output_dir". The default is the directory where the current command is executed. |

-   **API mode**

    Obtains kernel-side debug information and parses it into a readable file. The function prototype is as follows.

    ```
    def show_kernel_debug_data(bin_file_path: str, output_path: str = './') -> None
    ```

    The input parameters are described as follows. The function has no output parameters or return value.

    -   bin_file_path: path of the bin file written to disk for kernel-side debug information, or the directory containing the bin file. The type is string.
    -   output_path: path for saving the parsing result. The type is string. The default is the directory where the script that calls the API is located.

    The following code shows an example of the call.

    ```
    from show_kernel_debug_data import show_kernel_debug_data
    show_kernel_debug_data(./input/dump_workspace.bin)
    ```

## Output Description

The directory structure of the tool's parsing result files is as follows. The dump\_data directory contains the parsing results of the DumpTensor and PrintTimeStamp APIs. index0 corresponds to the print when the second parameter desc=0 in the DumpTensor API, and loop0 indicates the data print of the first block after splitting.

```
├ ${output_path}
├── PARSER_${timestamp}                     // ${timestamp} indicates the timestamp
     ├── dump_data
     │   ├── 0                             // core0 parsing result
     │   ├── core_0_index_0_loop_0.bin     // core0 desc0 progress0 information written to disk
     │   ├── core_0_index_0_loop_0.txt     // core0 desc0 progress0 parsing result
      ...
     │   ├── core_0_index_2_loop_15.bin    // core0 desc2 progress15 information written to disk
     │   ├── core_0_index_2_loop_15.txt    // core0 desc2 progress15 parsing result
     │   └── time_stamp_core_0.csv         // timestamp information
     │   ├── 1                             // core1 parsing result
     │   ├── 2                             // core2 parsing result
      ...
     │   └── index_dtype.json              // mapping between index and data type
     └── parser.log                         // parsing log, including the print information of the printf and ascendc_assert interfaces
```
