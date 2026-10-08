# Matmul M/N轴方向的流水并行样例
## 概述
M/N轴方向的流水并行的Matmul样例。该功能的应用场景为输入矩阵的K很小，但M或N很大的场景，即singleCoreK<=baseK，但singleCoreM远大于baseM或singleCoreN远大于baseN。使能M/N方向流水并行功能可能会带来性能收益。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构
```
├── matmul_mn_double_buffer
│   ├── scripts
│   │   ├── gen_data.py                     // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py                // 真值对比文件
│   ├── CMakeLists.txt                      // 编译工程文件
│   ├── data_utils.h                        // 数据读入写出函数
│   ├── matmul_mn_double_buffer.asc         // Ascend C样例实现 & 调用样例
│   └── README.md                           // 样例说明文档
```
## 样例描述
- 样例功能：  
  本样例调用Matmul高阶API对输入的A，B矩阵做矩阵乘和加bias偏置。在定义Matmul对象时，将配置scheduleType等参数的MatmulConfig作为模板参数传入，使能M/N轴方向的流水并行功能。本样例以MDL模板且N方向流水并行为例，对于Norm模板或M方向流水并行功能也可以参考该样例的实现。

- 样例规格：  
  本样例中：M = 128, N = 7680, K = 16
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">matmul_mn_double_buffer_custom</td></tr>
  </table>
- 样例实现
  - Kernel关键步骤
    - 创建Matmul对象时，自定义MatmulConfig参数，将其中的MatmulConfigMode设置成CONFIG_MDL，scheduleType参数设置为ScheduleType::OUTER_PRODUCT，iterateOrder参数设置为IterateOrder::ORDER_M，使能MDL模板的N方向流水并行功能，获得自定义地使用MDL模板的Matmul对象。
      ```cpp
      constexpr static MatmulConfigMode configModeMDL = MatmulConfigMode::CONFIG_MDL;
      constexpr static MatmulFuncParams funcParamsOrderM{false, false, false, false, 0, IterateOrder::ORDER_M, ScheduleType::OUTER_PRODUCT, true, true};
      constexpr static MatmulConfig CFG_MDL_OUTER_PRODUCT_ORDER_M = GetMMConfig<configModeMDL>(funcParamsOrderM);

      using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, ATYPE, false>;
      using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false>;
      using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
      using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
      AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL_OUTER_PRODUCT_ORDER_M> matmulObj;
      ```

  - Tiling关键步骤
    - 设置自定义MatmulConfig参数，将Kernel侧配置的参数如scheduleType等，同步到Tiling侧。
      ```cpp
      matmul_tiling::MatmulConfigParams matmulConfigParams(1, false, matmul_tiling::ScheduleType::OUTER_PRODUCT,
          matmul_tiling::MatrixTraverse::FIRSTM, false);
      cubeTiling.SetMatmulConfigParams(matmulConfigParams);
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
  mkdir -p build && cd build;    # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py    # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
