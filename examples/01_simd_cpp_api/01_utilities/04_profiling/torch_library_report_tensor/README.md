# torch.library调用集成Profiling记录Shape信息

## 概述

本样例展示在`torch.library`自定义算子调用方式下，如何让`msopprof`结果中显示算子的输入输出Shape、数据类型和Format信息。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── torch_library_report_tensor
│   ├── CMakeLists.txt                   // 编译工程文件
│   ├── torch_library_report_tensor.py   // PyTorch调用脚本
│   ├── torch_library_report_tensor.asc  // torch.library算子注册、Profiling上报和Add Kernel实现
│   └── README.md                        // 样例说明文档
```

## 样例描述

- 样例功能

  样例注册名为`ascendc_ops::ascendc_add`的PyTorch自定义算子。Python脚本输入两个`[8, 2048]`、`float16`、`ND`格式的NPU Tensor，调用自定义算子完成Add计算，并通过CPU结果校验精度。计算公式为：

  ```
  z = x + y
  ```

- 样例规格

  <table border="2">
  <caption>表1：AddCustom样例规格描述</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">AddCustom</td></tr>
  <tr><td align="center"></td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">样例输入</td><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">样例输入</td><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">样例输出</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- 样例实现

  本样例的实现分为三个部分：

  1. `torch.library`算子注册

     `torch_library_report_tensor.asc`中定义了`ascendc_ops`命名空间，并通过`TORCH_LIBRARY`和`TORCH_LIBRARY_IMPL`注册`ascendc_add`。Python侧可以通过如下方式调用：

     ```python
     torch.ops.ascendc_ops.ascendc_add(x.npu(), y.npu())
     ```

  2. Profiling Shape信息记录

     `ascendc_add`函数在下发Kernel前构造Profiling元信息，并通过`aclprofRangePushEx`传递给`msopprof`。Kernel下发完成后调用`aclprofRangePop`结束本次Profiling范围。

     这样做的目的是让`msopprof`在生成`PipeUtilization.csv`时，能够在当前自定义算子记录中展示输入输出Tensor信息，例如：

     ```text
     Input Shapes: "8,2048;8,2048"
     Input Data Types: FLOAT16;FLOAT16
     Input Formats: ND;ND
     Output Shapes: "8,2048"
     Output Data Types: FLOAT16
     Output Formats: ND
     ```

     上报信息由以下结构组成：

     - `aclprofTensor`：描述单个Tensor，包括输入/输出类型、Format、数据类型、Shape维度和Shape值。
     - `aclprofTensorInfo`：描述一次算子调用，包括算子名称、算子类型、block数量、stream以及输入输出Tensor数组。
     - `aclprofEventAttributes`：`aclprofRangePushEx`使用的外层消息结构，用于携带`aclprofTensorInfo`。

     代码中的`INPUT(x)`和`OUTPUT(z)`宏用于从PyTorch `at::Tensor`中提取Format、数据类型和Shape信息；`INIT_ACL_PROF_TENSOR_INFO`宏用于把算子信息和Tensor信息组装成`aclprofTensorInfo`。

  3. Add Kernel实现

     Device侧`add_custom` Kernel采用静态Tensor编程方式实现最小Add计算。Kernel模板参数只包含固定Shape：`<8, 2048>`；Kernel启动时通过`<<<blockNum, 0, stream>>>`传入block数量，Kernel内部通过`AscendC::GetBlockNum()`计算每个block处理的数据长度。

     本样例Kernel仅支持`[8, 2048]`、`float16`输入。如果输入Shape不符合要求，Host侧会通过`TORCH_CHECK`报错。

- Python测试脚本

  在`torch_library_report_tensor.py`调用脚本中，通过`torch.ops.load_library`加载生成的自定义动态库，调用注册的`ascendc_add`函数，并将NPU计算结果与CPU的`torch.add`结果进行比对。

## 编译运行

- 安装PyTorch以及Ascend Extension for PyTorch插件

  请参考[pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch)开源代码仓或[Ascend Extension for PyTorch昇腾社区](https://hiascend.com/document/redirect/Pytorch-index)的安装说明，选取支持的`Python`版本配套发行版，完成`torch`和`torch-npu`的安装。
  
  > torch_npu 的版本为 26.0.0。

- 安装前置依赖

  ```bash
  pip3 install expecttest
  ```

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例根目录下执行如下步骤，运行该样例。

  ```bash
  mkdir -p build; cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j
  msopprof --application="python3 ../torch_library_report_tensor.py" --output="../result"
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果
  执行结果如下，说明Profiling数据采集和解析成功，其中DIR_NAME和PATH分别为落盘文件名和数据存放目录。

  ```bash
  [INFO] Query all data in ${DIR_NAME} done.
  [INFO] Profiling finished.
  [INFO] Process profiling data complete. Data is saved in ${PATH}
  ```

- Shape信息展示

  打开`../result/OPPROF_*/PipeUtilization.csv`，查看Shape信息，样例中Shape信息写入如下字段。

  <table>
    <tr>
      <td align="center">...</td>
      <td align="center">Op Name</td>
      <td align="center">Op Type</td>
      <td align="center">Input Shapes</td>
      <td align="center">Input Data Types</td>
      <td align="center">Input Formats</td>
      <td align="center">Output Shapes</td>
      <td align="center">Output Data Types</td>
      <td align="center">Output Formats</td>
      <td align="center">...</td>
    </tr>
    <tr>
      <td align="center">...</td>
      <td align="center">xxx</td>
      <td align="center">xxx</td>
      <td align="center">"8,2048;8,2048"</td>
      <td align="center">FLOAT16;FLOAT16</td>
      <td align="center">ND;ND</td>
      <td align="center">"8,2048"</td>
      <td align="center">FLOAT16</td>
      <td align="center">ND</td>
      <td align="center">...</td>
    </tr>
  </table>
