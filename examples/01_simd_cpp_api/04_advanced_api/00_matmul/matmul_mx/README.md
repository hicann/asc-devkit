# Mx Matmul直调样例

## 概述

在MXFP4/MXFP8数据格式下的带有量化系数的矩阵乘法，即MxMatmul样例。计算公式为：C = (scaleA ⊗ A) * (scaleB ⊗ B) + Bias。“⊗”表示广播乘法。  
当K整除32向上取整的值为奇数时，scaleK的取值需要在该值的基础上向上对齐为偶数，scaleA和scaleB矩阵也要做对应的扩展。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── matmul_mx
│   ├── scripts
│   │   ├── gen_data.py            // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py       // 真值对比文件
│   ├── CMakeLists.txt             // 编译工程文件
│   ├── data_utils.h               // 数据读入写出函数
│   ├── matmul_mx.asc              // Ascend C样例实现 & 调用样例
│   └── README.md                  // 样例说明文档
```

## 样例描述

- 样例功能：  
  MatmulMxNormEvenCustom样例调用Matmul API计算时，左量化系数矩阵与左矩阵乘积，右量化系数矩阵与右矩阵乘积，对两个乘积的结果做矩阵乘法。

- 样例规格：  
  本样例中：M = 32，N = 128，K = 128，scaleK = 4。其中scaleK为K整除32的结果4。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">MatmulMxNormEvenCustom</td></tr>
  <tr><td rowspan="6" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">fp8_e5m2_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">scaleA</td><td align="center">[M, scaleK]</td><td align="center">fp8_e8m0_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">fp8_e5m2_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">scaleB</td><td align="center">[scaleK, N]</td><td align="center">fp8_e8m0_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmulMxNormEvenCustom</td></tr>
  </table>

- 样例实现： 
  - Kernel关键步骤
    - 创建Matmul对象：使用MatmulTypeWithScale使能scaleA、scaleB。
      ```cpp
      typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, false> aType;
      typedef AscendC::MatmulTypeWithScale<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, fp8_e5m2_t, false> bType;
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
      - 创建一个Tiling对象：使用SetMadType设置Matmul模式，使能MxMatmul场景，使用SetScaleAType设置ScaleA的信息、使用SetScaleBType设置scaleB的信息。
        ```cpp
        cubeTiling.SetMadType(matmul_tiling::MatrixMadType::MXMODE);
        cubeTiling.SetScaleAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, false);
        cubeTiling.SetScaleBType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, false);
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
  ./demo                        # 执行编译生成的可执行程序，执行样例
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
