# torch.library注册自定义算子直调样例

## 概述

本样例基于Add算子展示如何使用PyTorch的torch.library机制注册自定义算子。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.1.0 |
| Atlas A2系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── torch_library
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── add_custom.asc          // Ascend C算子实现 & torch.library注册
│   ├── add_custom_test.py      // PyTorch调用脚本
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：

  Add计算公式为：

  ```
  z = x + y
  ```

- 样例规格：
  <table border="2" align="center">
  <caption>表1：AddCustom样例规格描述</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">AddCustom</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- 自定义算子注册：

  本样例在`add_custom.asc`中定义了一个名为`ascendc_ops`的命名空间，并在其中注册了`ascendc_add`函数。

  PyTorch提供`TORCH_LIBRARY`宏作为自定义算子注册的核心接口，用于创建并初始化自定义算子库，注册后在Python侧可以通过`torch.ops.namespace.op_name`方式进行调用，例如：

  ```cpp
  TORCH_LIBRARY(ascendc_ops, m) {
      m.def("ascendc_add(Tensor x, Tensor y) -> Tensor");
  }
  ```

  `TORCH_LIBRARY_IMPL`用于将算子绑定到特定的`DispatchKey`（PyTorch设备调度标识）。针对NPU设备，需要将算子实现注册到`PrivateUse1`这一专属的`DispatchKey`上，例如：

  ```cpp
  TORCH_LIBRARY_IMPL(ascendc_ops, PrivateUse1, m)
  {
      m.impl("ascendc_add", TORCH_FN(ascendc_ops::ascendc_add));
  }
  ```

  在`ascendc_add`函数中通过`c10_npu::getCurrentNPUStream()`函数获取当前NPU上的流，并通过内核调用符`<<<>>>`调用自定义的Kernel函数`add_custom`，在NPU上执行算子。

- Python测试脚本

  在`add_custom_test.py`调用脚本中，通过`torch.ops.load_library`加载生成的自定义算子库，调用注册的`ascendc_add`函数，并通过对比NPU输出与CPU标准加法结果来验证自定义算子的数值正确性。

## 编译运行

- 安装PyTorch以及Ascend Extension for PyTorch插件

  请参考[pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch)开源代码仓或[Ascend Extension for PyTorch昇腾社区](https://hiascend.com/document/redirect/Pytorch-index)的安装说明，选取支持的`Python`版本配套发行版，完成`torch`和`torch-npu`的安装。

- 安装前置依赖

  ```bash
  pip3 install expecttest
  ```

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行
  在本样例根目录下执行如下步骤，运行该样例。

  ```bash
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程
  python3 ../add_custom_test.py    # 执行样例
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品 与 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  Ran 1 test in **s
  OK
  ```
