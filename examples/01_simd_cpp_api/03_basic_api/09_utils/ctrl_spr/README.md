# CtrlSpr样例

## 概述

本样例基于 `SetCtrlSpr`、`GetCtrlSpr`、`ResetCtrlSpr` 接口实现对CTRL寄存器（控制寄存器）的特定比特位的设置、读取和重置功能，并通过浮点数计算验证非饱和模式是否正确。

本样例演示了设置、读取和重置CTRL[48]、CTRL[60]比特位，验证非饱和模式下INF是否保持原值，同时通过GetCtrlSpr读取CTRL[48]设置后和reset后的值进行验证：
- CTRL[60]用于控制饱和模式的全局生效方式。设置为1时，使能全局设置饱和。
- CTRL[48]用于控制浮点数计算和浮点数精度转换时的饱和模式，仅在CTRL[60]使能时生效。
  - 设置为0时（饱和模式）：INF输出会被饱和为±MAX（half类型为65504），NaN输出会被饱和为0。
  - 设置为1时（非饱和模式）：INF/NaN保持原输出。
- 使用完寄存器后，用ResetCtrlSpr接口将寄存器设置回默认值，防止后续计算受影响。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── ctrl_spr
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── ctrl_spr.asc            // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  验证CTRL寄存器非饱和模式操作的完整流程及其功能效果：
  1. SetCtrlSpr：设置 `CTRL[60]=1` 使能全局生效，设置 `CTRL[48]=1` 选择非饱和模式（INF/NaN保持原输出）
  2. GetCtrlSpr：读取 `CTRL[48]` 设置后的值，存入 `ctrlLocal` 前4位
  3. Adds：执行half类型浮点数加法计算，输入包含INF值，存入 `dstLocal`，用于验证非饱和模式下INF是否保持原值
  4. ResetCtrlSpr：重置 `CTRL[48]`、`CTRL[60]` 为默认值
  5. GetCtrlSpr：读取 `CTRL[48]` 重置后的值，存入 `ctrlLocal` 后4个元素

- 样例规格：
  <table>
  <caption>表1：样例规格说明</caption>
  <tr><td rowspan="1" align="center">核数</td><td colspan="4" align="center">1</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td><td align="center">output_ctrl</td><td align="center">[1, 8]</td><td align="center">int64_t</td><td align="center">ND</td></tr>
  <tr><td align="center">output_sat</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">kernel_ctrl_spr</td></tr>  
  </table>

- 输出说明：
  - output_ctrl.bin：寄存器值验证输出，8个int64_t值（前4个为 `CTRL[48]` 设置后的值=1，后4个为 `CTRL[48]` 重置后的值=0）
  - output_sat.bin：非饱和模式功能验证输出，前128个为正常值+1，后128个为INF（非饱和模式下INF保持）

- 验证逻辑：
  - 输入数据：前128个为正常值（0~127），后128个为INF
  - 设置 `CTRL[48]=1`（非饱和模式）：INF + 1 = INF（保持INF）
  - `CTRL[48]` 设置后的值=1，reset后的值=0


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
  mkdir -p build && cd build;                                          # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                 # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py                                       # 生成测试输入数据和golden数据
  ./demo                                                               # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py                                  # 验证输出结果是否正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;   # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;   # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU 架构：dav-3510 对应 Ascend 950PR&950DT系列产品 |

  > **注意：** 本样例仅在 Ascend 950PR&950DT系列产品 上支持。

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
