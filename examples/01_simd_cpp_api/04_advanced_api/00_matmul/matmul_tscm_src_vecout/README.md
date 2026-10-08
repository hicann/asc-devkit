# Matmul 使用来源为VECOUT的TSCM输入直调样例

## 概述
使用数据来源为VECOUT的用户自定义TSCM的输入的Matmul样例，开发者可以自主管理L1 Buffer以高效利用硬件资源。TSCM即Temp Swap Cache Memory，用于临时把数据交换到额外空间。该场景由开发者管理L1 Buffer，再将输入矩阵数据对应的L1 Buffer地址作为Matmul的输入。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍
```
├── matmul_tscm_src_vecout
│   ├── scripts
│   │   ├── gen_data.py                         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py                    // 真值对比文件
│   ├── CMakeLists.txt                          // 编译工程文件
│   ├── data_utils.h                            // 数据读入写出函数
│   ├── matmul_tscm_src_vecout.asc              // Ascend C样例实现 & 调用样例
│   └── README.md                               // 样例说明文档
```
## 样例描述
- 样例功能：  
  Matmul样例中自定义A矩阵从VECOUT到L1的数据搬入，使A矩阵全部数据常驻在L1中，调用Matmul API计算时，A矩阵为TSCM输入，B矩阵设为GM输入，对输入的A、B矩阵做矩阵乘和加Bias偏置。

- 约束条件
  - TSCM输入的矩阵必须能在L1 Buffer上全载。

- 样例规格：  
  本样例中：M = 32, N = 256, K = 32。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">Matmul</td></tr>
  <tr><td rowspan="4" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">bias</td><td align="center">[1, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmul_tscm_src_vecout_custom</td></tr>
  </table>

- 样例实现： 
  - Kernel关键步骤
    - 创建Matmul对象。其中左矩阵A的MatmulType中，POSITION为TSCM，SRC_POSITION为VECOUT。
      ```cpp
      AscendC::MatmulType<AscendC::TPosition::TSCM, CubeFormat::NZ, AType, 
                          IS_TRANS_A, LayoutMode::NONE, false, AscendC::TPosition::VECOUT>
      ```
    - 自定义左矩阵A从VECOUT到TSCM的搬运，设置左矩阵A、右矩阵B、Bias，其中左矩阵A为TSCM输入。
      ```cpp
      // Copy aMatrix from vecout to tscm
      AscendC::TSCM<AscendC::TPosition::VECOUT, 1> scm;
      pipe->InitBuffer(scm, 1, tiling.M * tiling.Ka * sizeof(AType));
      auto scmTensor = scm.AllocTensor<AType>();
      DataCopy(scmTensor, vecoutLocal, tiling.M * tiling.Ka);
      scm.EnQue(scmTensor);
      AscendC::LocalTensor<AType> scmLocal = scm.DeQue<AType>();

      matmulObj.SetTensorA(scmLocal, isTransA); // Set aMatrix tscm input
      matmulObj.SetTensorB(bGlobal, isTransB);
      if (tiling.isBias) {
          matmulObj.SetBias(biasGlobal);
      }
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py    # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin    # 验证输出结果是否正确，确认算法逻辑正确
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

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
