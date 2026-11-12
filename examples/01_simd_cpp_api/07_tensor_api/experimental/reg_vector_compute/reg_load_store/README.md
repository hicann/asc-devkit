# 寄存器搬入与搬出接口样例

## 概述

本样例使用实验性 Tensor API 演示 UB 与寄存器之间的数据搬运。场景 1 和场景 2 使用 `unalign_load` 和 `unalign_store` 完成非对齐复制和分块归约；场景 3 使用 `load<bool>` 和 `store` 完成对齐的掩码寄存器搬运。GM 与 UB 之间通过 `copy` 搬运，流水同步使用 `asc::te::mutex`。

通过 `SCENARIO_NUM` 选择场景：

| 场景 | 输入 | 输出 | 功能 |
| --- | --- | --- | --- |
| 1 | `float [1024]` | `float [1024]` | 从第 1 个元素开始非对齐复制两个寄存器，即 128 个元素；其余输出元素为 0。 |
| 2 | `uint16_t [14, 255]` | `uint16_t [14, 16]` | 4 个核按行分工，每核最多 4 行，尾核处理 2 行；每 16 个输入元素取最大值，最后一个数据块只包含 15 个有效元素。 |
| 3 | `uint32_t [16]` | `uint32_t [32]` | 掩码寄存器的正常、上采样和下采样搬入，以及正常和压缩搬出。 |

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR&950DT系列产品 | >= CANN 9.3.0 |

## 目录结构介绍

```text
├── CMakeLists.txt                 // 编译工程及 CTest 校验入口
├── data_utils.h                   // 样例本地工具头文件
├── README.md                      // 中文说明
├── README_en.md                   // 英文说明
└── reg_load_store.asc             // 核函数、输入生成和结果校验
```

## 样例描述

### 场景 1：地址非对齐复制

1. 将输入从 GM 搬入 UB，并使用 `fill/store` 将输出 UB 清零。
2. 在核函数中通过 `slice` 创建从第 1 个元素开始的源、目的 Tensor 视图，计算 `repeat_time = ceil(copy_elements / elements_per_register)` 并传入 VF。在 `Ascend 950PR/Ascend 950DT` 上每个 `float` 寄存器包含 64 个元素，循环执行 2 次。
3. 在 VF 中创建 `unalign_load` 和 `unalign_store` 会话，并在循环外调用一次 `loader.prepare(src_stream)`。
4. 每次循环以 `repeat * elements_per_register` 为相对元素坐标，通过 `loader.load(src_stream, coord)` 加载一个完整寄存器，再通过 `storer.store(dst_stream, coord, src, elements_per_register)` 连续存储。
5. 调用 `storer.flush(dst_stream, repeat_time * elements_per_register)` 提交尚未写出的数据，再将输出搬回 GM。输出索引 1 至 128 与输入一致，其余元素为 0。

核心代码如下：

```cpp
asc::te::experimental::unalign_load loader;
asc::te::experimental::unalign_store storer;
loader.prepare(src_stream);
for (uint32_t repeat = 0; repeat < repeat_time; ++repeat) {
    auto coord = asc::te::make_coord(repeat * elements_per_register);
    auto src = loader.load(src_stream, coord);
    storer.store(dst_stream, coord, src, elements_per_register);
}
storer.flush(dst_stream, static_cast<int32_t>(repeat_time * elements_per_register));
```

### 场景 2：多核地址和数据量非对齐归约

1. 使用 `block_idx` 计算当前核的起始行和实际行数，将各核输入按连续的一维数据搬入 UB。
2. 每行包含 255 个 `uint16_t` 元素，行间不插入填充；后续行的 UB 起始地址非 32 字节对齐，后续核的 GM 输入地址也非 32 字节对齐。
3. 在核函数中为每行创建源、目的 Tensor 视图，计算 `repeat_time = ceil(input_columns / elements_per_register)` 并传入 VF。在循环外调用一次 `loader.prepare(src_row)`，循环内通过 `loader.load(src_row, make_coord(repeat * elements_per_register))` 读取两个 256 字节寄存器。源 UB 区域额外预留一个寄存器的空间，避免尾部完整寄存器加载访问输出区域。
4. 使用 `update_mask<uint16_t>` 屏蔽最后一个无效元素，调用 `reduce_max<reduce_scope::datablock>` 完成每 32 字节数据块的最大值归约，数据类型由输入推导。
5. 使用 `storer.store` 在目的行的坐标 `repeat * results_per_register` 各非对齐存储 8 个结果，最后调用 `storer.flush(dst_row, repeat_time * results_per_register)`，将输出搬回 GM。

### 场景 3：对齐的掩码寄存器搬运

先将 64 字节输入从 GM 搬入 UB。`load<bool>` 从 `uint32_t` UB Tensor 中读取掩码比特，`store` 直接搬出 `reg_tensor<bool>` 的 `reg` 成员，不读取其 `mask` 成员。以下偏移均以 `uint32_t` 元素为单位，从 0 开始计数。

| 输出偏移 | 写入量 | 搬入与搬出方式 |
| --- | --- | --- |
| 0 | 32 字节 | 从输入起始位置正常搬入并原样搬出。 |
| 8 | 32 字节 | 从输入偏移 4 处搬入 16 字节，将每个比特重复两次后搬出。 |
| 16 | 32 字节 | 从输入起始位置下采样搬入，保留 64 字节输入中的偶数索引比特后搬出。 |
| 24 | 16 字节 | 将正常搬入结果隔位压缩搬出，只保留偶数索引比特。 |
| 28 | 16 字节 | 将上采样结果隔位压缩搬出，恢复输入偏移 4 处的 16 字节。 |

正常搬入、下采样搬入和正常搬出的实际地址按 32 字节对齐；上采样搬入和压缩搬出的实际地址按 16 字节对齐。

### 结果校验

场景 1 校验全部 1024 个输出，包括复制范围之外的零值；场景 2 校验全部 224 个归约结果，包括每行的尾数据块和尾核；场景 3 在主机侧逐比特生成期望值，并校验全部 32 个输出字。ACL 调用失败或结果不一致时程序返回非零值。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  根据 CANN 软件包安装目录加载环境变量：
  ```bash
  source ${install_path}/set_env.sh
  ```
  `${install_path}` 为 CANN 软件包安装目录。

- 样例执行

  下面以场景 1 为例，默认使用 NPU 模式：

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
  make -j
  ./demo
  ```

  运行场景 2 或 3 时，将 `SCENARIO_NUM` 修改为 `2` 或 `3` 后重新配置、编译并执行。输入和 golden 由主程序生成，无需执行数据生成脚本。

  使用 NPU 仿真时，增加 `-DCMAKE_ASC_RUN_MODE=sim` 编译选项：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
  make -j
  ./demo
  ```

  切换运行模式、芯片型号或场景时，建议清除 build 目录中的 CMake 缓存后重新配置。可在 build 目录下执行 `rm CMakeCache.txt`，然后使用所需的 `SCENARIO_NUM` 重新运行上述编译命令。

  > **注意：** 更新 CANN 包中的头文件后，应执行 `make clean` 再 `make -j`，或使用新的构建目录，避免复用更新前的编译产物。

- 编译选项说明

| 选项 | 可选值 | 说明 |
| --- | --- | --- |
| `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | NPU 运行或 NPU 仿真 |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | 对应 Ascend 950PR/Ascend 950DT |
| `SCENARIO_NUM` | `1`（默认）、`2`、`3` | 非对齐复制、datablock 最大值归约或掩码寄存器搬运 |

- 执行结果

验证成功时，对应场景输出：

```text
scenario 1: test pass!
scenario 2: test pass!
scenario 3: test pass!
```
