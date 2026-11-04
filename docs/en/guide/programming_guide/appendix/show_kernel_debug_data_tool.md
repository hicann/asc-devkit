# show\_kernel\_debug\_data Tool

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T14:50:06.586Z pushedAt=2026-11-03T07:05:13.442Z nl=2 -->

In the Ascend C operator program code, you can use the AscendC::[DumpTensor](../../../api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md), AscendC::[printf](../../../api/Utils-API/tuning_interface/printf.md), AscendC::[PrintTimeStamp](../../../api/SIMD-API/basic_api/debug_interface/onboard_print/PrintTimeStamp.md), and [ascendc\_assert](../../../api/Utils-API/tuning_interface/assert.md) interfaces to print related debugging information, and enable the Dump configuration through the aclInit interface or by directly configuring the acl.json file to export the debugging information of the Ascend C operator kernel function. This tool provides offline parsing of the debugging information, helping you obtain and parse the debugging information, that is, parse the exported bin files into a readable format. For an example of using this tool, see the [show\_kernel\_debug\_data sample](https://gitcode.com/cann/asc-tools/tree/9.2.0/examples/01_show_kernel_debug_data).

>[!NOTE] Note
>show\_kernel\_debug\_data supports concurrent call by multiple users. However, users must specify different output paths; otherwise, the output content may be overwritten.

## Supported Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products: Supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->

## Tool Installation

1.  Install the tool.

    The tool is released with the CANN software package (see [Environment Setup](../../getting_started/environment_setup.md) to complete the CANN installation). By default, it is located at `${INSTALL_DIR}/tools/show\_kernel\_debug\_data`, where `${INSTALL_DIR}` must be replaced with the storage path of the CANN software after installation. For example, when installed by the root user, the default storage path is /usr/local/Ascend/cann.

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
     │   ├── core_0_index_0_loop_0.bin     // core0 desc0 progress0 dump information
     │   ├── core_0_index_0_loop_0.txt     // core0 desc0 progress0 parsing result
      ...
     │   ├── core_0_index_2_loop_15.bin    // core0 desc2 progress15 dump information
     │   ├── core_0_index_2_loop_15.txt    // core0 desc2 progress15 parsing result
     │   └── time_stamp_core_0.csv         // timestamp information
     │   ├── 1                             // core1 parsing result
     │   ├── 2                             // core2 parsing result
      ...
     │   └── index_dtype.json              // mapping between index and data type
     └── parser.log                         // parsing log, including print information of the printf and ascendc_assert APIs
```
