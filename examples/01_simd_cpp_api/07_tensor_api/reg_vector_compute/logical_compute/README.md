# Tensor API逻辑计算样例

## 概述

本样例基于Tensor API演示寄存器逻辑计算接口中的标量左移操作。样例覆盖`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`和`uint32_t`六种整数类型，每种类型输入64个元素，执行左移1位后输出。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 目录结构介绍

```plain
├── logical_compute
│   ├── scripts
│   │   ├── gen_data.py          // 输入数据和真值数据生成脚本
│   │   └── verify_result.py     // 输出结果校验脚本
│   ├── CMakeLists.txt           // 编译工程文件
│   ├── data_utils.h             // 数据读写工具
│   ├── README.md
│   ├── README_En.md
│   └── logical_compute_tensor_api.asc  // Ascend C算子实现与调用样例
```

## 样例描述

- 样例功能：
  对输入Tensor中的每个元素执行标量左移计算，计算公式如下：

  ```text
  output_i = input_i << 1
  ```

- 样例规格：
  <table>
  <caption>表1：样例输入输出规格</caption>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">logical_compute_tensor_api</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1, 64]</td><td align="center">int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1, 64]</td><td align="center">int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">logical_compute_tensor_api</td></tr>
  </table>

- 样例实现：

  - Kernel实现

    1. 使用`asc::te::make_tensor`、`asc::te::make_mem_ptr`和`asc::te::make_frame_layout`创建GM和UB上的Tensor对象。
    2. 使用`asc::te::make_copy`和`asc::te::copy`完成GM到UB的数据搬运。
    3. 使用Tensor `load`接口加载为`reg_tensor`，通过`make_mask<mask_pattern::vl64, T>()`指定64个元素参与计算。
    4. 导入`asc::te::experimental::operator<<`，执行`value << 1`，并使用Tensor `store`接口写回UB。
    5. 使用`asc::te::copy`将结果从UB搬运回GM。

  - 调用实现

    Host侧程序读取`scripts/gen_data.py`生成的输入文件，使用内核调用符`<<<>>>`调用核函数，执行完成后保存输出文件并通过`scripts/verify_result.py`进行逐元素校验。

## 核心接口说明

### asc::te::experimental::operator<<

```cpp
template <typename T, typename U>
__simd_callee__ inline decltype(auto) operator<<(const reg_tensor<T>& src, const U& shift);
```

该接口对输入`reg_tensor`执行标量左移计算，返回相同元素类型的`reg_tensor`。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的安装路径配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

- 样例执行

  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..
  make -j
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构 |
  | `CANN_ASC_USE_EXPERIMENTAL` | `ON`（本样例必选）、`OFF`（默认） | 开启实验性ASC接口 |

- 执行结果

  执行成功时输出如下：

  ```text
  logical_compute_tensor_api executed successfully.
  test pass! verified scalar shift-left for six integer types.
  ```
