# UB Bank冲突场景验证与分析(架构3510)

## UB内存结构介绍

![UB Architecture](./figures/ub_arch.png)

- 在Ascend 950PR&950DT系列产品中，UB空间大小为256KB；
- 共有16个 **bank**，组成8个 **bank group**，每个bank group包含2个bank（`bank_id` = 0, 1）；
- 每个bank由512行组成（`BANK_DEPTH = 512`），每行长度为32B（`BANK_WIDTH = 32B`），即每个bank大小为16KB；
- 每个bank group最多支持两读或者一读一写。
- bank_number相同代表是同一个bank；bank_group_id相同代表是同一个bank group。

特别的，每个bank（32B宽）分为4个subbank，各8B宽 × 512行深度，**subbank内存结构只有在使用离散与聚合API时需要关注**。
```
bank内部layout（一行32B）：
┌────────┬────────┬────────┬────────┐
│ sub0   │ sub1   │ sub2   │ sub3   │
│  8B    │  8B    │  8B    │  8B    │
└────────┴────────┴────────┴────────┘
```

## UB地址编码规则

Ascend 950PR&950DT系列产品的UB 18位地址采用**低位交织**编码规则：

`UB_ADDR[17:0] = { DEPTH[8:0], BANK[0], BG[2:0], WIDTH[4:0] }`

| 位域 | DEPTH[8:0] | BANK[0] | BG[2:0] | WIDTH[4:0] |
| --- | --- | --- | --- | --- |
| 位数 | 9 bits | 1 bit | 3 bits | 5 bits |
| 含义 | row_id（bank内行号） | bank_id（bank group内bank编号） | bank_group_id | offset（行内偏移） |

![UB Address](./figures/ub_address.png)

**解码示例**：地址 `0x4100`

将地址转为18位二进制，按位域划分：`00 0100 0001 0000 0000`

| 位域 | 二进制值 | 十进制 | 含义 |
| --- | --- | --- | --- |
| DEPTH[8:0] | `00 0100 000` | 32 | row_id = 32 |
| BANK[0] | `1` | 1 | bank_id = 1 |
| BG[2:0] | `000` | 0 | bank_group_id = 0 |
| WIDTH[4:0] | `00000` | 0 | offset = 0 |

## DataBlock映射

一条读/写指令每次处理256B = 8个DataBlock。每个DataBlock = 32B。

从 `base` 地址开始的8个DataBlock映射到8个不同bank group：

| DataBlock | 偏移 | 物理bank | BG |
|:--:|:----:|:---------:|:--:|
| 0 | +0x00 | b | b%7 |
| 1 | +0x20 | b+1 | (b+1)%7 |
| ... | ... | ... | ... |
| 7 | +0xE0 | b+7 | (b+7)%7 |

其中 `b = (base >> 5) & 0xF`。8个DataBlock覆盖BG 0-7各一次。

## bank冲突类型

根据内存结构，bank冲突主要分为以下三种类型：
- **读读冲突**：多个读操作同时访问同一个bank或两个以上读操作同时访问同一个bank group才会引发冲突（Atlas A2/A3 系列产品中多个读操作同时访问同一个bank group就会引发冲突）。
- **写写冲突**：多个写操作同时访问同一个bank group。
- **读写冲突**：读操作和写操作同时访问同一个bank，或两个读操作和一个写操作同时访问同一个bank group。

**特别的，对于当使用离散与聚合API时，需要注意sub bank冲突规则**：
- 离散数据属于同subbank的同起始地址，无冲突（硬件合并为一次读）。
- 离散数据属于同subbank的同depth地址，无冲突，例如下图的data1和data2不发生冲突。
    ```
    bank0：
    ┌─────────────┬───────────┬───────────┬───────────┐
    │ sub0        │ sub1      │ sub2      │ sub3      │
    │ data1 data2 │           │           │           │
    └─────────────┴───────────┴───────────┴───────────┘
    ```
- 离散数据属于同subbank的不同depth地址，发生冲突，例如下图的data1和data2会发生冲突。
    ```
    bank0：
    ┌────────┬────────┬────────┬────────┐
    │ sub0   │ sub1   │ sub2   │ sub3   │
    │ data1  │        │        │        │
    │ data2  |        │        │        │
    └────────┴────────┴────────┴────────┘
    ```
- 离散数据属于同一bank group的同编号subbank发生冲突，下图的data1和data2会发生冲突。
    ```
    bank0：
    ┌────────┬────────┬────────┬────────┐
    │ sub0   │ sub1   │ sub2   │ sub3   │
    │ data1  │        │        │        │
    └────────┴────────┴────────┴────────┘
    bank1：
    ┌────────┬────────┬────────┬────────┐
    │ sub0   │ sub1   │ sub2   │ sub3   │
    │ data2  |        │        │        │
    └────────┴────────┴────────┴────────┘
    ```

## 测试场景概览

本文围绕连续对齐搬入/搬出、离散与聚合、非连续对齐搬入/搬出三类API的bank冲突场景展开验证分析。所有场景的输入数据类型为 `float`（离散与聚合场景的index为 `uint32_t`），VF重复执行1000次以放大性能差异。

> **性能说明**：下表"打点性能"为VF连续执行1000次的总cycles数（使用`GetSystemCycle` 打点），数值越低性能越好。"LDU冲突率"和"STU冲突率"来自 `msopprof --aic-metrics=Default` 采集的 `ResourceConflictRatio` 指标，分别表示向量单元的load单元冲突比例和store单元冲突比例（单位：%）。

### 连续对齐搬入/搬出的bank冲突场景（Case 1-8）

**注意：在Ascend 950PR&950DT系列产品中，src0/src1/dst首地址分布在不同bank group的不同bank的场景不存在，二读一写场景无法避免读写冲突**

| case | 计算公式 | 场景描述 | bank冲突情况 | src0首地址 | src1首地址 | dst首地址 | 单次VF计算数据量(B) | 打点性能(cycles) | LDU冲突率(%) | STU冲突率(%) | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | dst = src0 + src1 | src0/src1首地址在同一个bank，dst首地址在不同bank | 读读冲突+读写冲突 | 0x0000 | 0x5000 | 0xA100 | 16384 | 1,332,802 | 40.9 | 17.6 | LDU冲突显著 |
| 2 | dst = src0 + src1 | src0/dst首地址在同一个bank，src0/src1首地址在同一个bank group | 读写冲突 | 0x0000 | 0x5100 | 0xA000 | 16384 | 1,470,520 | 0.0 | 27.1 | LDU无冲突，STU冲突率高 |
| 3 | dst = src0 + src1 | src0/dst首地址在同一个bank，且地址重叠，src1在不同bank | 读写冲突 | 0x0000 | 0x5100 | 0x0000 | 16384 | 1,470,753 | 0.0 | 27.2 | 对比case2，地址重叠未改善STU冲突 |
| 4 | dst = src0 + src1 | src1/dst首地址在同一个bank，src0/src1首地址在同一个bank group | 读写冲突 | 0x0000 | 0x5100 | 0xA100 | 16384 | 1,440,590 | 0.0 | 25.6 | 对比case2，冲突特征相同 |
| 5 | dst = src0 + src1 | src1/dst首地址在同一个bank，且地址重叠，src0在不同bank | 读写冲突 | 0x0000 | 0x5100 | 0x5100 | 16384 | 1,440,708 | 0.0 | 25.8 | 对比case3，地址重叠未改善STU冲突 |
| 6 | dst = src0 + src1 | src0/src1/dst同bank，既有读读冲突，又有读写冲突 | 读读冲突+读写冲突 | 0x0000 | 0x5000 | 0xA000 | 16384 | 1,420,560 | 4.1 | 23.8 | STU冲突显著 |
| 7 | dst = src0 + src1 | 冲突特征同case6，**对指令进行循环展开** | 读读冲突+读写冲突 | 0x0000 | 0x5000 | 0xA000 | 16384 | 1,330,688 | 40.8 | 18.2 | 对比case6，LDU冲突增加，STU冲突减少，性能提升6.3% |
| 8 | dst = src0 + src1 | 地址同Case6，kernel增加大量计算，使kernel由访存bound变为计算bound | 读读冲突+读写冲突 | 0x0000 | 0x5000 | 0xA000 | 16384 | 1,810,355,431 | 0.1 | 0.0 | 计算bound下LDU/STU冲突几乎消失，对比case6，cycles高约1274倍 |

### 离散与聚合场景（Case 9-10）

**注：subbank冲突属于更低层级的存储冲突，不被LDU/STU冲突率指标捕获**

| case | 计算公式 | 场景描述 | subbank冲突情况 | index地址 | data地址 | index模式 | 单次VF计算数据量(B) | 打点性能(cycles) | LDU冲突率(%) | STU冲突率(%) | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 9 | Gather + Add + Scatter | Gather数据索引[0..255]连续 | 跨bank分散，无冲突 | 0x0000 | 0x0400 | 索引[0..255]连续 | 1024 | 650,670 | NA | NA | 数据分散在16个bank的不同sub bank中，冲突率低 |
| 10 | Gather + Add + Scatter | Gather数据索引0、128交替重复 | 同subbank不同行冲突 | 0x0000 | 0x0400 | 索引0、128交替 | 1024 | **750,707** | NA | NA | 对比Case 9，性能劣化15.4% |

### 非连续搬运跨步对比（Case 11-14）

| case | 计算公式 | 场景描述 | src0首地址 | dst首地址 | 搬运方式 | 单次VF计算数据量(B) | 打点性能(cycles) | LDU冲突率(%) | STU冲突率(%) | 备注 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 11 | dst = src0 | 512元素按行计算，repeat内8个DataBlock连续排布，repeat间间隔256B | 0x0000 | 0x4100 | DataBlock连续搬运 | 2048 | 443,135 | 0 | 0 | LDU/STU均无冲突 |
| 12 | dst = src0 | 512元素按列计算，repeat内8个DataBlock间间隔256B，repeat间间隔32B  | 0x0000 | 0x4100 | DataBlock非连续搬运 | 2048 | 1,510,497 | 0 | **53.5** | 对比case11，STU冲突率极高，性能劣化明显 |
| 13 | dst = src0 | 同一repeat内8个DataBlock均从同一起始地址读取数据 | 0x0000 | 0x4100 | DataBlock重复读取 | 2048 | 440,911 | 0.0 | 0.0 | 对比case11，无劣化，同地址合并，无额外开销 |
| 14 | dst = src0 | 8轮迭代，每次迭代数据从同一起始地址读取数据  | 0x0000 | 0x4100 | repeat重复读取 | 2048 | 440,847 | 10.1 | 0.0 | 对比case11，无劣化，同地址合并，无额外开销 |


**各case的详细分析见下文对应章节。**

## Case 1：二读一写，src0/src1同bank

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5000` | `000101000 0 000 00000` | 0 | 0 |
| dst | `0xA100` | `001010000 1 000 00000` | 1 | 0 |

### UB读写示意

![case1](./figures/case1.png)

### 性能数据

- 打点性能：1,332,802 cycles
- LDU冲突率：40.9%
- STU冲突率：17.6%

### 分析结论

- src0和src1起始地址同落在bank0，存在读读冲突。虽然dst起始地址在bank1，但由于二读一写限制，仍然会有读写冲突。
- 性能好于Case2/Case3，当同一repeat的读指令冲突后，由于读读之间无数据依赖，可以同时执行其它repeat不冲突的读指令，缓解读读冲突。


## Case 2：二读一写，src0/dst同bank，src0/src1同bank group

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5100` | `000101000 1 000 00000` | 1 | 0 |
| dst | `0xA000` | `001010000 0 000 00000` | 0 | 0 |

### UB读写示意

![case2](./figures/case2.png)

### 性能数据

- 打点性能：1,470,520 cycles
- LDU冲突率：0.0%
- STU冲突率：27.1%

### 分析结论

- src0/dst落在bank0，src1落在bank1，三者位于同一bank group，由于二读一写限制发生读写冲突。
- LDU=0% 表明读操作无直接冲突；STU=27.1% 表明写端口因bank group总带宽不足而排队，与Case1（读读冲突主导）的冲突模式不同。
- 性能低于Case1（1,470,520 vs 1,332,802 cycles，慢约 10.3%），说明读写冲突开销高于读读冲突。

## Case 3：二读一写，src0与dst地址重叠

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5100` | `000101000 1 000 00000` | 1 | 0 |
| dst | `0x0000` | `000000000 0 000 00000` | 0 | 0 |

### UB读写示意

![case3](./figures/case3.png)

### 性能数据

- 打点性能：1,470,753 cycles
- LDU冲突率：0.0%
- STU冲突率：27.2%

### 分析结论

- src0 与 dst 起始地址完全相同（0x0000），src1 在 bank1（0x5100），三者同属 BG=0。
- 对比Case2，性能基本一致，说明在 Ascend 950PR&950DT系列产品 架构上，读写地址重叠不会消除 bank 冲突，这与部分旧架构的行为不同。


## Case 4：二读一写，src1/dst同bank，src0/src1同bank group

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5100` | `000101000 1 000 00000` | 1 | 0 |
| dst | `0xA100` | `001010000 1 000 00000` | 1 | 0 |

### UB读写示意

![case4](./figures/case4.png)

### 性能数据

- 打点性能：1,440,590 cycles
- LDU冲突率：0.0%
- STU冲突率：25.6%

### 分析结论

- src1/dst同落在 bank1，src0在bank0，三者同属 BG=0，性能接近，与Case2冲突模式不变。

## Case 5：二读一写，src1与dst地址重叠

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5100` | `000101000 1 000 00000` | 1 | 0 |
| dst | `0x5100` | `000101000 1 000 00000` | 1 | 0 |

### UB读写示意

![case5](./figures/case5.png)

### 性能数据

- 打点性能：1,440,708 cycles
- LDU冲突率：0.0%
- STU冲突率：25.8%

### 分析结论

- src1 与 dst 起始地址完全相同，src0 在 bank1，三者同属 BG=0。
- 对比Case4，性能基本一致，说明在 Ascend 950PR&950DT系列产品 架构上，读写地址重叠不会消除 bank 冲突，这与部分旧架构的行为不同。


## Case 6：二读一写，src0/src1/dst同bank

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| src1 | `0x5000` | `000101000 0 000 00000` | 0 | 0 |
| dst | `0xA000` | `001010000 0 000 00000` | 0 | 0 |

### UB读写示意

![case6——7](./figures/case6_7.png)

### 性能数据

- 打点性能：1,420,560 cycles
- LDU冲突率：4.1%
- STU冲突率：23.8%

### 分析结论

- 三个操作数全落在 bank0（BG=0）。既存在 src0/src1 之间的读读冲突，又存在读写冲突。

## Case 7：二读一写，src0/src1/dst同bank，循环展开

### 地址解码

与Case 6一致。

### UB读写示意

与Case 6一致。

### 性能数据

- 打点性能：1,330,688 cycles
- LDU冲突率：40.8%
- STU冲突率：18.2%

### 分析结论

- 地址配置与 Case 6 完全相同，采用 2 路循环展开，前半段与后半段交替发射指令，使指令双发概率提高，LDU 从 4.1% → 40.2%，同时 STU 从 24.2% → 18.0%，说明指令并行释放了一部分存储流水的竞争压力。
- 总 cycles 从 1,420,560 → 1,330,688，性能提升 6.3%。

## Case 8：二读一写，src0/src1/dst同bank，计算bound

### 地址解码

与Case 6一致。

### UB读写示意

与Case 6一致。

### 性能数据

- 打点性能：1,810,355,431 cycles
- LDU冲突率：0.1%
- STU冲突率：0.0%

### 分析结论

- 地址配置与 Case 6 完全相同（三个操作数全落在 bank0，BG=0），区别在于 kernel 内部额外增加了 1000 次 `Adds` 计算循环，使 kernel 由访存 bound 变为计算 bound，用于对比分析计算 bound 场景下 LDU/STU 冲突率指标的变化。
- 实测 LDU 冲突率从 Case 6 的 4.1% 降至 0.1%，STU 冲突率从 23.8% 降至 0.0%，说明计算耗时占比大幅增加后，存储指令等待时间被大量计算指令稀释，bank 冲突几乎不可见。总 cycles 增加约 1274 倍，与额外计算量成正比。

## Case 9：离散与聚合，Gather数据索引连续，跨bank分散

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| index (src0) | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| data (src1) | `0x0400` | `000001000 0 000 00000` | 0 | 0 |
| dst | `0x0400` | `000001000 0 000 00000` | 0 | 0 |

### UB读写示意

![case9](./figures/case8.png)

### 性能数据

- 打点性能：650,670 cycles
- LDU冲突率：NA
- STU冲突率：NA

### 案例分析

- 索引取值 [0..255] 连续递增（共 256 个 unique 值），256 个元素分散到不同 bank/sub bank 的不同 depth 行，各元素跨 bank 分散，Gather 读取和 Scatter 写入均无明显的 bank/sub bank 冲突。
- LDU/STU冲突率指标在此场景下失效，需通过打点性能差异对比诊断。


## Case 10：离散与聚合，Gather数据索引交替重复，同subbank冲突

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| index (src0) | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| data (src1) | `0x0400` | `000001000 0 000 00000` | 0 | 0 |
| dst | `0x0400` | `000001000 0 000 00000` | 0 | 0 |

### UB读写示意

![case10](./figures/case9.png)

### 性能数据

- 打点性能：750,707 cycles
- LDU冲突率：NA
- STU冲突率：NA

### 案例分析

- 地址配置与 Case 9 完全相同，区别在于 index 模式为 0、128 交替重复（每个 repeat 内一半元素通过 index=0 读取，另一半通过 index=128 读取），二者均落在同一 bank 的同一 subbank（sub0），但位于不同 depth 行，离散读取需要在同一 subbank 的不同行间切换，触发 subbank 冲突。
- 对比Case 9劣化 15.4%，说明 subbank 同地址行冲突对离散与聚合API性能有显著影响。


## Case 11：一读一写，按行搬运，DataBlock连续排布，无bank冲突

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| dst | `0x4100` | `000100000 1 000 00000` | 1 | 0 |

### UB读写示意

![case11](./figures/case10.png)

### 性能数据

- 打点性能：443,135 cycles
- LDU冲突率：0%
- STU冲突率：0%

### 分析结论

- 每个 repeat 内 8 个 DataBlock 间隔 32B 连续排布，沿 UB 地址递增，由于 32B 恰好是 bank group 轮转周期（每 32B 切换一个 bank group），8 个 DataBlock 自然命中 BG 0-7 各一次，无 bank group 竞争。
- src0 和 dst 分属不同 bank（bank0 vs bank1），且一读一写在 bank group 能力范围内，不存在冲突，LDU=0%/STU=0%。

## Case 12：一读一写，按列搬运，DataBlock非连续排布，严重bank冲突

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| dst | `0x4100` | `000100000 1 000 00000` | 1 | 0 |

### UB读写示意

![case12](./figures/case11.png)

### 性能数据

- 打点性能：1,510,497 cycles
- LDU冲突率：0%
- STU冲突率：53.5%

### 分析结论

- 每个 repeat 内 8 个 DataBlock 间隔 8 × 32B = 256B，正好是 8 个 bank group 的完整轮转周期。因此 8 个 DataBlock 全部回到**同一个 bank group 的同一个 bank**，仅 depth 不同（每 256B depth+8）。
- STU 冲突率高达 53.5%（写冲突为主要瓶颈），性能较 Case 11 劣化约 3.4 倍。

## Case 13：一读一写，repeat内同地址重复读取

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| dst | `0x4100` | `000100000 1 000 00000` | 1 | 0 |

### UB读写示意

![case13](./figures/case12.png)

### 性能数据

- 打点性能：440,911 cycles
- LDU冲突率：0.0%
- STU冲突率：0.0%

### 分析结论

- 设置blockStride=0。同一repeat 内8个DataBlock 均从同一32B基地址读取，硬件自动检测到同一repeat内的相同地址读取请求，合并为单次UB读取，8个DataBlock共享同一份数据。
- repeat内同地址重复读取无需额外优化，硬件自动合并避免冗余访问，LDU=0%/STU=0%，与 Case11一致。

## Case 14：一读一写，repeat间首地址重复读取

### 地址解码

| 操作数 | 首地址 | 18位二进制（DEPTH BANK BG WIDTH） | bank_id | bank_group_id |
| --- | --- | --- | --- | --- |
| src0 | `0x0000` | `000000000 0 000 00000` | 0 | 0 |
| dst | `0x4100` | `000100000 1 000 00000` | 1 | 0 |

### UB读写示意

![case14](./figures/case13.png)

### 性能数据

- 打点性能：440,847 cycles
- LDU冲突率：10.1%
- STU冲突率：0.0%

### 分析结论

- 所有repeat 迭代均从同一首地址加载数据，dst 按不同 offset 写入，硬件缓存命中，无额外 UB 访问开销。
- LDU 10.1%/STU 0%，与 Case 11 基准持平。

## 小结

1. Ascend 950PR&950DT系列产品上二读一写必然存在bank冲突，而在实际场景中，由于采用Regbase架构，UB读写频率降低，bank冲突往往为非瓶颈点。
7. 使用离散与聚合API时，连续index跨subbank分散可减少冲突
8. 循环展开可以优化bank冲突，可结合以下trace分析。
    | case6流水 | case7流水 |
    | ---   | --- |
    | ![case6_trace](./figures/case6_trace.png) | ![case7_trace](./figures/case7_trace.png) |
9. 当读写指令之间存在大量Reg计算指令时，读写指令冲突减少，可结合以下trace分析。
    | case6流水 | case8流水 |
    | ---   | --- |
    | ![case6_trace](./figures/case6_trace.png) | ![case8_trace](./figures/case8_trace.png) |
10. 二读一写场景下，读写冲突的影响大于读读冲突的影响，可结合以下trace分析。
    | case1流水 | case3流水 |
    | ---   | --- |
    | ![case1_trace](./figures/case1_trace.png) | ![rw_conflict](./figures/case3_trace.png) |
11. 优化建议：使用连续对齐搬入/搬出接口时，可使用循环展开缓解冲突瓶颈，使用非连续对齐搬入/搬出接口时，建议DataBlock落在不同的bank group中，使用离散与聚合接口时，数据分散到不同bank group的不同subbank中。


## 目录结构

```text
├── bank_conflict.asc             // 算子host + device代码
├── CMakeLists.txt                // 编译工程
├── config.h                      // 14个Case的地址/数据类型类型配置
├── data_utils.h                  // 数据读写工具
├── figures/                      // 各Case UB内存分布图
│   ├── ub_arch.png               // UB架构图
│   ├── ub_address.png            // UB地址编码图
│   ├── case1.png
│   ├── case1_trace.png           // Case1流水trace
│   ├── case2.png
│   ├── case3.png
│   ├── case3_trace.png           // Case3流水trace（读写冲突）
│   ├── case4.png
│   ├── case5.png
│   ├── case6_7.png
│   ├── case6_trace.png           // Case6流水trace
│   ├── case7_trace.png           // Case7流水trace
│   ├── case8.png
│   ├── case9.png
│   ├── case10.png
│   ├── case11.png
│   ├── case12.png
│   └── case13.png
├── README.md                     // 本文档
├── scripts/
│   ├── gen_data.py               // 测试数据生成
│   └── verify_result.py          // 精度验证
└── vf_func.h                     // vf函数实现
```

## 运行方式

### 环境准备

```bash
source /path/to/cann/set_env.sh
```

### 编译与运行

```bash
export SCENARIO_NUM=1          # 有效值: 1-14
mkdir -p build && cd build
cmake .. -DSCENARIO_NUM=${SCENARIO_NUM}
make -j
python3 ../scripts/gen_data.py --case ${SCENARIO_NUM}
./demo                     # 精度验证 + 打点性能（输出cycles计数到stdout）
python3 ../scripts/verify_result.py output/output_z.bin output/golden.bin
```

### CMake选项

| 选项 | 可选值 | 说明 |
|:----:|:------:|:----|
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构版本 |
| `SCENARIO_NUM` | 1-14 | 切换测试场景 |
| `CMAKE_ASC_RUN_MODE` | `npu` | 上板运行模式 |

### 冲突性能采集

通过 `msopprof` 采集硬件性能计数器，量化bank冲突程度：

```bash
# 编译特定case后，在build目录执行
msopprof --aic-metrics=Default ./demo
```

监控结果保存在 `OPPROF_<timestamp>/` 目录下，关键指标如下：

#### ResourceConflictRatio.csv

| 指标 | 含义 | 说明 |
|------|------|------|
| `aiv_vec_ldu_cflt_ratio` | 向量Load单元冲突率 | 读操作因bank冲突被阻塞的时间比例，越高说明读取冲突越严重 |
| `aiv_vec_stu_cflt_ratio` | 向量Store单元冲突率 | 写操作因bank冲突被阻塞的时间比例，越高说明写入冲突越严重 |
