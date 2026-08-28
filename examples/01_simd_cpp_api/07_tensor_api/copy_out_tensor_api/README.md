# 基于Tensor API实现的copy搬出样例

## 概述

本样例基于Tensor API编程方式实现带Bias的动态Shape矩阵乘法运算，展示了copy搬出接口（L0C到GM、L0C到UB）和mmad（矩阵乘加）等Tensor API的使用方法。通过编译时参数`SCENARIO_NUM`选择不同的回写模式，Shape参数通过`MatmulTiling`结构体在运行时动态传入。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | > CANN 9.1.0 |

> **说明：** 该样例依赖尚未正式发布的CANN特性，请使用最新的CANN master包。

## 目录结构介绍

```text
├── copy_out_tensor_api
│   ├── scripts
│   │   ├── gen_data.py                    // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py               // 真值对比文件
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── data_utils.h                       // 数据读入写出函数
│   ├── copy_out_tensor_api.asc            // Ascend C样例实现 & 调用样例
│   └── README.md                          // 样例说明文档
```

## 样例描述

- 样例功能：
  本样例实现了带Bias的动态Shape多核Matmul功能，通过copy接口完成L0C到GM或L0C到UB的数据搬运，通过mmad接口完成矩阵乘加计算。

  1. 动态Matmul功能

  本样例实现了多核Matmul功能。Shape参数（M、N、K、singleCoreM、singleCoreN、singleCoreK）通过`MatmulTiling`结构体在运行时动态传入kernel。`BASE_M`、`BASE_N`、`BASE_K`和`STEP_M`、`STEP_N`、`STEP_K`作为编译期模板参数，决定了L0/L1级buffer的分配大小。

  2. Bias功能

  本样例在Matmul计算的首次迭代中通过mmad接口将Bias向量加到矩阵乘结果上，Bias为float类型的一维向量，长度为N。

  3. 多场景支持

  通过编译时宏参数`SCENARIO_NUM`选择不同的回写模式：

  | SCENARIO_NUM | 回写模式 | 回写路径 | 说明 |
  | :--- | :--- | :--- | :--- |
  | 0 | 普通模式 | L0C -> GM | 使用`copy_l0c_to_gm`直接将计算结果写回GM |
  | 1 | unitFlag模式 | L0C -> GM | 在`copy_l0c_to_gm`中显式使能`unitFlag` |
  | 2 | UB搬运模式M拆分 | L0C -> UB -> GM | `copy_l0c_to_ub`采用`DUAL_DST_SPLIT_M`，由AIV按M维拆分回写 |
  | 3 | UB搬运模式N拆分 | L0C -> UB -> GM | `copy_l0c_to_ub`采用`DUAL_DST_SPLIT_N`，由AIV按N维拆分回写 |

  - 普通模式（SCENARIO_NUM=0/1）：L0C结果直接搬运到GM，场景1额外使能`unitFlag`
  - UB搬运模式（SCENARIO_NUM=2/3）：L0C结果先搬到UB，再由AIV侧写回GM

  注：A/B矩阵在所有场景下数据类型均为half，C矩阵和Bias在所有场景下数据类型均为float。

  4. MatmulTiling结构体定义

  ```cpp
  struct MatmulTiling {
      int32_t m;            // 矩阵A的行数
      int32_t n;            // 矩阵B的列数
      int32_t k;            // 矩阵A的列数/矩阵B的行数
      int32_t singleCoreM;  // 单核处理的M维度大小
      int32_t singleCoreN;  // 单核处理的N维度大小
      int32_t singleCoreK;  // 单核处理的K维度大小
  };
  ```

- 样例规格：
  在核函数直调样例中，样例默认支持的shape为：M = 1024, N = 1024, K = 256。

  **场景0：普通模式，L0C -> GM**

  <table border="2">
  <caption>表1：场景0规格表</caption>
  <tr><td rowspan="1" align="left">样例类型(OpType)</td><td colspan="4" align="left">Matmul with Bias</td></tr>
  <tr><td rowspan="5" align="left">样例输入</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">x（矩阵A）</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">y（矩阵B）</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling结构体</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">样例输出</td><td align="left">z（矩阵C）</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">核函数名</td><td colspan="4" align="left">copy_out_tensor_api</td></tr>
  </table>

  **场景1：unitFlag模式，L0C -> GM**

  <table border="2">
  <caption>表2：场景1规格表</caption>
  <tr><td rowspan="1" align="left">样例类型(OpType)</td><td colspan="4" align="left">Matmul with Bias</td></tr>
  <tr><td rowspan="5" align="left">样例输入</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">x（矩阵A）</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">y（矩阵B）</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling结构体</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">样例输出</td><td align="left">z（矩阵C）</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">核函数名</td><td colspan="4" align="left">copy_out_tensor_api</td></tr>
  </table>

  **场景2：UB搬运模式M拆分，L0C -> UB -> GM**

  <table border="2">
  <caption>表3：场景2规格表</caption>
  <tr><td rowspan="1" align="left">样例类型(OpType)</td><td colspan="4" align="left">Matmul with Bias</td></tr>
  <tr><td rowspan="5" align="left">样例输入</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">x（矩阵A）</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">y（矩阵B）</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling结构体</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">样例输出</td><td align="left">z（矩阵C）</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">核函数名</td><td colspan="4" align="left">copy_out_tensor_api</td></tr>
  </table>

  **场景3：UB搬运模式N拆分，L0C -> UB -> GM**

  <table border="2">
  <caption>表4：场景3规格表</caption>
  <tr><td rowspan="1" align="left">样例类型(OpType)</td><td colspan="4" align="left">Matmul with Bias</td></tr>
  <tr><td rowspan="5" align="left">样例输入</td><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
  <tr><td align="left">x（矩阵A）</td><td align="left">[M, K]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">y（矩阵B）</td><td align="left">[K, N]</td><td align="left">half</td><td align="left">ND</td></tr>
  <tr><td align="left">bias</td><td align="left">[N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td align="left">tiling</td><td align="left">MatmulTiling结构体</td><td align="left">int32_t</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">样例输出</td><td align="left">z（矩阵C）</td><td align="left">[M, N]</td><td align="left">float</td><td align="left">ND</td></tr>
  <tr><td rowspan="1" align="left">核函数名</td><td colspan="4" align="left">copy_out_tensor_api</td></tr>
  </table>

  编译期模板参数默认值：

  | 参数    | 默认值 | 说明                     |
  | :------ | :----- | :----------------------- |
  | BASE_M  | 128    | M维度基础分块大小        |
  | BASE_N  | 128    | N维度基础分块大小        |
  | BASE_K  | 64     | K维度基础分块大小        |
  | STEP_M  | 1      | M维度L1缓存步进          |
  | STEP_N  | 1      | N维度L1缓存步进          |
  | STEP_K  | 4      | K维度L1缓存步进          |

  运行时动态参数默认值：

  | 参数         | 默认值 | 说明                   |
  | :----------- | :----- | :--------------------- |
  | m            | 1024   | 矩阵A的行数            |
  | n            | 1024   | 矩阵B的列数            |
  | k            | 256    | 矩阵A的列数/矩阵B的行数 |
  | singleCoreM  | 256    | 单核M维度大小           |
  | singleCoreN  | 128    | 单核N维度大小           |
  | singleCoreK  | 256    | 单核K维度大小           |

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。**当前仅支持使用[CANN master](../../../../docs/zh/quick_start.md#cann-install)**。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行
  以场景0（普通模式，L0C -> GM）为例：

  ```bash
  SCENARIO_NUM=0
  mkdir -p build && cd build;                                                    # 创建并进入build目录
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # 编译工程，默认NPU模式
  python3 ../scripts/gen_data.py;                                                # 生成测试输入数据
  ./demo;                                                                       # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin        # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。示例如下：

  ```bash
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

  编译其他场景时，修改`SCENARIO_NUM`变量即可，例如编译场景2（UB搬运模式M拆分）时设置`SCENARIO_NUM=2`。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
