# Data Reorder Tensor API 样例

## 概述

本样例演示实验性寄存器 Tensor API `asc::te::experimental::deinterleave`。样例读取两组各包含256个 `float` 元素的输入，每次将两个寄存器中的数据解交织后写入两组输出。

## 目录结构

```text
data_reorder_api
├── scripts/gen_data.py
├── CMakeLists.txt
├── data_reorder_api.asc
├── data_utils.h
└── README.md
```

## 样例实现

1. 使用 Tensor API `copy` 将两组输入从GM搬运到UB，并使用 `asc_lock/asc_unlock` 管理MTE2流水。
2. 在Vector流水中通过 `asc::te::experimental::load` 加载寄存器，调用 `asc::te::experimental::deinterleave` 完成解交织，再通过 `asc::te::experimental::store` 写回UB。
3. 使用 Tensor API `copy` 将两组输出从UB搬运到GM，并使用 `asc_lock/asc_unlock` 管理MTE3流水。

## 编译运行

在本目录执行：

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
make -j
python3 ../scripts/gen_data.py
./demo
```

使用NPU仿真模式时，在CMake命令中增加 `-DCMAKE_ASC_RUN_MODE=sim`。切换运行模式前，应清理CMake缓存并重新配置。

### 编译选项说明

| 选项 | 可选值 | 说明 |
|------|--------|------|
| `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构 |
| `CANN_ASC_USE_EXPERIMENTAL` | `ON`（本样例必选）、`OFF`（默认） | 开启实验性ASC接口 |

运行成功时输出：

```text
test pass!
```
