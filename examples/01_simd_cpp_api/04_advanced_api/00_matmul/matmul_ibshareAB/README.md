# Matmul IBShareAB特性样例

## 概述

调用Matmul高阶API开启IBShare功能，复用L1 Buffer上相同的A矩阵或者B矩阵数据的样例，本样例为A矩阵和B矩阵同时复用场景。开启功能可以减少数据搬运开销。支持两种场景，通过CMake编译参数 `SCENARIO_NUM` 选择场景。
    <table>
 	<tr>
 		<td>scenarioNum</td>
 		<td>场景类型</td>
 	</tr>
 	<tr>
 		<td>1</td>
 		<td>使能AB矩阵IBShare（A和B矩阵不切分）</td>
 	</tr>
 	<tr>
 		<td>2</td>
 		<td>不使能AB矩阵IBShare（A和B矩阵按K轴切分）</td>
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
├── matmul_ibshareAB
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── figures                 // 图示
│   ├── matmul_ibshareAB.asc    // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  调用Matmul高阶API时开启A、B矩阵的IBShare功能，计算过程中同一AIC对应的两个AIV在每次迭代时用到的A、B矩阵在L1 Buffer上的数据一致。

  A矩阵与B矩阵均使能IBShare时，不对k列进行切分计算；均未使能IBShare时，按照k列进行切分计算。通过对比两种场景的运行时间，可以得出该特性的性能提升。
  使能AB矩阵IBShare场景的数据处理说明图示(A矩阵和B矩阵不切分处理)：![alt text](./figures/matmul_ABshare.png)  
  不使能AB矩阵ibshareAB场景的数据处理说明图示(A矩阵和B矩阵切分处理)：![alt text](./figures/matmul_noABshare.png)  


- 样例规格：  
  本样例中：M = 128, N = 256, K = 384。
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">matmul_ABshare_custom</td></tr>
  </table>

- 样例实现： 

  - Kernel关键步骤  
    - 创建Matmul对象：  
        根据CMakeLists里的SCENARIO_NUM编译选项，配置A、B矩阵IBShare参数为true或者false。
        ```cpp
        #if SCENARIO_NUM == 1
        constexpr bool isABshare = true;
        #else
        constexpr bool isABshare = false;
        #endif

        Matmul<
            MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType, false, LayoutMode::NONE, isABshare>,
            MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType, false, LayoutMode::NONE, isABshare>,
            MatmulType<AscendC::TPosition::VECIN, CubeFormat::ND, CType>> matmulObj;
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
  SCENARIO_NUM=1                                                                # 设置场景编号
  mkdir -p build && cd build;                                                   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py                                                # 生成测试输入数据
  ./demo                                                                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin       # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或场景前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品 与 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2` | 场景编号：1=使能AB矩阵IBShare，2=不使能AB矩阵IBShare |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
