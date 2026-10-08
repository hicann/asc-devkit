# Matmul IBShareB直调样例

## 概述

开启IBShare功能，复用L1 Buffer上相同的A矩阵或者B矩阵数据的样例，本样例为仅B矩阵复用场景。开启功能可以减少数据搬运开销。支持多种场景，通过环境变量选择场景。
    <table>
	 	<tr>
	 		<td>scenarioNum</td>
	 		<td>场景类型</td>
	 	</tr>
	 	<tr>
	 		<td>1</td>
	 		<td>默认实现，使能IBShare模板</td>
	 	</tr>
	 	<tr>
	 		<td>2</td>
	 		<td>使能纯Cube模式 + IBShareB</td>
	 	</tr>
	 </table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── matmul_ibshareB
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── matmul_ibshareB.asc     // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  调用Matmul高阶API时开启B矩阵的IBShare功能，计算过程中同一AIC对应的两个AIV在每次迭代时用到的B矩阵在L1 Buffer上的数据一致。

- 约束条件：
  - A矩阵或B矩阵单独使能IBShare的场景，要求复用的矩阵必须在L1 Buffer上全载。

- 样例规格：  
  本样例中：M = 64, N = 256, K = 384。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">MatmulIBShareBCustom</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td><td align="center">-</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">matmul_ibshareb_custom</td></tr>
  </table>

- 样例实现： 
  - Kernel关键步骤
    - 创建Matmul对象，设置B矩阵的IBShare参数为true。
      - 方式一：默认实现，使用默认的IBShare模板CFG_IBSHARE_NORM创建Matmul对象。
        ```cpp
        #include "lib/matmul_intf.h"
    
        using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
        using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NONE, true>;
        using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
        using BIAS_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
        AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_IBSHARE_NORM> matmulObj;
        ```
      - 方式二：使能纯Cube模式实现，在定义Matmul对象的代码中，设置ASCENDC_CUBE_ONLY宏，且必须在#include "lib/matmul_intf.h"之前设置。
        ```cpp
        #define ASCENDC_CUBE_ONLY // 设置ASCENDC_CUBE_ONLY宏
        #include "lib/matmul_intf.h"

        using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
        using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NONE, true>;
        using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
        using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
        AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_NORM> matmulObj;
        ```

  - 核函数  
    使用纯Cube模式时，在核函数实现的入口处，指定__cube__。

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。
    ```cpp
    matmul_ibshareb_custom<<<tilingData.usedCoreNum / MIX_RATIO, 0, stream>>>(x1, x2, bias, y, workspaceDevice,
                          tilingDevice);       // 方式一：非纯Cube模式，SetDim设置为AIV:AIC组合的核数
    matmul_ibshareb_custom<<<tilingData.usedCoreNum, 0, stream>>>(x1, x2, bias, y, workspaceDevice,
                          tilingDevice);       // 方式二：纯Cube模式，SetDim设置为AIC的核数                          
    ```

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
  SCENARIO_NUM=1                                                                # 默认实现。使能IBShare模板
  mkdir -p build && cd build;                                                   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py                                                # 生成测试输入数据
  ./demo                                                                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin       # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  SCENARIO_NUM=1
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　|
  | ----------------| -----------------------------| ---------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品 与 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2` | 场景编号：1=默认实现，使能IBShare模板，2=使能纯Cube模式 + IBShareB |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
