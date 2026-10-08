# Matmul 反量化场景直调样例

## 概述
输出随路反量化的Matmul样例，支持同一系数的反量化模式和向量的反量化模式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍
```
├── matmul_quant
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul_quant.asc        // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述
- 样例功能：  
  Matmul样例调用Matmul API计算时int8_t类型输入，计算结果以half类型反量化输出，同时支持同一系数的反量化模式与向量的反量化模式。该场景下将C矩阵数据从CO1搬出到Global Memory时，会执行反量化操作，对输出矩阵的所有值采用同一系数或向量进行反量化。

- 样例规格：  
  本样例中：M = 1024, N = 1024, K = 1024。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">int32_t</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmul_quant_custom</td></tr>
  </table>

- 样例实现： 
  - Kernel关键步骤
    - 设置反量化参数。  
      当编译选项QUANT_MODE的值为1时，设置编译宏：CUSTOM_QUANT_VECTOR，编译执行向量的反量化模式。    
      根据是否定义宏：CUSTOM_QUANT_VECTOR，设置对应的反量化参数。
      ```cpp
      #if defined(CUSTOM_QUANT_VECTOR)
          matmulObj.SetQuantVector(quantGlobal);
      #else
          float quantFloat = 0.1f;
          uint64_t quantValue = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&quantFloat));
          matmulObj.SetQuantScalar(quantValue);
      #endif
      ```

  - Tiling关键步骤
    - 设置Matmul反量化模式。
      ``` cpp
      #if defined(CUSTOM_QUANT_VECTOR)
          tilingApi.SetDequantType(matmul_tiling::DequantType::TENSOR); // set TENSOR quant mode
      #else
          tilingApi.SetDequantType(matmul_tiling::DequantType::SCALAR); // set SCALAR quant mode
      #endif
      ```

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

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
  # -DQUANT_MODE=0：使能同一系数的反量化模式；
  # -DQUANT_MODE=1：使能向量反量化模式；
  # -m=0：使能同一系数的反量化模式；
  # -m=1：使能向量反量化模式；
  mkdir -p build && cd build;    # 创建并进入build目录
  cmake -DQUANT_MODE=0 -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，以使能同一系数的反量化模式为例，默认npu模式
  python3 ../scripts/gen_data.py -m=0   # 生成测试输入数据，以使能同一系数的反量化模式为例
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  cmake -DQUANT_MODE=0 -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DQUANT_MODE=0 -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
