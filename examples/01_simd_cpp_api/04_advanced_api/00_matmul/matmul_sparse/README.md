# Sparse Matmul场景直调样例

## 概述
4:2稀疏矩阵乘（Sparse Matmul）的Matmul样例，可以减少矩阵乘计算时的内存占用和计算量。稀疏矩阵乘会跳过稀疏矩阵B中的零元素，仅对非零元素进行数据搬运存储和计算。  
该场景下输入的原始左矩阵A、右矩阵为稀疏矩阵，稀疏矩阵B中每4个元素中至少有2个零元素；在进行Matmul计算前，用户需要自行对B矩阵进行4：2稠密化，即基于原始稀疏矩阵B在每4个元素中过滤掉2个零元素，使B矩阵稠密化为稠密矩阵。Sparse Matmul场景调用Matmul API完成A矩阵与4：2稠密化后的B矩阵的矩阵乘计算。  
> **注：** 4:2稀疏矩阵乘（Sparse Matmul）当前只支持B矩阵转置。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍
```
├── matmul_sparse
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul_sparse.asc       // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```
## 样例描述
- 样例功能：  
  Matmul样例调用Matmul高阶API对输入的左矩阵A和4:2稠密化后的右矩阵B，进行矩阵乘和加bias偏置。   
  在计算执行前的数据准备阶段，通过真值数据生成脚本自行完成B矩阵的稠密化，并得到索引矩阵。在实现Matmul计算时，调用Matmul高阶API传入4:2稠密化后的B矩阵和索引矩阵，最终完成Sparse Matmul场景的矩阵乘计算。

- 样例规格：  
  本样例中：M = 128, N = 7680, K = 64。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">int8_t</td><td align="center">ND</td><td align="center">true</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">int32_t</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">int32_t</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmul_sparse_custom</td></tr>
  </table>

- 样例实现： 
  - Kernel关键步骤
    - 创建Matmul对象时，通过SparseMatmulType定义矩阵B的参数类型信息。
      ```cpp
      using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, ATYPE, false>;
      // 使用SparseMatmulType定义矩阵B的参数类型信息
      using B_TYPE = AscendC::SparseMatmulType<AscendC::TPosition::GM, AscendC::TPosition::GM, CubeFormat::ND, BType, true>;
      using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
      using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
      AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_MDL> matmulObj;
      ```
    - 设置索引矩阵。
      ```cpp
      matmulObj.SetSparseIndex(gm_index); // 设置B矩阵稠密化过程中生成的索引矩阵gm_index
      ```

  - Tiling关键步骤
    - 使能Sparse Matmul稀疏矩阵计算场景。
      ```cpp
      auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
      matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
      tiling.SetSparse(true); // 设置使能Sparse Matmul稀疏矩阵计算场景
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认） | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
