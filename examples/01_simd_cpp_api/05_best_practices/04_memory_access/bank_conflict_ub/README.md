# UB Bank冲突及地址重叠场景分析示例

## 概述

本样例基于Add指令，通过配置不同的LocalTensor地址，验证和分析UB（Unified Buffer）中的bank冲突和地址重叠对性能的影响。样例通过编译参数`SCENARIO_NUM`选择不同的地址配置场景，共8个scenario，覆盖无冲突、读读冲突、读写冲突、地址完全重叠等典型场景。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Atlas A3系列产品 | >=CANN 9.0.0 |
| Atlas A2系列产品 | >=CANN 9.0.0 |

## 目录结构介绍

``` text
├── bank_conflict_ub
│   ├── scripts
│   │   ├── gen_data.py                    // 输入数据和真值数据生成脚本
│   │   └── verify_result.py               // 验证输出数据和真值数据是否一致的验证脚本
│   ├── figures                            // UB内存结构图及各场景读写示意图
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── config.h                           // 各SCENARIO_NUM的地址配置
│   ├── data_utils.h                       // 数据读入写出函数
│   ├── bank_conflict_ub.asc               // Ascend C样例实现及调用样例
│   ├── README.md                          // 样例说明文档（中文版）
│   └── README_en.md                       // 样例说明文档（英文版）
```

## UB（Unified Buffer）内存结构

![UB内存结构图](./figures/ub_memory_layout.png)

- 在A2/A3架构上，UB空间大小为192KB；
- 共有48个**bank**，组成16个**bank group**，每个bank group包含3个bank（`bank_id` = 0, 1, 2）；
- 每个bank由128行组成（`BANK_DEPTH = 128`），每行长度为32B（`BANK_WIDTH = 32B`），即每个bank大小为4KB；
- **bank全局编码规则**：`bank_number = bank_group_id + bank_id * 16`；
- bank_number相同代表是同一个bank；bank_group_id相同代表是同一个bank group。

## UB地址编码规则

UB的18位地址编码遵循如下规则：

`UB_ADDR[17:0] = { BANK[1:0], BANK_DEPTH[6:0], BG[3:0], BANK_WIDTH[4:0] }`

| 位域 | BANK[1:0] | BANK_DEPTH[6:0] | BG[3:0] | BANK_WIDTH[4:0] |
| --- | --- | --- | --- | --- |
| 位数 | 2 bits | 7 bits | 4 bits | 5 bits |
| 含义 | bank_id | row_id（bank内行号） | bank_group_id（bank group编号） | offset（行内偏移） |

**解码示例**：地址`0x10020`

将地址转为18位二进制，按位域划分：`01 0000000 0001 00000`

| 位域 | 二进制值 | 十进制 | 含义 |
| --- | --- | --- | --- |
| BANK[1:0] | `01` | 1 | bank_id = 1 |
| BANK_DEPTH[6:0] | `0000000` | 0 | row_id = 0 |
| BG[3:0] | `0001` | 1 | bank_group_id = 1 |
| BANK_WIDTH[4:0] | `00000` | 0 | offset = 0 |

结合上述的UB内存结构图和bank编码规则，该地址位于bank group 1内bank_id为1的bank，即全局bank 17（`1 + 1 × 16 = 17`），行号为0。

## bank冲突类型

根据内存结构，bank冲突主要分为以下三种类型：

- 读读冲突：多个读操作，同时尝试访问同一个bank group；
- 写写冲突：多个写操作，同时尝试访问同一个bank group；
- 读写冲突：读操作和写操作，同时尝试访问同一个bank。

## 场景详细说明

本样例围绕bank冲突和地址重叠两个角度展开验证分析。所有场景的输入数据类型为`float`，数据长度为4096，采用连续读写方式（`BlkStride = 1`，`RepStride = 8`，`mask = 64`），特殊配置在对应scenario中单独说明。核函数名为`add_custom`。

> **性能说明**：下表"打点性能"为Add指令连续执行1000次的cycle数，数值越低性能越好。

| 场景（scenario） | 计算公式 | 场景描述 | bank冲突情况 | src0首地址 | src1首地址 | dst首地址 | 打点性能 | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | dst = src0 + src1 | src0/src1/dst首地址分布在不同bank group的不同bank | 无读读、读写冲突 | 0 | 0x10100 | 0x20000 | 2390 | 基线性能参照（无bank冲突） |
| 2 | dst = src0 + src1 | src0/src1首地址在同一个bank | **有读读冲突** | 0 | 0x4000 | 0x10000 | 4167 | 验证读读冲突（多个读操作访问同一个bank） |
| 3 | dst = src0 + src1 | src0/src1首地址在同一个bank group | **有读读冲突** | 0 | 0x10000 | 0x14100 | 4167 | 验证读读冲突（多个读操作访问同一个bank group） |
| 4 | dst = src0 + src1 | src0/dst首地址在同一个bank | 无读写冲突（硬件优化） | 0 | 0x10100 | 0x4000 | 2389 | 作为scenario 5的基线参照 |
| 5 | dst = src0 + src1 | src0/dst首地址不在同一个bank | **有读写冲突** | 0 | 0x10100 | 0x4020 | 3751 | 验证读写冲突（对比scenario 4） |
| 6 | dst = src0 + src1 | src1/dst首地址在同一个bank，<br>src1/dst地址完全重叠 | 无读写冲突（硬件优化） | 0 | 0x10100 | 0x10100 | 2390 | 验证src/dst地址重叠（无读写冲突，硬件优化） |
| 7 | dst = src0 + src1 | src0每次repeat重复读取同一datablock（src0BlkStride=0, src0RepStride=1），<br>src1/dst地址完全重叠 | **有读读冲突** | 0 | 0x10100 | 0x10100 | 3584 | 对比scenario 6/scenario 8，验证src0RepStride配置跟bank冲突的关联 |
| 8 | dst = src0 + src1 | src0每次repeat重复读取同一datablock（src0BlkStride=0, src0RepStride=8），<br>src1/dst地址完全重叠 | 无读读、读写冲突 | 0 | 0x10100 | 0x10100 | 2389 | 对比scenario 7，验证src0RepStride配置跟bank冲突的关联 |

各scenario的详细分析见下文对应章节。

### Scenario 1：无读读冲突、无读写冲突

#### 地址解码

| 操作数 | 首地址 | 18位二进制（BANK DEPTH BG WIDTH） | bank_id | bank_group_id | 全局bank编号 |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x20000` | `10 0000000 0000 00000` | 2 | 0 | 32 |

#### UB读写示意

![scenario1](./figures/scenario1.png)

- 每8个绿色数据块（32B）代表src0每次repeat读取的数据（256B）；
- 每8个黄色数据块（32B）代表src1每次repeat读取的数据（256B）；
- 每8个蓝色数据块（32B）代表dst每次repeat写入的数据（256B）；
- 处理完所有数据所需迭代次数：`repeat_times = 数据长度 × sizeof(float) ÷ 每次repeat处理量 = 4096 × 4 ÷ 256 = 64`。

#### 分析结论

- 读读冲突分析：第1次repeat处理时，src0（bank_id=0）的8个datablock位于bg 0-7，src1（bank_id=1）的8个datablock位于bg 8-15，两者bank group完全不重叠；后续的repeat处理模式可以类推，故**无读读冲突**。
- 读写冲突分析：第1次repeat处理时，dst（bank_id=2）的8个datablock位于bg 0-7，与src0（bank_id=0）、src1（bank_id=1）的bank_id均不同，不在同一个bank；后续的repeat处理模式可以类推，故**无读写冲突**。

### Scenario 2：src0/src1首地址在同一个bank——读读冲突

#### 地址解码

| 操作数 | 首地址 | 18位二进制（BANK DEPTH BG WIDTH） | bank_id | bank_group_id | 全局bank编号 |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x4000` | `00 0100000 0000 00000` | 0 | 0 | 0 |
| dst | `0x10000` | `01 0000000 0000 00000` | 1 | 0 | 16 |

#### UB读写示意

![scenario2](./figures/scenario2.png)

- 每8个绿色数据块（32B）代表src0每次repeat读取的数据（256B）；
- 每8个黄色数据块（32B）代表src1每次repeat读取的数据（256B）；
- 每8个蓝色数据块（32B）代表dst每次repeat写入的数据（256B）；
- 处理完所有数据所需迭代次数：`repeat_times = 4096 × 4 ÷ 256 = 64`。

#### 分析结论

- 读读冲突分析：第1次repeat处理时，src0（bank_id=0）与src1（bank_id=0）的8个datablock均位于bg 0-7，两者bank_id相同且bank group完全重叠；后续的repeat处理模式可以类推，故**存在读读冲突**。
- 读写冲突分析：第1次repeat处理时，src0/src1读取（bank_id=0）与dst写入（bank_id=1）的bank_id不同，不在同一个bank；后续的repeat处理模式可以类推，故**无读写冲突**。

### Scenario 3：src0/src1首地址在同一个bank group——读读冲突

#### 地址解码

| 操作数 | 首地址 | 18位二进制（BANK DEPTH BG WIDTH） | bank_id | bank_group_id | 全局bank编号 |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10000` | `01 0000000 0000 00000` | 1 | 0 | 16 |
| dst | `0x14100` | `01 0100000 1000 00000` | 1 | 8 | 24 |

#### UB读写示意

![scenario3](./figures/scenario3.png)

- 每8个绿色数据块（32B）代表src0每次repeat读取的数据（256B）；
- 每8个黄色数据块（32B）代表src1每次repeat读取的数据（256B）；
- 每8个蓝色数据块（32B）代表dst每次repeat写入的数据（256B）；
- 处理完所有数据所需迭代次数：`repeat_times = 4096 × 4 ÷ 256 = 64`。

#### 分析结论

- 读读冲突分析：第1次repeat处理时，src0（bank_id=0）与src1（bank_id=1）的8个datablock均位于bg 0-7，虽bank_id不同，但bank group完全重叠；后续的repeat处理模式可以类推，故**存在读读冲突**。
- 读写冲突分析：第1次repeat处理时，src0/src1读取（bank_id=0/1，bg 0-7）与dst写入（bank_id=1，bg 8-15）的bank group不重叠，不在同一个bank；后续的repeat处理模式可以类推，故**无读写冲突**。

> **对比scenario 2**：scenario 2中src0/src1在同一bank（bank_id和bank_group_id均相同），scenario 3中在同一bank group（仅bank_group_id相同），两种场景均会触发读读冲突，性能开销接近。

### Scenario 4：src0/dst首地址在同一个bank——无读写冲突（硬件优化）

#### 地址解码

| 操作数 | 首地址 | 18位二进制（BANK DEPTH BG WIDTH） | bank_id | bank_group_id | 全局bank编号 |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x4000` | `00 0100000 0000 00000` | 0 | 0 | 0 |

#### UB读写示意

![scenario4](./figures/scenario4.png)

- 每8个绿色数据块（32B）代表src0每次repeat读取的数据（256B）；
- 每8个黄色数据块（32B）代表src1每次repeat读取的数据（256B）；
- 每8个蓝色数据块（32B）代表dst每次repeat写入的数据（256B）；
- 处理完所有数据所需迭代次数：`repeat_times = 4096 × 4 ÷ 256 = 64`。

#### 分析结论

- 读读冲突分析：第1次repeat处理时，src0（bank_id=0，bg 0-7）与src1（bank_id=1，bg 8-15）的bank group完全不重叠；后续的repeat处理模式可以类推，故**无读读冲突**。
- 读写冲突分析：第1次repeat处理时，src0读取（bank_id=0）与dst写入（bank_id=0）的8个datablock均位于bg 0-7，bank_id相同，理论上存在读写冲突；但硬件自动将dst写入延后1个repeat，使读写操作错开；后续的repeat处理模式可以类推，故**实际无读写冲突**。

**理论执行时序**（未优化）：

| | 第1次repeat | 第2次repeat | 第3次repeat | ... |
| --- | --- | --- | --- | --- |
| src0读 | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1读 | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst写 | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |

理论结果：src0与dst每次repeat访问相同bank，存在读写冲突。

**实际执行时序**（硬件优化）：

| | 第1次repeat | 第2次repeat | 第3次repeat | ... |
| --- | --- | --- | --- | --- |
| src0读 | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1读 | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst写 | — | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | ... |

实际结果：硬件将dst写入延后1个repeat，与src0读取错开，避免读写冲突。

### Scenario 5：src0/dst首地址不在同一个bank——读写冲突

#### 地址解码

| 操作数 | 首地址 | 18位二进制（BANK DEPTH BG WIDTH） | bank_id | bank_group_id | 全局bank编号 |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x4020` | `00 0100000 0001 00000` | 0 | 1 | 1 |

#### UB读写示意

![scenario5](./figures/scenario5.png)

- 每8个绿色数据块（32B）代表src0每次repeat读取的数据（256B）；
- 每8个黄色数据块（32B）代表src1每次repeat读取的数据（256B）；
- 每8个蓝色数据块（32B）代表dst每次repeat写入的数据（256B）；
- 处理完所有数据所需迭代次数：`repeat_times = 4096 × 4 ÷ 256 = 64`。

#### 分析结论

- 读读冲突分析：第1次repeat处理时，src0（bank_id=0，bg 0-7）与src1（bank_id=1，bg 8-15）的bank group完全不重叠；后续的repeat处理模式可以类推，故**无读读冲突**。
- 读写冲突分析：第1次repeat处理时，src0读取（bank_id=0，bg 0-7）与dst写入（bank_id=0，bg 1-8）的bank_id相同，且bank group部分重叠（bg 1-7），会访问到相同的bank；后续的repeat处理模式可以类推，故**存在读写冲突**。

**UB读写过程示意**：

| | 第1次repeat | 第2次repeat | 第3次repeat | ... |
| --- | --- | --- | --- | --- |
| src0读 | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1读 | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst写 | bank 1-8 (bg 1-8) | bank 9-15-0 (bg 9-15-0) | bank 1-8 (bg 1-8) | ... |

分析结果：src0与dst每次repeat都会访问到相同的bank，导致读写冲突。

### Scenario 6：src1/dst首地址在同一个bank，src1/dst地址重叠——无读写冲突（硬件优化）

#### 地址解码

| 操作数 | 首地址 | 18位二进制（BANK DEPTH BG WIDTH） | bank_id | bank_group_id | 全局bank编号 |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |

#### UB读写示意

![scenario6](./figures/scenario6.png)

- 每8个绿色数据块（32B）代表src0每次repeat读取的数据（256B）；
- 每8个黄色数据块代表每次repeat，src1读取与dst写入同一块数据空间（256B），即src1和dst地址完全重叠；
- 处理完所有数据所需迭代次数：`repeat_times = 4096 × 4 ÷ 256 = 64`。

#### 分析结论

- 读读冲突分析：第1次repeat处理时，src0（bank_id=0，bg 0-7）与src1（bank_id=1，bg 8-15）的bank group完全不重叠；后续的repeat处理模式可以类推，故**无读读冲突**。
- 读写冲突分析：第1次repeat处理时，src1读取（bank_id=1）与dst写入（bank_id=1）的地址完全重叠，理论上存在读写冲突风险；但硬件自动将dst写入延后1个repeat，使读写操作错开；后续的repeat处理模式可以类推，故**实际无读写冲突**。

**理论执行时序**（未优化）：

| | 第1次repeat | 第2次repeat | 第3次repeat | ... |
| --- | --- | --- | --- | --- |
| src0读 | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1读 | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst写 | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |

理论结果：src1与dst每次repeat都会访问到相同的bank，会出现读写冲突。

**实际执行时序**（硬件优化）：

| | 第1次repeat | 第2次repeat | 第3次repeat | ... |
| --- | --- | --- | --- | --- |
| src0读 | bank 0-7 (bg 0-7) | bank 8-15 (bg 8-15) | bank 0-7 (bg 0-7) | ... |
| src1读 | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |
| dst写 | — | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | ... |

实际结果：硬件将dst写入延后1个repeat，与src1读取错开，避免读写冲突。

### Scenario 7：src0重复读取同一datablock（src0BlkStride=0, src0RepStride=1），src1/dst地址重叠——读读冲突

> **特殊配置**：src0的`src0BlkStride=0`（单次repeat内重复读取同一datablock），`src0RepStride=1`（每次repeat间隔1个datablock，即32B）。

#### 地址解码

| 操作数 | 首地址 | 18位二进制（BANK DEPTH BG WIDTH） | bank_id | bank_group_id | 全局bank编号 |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |

#### UB读写示意

![scenario7](./figures/scenario7.png)

- 每次重复读取同1个绿色数据块（32B）共8次（src0BlkStride=0）；
- 每8个黄色数据块代表每次repeat，src1读取与dst写入同一块数据空间（256B），即src1和dst地址完全重叠；
- 处理完所有数据所需迭代次数：`repeat_times = 4096 × 4 ÷ 256 = 64`。

#### 分析结论

- 读读冲突分析：第1次repeat处理时，src0（bank_id=0）仅读取单个datablock（bg 0），与src1（bank_id=1，bg 8-15）不在相同bank group，首拍无冲突；但后续repeat中src0的bank group依次递增（bg 1, 2, ...），每逢偶数次repeat与src1的bank group范围重叠，整体**存在读读冲突**。
- 读写冲突分析：第1次repeat处理时，src1读取（bank_id=1）与dst写入（bank_id=1）的地址完全重叠，硬件自动将dst写入延后1个repeat，使读写操作错开；后续的repeat处理模式可以类推，故**无读写冲突**。

**UB读写过程示意**：

| | 第1次repeat | 第2次repeat | 第3次repeat | 第4次repeat | ... |
| --- | --- | --- | --- | --- | --- |
| src0读 | bank 0 (bg 0) | bank 1 (bg 1) | bank 2 (bg 2) | bank 3 (bg 3) | ... |
| src1读 | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | ... |
| dst写 | - | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |

分析结果：
- 第2次repeat：src0读bg 1，src1读bg 0-7 → bg 1重叠，读读冲突；
- 第4次repeat：src0读bg 3，src1读bg 0-7 → bg 3重叠，读读冲突；
- ...
- dst写入延后1个repeat，不与src1产生读写冲突。

### Scenario 8：src0重复读取同一datablock（src0BlkStride=0, src0RepStride=8），src1/dst地址重叠——无冲突

> **特殊配置**：src0的`src0BlkStride=0`（单次repeat内重复读取同一datablock），`src0RepStride=8`（每次repeat间隔8个datablock，即256B）。

#### 地址解码

| 操作数 | 首地址 | 18位二进制（BANK DEPTH BG WIDTH） | bank_id | bank_group_id | 全局bank编号 |
| --- | --- | --- | --- | --- | --- |
| src0 | `0x0` | `00 0000000 0000 00000` | 0 | 0 | 0 |
| src1 | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |
| dst | `0x10100` | `01 0000000 1000 00000` | 1 | 8 | 24 |

#### UB读写示意

![scenario8](./figures/scenario8.png)

- 每次重复读取同1个绿色数据块（32B）共8次（src0BlkStride=0）；
- 每8个黄色数据块代表每次repeat，src1读取与dst写入同一块数据空间（256B），即src1和dst地址完全重叠；
- 处理完所有数据所需迭代次数：`repeat_times = 4096 × 4 ÷ 256 = 64`。

#### 分析结论

- 读读冲突分析：第1次repeat处理时，src0（bank_id=0）仅读取单个datablock，与src1（bank_id=1，bg 8-15）不在相同bank group；后续repeat中src0的bank group按bg 0, 8, 0, 8, ...交替，始终与src1的bank group范围错开，整体**无读读冲突**。
- 读写冲突分析：第1次repeat处理时，src1读取（bank_id=1）与dst写入（bank_id=1）的地址完全重叠，硬件自动将dst写入延后1个repeat，使读写操作错开；后续的repeat处理模式可以类推，故**无读写冲突**。

**UB读写过程示意**：

| | 第1次repeat | 第2次repeat | 第3次repeat | 第4次repeat | ... |
| --- | --- | --- | --- | --- | --- |
| src0读 | bank 0 (bg 0) | bank 8 (bg 8) | bank 0 (bg 0) | bank 8 (bg 8) | ... |
| src1读 | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | ... |
| dst写 | - | bank 24-31 (bg 8-15) | bank 16-23 (bg 0-7) | bank 24-31 (bg 8-15) | ... |

分析结果：
- src0（bg 0或8）与src1（bg 0-7或8-15）始终不重叠：当src0在bg 0时，src1在bg 8-15；当src0在bg 8时，src1在bg 0-7。因此无读读冲突。
- dst写入延后1个repeat，不与src1产生读写冲突。

> **对比Scenario 7**：Scenario 7的`src0RepStride=1`导致src0按步长1递增遍历bank group（bg 0, 1, 2, 3, ...），与src1的bank group范围频繁重叠；Scenario 8的`src0RepStride=8`使src0在bg 0和8之间交替，恰好与src1错开。可以有效消除读读冲突。

### 小结

- 读读冲突：只要两个读操作访问同一bank group（不论bank_id是否相同），即触发读读冲突。
- 读写冲突：读写操作访问同一bank，会触发读写冲突（同一个bank一拍只能1读或1写）。例外情况：读操作与写操作repeat首个datablock在同一个bank，后面的7个datablock也分别在同一个bank，硬件可将写入延后1个repeat以规避读写冲突的情况。
- blkStride=0的场景：repeat内不会重复读取，只读1次。repStride=0的场景：repeat间不会重复读取，只读1次。

## 样例规格

**表 1**  样例输入输出规格

| 类别 | name | shape | data type | format |
| --- | --- | --- | --- | --- |
| 样例输入 | x | [1, 4096] | float | ND |
| 样例输入 | y | [1, 4096] | float | ND |
| 样例输出 | z | [1, 4096] | float | ND |
| 核函数名 | add_custom | - | - | - |

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 单个场景测试

  ```bash
  SCENARIO_NUM=1    # 设置场景编号
  mkdir -p build && cd build;                              # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;  # 编译工程
  python3 ../scripts/gen_data.py --scenario $SCENARIO_NUM          # 生成测试输入数据
  ./demo                                                   # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output_z.bin output/golden.bin  # 验证输出结果是否正确
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` | NPU架构：dav-2201对应Atlas A2系列产品和Atlas A3系列产品 |
  | `SCENARIO_NUM` | `1`~`8` | 场景编号：1（无冲突基线）、2（同bank读读冲突）、3（同bank group读读冲突）、4（同bank读写冲突-硬件优化）、5（读写冲突）、6（地址重叠-硬件优化）、7（src0RepStride=1读读冲突）、8（src0RepStride=8无冲突） |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
