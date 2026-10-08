# Ascend C算子接入PyTorch图模式样例

## 概述

本样例以AddCustom算子为例，介绍Ascend C算子如何通过TorchAir接入PyTorch图模式。用户在模型中调用 `torch.ops.ascendc_ops.ascendc_add` 后，`torch.compile` 负责捕获该调用，TorchAir负责将其转换为GE `AddCustom`节点，最终由GE执行入口启动Ascend C kernel。

> **什么是TorchAir图模式接入？** <br>
> TorchAir承担PyTorch图到GE图的转换工作。关于PyTorch侧接口如何进入TorchAir/GE图执行链路，可参考[TorchAir图模式接入概述](https://www.hiascend.com/document/detail/zh/Pytorch/2600/modthirdparty/torchairuseguide/docs/zh/custom_op_graph/overview.md)。

**核心链路**：

1. 在 `add_custom_kernel.asc` 中实现Ascend C Add kernel。
2. 在 `add_custom_ge.asc` 中实现GE `AddCustom` 交付件，包括算子原型、shape/dtype推导和 `Execute` 执行入口。
3. `add_custom_test.py` 通过 `torch.ops.load_library` 加载编译生成的 `libascendc_ops.so`，使PyTorch侧接口和GE `AddCustom` 交付件注册生效。
4. 在 `add_custom_pytorch.asc` 和 `add_custom_test.py` 中补充PyTorch侧schema/Meta注册和TorchAir converter，使模型中的 `ascendc_add` 节点可以映射到GE `AddCustom` 节点。
5. 通过 `add_custom_test.py` 使用TorchAir后端执行 `torch.compile` 图模式测试，并校验GE图执行结果。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.1.0 |
| Atlas A2系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```text
├── ge_torchair
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── add_custom_kernel.asc   // Ascend C kernel实现
│   ├── add_custom_pytorch.asc  // PyTorch侧schema和Meta注册
│   ├── add_custom_ge.asc       // GE AddCustom原型注册和执行实现
│   ├── add_custom_test.py      // TorchAir图模式验证脚本
│   └── README.md               // 样例说明文档
```

## 样例实现

### Ascend C计算实现

`add_custom_kernel.asc` 实现Add计算对应的Ascend C kernel，Add的计算公式为：

$$
z = x + y
$$

- x：输入，形状为 `[8, 2048]`，数据类型为float，格式为ND；
- y：输入，形状为 `[8, 2048]`，数据类型为float，格式为ND；
- z：输出，形状为 `[8, 2048]`，数据类型为float，格式为ND。

### GE交付件实现

`add_custom_ge.asc` 提供GE侧需要的 `AddCustom` 原型、shape/dtype推导和执行入口。GE运行到 `AddCustom` 节点时进入执行入口，并启动 `add_custom_kernel.asc` 中的Ascend C kernel。

通过算子原型构建GE节点的基础概念可参考[什么是算子原型](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta1/programug/graphdevg/atlasag_25_0008.html)。

### TorchAir入图需要的内容

`add_custom_pytorch.asc` 提供PyTorch侧可见的 `ascendc_ops::ascendc_add` 接口。图捕获阶段主要需要两类信息：接口签名和Meta实现。

接口签名用于让Python侧能够通过 `torch.ops.ascendc_ops.ascendc_add(x, y)` 找到该接口：

```cpp
TORCH_LIBRARY_FRAGMENT(ascendc_ops, m)
{
    m.def("ascendc_add(Tensor x, Tensor y) -> Tensor");
}
```

`torch.compile` 捕获图时不会执行真实NPU计算，但需要推导输出Tensor信息，因此还需要注册Meta实现：

```cpp
TORCH_LIBRARY_IMPL(ascendc_ops, Meta, m)
{
    m.impl("ascendc_add", TORCH_FN(AscendcAddMeta));
}
```

### TorchAir到GE节点转换

`add_custom_test.py` 注册TorchAir converter，将 `torch.ops.ascendc_ops.ascendc_add` 转换为GE `AddCustom`节点：

```python
@torchair.register_fx_node_ge_converter(torch.ops.ascendc_ops.ascendc_add.default)
def convert_ascendc_add(x, y, z=None, meta_outputs=None):
    return torchair.ge.custom_op(
        "AddCustom",
        inputs={"x": x, "y": y},
        outputs=["z"],
    )
```

### 加载与图模式执行

实现和注册内容编译进 `libascendc_ops.so` 后，`add_custom_test.py` 先加载该动态库，使PyTorch侧接口和GE `AddCustom` 交付件注册生效：

```python
torch.ops.load_library(LIB_PATH)
```

模型中通过 `torch.ops.ascendc_ops.ascendc_add` 调用该接口，再交给 `torch.compile` 捕获并使用TorchAir后端执行：

```python
class AddCustomModel(torch.nn.Module):
    def forward(self, x, y):
        return torch.ops.ascendc_ops.ascendc_add(x, y)

opt_model = torch.compile(model, fullgraph=True, backend=npu_backend, dynamic=False)
```

编译后的模型在NPU侧执行，GE运行到 `AddCustom` 节点时进入 `add_custom_ge.asc` 中的执行入口，并启动 `add_custom_kernel.asc` 中的Ascend C kernel。

## 编译运行

- 安装PyTorch、Ascend Extension for PyTorch插件以及TorchAir

  请参考[pytorch: Ascend Extension for PyTorch](https://gitcode.com/Ascend/pytorch)开源代码仓、[Ascend Extension for PyTorch昇腾社区](https://hiascend.com/document/redirect/Pytorch-index)和TorchAir配套安装说明，选取支持的 `Python` 版本配套发行版，完成 `torch`、`torch-npu` 和 `torchair` 的安装。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例根目录下执行如下步骤，运行该样例。

  ```bash
  # 创建并进入build目录
  mkdir -p build && cd build;

  # 编译单算子，生成libascendc_ops.so
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;

  # 加载libascendc_ops.so，并执行TorchAir图模式测试
  python3 ../add_custom_test.py
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：`dav-2201` 对应Atlas A2系列产品与Atlas A3系列产品，`dav-3510` 对应Ascend 950PR&950DT系列产品 |

- 执行结果

  `add_custom_test.py` 会在CPU侧生成输入Tensor和标准加法结果，再将输入拷贝到NPU侧执行编译后的图模式模型。执行完成后，脚本将NPU输出拷回CPU，并通过 `torch.testing.assert_close` 对比图模式输出和CPU标准结果。

  执行结果如下，说明TorchAir图模式执行结果与CPU标准加法结果对齐。

  ```bash
  Ran 1 test in **s
  OK
  ```
