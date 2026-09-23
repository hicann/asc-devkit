# reduce_sum/reduce_max/reduce_min接口样例

## 概述

本样例组合实验性 Tensor API 的寄存器加载、element scope归约和存储能力，演示 `asc::te::experimental::reduce_sum`、`reduce_max` 和 `reduce_min` 的用法。输入 `src` 是 shape 为 `[64]` 的一维 `float` Tensor，三个归约结果分别写入输出 Tensor 的三个 64 元素结果区。

计算关系如下：

```text
dst[0]   = sum(src[0:64])
dst[64]  = max(src[0:64])
dst[128] = min(src[0:64])
```

每个归约结果区只有首元素有效，其余元素不参与结果校验。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```text
├── CMakeLists.txt          // 编译工程
├── README.md               // 中文样例说明
├── README_en.md            // 英文样例说明
├── data_utils.h            // Host侧读写二进制文件工具
├── reduction_compute.asc   // 算子Kernel、Host调用与结果校验
└── scripts
    └── gen_data.py         // 输入数据和真值数据生成脚本
```

## 样例描述

- 样例功能：

  样例从 `input/input.bin` 读取输入，将输入 Tensor 从 GM 搬运到 UB，通过 `load` 加载到寄存器，依次调用三个实验性 Reg Tensor 归约接口，再通过 `store` 将结果写回 UB 和 GM。Host 将完整输出保存到 `output/output.bin`，并使用 `output/golden.bin` 中的三个真值校验有效结果。

- 样例规格：

  | 文件 | 内容 | 大小 |
  | --- | --- | --- |
  | `input/input.bin` | 64个 `float` 输入元素 | 256 B |
  | `output/output.bin` | 3个长度为64的 `float` 结果区 | 768 B |
  | `output/golden.bin` | 按 sum、max、min 顺序存放的3个 `float` 真值 | 12 B |

- 样例实现：

  - Kernel关键步骤

    1. 使用 `asc::te::make_tensor`、`asc::te::make_mem_ptr` 和 layout 构造 GM、UB Tensor。
    2. 使用 `asc_lock/asc_unlock` 包围 MTE2 流水，通过 `make_copy(copy_gm_to_ub{})` 和 `copy` 将输入从 GM 搬运到 UB。
    3. 使用 `all_mask<float>` 和 `load` 将64个 `float` 数据加载为 Reg Tensor。
    4. 调用 `reduce_sum<reduce_scope::element, float>`、`reduce_max` 和 `reduce_min` 完成 element scope归约。
    5. 使用 `store` 写入三个 UB 结果区，再通过 `make_copy(copy_ub_to_gm{})` 和 `copy` 写回 GM。

  核心计算代码如下：

  ```cpp
  const auto coord = asc::te::make_coord(0);
  auto mask = asc::te::experimental::all_mask<float>();
  auto src = asc::te::experimental::load(src_tensor, coord).with_mask(mask);

  auto sum = asc::te::experimental::reduce_sum<asc::te::experimental::reduce_scope::element, float>(src);
  auto max = asc::te::experimental::reduce_max(src);
  auto min = asc::te::experimental::reduce_min(src);

  asc::te::experimental::store(sum_tensor, coord, sum);
  asc::te::experimental::store(max_tensor, coord, max);
  asc::te::experimental::store(min_tensor, coord, min);
  ```

  `reduce_sum` 还支持 `reduce_scope::datablock` 和 `reduce_scope::pair`，需要按 `<scope, DstType>` 显式指定归约范围和输出类型；`reduce_max`、`reduce_min` 还支持 `reduce_scope::datablock`，输出类型由输入 `reg_tensor` 的元素类型推导，显式指定归约范围时只需传入 `<scope>`。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  ```bash
  source ${install_path}/set_env.sh
  ```

  `${install_path}` 为 CANN 软件包安装目录，例如 `/usr/local/Ascend/cann`。

- 编译并执行

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  python3 ../scripts/gen_data.py
  ./demo
  ```

  在支持的 NPU 环境运行时，可省略 `-DCMAKE_ASC_RUN_MODE=sim`，默认运行模式为 `npu`。切换运行模式或芯片型号时，建议先清除 `build` 目录中的 CMake 缓存。

- 编译选项说明

  | 选项 | 说明 |
  | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | 算子执行模式，可选 `npu`、`sim`，默认值为 `npu`。 |
  | `CMAKE_ASC_ARCHITECTURES` | NPU芯片型号，本样例仅支持 `dav-3510`。 |
  | `CANN_ASC_USE_EXPERIMENTAL` | 实验性ASC接口开关，本样例必须设为 `ON`。 |

- 执行结果

  验证成功时输出：

  ```text
  test pass!
  ```
