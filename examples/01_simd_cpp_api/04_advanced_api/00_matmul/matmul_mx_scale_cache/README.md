# Mx Matmul Scale多倍缓存直调样例

## 概述

在MXFP4/MXFP8数据格式下，量化系数矩阵scale数据在L1 Buffer上开启多倍缓存的MxMatmul样例。使能scale多倍缓存可以减少MTE2重复搬运，从而提升性能。

以左量化系数矩阵scaleA K方向多倍缓存，且倍数为scaleFactorK为例：在数据从GM搬运到A1（L1 Buffer）的MTE2搬运过程中，A矩阵一次性搬运stepM * stepK个base块，而scaleA矩阵则一次搬运stepM * (scaleFactorK * stepK)个base块并缓存保持在A1中。后续Iterate计算时，scaleA矩阵缓存数据依次与A矩阵中不同的数据完成广播乘法运算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍
```
├── matmul_mx_scale_cache
│   ├── scripts
│   │   ├── gen_data.py            // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py       // 真值对比文件
│   ├── CMakeLists.txt             // 编译工程文件
│   ├── data_utils.h               // 数据读入写出函数
│   ├── matmul_mx_scale_cache.asc  // Ascend C样例实现 & 调用样例
│   └── README.md                  // 样例说明文档
```

## 样例描述

- 样例功能：  
  Matmul Tiling中的mxTypePara参数表示scale多倍缓存。其中：  
  - mxTypePara[0:7]表示 scaleA 与 A 矩阵在 K 方向载入数据量的比例系数scaleFactorKa。
  - mxTypePara[8:15]表示 scaleB 与 B 矩阵在 K 方向载入数据量的比例系数scaleFactorKb。
  - mxTypePara[16:23]表示 scaleA 与 A 矩阵在 M 方向载入数据量的比例系数scaleFactorM。
  - mxTypePara[24:31]表示 scaleA 与 A 矩阵在 N 方向载入数据量的比例系数scaleFactorN。
  
  例如 tilingData.mxTypePara = 0x01010104，则 scaleFactorKa = 4，表示scaleA在K方向开启4倍缓存。

- 约束条件
  - 对于scaleA矩阵，仅scaleA的K方向在L1上全载时，才允许使能M方向多倍缓存。
  - 对于scaleB矩阵，仅scaleB的K方向在L1上全载时，才允许使能N方向多倍缓存。

- 样例规格：  
  本样例中：M = 32，N = 128，K = 128，scaleK = 4。其中scaleK为K整除32的结果4。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">MatmulMxTypeParaCustom</td></tr>
  <tr><td rowspan="6" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">fp8_e5m2_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">scaleA</td><td align="center">[M, scaleK]</td><td align="center">fp8_e8m0_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">fp8_e5m2_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">scaleB</td><td align="center">[scaleK, N]</td><td align="center">fp8_e8m0_t</td><td align="center">ND</td><td align="center">true</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmulMxTypeParaCustom</td></tr>
  </table>
- 样例实现： 
  - Kernel关键步骤
      - 创建Matmul对象：使用MatmulTypeWithScale定义A、scaleA、B、scaleB的参数类型信息，包括：内存逻辑位置、数据格式、数据类型、转置信息。
        ```cpp
        typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, false, AscendC::TPosition::GM, CubeFormat::ND, false> aType;
        typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, false, AscendC::TPosition::GM, CubeFormat::ND, true> bType;
        typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> cType;
        typedef AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, float> biasType;
        // 定义matmul对象时， 传入MatmulWithScalePolicy表明使能MxMatmul模板策略
        AscendC::Matmul<aType, bType, cType, biasType, CFG_MDL, AscendC::MatmulCallBackFunc<nullptr, nullptr, nullptr>, AscendC::Impl::Detail::MatmulWithScalePolicy> matmulObj;
        ```
      - 设置左矩阵A与左量化系数矩阵scaleA、右矩阵B与右量化系数矩阵scaleB、Bias。
        ```cpp
        matmulObj.SetTensorA(aGlobal, isTransA);
        matmulObj.SetTensorB(bGlobal, isTransB);
        matmulObj.SetTensorScaleA(asGlobal, isTransScaleA);
        matmulObj.SetTensorScaleB(bsGlobal, isTransScaleB);

        if (tiling.isBias) {
            matmulObj.SetBias(biasGlobal);
        }
        ```

  - Tiling关键步骤
    - 创建一个Tiling对象：使用SetMadType使能Mx特性，使用SetScaleAType设置ScaleA的信息、使用SetScaleBType设置scaleB的信息。
      ```cpp
      cubeTiling.SetMadType(matmul_tiling::MatrixMadType::MXMODE);
      cubeTiling.SetScaleAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, false);
      cubeTiling.SetScaleBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, true);
      if (cubeTiling.GetTiling(tilingData) == -1) {
          std::cout << "Generate tiling failed." << std::endl;
          return {};
      }
      // 0-6bit表示scaleA与A矩阵在K方向载入数据量的比例系数，8-14bit表示scaleB与B矩阵在K方向载入数据量的比例系数, 260代表二进制0000 0001 0000 0100，0-6bit中使用100，开启4倍缓存。
      tilingData.mxTypePara = 16843012;
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
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;             # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                       # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
