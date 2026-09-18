# ELU样例

## 概述

本样例基于C API编程接口实现ELU（Exponential Linear Unit）激活运算，主要调用 `asc_exp`、`asc_mul_scalar`、`asc_select` 接口。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── elu
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── elu.asc                        // AscendC样例实现 & 调用样例
│   ├── README.md                      // 样例介绍
│   └── README_en.md                   // 样例介绍（英文版）
```

## 样例描述

- 样例功能：  
  对输入做逐元素的ELU激活运算。数学表达式为：
  $$
  ELU(x)=
  \begin{cases}
  scale \cdot x, & x > 0 \\
  \alpha \cdot scale \cdot (e^{x \cdot inputScale} - 1), & x \le 0
  \end{cases}
  $$

- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">dst</td><td align="center">与src一致</td><td align="center">float / float16</td></tr>
  <tr><td rowspan="5" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">src</td><td align="center">任意合法shape</td><td align="center">float / float16</td></tr>
  <tr><td align="center">alpha</td><td align="center">标量</td><td align="center">float</td></tr>
  <tr><td align="center">scale</td><td align="center">标量</td><td align="center">float</td></tr>
  <tr><td align="center">input_scale</td><td align="center">标量</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="3" align="center">elu_custom（fp32）/ elu_custom_half（fp16）</td></tr>
  </table>

- 样例实现：  
  采用段式动态tiling将输入分段搬入UB，在寄存器域（`vector_float`）完成ELU计算后搬出至GM；fp16在fp32域计算后转回half写回。
  - 调用实现
    使用内核调用符`<<<>>>`调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;                                                    # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                           # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                                 # 生成测试输入数据
  ./demo                                                                         # 执行编译生成的可执行程序
  ```

  使用 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  | --- | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明精度对比成功。

  ```bash
  [Success] File mode verification passed.
  ```

  内置全量自测（验证算子在不同 shape 下的正确性）：

  ```bash
  ./demo --selftest
  ```

  输出 `[Success] All cases verification passed.`，程序返回 0。
