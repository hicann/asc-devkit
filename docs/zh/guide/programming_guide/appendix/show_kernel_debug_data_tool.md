# show\_kernel\_debug\_data工具

在Ascend C算子程序代码中，用户可以使用AscendC::[DumpTensor](../../../api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md)、AscendC::[printf](../../../api/Utils-API/tuning_interface/printf.md)、AscendC::[PrintTimeStamp](../../../api/SIMD-API/basic_api/debug_interface/onboard_print/PrintTimeStamp.md)、[ascendc\_assert](../../../api/Utils-API/tuning_interface/assert.md)接口打印相关调试信息，并通过aclInit接口或直接配置acl.json文件，启用Dump配置，导出Ascend C算子核函数（Kernel）的调测信息。本工具提供了对调测信息的离线解析能力，帮助用户获取并解析调试信息，即将导出的bin文件解析成可读格式。本工具的使用示例可参考[show\_kernel\_debug\_data样例](https://gitcode.com/cann/asc-tools/tree/master/examples/01_show_kernel_debug_data)。

>[!NOTE]说明
>show\_kernel\_debug\_data支持多用户并发调用，但用户需要指定不同的落盘路径，否则可能出现落盘内容被覆盖等问题。

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品：支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 工具安装

1.  安装工具。

    工具跟随CANN软件包发布（参考[环境准备](../../getting_started/environment_setup.md)完成CANN安装），其路径默认为`${INSTALL_DIR}/tools/show\_kernel\_debug\_data`，其中`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。以root用户安装为例，安装后文件默认存储路径为：/usr/local/Ascend/cann。

2.  设置环境变量。
    -   root用户安装Ascend-cann-toolkit包时

        ```
        source /usr/local/Ascend/cann/set_env.sh
        ```

    -   非root用户安装Ascend-cann-toolkit包时

        ```
        source ${HOME}/Ascend/cann/set_env.sh
        ```

3.  检查工具是否安装成功。

    执行如下命令，若能正常显示--help或-h信息，则表示工具环境正常，功能可正常使用。

    ```
    show_kernel_debug_data -h
    ```

## 使用方法

-   **命令行方式**

    ```
    show_kernel_debug_data <bin_file_path> [<output_path>]
    ```

    | 参数 | 可选/必选 | 说明 |
    | --- | --- | --- |
    | <bin_file_path> | 必选 | kernel侧调试信息落盘的bin文件或包含bin文件的目录路径，例如“/input/dump_workspace.bin”。 |
    | <output_path> | 可选 | 解析结果的保存路径，例如“/output_dir”。默认是当前命令行执行目录下。 |

-   **API方式**

    获取kernel侧调试信息并解析成可读文件。函数原型如下。

    ```
    def show_kernel_debug_data(bin_file_path: str, output_path: str = './') -> None
    ```

    其中，输入参数说明如下。函数无输出参数和返回值。

    -   bin\_file\_path：kernel侧调试信息落盘的bin文件或包含bin文件的目录路径，字符串类型。
    -   output\_path：解析结果的保存路径，字符串类型，默认是当前接口调用脚本所在目录下。

    调用示例参考如下代码。

    ```
    from show_kernel_debug_data import show_kernel_debug_data
    show_kernel_debug_data(./input/dump_workspace.bin)
    ```

## 产物说明

工具解析结果文件目录结构如下。其中，dump\_data目录是对DumpTensor、PrintTimeStamp接口解析的结果，index0对应DumpTensor接口中第二个参数desc=0时的打印，loop0表示切分后首个分块的数据打印。

```
├ ${output_path}
├── PARSER_${timestamp}                     // ${timestamp}表示时间戳
     ├── dump_data
     │   ├── 0                             // core0解析结果
     │   ├── core_0_index_0_loop_0.bin     // core0 desc0 progress0落盘信息
     │   ├── core_0_index_0_loop_0.txt     // core0 desc0 progress0解析结果
      ...
     │   ├── core_0_index_2_loop_15.bin    // core0 desc2 progress15落盘信息
     │   ├── core_0_index_2_loop_15.txt    // core0 desc2 progress15解析结果
     │   └── time_stamp_core_0.csv         // 时间戳信息
     │   ├── 1                             // core1解析结果
     │   ├── 2                             // core2解析结果
      ...
     │   └── index_dtype.json              // index与数据类型的映射关系
     └── parser.log                         // 解析日志，包括printf、ascendc_assert接口的打印信息
```
