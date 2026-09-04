# compare样例

## 概述
本样例基于Reg编程接口实现Compare、Compares接口完成多场景下的数据比较功能。  
本样例支持两种比较场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
  <table>
    <tr>
      <td>SCENARIO_NUM</td>
        <td>比较场景</td>
    </tr>
    <tr>
      <td>1</td>
      <td>Compare：一个向量逐元素和另一个向量逐元素比较</td>
    </tr>
    <tr>
      <td>2</td>
      <td>Compares：一个向量逐元素和同一个标量比较</td>
    </tr>
    </table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── compare
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── figures                        // 图示
│   ├── compare.asc                    // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述
Compare接口一般与Select接口配合使用，该样例仅演示Compare和Select配合的用法。  
本样例通过编译参数`SCENARIO_NUM`来切换不同的场景：  
**场景1：Compare**  
- 样例功能：  
  对两个相同大小的向量xReg、yReg逐元素取较大值。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">compare</td></tr>
  </table>
- 样例实现：
  - 调用Compare接口的GT（大于）模式比较两个向量的大小，float数据类型的MaskReg格式为每4bits保存一个mask，所以Compare从xReg、yReg依次读取数据，比较后依次写入至MaskReg的4 * N的bit位置，若xReg大于yReg，则maskReg相应比特位写入1，否则写入0
  - 调用Select接口，根据MaskReg的4 * N的bit决定从xReg还是yReg选择数据：若maskReg比特位为1，则对应位置选择xReg的元素，否则选择yReg的元素
  - 调用实现：使用内核调用符<<<>>>调用核函数。 
  <img src="figures/compare.png">

**场景2：Compares**  
- 样例功能：  
  对向量xReg逐元素与标量0比较，若xReg[i]大于0，则zReg[i]取xReg[i]，否则取yReg[i]。
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">compare</td></tr>
  </table>
- 样例实现：  
  - 调用Compares接口的GT（大于）模式比较xReg向量和标量0，float数据类型的MaskReg格式为每4bits保存一个mask，所以Compares从xReg读取数据，与标量0比较后依次写入至MaskReg的4 * N的bit位置，若xReg大于0，则maskReg相应比特位写入1，否则写入0
  - 调用Select接口，根据MaskReg的4 * N的bit决定从xReg还是yReg选择数据：若maskReg比特位为1，则对应位置选择xReg的元素，否则选择yReg的元素
  - 调用实现：使用内核调用符<<<>>>调用核函数。

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
  SCENARIO_NUM=1                                                                # 执行场景1
  mkdir -p build && cd build;                                                   # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM                     # 生成测试输入数据
  ./demo                                                                        # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `SCENARIO_NUM` | 1、2 | 样例执行场景：场景1：Compare、场景2：Compares |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
