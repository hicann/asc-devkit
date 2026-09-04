# mergemode样例

## 概述
本样例基于Reg编程接口验证MaskMergeMode::MERGING模式的行为特性，演示未激活mask位保留dstReg原值的机制，使用Max接口进行验证。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍
```
├── mergemode
│   ├── scripts
│   │   ├── gen_data.py                // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt                 // 编译工程文件
│   ├── data_utils.h                   // 数据读入写出函数
│   ├── mergemode.asc                  // AscendC样例实现 & 调用样例
│   └── README.md                      // 样例介绍
```

## 样例描述
- 样例功能：  
  验证`MERGING`模式：当mask未激活时，dstReg的对应位保留原值而非参与计算。输入为200个负数（非VL整数倍），输出为2（未激活位保留的dstReg原值）。

  **验证原理**
  - 输入：200个float负数（-100到-1），VL=256Byte，共4个repeat
  - repeat 0-3：每个repeat前，Duplicate初始化dstReg=2
  - Max MERGING：激活位=max(负数,负数)=负数，未激活位=保留dstReg原值=2
  - Repeat 3（200个元素，最后一个repeat只有8个激活位）：
    - yAddr[192:200] = 负数（8个激活位）
    - yAddr[200:256] = 2（56个未激活位，MERGING保留dstReg原值）
  - ReduceMax(yAddr[0:256]) = 2，验证MERGING模式确实保留了未激活位的dstReg原值

  - 样例规格：
    <table>
    <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="3" align="center">AIV样例</td></tr>
    <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 200]</td><td align="center">float（负数）</td></tr>
    <tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[1, 8]</td><td align="center">float</td></tr>
    <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">mergemode</td></tr>
    </table>

  - 样例实现：
    1. **MaxMergeModeVF**：
       - 每轮循环先`Duplicate(dstReg, 2)`初始化dstReg
       - 使用`UpdateMask`处理非VL整数倍数据
       - 使用`Max`：激活位计算max值，未激活位保留dstReg原值=2
       - 使用`StoreAlign(allMask)`写入整个VL，验证dstReg未激活位确实是2
    2. **ReduceMaxVF**：对yAddr[0:256]做ReduceMax归约，结果应为2（未激活位保留的值）
    3. 输出为32B对齐的8个float，ReduceMax结果在第一个元素，预期值为2

    - 调用实现
      使用内核调用符`<<<>>>`调用核函数，启动1个核。

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
  mkdir -p build && cd build;                                                    # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                           # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                                                 # 生成测试输入数据
  ./demo                                                                         # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```