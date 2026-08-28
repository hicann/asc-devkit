# 自定义simplified key编译样例

## 概述

本样例以`AddCustomTemplate`为例，展示通过Host回调生成自定义`simplifiedKey`，并通过`BINARY_JSON`为不同输入数据类型选择不同Kernel Binary的编译流程。

本样例是独立的自定义算子工程，不会改变`custom_op`综合样例中原有算子的编译行为。

## 目录结构

```text
├── custom_op_simplified_key
│   ├── CMakeLists.txt
│   ├── op_host
│   │   ├── CMakeLists.txt
│   │   ├── add_custom_template
│   │   └── config
│   │       └── ascend910b
│   │           └── add_custom_template_binary.json
│   ├── op_kernel
│   │   ├── CMakeLists.txt
│   │   └── add_custom_template
│   └── README.md
```

## 样例说明

`AddCustomTemplate`实现`z = x + y`，支持`float16`和`float32`输入输出。

Host侧的`GenSimplifiedKeyFunc`根据第一个输入的`dtype`返回：

| 输入dtype | simplified key | Kernel Binary |
| --- | --- | --- |
| `float16` | `custom_fp16` | `AddCustomTemplate_FP16.o` |
| `float32` | `custom_fp32` | `AddCustomTemplate_FP32.o` |

`op_host/config/ascend910b/add_custom_template_binary.json`描述上述两个Kernel Binary及其输入输出的`dtype`、`format`和`shape`。Kernel编译选项通过`--kernel-json-file`传入该文件。

## 编译

配置CANN环境：

```bash
source ${install_path}/cann/set_env.sh
```

在本样例根目录执行：

```bash
rm -rf build && mkdir build && cd build
cmake ..
make AddCustomTemplate_ascend910b
```

也可以执行完整的二进制和安装包构建：

```bash
make -j binary package
```

## 编译结果验证

编译完成后，应能在`build`目录下找到：

- `AddCustomTemplate_FP16.o`和`AddCustomTemplate_FP32.o`；
- 包含`simplifiedKey`和`simplifiedKeyMode`的算子JSON；
- 汇总所有Kernel查找信息的`binary_info_config.json`。

可以使用以下命令查看生成文件：

```bash
find . -name 'AddCustomTemplate_*.o' -o -name 'binary_info_config.json'
```
