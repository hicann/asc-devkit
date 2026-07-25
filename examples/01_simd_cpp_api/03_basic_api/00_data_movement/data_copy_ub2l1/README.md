# DataCopy ub2l1样例

## 概述

本样例在Mmad矩阵乘场景下，基于DataCopy实现UB（Unified Buffer）到L1（L1 Buffer）的数据搬运。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```
├── data_copy_ub2l1
│   ├── scripts
│   │   ├── gen_data.py                   // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                    // 编译工程文件
│   ├── data_utils.h                      // 数据读入写出函数
│   ├── data_copy_ub2l1.asc               // AscendC样例实现 & 调用样例
│   └── README.md                         // 样例说明文档
```

## 样例描述

- 样例功能：  
  将数据从UB（Unified Buffer）搬运到L1（L1 Buffer），然后进行Mmad矩阵乘计算，最后通过Fixpipe搬出到GM（Global Memory）。接口资料参考[随路基础数据搬运](../../../../../docs/api/SIMD-API/基础API/cube_compute_ISASI/矩阵计算的搬入/DataCopy_UBToL1_continuous.md)。
- 样例规格：
  <table>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">NZ</td></tr>
  <tr><td align="center">y</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">NZ</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">data_copy_ub2l1</td></tr>
  </table>
- 样例实现：  
  - 将数据从GM（Global Memory）搬运到UB（Unified Buffer）。
  - 将数据从UB（Unified Buffer）搬运到L1（L1 Buffer）。
  - 调用基础API LoadData将数据从L1（L1 Buffer）搬运到A2（L0A Buffer）与B2(L0B Buffer)。
  - 调用基础API Mmad进行矩阵乘计算。
  - 调用基础API Fixpipe将数据从L0C Buffer搬运到GM（Global Memory）。
- 调用实现  
  使用内核调用符<<<>>>调用核函数。
    
## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;                                               # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py                                            # 生成测试输入数据
  ./demo                                                                    # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
