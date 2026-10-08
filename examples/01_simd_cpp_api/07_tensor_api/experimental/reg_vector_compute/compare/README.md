# Compare 样例

## 概述
本样例基于Tensor API实现operator>接口完成多场景下的数据比较功能。  
本样例支持两种比较场景，通过 CMake 编译参数 `SCENARIO_NUM` 选择场景。
  <table>
    <tr>
      <td>SCENARIO_NUM</td>
        <td>比较场景</td>
    </tr>
    <tr>
      <td>1</td>
      <td>右操作数为reg_tensor：一个向量逐元素和另一个向量逐元素比较</td>
    </tr>
    <tr>
      <td>2</td>
      <td>右操作数为立即数：一个向量逐元素和一个标量比较</td>
    </tr>
    </table>

## 支持的产品

- Ascend 950PR&950DT系列产品

## 目录结构介绍

```plain
├── compare
│   ├── scripts
│   │   └── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   └── compare.asc             // Ascend C算子实现 & 调用样例
```

## 样例描述
operator>之类的比较接口一般与select接口配合使用，该样例仅演示operator>和select配合的用法。  
本样例通过编译参数`SCENARIO_NUM`来切换不同的场景：  
**场景1：右操作数为reg_tensor**  
- 样例功能：  
  对两个相同大小的矢量数据寄存器src0、src1逐元素取较大值。
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
  - 调用operator>比较两个矢量数据寄存器的大小，返回存放bool类型的矢量寄存器condition：若src0_reg大于src1_reg，则返回值相应比特位写入1，否则写入0
  - 调用Select接口，传入上一步比较结果mask_reg选择：若mask_reg比特位为1，则对应位置选择src0_reg的元素，否则选择src1_reg的元素
  - float数据类型的mask格式为每4bits保存一个mask，所以Compare从src0_reg、src1_reg依次读取数据，比较后依次写入至mask_reg的4 * N的bit位置；Select根据mask_reg的4 * N的bit决定从src0_reg还是src1_reg选择数据。
  - 调用实现：使用内核调用符<<<>>>调用核函数。  

**场景2：右操作数为立即数**  
- 样例功能：  
  对向量src0_reg逐元素与标量0比较，若src0_reg[i]大于0，则dst_reg[i]取src0_reg[i]，否则取src1_reg[i]。
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
  - 调用Compares接口的GT（大于）模式比较src0_reg向量和标量0，输出至mask_reg：若src0_reg大于0，则mask_reg相应比特位写入1，否则写入0
  - 调用Select接口，传入上一步比较结果mask_reg选择：若mask_reg比特位为1，则对应位置选择src0_reg的元素，否则选择src1_reg中的元素
  - float数据类型的mask_reg格式为每4bits保存一个mask，所以Compare从src0_reg、src1_reg依次读取数据，比较后依次写入至mask_reg的4 * N的bit位置；Select根据mask_reg的4 * N的bit决定从src0_reg还是src1_reg选择数据。
  - 调用实现：使用内核调用符<<<>>>调用核函数。

## 编译运行
在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量，**当前仅支持使用[CANN master](../../../../../../docs/zh/quick_start.md#cann-install)**。

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  下面以场景1为例：
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build
  cmake -DSCENARIO_NUM=${SCENARIO_NUM} -DCMAKE_ASC_ARCHITECTURES=dav-3510  -DCANN_ASC_USE_EXPERIMENTAL=ON..
  make -j
  python3 ../scripts/gen_data.py -scenarioNum=${SCENARIO_NUM}
  ./demo
  ```

  使用 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 说明 |
  | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | 算子执行模式，可选 `npu`、`sim`，默认值为 `npu`。 |
  | `CMAKE_ASC_ARCHITECTURES` | NPU芯片型号，默认值为 `dav-3510`。 |
  | `CANN_ASC_USE_EXPERIMENTAL` | 实验性ASC接口开关，本样例必须设为 `ON`，默认值为 `OFF`。 |
  | `SCENARIO_NUM` | 类型转换场景编号，必选，取值范围为1至2。 |


- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
