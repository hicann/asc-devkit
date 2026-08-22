# Truncate Reg API样例

## 概述

本样例介绍如何使用实验性寄存器 Tensor API `asc::te::trunc` 对浮点输入执行逐元素向零取整。样例输入和输出均为 `[16, 16]` 的二维 ND Tensor，共包含256个元素，输入与输出数据类型保持一致。

计算关系如下：

```text
y[i] = trunc(x[i])
```

例如，`3.75` 的输出为 `3.0`，`-3.75` 的输出为 `-3.0`。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```text
├── CMakeLists.txt              // 编译工程
├── README.md                   // 样例说明
├── truncate_reg_api.asc        // 算子Kernel、Host调用与结果校验
└── scripts
    └── gen_data.py             // 输入数据和真值数据生成脚本
```

## 样例描述

- 样例功能：

  `SCENARIO_NUM` 用于选择编译和执行的数据类型场景。

  | SCENARIO_NUM | 输入类型 | 输出类型 | 计算说明 |
  | --- | --- | --- | --- |
  | 1 | `half` | `half` | 对FP16数据逐元素向零取整 |
  | 2 | `bfloat16_t` | `bfloat16_t` | 对BF16数据逐元素向零取整 |
  | 3 | `float` | `float` | 对FP32数据逐元素向零取整 |

- 样例规格：

  <table>
    <tr>
      <td align="center">样例类型（OpType）</td>
      <td colspan="4" align="center">truncate_reg_api</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">样例输入</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">layout</td>
    </tr>
    <tr>
      <td align="center">src</td>
      <td align="center">[16, 16]</td>
      <td align="center">half/bfloat16_t/float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">样例输出</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">layout</td>
    </tr>
    <tr>
      <td align="center">dst</td>
      <td align="center">[16, 16]</td>
      <td align="center">与输入类型一致</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">样例Kernel名</td>
      <td colspan="4" align="center">truncate_reg_api</td>
    </tr>
  </table>

  | SCENARIO_NUM | `input/input_x.bin` | 输入大小 | `output/output.bin` / `output/golden.bin` | 输出大小 |
  | --- | --- | --- | --- | --- |
  | 1 | 256个 `half` | 512 B | 256个 `half` | 512 B |
  | 2 | 256个BF16位模式（`uint16_t` 存储） | 512 B | 256个BF16位模式（`uint16_t` 存储） | 512 B |
  | 3 | 256个 `float` | 1024 B | 256个 `float` | 1024 B |

- 样例实现：

  - Kernel关键步骤

    1. 使用 `asc::te::make_tensor`、`asc::te::make_mem_ptr` 和 `asc::te::make_frame_layout` 构造GM、UB上的二维Tensor。
    2. 使用 `asc_lock/asc_unlock` 包围MTE2流水，通过 `asc::te::copy` 将输入从GM搬运到UB。
    3. 根据寄存器字节数和元素类型大小计算单次处理元素数，使用 `asc::te::all_mask<T>()` 生成全有效掩码。
    4. 使用Tensor的 `load` 接口将UB数据加载到 `asc::te::reg_tensor<T>`，调用 `asc::te::trunc` 向零取整，再通过Tensor的 `store` 接口写回UB。
    5. 使用同一个mutex依次锁定Vector和MTE3流水，再将结果从UB搬运回GM。

    核心计算代码如下：

    ```cpp
    const auto coord = asc::te::make_coord(offset / columnCount, offset % columnCount);
    auto srcReg = src.load(coord);
    srcReg.with_mask(asc::te::all_mask<T>());

    auto dstReg = asc::te::trunc(srcReg);
    dst.store(coord, dstReg);
    ```

  - 调用实现

    Host侧读取 `input/input_x.bin`，申请Host和Device内存，并通过 `<<<1, 0, stream>>>` 启动一个Vector Block。Kernel执行完成后，Host侧将结果写入 `output/output.bin`，并与 `output/golden.bin` 逐字节比较。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量

  配置CANN软件包环境变量，详细操作请参考[环境变量配置](../../../../../docs/quick_start.md#prepare&install)。

  ```bash
  source ${install_path}/set_env.sh
  ```

  `${install_path}` 为CANN软件包安装目录，请根据实际安装路径替换。

- 样例执行

  下面以场景1为例：

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
  make -j
  python3 ../scripts/gen_data.py -scenarioNum=${SCENARIO_NUM}
  ./demo
  ```

  生成数据时传入的 `scenarioNum` 必须与编译时的 `SCENARIO_NUM` 一致。运行其他场景时，将其修改为1至3之间的对应值。

  使用NPU仿真时，可分别增加以下编译选项：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=${SCENARIO_NUM} ..
  ```

  切换运行模式、芯片型号或场景时，建议清除 `build` 目录中的CMake缓存后重新配置。

- 编译选项说明

  | 选项 | 说明 |
  | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | 算子执行模式，可选 `npu`、`sim`，默认值为 `npu`。 |
  | `CMAKE_ASC_ARCHITECTURES` | NPU芯片型号，默认值为 `dav-3510`。 |
  | `SCENARIO_NUM` | 数据类型场景编号，必选，取值范围为1至3。 |

- 执行结果

  执行成功时输出：

  ```text
  test pass!
  ```
