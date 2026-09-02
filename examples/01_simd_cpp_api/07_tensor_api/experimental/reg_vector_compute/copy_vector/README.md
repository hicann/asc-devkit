# Copy Vector Tensor API样例

## 概述

本样例介绍 Tensor API 在 AIV 侧的 `GM->UB`、`UB->GM` 和 `UB->UB` 搬运，并演示多种分形的搬运场景，
以及`GM->UB`通路的左右常量填充场景。场景1至3使用shape为`[32, 32]`的`int8_t`tensor，场景4使用
shape为`[1024]` 的一维 tensor，场景5将 `[32, 24]` 输入填充为 `[32, 32]` 输出。每个场景均做完整输出校验。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 样例场景

| SCENARIO_NUM | 重点通路 | Layout | 维度 | 主要写法 |
| --- | --- | --- | --- | --- |
| 1 | GM->UB | ND | 四维 | `copy(dst, src)` 自动推导通路 |
| 2 | UB->GM | DN | 四维 | `make_copy(..., trait)` 后调用 `atom.call(dst, src)` |
| 3 | UB->UB | NZ | 四维 | 显式 atom 与坐标/shape 区域搬运重载 |
| 4 | GM->UB | ND | 一维 | `make_shape(1024)`、`make_stride(1)`，走 one-dim copy 路由 |
| 5 | GM->UB | ND | 四维 | 左填充3个、右填充5个元素，并使用 `asc_set_copy_pad_val` 配置填充值-9 |

五个场景的 Kernel 均声明为 `__global__ __vector__`。

## 目录结构

```text
├── CMakeLists.txt
├── README.md
├── README_En.md
├── copy_vector.asc
├── data_utils.h
└── scripts
    └── gen_data.py
```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。**当前仅支持使用[CANN master](../../../../../../docs/zh/quick_start.md#cann-install)**。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

  ```bash
  source ${install_path}/set_env.sh
  ```

  `${install_path}` 为CANN软件包安装目录，请根据实际安装路径替换。

- 样例执行

  下面以场景1为例：

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  python3 ../scripts/gen_data.py --scenario_num=${SCENARIO_NUM}
  ./demo
  ```

  生成数据时传入的 `scenario_num` 必须与编译时的 `SCENARIO_NUM` 一致。运行其他场景时，将其修改为1至6之间的对应值。

  使用NPU仿真时，可分别增加以下编译选项：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=${SCENARIO_NUM} -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  ```

  切换运行模式、芯片型号或场景时，建议清除 `build` 目录中的CMake缓存后重新配置。

- 编译选项说明

  | 选项 | 说明 |
  | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | 算子执行模式，可选 `npu`、`sim`，默认值为 `npu`。 |
  | `CMAKE_ASC_ARCHITECTURES` | NPU芯片型号，默认值为 `dav-3510`。 |
  | `CANN_ASC_USE_EXPERIMENTAL` | 实验性ASC接口开关，本样例必须设为 `ON`，默认值为 `OFF`。 |
  | `SCENARIO_NUM` | 类型转换场景编号，必选，取值范围为1至6。 |

- 执行结果

  执行成功时输出：

  ```text
  test pass!
  ```
