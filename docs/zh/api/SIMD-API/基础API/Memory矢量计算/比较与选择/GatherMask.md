# GatherMask<a name="ZH-CN_TOPIC_0000001500267978"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_gather_mask_intf.h"`。

以**内置固定模式**或者**用户自定义输入的Tensor**数值对应的二进制为gather mask（数据收集的掩码），从源操作数中选取元素写入目的操作数中。1为选取，0为不选取。

- 内置固定模式：对应函数原型中src1Pattern的数据类型为uint8\_t类型。gather mask由src1Pattern参数指定，并在所有repeat迭代中保持固定。src1Pattern取值范围为\[1, 7\]。
    - 1：01010101…0101 \# 每个repeat取偶数索引元素
    - 2：10101010…1010 \# 每个repeat取奇数索引元素
    - 3：00010001…0001 \# 每个repeat内每四个元素取第一个元素
    - 4：00100010…0010 \# 每个repeat内每四个元素取第二个元素
    - 5：01000100…0100 \# 每个repeat内每四个元素取第三个元素
    - 6：10001000…1000 \# 每个repeat内每四个元素取第四个元素
    - 7：11111111…1111 \# 每个repeat内取全部元素

    <!-- npu="950" id8 -->
    Ascend 950PR/Ascend 950DT，支持模式1-7。
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持模式1-7。
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持模式1-7。
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    Atlas 推理系列产品AI Core，支持模式1-6。
    <!-- end id11 -->

    <!-- npu="310b" id12 -->
    Atlas 200I/500 A2 推理产品，支持模式1-7。
    <!-- end id12 -->

- 用户自定义模式：对应函数原型中src1Pattern的数据类型为LocalTensor，迭代间间隔由src1RepeatStride决定，迭代内src1Pattern连续消耗。

GatherMask接口计算过程如下图所示，参数设置为：mask=70，repeatTimes=2，src0BlockStride=1，src0RepeatStride=8，src1RepeatStride=0。

以下示例均基于Counter模式，该模式支持两种配置方式：

- 配置方式一：每次repeat操作mask个元素，总的数据计算量为repeatTimes * mask个元素。mask值配置为每一次repeat计算的元素个数。

- 配置方式二：总的数据计算量为mask个元素。mask配置为总的数据计算量。
repeatTimes值不生效，指令的迭代次数由源操作数和mask共同决定。

    <!-- npu="950" id13 -->
    Ascend 950PR/Ascend 950DT，支持配置方式一。
    <!-- end id13 -->

    <!-- npu="A3" id14 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持配置方式一。
    <!-- end id14 -->

    <!-- npu="910b" id15 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持配置方式一。
    <!-- end id15 -->

    <!-- npu="310b" id16 -->
    Atlas 200I/500 A2 推理产品，支持配置方式一。
    <!-- end id16 -->

    <!-- npu="310p" id17 -->
    Atlas 推理系列产品AI Core，支持配置方式二。
    <!-- end id17 -->

图1采用内置固定模式2进行计算，每个repeat取奇数索引元素。

图2、图3采用用户自定义模式进行计算，自定义输入的src1Pattern为\[0x99999999,...,0x99999999\]，对应二进制为1001…1001，每四个元素取第一个和第四个元素。

**图1**  GatherMask内置固定模式计算示意图

![](../../../../figures/GatherMask1.png)

<!-- npu="950,A3,910b,310b,310p" id18 -->
**图2**  GatherMask用户自定义模式计算示意图（配置方式一）

![](../../../../figures/GatherMask2.png)
<!-- end id18 -->

<!-- npu="310p" id19 -->
**图3**  GatherMask用户自定义模式计算示意图（配置方式二）

![](../../../../figures/GatherMask3.png)
<!-- end id19 -->

## 函数原型<a name="section620mcpsimp"></a>

- 用户自定义模式

    ```cpp
    template <typename T, typename U, GatherMaskMode mode = defaultGatherMaskMode>
    __aicore__ inline void GatherMask(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1Pattern, const bool reduceMode, const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)
    ```

- 内置固定模式

    ```cpp
    template <typename T, GatherMaskMode mode = defaultGatherMaskMode>
    __aicore__ inline void GatherMask(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const uint8_t src1Pattern, const bool reduceMode, const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| --- | --- |
| T | 源操作数src0和目的操作数dst的数据类型。 |
| U | 用户自定义模式下src1Pattern的数据类型。 |
| mode | 预留参数，为后续功能做预留，当前提供默认值，用户无需设置该参数。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| src0 | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。<br>数据类型需要与目的操作数保持一致。 |
| src1Pattern | 输入 | gather mask（数据收集的掩码），分为内置固定模式和用户自定义模式两种，根据内置固定模式对应的二进制掩码或者用户自定义输入的Tensor数值对应的二进制掩码从源操作数中选取元素写入目的操作数中。1为选取，0为不选取。具体说明请参考[功能说明](#section618mcpsimp)。<br>注：内置固定模式下src1RepeatStride参数无效。 |
| reduceMode | 输入 | 用于选择mask参数模式，数据类型为bool，支持如下取值：<br>&bull; false：Normal模式。该模式下，每次repeat操作256Bytes数据，总的数据计算量为repeatTimes * 256Bytes。此模式下mask参数无效，建议设置为0。<br>&bull; true：Counter模式。根据mask等参数含义的不同，该模式存在两种配置方式。具体说明请参考[功能说明](#section618mcpsimp)。<br>两种取值都支持：<br>&bull;按需配置src0BlockStride、src0RepeatStride参数。<br>&bull;支持src1Pattern配置为内置固定模式或用户自定义模式。用户自定义模式下可根据实际情况配置src1RepeatStride。<br> |
| mask | 输入 | 用于控制每次迭代内参与计算的元素。根据reduceMode，分为两种模式：<br>&bull; Normal模式：mask无效，建议设置为0。<br>&bull; Counter模式：取值范围[1, 2<sup>32</sup> – 1]。不同的版本型号Counter模式下，mask参数表示含义不同。具体配置规则参考上文reduceMode参数描述。<br> |
| gatherMaskParams | 输入 | 控制操作数地址步长的数据结构，GatherMaskParams类型。<br>具体参数说明[表3 GatherMaskParams结构体参数说明](#GatherMaskParams)。 |
| rsvdCnt | 输出 | 该条指令筛选后保留下来的元素计数，对应dstLocal中有效元素个数，数据类型为uint64_t。 |

**表3**  GatherMaskParams结构体参数说明<a id="GatherMaskParams"></a>

| 参数名称 | 描述 |
| --- | --- |
| src0BlockStride | 用于设置src0同一迭代不同DataBlock间的地址步长（起始地址之间的间隔）。单位为DataBlock。详细说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |
| repeatTimes | 重复迭代次数。矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTimes表示迭代的次数。<br>关于该参数的具体描述请参考[高维切分](../SIMD计算说明/高维切分.md)。 |
| src0RepeatStride | 用于设置src0相邻迭代间的地址步长（起始地址之间的间隔）。单位为DataBlock。详细说明请参考[repeatStride](../SIMD计算说明/高维切分.md)。 |
| src1RepeatStride | 用于设置src1相邻迭代间的地址步长（起始地址之间的间隔）。单位为DataBlock。详细说明请参考[repeatStride](../SIMD计算说明/高维切分.md)。 |

## 数据类型

<!-- npu="950" id20 -->

**表**  Ascend 950PR/Ascend 950DT数据类型组合情况

| src0数据类型 | src1Pattern数据类型 | dst数据类型 |
| --- | --- | --- |
| int8_t | uint8_t | int8_t |
| uint8_t | uint8_t | uint8_t |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| bfloat16_t | uint16_t | bfloat16_t |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id20 -->

<!-- npu="A3" id21 -->

**表**  Atlas A3 训练系列产品/Atlas A3 推理系列产品数据类型组合情况

| src0数据类型 | src1Pattern数据类型 | dst数据类型 |
| --- | --- | --- |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| bfloat16_t | uint16_t | bfloat16_t |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id21 -->

<!-- npu="910b" id22 -->

**表**  Atlas A2 训练系列产品/Atlas A2 推理系列产品数据类型组合情况

| src0数据类型 | src1Pattern数据类型 | dst数据类型 |
| --- | --- | --- |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| bfloat16_t | uint16_t | bfloat16_t |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id22 -->

<!-- npu="310b" id23 -->

**表**  Atlas 200I/500 A2 推理产品数据类型组合情况

| src0数据类型 | src1Pattern数据类型 | dst数据类型 |
| --- | --- | --- |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id23 -->

<!-- npu="310p" id24 -->

**表**  Atlas 推理系列产品AI Core数据类型组合情况

| src0数据类型 | src1Pattern数据类型 | dst数据类型 |
| --- | --- | --- |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id24 -->

## 返回值说明<a name="section198548421851"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md)。

- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md)。

- 采用内置固定模式时，配置src1RepeatStride参数无效。

- 若调用该接口前为Counter模式，在调用该接口后需要显式设置回Counter模式（接口内部执行结束后会设置为Normal模式）。

<!-- npu="950" id25 -->
- 对UB空间的占用说明。针对Ascend 950PR/Ascend 950DT：
  - 内置固定模式占用8KB Unified Buffer临时空间。
  - 用户自定义模式不涉及8KB Unified Buffer临时空间的占用。
<!-- end id25 -->

## 调用示例<a name="section642mcpsimp"></a>

-   用户自定义Tensor示例，完整的调用样例可参考[Gather类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather)场景二。

    ```cpp
    uint32_t mask = 70; // 每次迭代内参与计算的元素
    uint64_t rsvdCnt = 0; // 保留下来的元素个数

    // src0Local：源操作数
    // src1Local：存放数据收集的掩码的Tensor
    // dstLocal：目的操作数
    // reduceMode = true;使用Counter模式
    // {}中的参数为：
    // src0BlockStride = 1;单次迭代内数据间隔1个DataBlock，即数据连续读取和写入
    // repeatTimes = 2;      Counter模式时，仅在部分产品型号下会生效
    // src0RepeatStride = 4;源操作数迭代间数据间隔4个DataBlock
    // src1RepeatStride = 0; src1迭代间数据间隔0个DataBlock，即原位置读取
    AscendC::GatherMask (dstLocal, src0Local, src1Local, true, mask, { 1, 2, 4, 0 }, rsvdCnt);
    ```

- 内置固定模式示例，完整的调用样例可参考[Gather类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather)场景一。

    ```cpp
    uint32_t mask = 0; // 每次迭代内参与计算的元素，normal模式下mask建议设置为0
    uint64_t rsvdCnt = 0; // 用于保存筛选后保留下来的元素个数
    uint8_t src1Pattern = 2; // 内置固定模式

    // src0Local：源操作数
    // src1Pattern：内置固定模式取值
    // src0Local：目的地址与源地址复用
    // reduceMode = false;使用normal模式
    // {}中的参数为：
    // src0BlockStride = 1;单次迭代内数据间隔1个Block，即数据连续读取和写入
    // repeatTimes = 4;重复迭代4次
    // src0RepeatStride = 8;源操作数迭代数据间隔8个DataBlock
    // src1RepeatStride = 0;重复一次，故设置为0
    AscendC::GatherMask(src0Local, src0Local, src1Pattern, false, mask, { 1, 4, 8, 0 }, rsvdCnt);
    ```

    结果示例如下：

    ```plain
    输入数据src0Local：[1 2 3 ... 256]
    输入数据src1Pattern：src1Pattern = 2;
    输出数据dstLocal：[
    2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62 64 66 68 70 72 74 76 78 80 82 84 86 88 90 92 94 96 98 100 102 104 106 108 110 112 114 116 118 120 122 124 126 128 130 132 134 136 138 140 142 144 146 148 150 152 154 156 158 160 162 164 166 168 170 172 174 176 178 180 182 184 186 188 190 192 194 196 198 200 202 204 206 208 210 212 214 216 218 220 222 224 226 228 230 232 234 236 238 240 242 244 246 248 250 252 254 256 undefined ..undefined]
    输出数据rsvdCnt：128
    ```
