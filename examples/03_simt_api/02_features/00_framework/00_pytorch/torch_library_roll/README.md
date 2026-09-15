# torch.library注册自定义算子直调样例

## 概述

本样例基于Roll算子展示如何使用PyTorch的torch.library机制注册自定义算子。
## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|---|---|
| Ascend 950PR/Ascend 950DT | \>= CANN 9.1.0 |

## 目录结构介绍

```
├── torch_library_roll
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── roll_custom.asc         // Ascend C算子实现 & torch.library注册
│   ├── roll_custom_test.py     // PyTorch调用脚本
│   └── figures                 // README中的图片资源
```

## 样例描述

- 样例功能：

  Roll算子对2维输入张量沿指定维度 `dims` 进行元素滚动，滚动步数为 `shifts`。超出边界的元素从另一侧绕回。

  以 `input.shape=[4,4]`、`shifts=1`、`dims=0` 为例，沿dim0（行方向）正向滚动1步的效果如下：

  <p align="center">
    <img src="./figures/roll.png" width="50%">
     </p>
  <p align="center">
  图1：Roll算子滚动效果示意
  </p>

  每行元素向下移动一行，最后一行绕回到第一行。

- 样例规格：
  <table border="2" align="center">
  <caption>表1：RollCustom样例规格描述</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">RollCustom</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[8,2048]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td align="center">shifts</td><td align="center">[1]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td align="center">dims</td><td align="center">[1]</td><td align="center">int32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[8,2048]</td><td align="center">float32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">roll_custom</td></tr>
  </table>

- 自定义算子注册：

  本样例在`roll_custom.asc`中按以下顺序实现了自定义算子的完整注册与调用链路，如下图所示：

  <p align="center">
    <img src="./figures/自定义算子注册.png" width="50%">
     </p>
  <p align="center">
  图2：自定义算子注册
  </p>


  （1）实现Device侧核函数

  `roll_custom`是本样例的核函数，使用Ascend C SIMT编程模型在Device侧实现Roll算子的滚动逻辑。

  （2）实现Host侧函数

  `ascendc_roll`是Host侧的函数，定义在`ascendc_ops`命名空间中，负责调用Device核函数。该函数首先通过`c10_npu::getCurrentNPUStream()`获取当前NPU上的流（用于管理算子执行的异步任务队列），随后将获取到的流作为内核调用符`<<<>>>`的输入参数，调用核函数`roll_custom`在NPU上异步执行算子。

  （3）完成算子注册

  使用PyTorch的`TORCH_LIBRARY`和`TORCH_LIBRARY_IMPL`宏完成自定义算子的声明与分发绑定。

  PyTorch提供`TORCH_LIBRARY`宏作为自定义算子注册的核心接口，用于创建并初始化自定义算子库，注册后在Python侧可以通过`torch.ops.namespace.op_name`方式进行调用：

  ```cpp
  TORCH_LIBRARY(ascendc_ops, m) {
      m.def("ascendc_roll(Tensor x, int[] shifts, int[] dims) -> Tensor");
  }
  ```

  `TORCH_LIBRARY_IMPL`用于将算子绑定到特定的`DispatchKey`（PyTorch设备调度标识）。针对NPU设备，需要将算子实现注册到`PrivateUse1`这一专属的`DispatchKey`上：

  ```cpp
  TORCH_LIBRARY_IMPL(ascendc_ops, PrivateUse1, m)
  {
      m.impl("ascendc_roll", TORCH_FN(ascendc_ops::ascendc_roll));
  }
  ```

  注册完成后，在Python侧即可通过`torch.ops.ascendc_ops.ascendc_roll(...)`直接调用该自定义算子。

- Python调用

  在`roll_custom_test.py`调用脚本中，通过`torch.ops.load_library`加载生成的自定义算子库，调用注册的`ascendc_roll`函数，并通过对比NPU输出与CPU标准roll结果来验证自定义算子的数值正确性。

## 编译运行

- 安装PyTorch以及Ascend Extension for PyTorch插件

  请参考[pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch)开源代码仓或[Ascend Extension for PyTorch昇腾社区](https://hiascend.com/document/redirect/Pytorch-index)的安装说明，选取支持的`Python`版本配套发行版，完成`torch`和`torch-npu`的安装。

- 安装前置依赖

  ```bash
  pip3 install expecttest     # torch_npu.testing.testcase 的依赖
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                # 编译工程
  python3 ../roll_custom_test.py   # 执行样例
  ```

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　 | 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  Ran 1 test in **s
  OK
  ```
