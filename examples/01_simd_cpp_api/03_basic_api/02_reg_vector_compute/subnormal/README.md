# Subnormal样例

## 概述

本样例演示了在Ascend C Reg矢量计算中，Div操作在Subnormal模式开启或关闭情况下的行为差异。通过两种不同的Div精度算法配置（PRECISION_1ULP_FTZ_FALSE和PRECISION_1ULP_FTZ_TRUE），展示是否支持Subnormal浮点数对计算结果的影响。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
subnormal
│   ├── scripts/             // 测试脚本目录
│   │   └── gen_data.py      // 生成测试输入和golden数据
│   ├── CMakeLists.txt       // 编译工程文件
│   ├── data_utils.h         // 数据读写工具函数
│   ├── subnormal.asc        // Ascend C算子实现 & 调用样例
│   └── README.md            // 样例说明文档
```

## 样例描述

- 样例功能：

  样例计算float类型数据，分别使用PRECISION_1ULP_FTZ_FALSE和PRECISION_1ULP_FTZ_TRUE配置DivSpecificMode作为Div操作的精度算法参数，执行除法运算。计算公式如下：
  $$z_i = \frac{x_i}{y_i}$$

- 样例规格
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center" colspan="2">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1024]</td><td align="center" colspan="2">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1024]</td><td align="center" colspan="2">float</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1024]</td><td align="center" colspan="2">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">subnormal</td></tr>
  </table>

- 样例实现
    - 实现流程：
        1. 在Host侧分配内存并初始化输入数据
        2. 将数据从Host内存搬运至Device全局内存
        3. 调用核函数，在Vector核上执行计算
        4. 核函数内部：通过DataCopy将输入数据从Global Memory搬入Unified Buffer
        5. 在Unified Buffer上通过asc_vf_call调用VF（Vector Function）函数
        6. VF函数内部使用DivSpecificMode模板参数，执行LoadAlign、Div、StoreAlign操作
        7. 将计算结果从Unified Buffer搬回Global Memory
        8. 在Host端进行精度验证和golden data对比

  - Subnormal与FTZ概念说明：
      - Subnormal：指数部分全为0，尾数部分不全为0的浮点数，表示接近0的极小数值。IEEE 754标准支持Subnormal数，提供更好的数值精度和渐进下溢出特性。
      - FTZ：将Subnormal数强制置为0的模式。虽然简化了硬件设计和提高性能，但会损失数值精度。

  - 多场景说明：
      - 场景1（SCENARIO_NUM=1）：使用PRECISION_1ULP_FTZ_FALSE作为Div的精度算法参数
          - 实现函数：SubnormalFTZFalseVF<T>
          - 实现：
            ```cpp
            static constexpr AscendC::Reg::DivSpecificMode mode = {ZEROING, false, PRECISION_1ULP_FTZ_FALSE};
            AscendC::Reg::Div<T, &mode>(zReg, xReg, yReg, mask);
            ```
          - 说明：支持Subnormal数据计算，使用单指令计算得出结果，最大精度误差为1 ulp。除法结果为Subnormal数时，保留该值。
          - 应用场景：需要精确处理Subnormal浮点数的科学计算、数值模拟、高精度计算等场景。

      - 场景2（SCENARIO_NUM=2）：使用PRECISION_1ULP_FTZ_TRUE作为Div的精度算法参数
          - 实现函数：SubnormalFTZTrueVF<T>
          - 实现：
            ```cpp
            static constexpr AscendC::Reg::DivSpecificMode mode = {ZEROING, false, PRECISION_1ULP_FTZ_TRUE};
            AscendC::Reg::Div<T, &mode>(zReg, xReg, yReg, mask);
            ```
          - 说明：不支持Subnormal数据计算，FTZ模式，使用单指令计算得出结果，最大精度误差为1 ulp。除法结果为Subnormal数时，被强制置为0。
          - 应用场景：对Subnormal处理精度要求不高的深度学习、图像处理、实时推理等场景，可简化硬件实现，提高计算性能。

    - 计算数据场景对比：

      | 序号 |   Div组合   |     被除数      |     除数     | 场景1（golden一致） | 原因解析              | 场景2输出（丢失精度） | 原因解析                            |
      |:---:|:---------:|:------------:|:----------:|:-------------:|:------------------|:-----------:|:--------------------------------|
      | 1  | N / N = S | $2^{-126}$ |    4.0     |  0x00200000   | 结果是次正规数$2^{-128}$ | 0x00000000  | 计算结果落在Subnormal区间，结果强制归零        |
      | 2  | N / S = N | $2^{-125}$ | $2^{-127}$ |  0x40800000   | 结果是正规数$2^{2}$     | 0x7f800000  | 除数S被视作0，任何非零正数除以0结果为无穷大         |
      | 3  | S / N = S | $2^{-130}$ |    2.0     |  0x00040000   | 结果为次正规数$2^{-131}$ | 0x00000000  | 被除数S输入时即被视为0，结果为0               |
      | 4  | S / N = N | $2^{-130}$ | $2^{-20}$  |  0x08800000   | 结果是正规数$2^{110}$   | 0x00000000  | 被除数S输入时即被视为0，结果为0               |
      | 5  | S / S = N | $2^{-140}$ | $2^{-145}$ |  0x42000000   | 结果为正规数$2^{5}$     | 0x7fffffff  | 被除数S与除数S都被视为0，$0 \div 0$，结果是NaN |
        
      - N：normal数据，正规数。
      - S：Subnormal数据，次正规数。

  - 约束条件：
      - 输入数据长度必须为GetVecLen()的整数倍
      - 除数不能为0，实际应用中需要增加除数为0的检查
      - 样例目前仅支持float数据类型

## 编译运行

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  # 场景2：使用PRECISION_1ULP_FTZ_TRUE（FTZ模式）
  SCENARIO_NUM=2
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM; make -j;                           # 编译工程
  python3 ../scripts/gen_data.py;                                           # 生成测试输入数据
  ./demo                                                                    # 执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。
- 编译选项说明

  | 选项 | 可选值                   | 说明                                           |
  |:--------------------------|:----------------------|:---------------------------------------------|
  | `SCENARIO_NUM`            | `1`（默认）、`2`        | 1：支持subnormal计算；<br/>2：不支持subnormal计算；       |
  | `CMAKE_ASC_RUN_MODE`      | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真                      |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`            | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```