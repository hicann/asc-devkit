# Batch Matmul直调样例

## 概述

批量处理Matmul计算的样例。

通过单次搬运多个Matmul输入数据，减少搬运次数，提升性能。适用于需要进行多次Matmul计算且单次Matmul计算的输入shape较小时，搬运开销在整体耗时中占比较大的场景。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── batch_matmul
│   ├── scripts
│   │   ├── gen_data.py              // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py         // 真值对比文件
│   ├── CMakeLists.txt               // 编译工程文件
│   ├── data_utils.h                 // 数据读入写出函数
│   ├── batch_matmul.asc             // Ascend C样例实现 & 调用样例
│   └── README.md                    // 样例说明文档
```
## 样例描述

- 样例功能：  
  调用Matmul高阶API，实现批量处理3组Matmul计算，每组对BSNGD格式的A、B矩阵分别做矩阵乘和加bias偏置。

  BSNGD格式具体可参考[IterateBatch](../../../../../docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/IterateBatch.md)中对该数据排布的介绍。

- 样例规格：   
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="6" align="center">BatchMatmulCustom</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td><td align="center">layout</td></tr>
  <tr><td align="center">a</td><td align="center">[2, 32, 1, 3, 64]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td><td align="center">BSNGD</td></tr>
  <tr><td align="center">b</td><td align="center">[2, 256, 1, 3, 64]</td><td align="center">half</td><td align="center">ND</td><td align="center">true</td><td align="center">BSNGD</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[2, 32, 1, 3, 256]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td><td align="center">BSNGD</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="6" align="center">batch_matmul_custom</td></tr>
  </table>

- 样例实现：  
  - Kernel关键步骤
    - 完成多batch矩阵乘操作。
      ```cpp
      matmulObj.IterateBatch(cGlobal[batchOffsetC], batchA, batchB, false);
      ```

  - Tiling关键步骤
    - 调用SetALayout、SetBLayout、SetCLayout、SetBatchNum设置A/B/C的Layout轴信息和最大BatchNum数。
      ```cpp
      cubeTiling->SetALayout(A_BNUM, A_SNUM, 1, A_GNUM, A_DNUM);
      cubeTiling->SetBLayout(B_BNUM, B_SNUM, 1, B_GNUM, B_DNUM);
      cubeTiling->SetCLayout(C_BNUM, C_SNUM, 1, C_GNUM, C_DNUM);
      cubeTiling->SetBatchNum(BATCH_NUM);
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;             # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
