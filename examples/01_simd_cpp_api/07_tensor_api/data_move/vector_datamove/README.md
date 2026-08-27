# Vector Datamove样例

## 概述

本样例介绍 Tensor API 在 AIV 侧的四个 Vector 搬运通路：`GM->UB`、`UB->GM`、`UB->UB` 和
`UB->L1`，并演示 `GM->UB` 搬运的左右常量填充。场景1至4使用 `[32, 32]` 的 `int8_t` Tensor；场景5将 `[32, 24]` 输入填充为 `[32, 32]` 输出。每个场景均做完整输出校验。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 样例场景

| SCENARIO_NUM | 重点通路 | Layout | 主要写法 |
| --- | --- | --- | --- |
| 1 | GM->UB | ND | `copy(dst, src)` 自动推导通路 |
| 2 | UB->GM | DN | `make_copy(..., trait)` 后调用 `atom.call(dst, src)` |
| 3 | UB->UB | NZ | 显式 atom 与坐标/shape 区域搬运重载 |
| 4 | UB->L1 | ZN | 显式 `copy(atom, dst, src)` 并验证 AIV 搬运可完成执行 |
| 5 | GM->UB | ND | 左填充3个、右填充5个元素，并使用 `asc_set_copy_pad_val` 配置填充值 |

五个场景的 Kernel 均声明为 `__global__ __vector__`。场景中的辅助搬运仅用于准备输入或将结果回传
到 GM。由于 dav-3510 的 `L1->UB` 通路只在 AIC 生效，场景4无法在纯 AIV Kernel 中回读 L1；该场景
验证 `UB->L1` 能正确编译、下发并完成同步，然后从源 UB 回传数据作为 Kernel 完成性校验。

场景5通过 `gm_to_ub_params{3, 5, true}` 配置每行左侧3个、右侧5个 padding，并通过
`asc_set_copy_pad_val(PADDING_VALUE)` 将 padding 元素配置为 `-9`。

## 目录结构

```text
├── CMakeLists.txt
├── README.md
├── README_En.md
├── vector_datamove.asc
├── data_utils.h
└── scripts
    └── gen_data.py
```

## 编译运行

配置CANN环境后，在样例目录下执行。以下以场景1的 sim 模式为例：

```bash
source ${install_path}/set_env.sh
cmake -S . -B build \
    -DCMAKE_ASC_RUN_MODE=sim \
    -DCMAKE_ASC_ARCHITECTURES=dav-3510 \
    -DSCENARIO_NUM=1
cmake --build build -j
cd build
python3 ../scripts/gen_data.py -scenarioNum=1
./demo
```

生成数据时的 `scenarioNum` 必须与编译时的 `SCENARIO_NUM` 保持一致。

运行成功时输出：

```text
test pass!
```
