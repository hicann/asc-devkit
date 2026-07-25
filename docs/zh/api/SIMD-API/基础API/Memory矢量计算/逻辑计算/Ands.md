# Ands<a name="ZH-CN_TOPIC_0000002167909713"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

矢量内每个元素和标量间做与操作，支持标量在前和标量在后两种场景，其中标量输入支持配置LocalTensor单点元素。计算公式如下，idx表示LocalTensor单点元素的位置系数。

$dst_i = src_i \& scalar$

$dst_i = scalar \& src_i$

$dst_i = src_i \& scalar_{idx}$

$dst_i = scalar_{idx} \& src_i$

## 函数原型<a name="section620mcpsimp"></a>

-   Tensor前n个数据计算

    ```cpp
    template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
    __aicore__ inline void Ands(const U& dst, const S& src0, const V& src1, const int32_t& count)
    ```

-   Tensor高维切分计算
    -   mask逐比特模式

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Ands(const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    -   mask连续模式

        ```cpp
        template <typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename U, typename S, typename V>
        __aicore__ inline void Ands(const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T | 预留参数，暂未启用，为后续的功能扩展做保留，需要指定时，传入默认值BinaryDefaultType即可。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下：<br>&nbsp;&nbsp;&bull; 针对tensor前n个数据计算接口，接口入参中的count不生效，建议设置成1。<br>&nbsp;&nbsp;&bull; 针对tensor高维切分计算接口，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。|
| config | 类型为BinaryConfig，当标量为LocalTensor单点元素类型时生效，用于指定单点元素操作数位置。默认值DEFAULT_BINARY_CONFIG，表示右操作数为标量。<br>struct BinaryConfig {<br>&nbsp;&nbsp;&nbsp;&nbsp;int8_t scalarTensorIndex = 1; // 用于指定标量为LocalTensor单点元素时标量的位置，0表示左操作数，1表示右操作数<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| U | LocalTensor类型，根据输入参数dst自动推导相应的数据类型，开发者无需配置该参数，保证dst满足数据类型的约束即可。 |
| S | LocalTensor类型或标量类型，根据输入参数src0自动推导相应的数据类型，开发者无需配置该参数，保证src0满足数据类型的约束即可。 |
| V | LocalTensor类型或标量类型，根据输入参数src1自动推导相应的数据类型，开发者无需配置该参数，保证src1满足数据类型的约束即可。 |

**表2**  参数说明

| **参数名称** | **类型** | **说明** |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。|
| src0/src1 | 输入 | 源操作数。<br>支持类型为LocalTensor或标量。数据类型需要与目的操作数保持一致。<br> 类型为LocalTensor时，支持当作矢量操作数或标量单点元素，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| count | 输入 | 参与计算的元素个数。 |
| mask[]/mask | 输入 | mask用于控制每次迭代内参与计算的元素。详细设置参考[掩码](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体描述请参考[高维切分API](../SIMD计算说明/高维切分.md)。 |
| repeatParams | 输入 | 元素操作控制结构信息，具体请参考[UnaryRepeatParams](../../辅助数据结构//UnaryRepeatParams.md)。 |

## 数据类型

<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT，目的操作数dst和源操作数src支持的数据类型为：int16_t、uint16_t、int64_t、uint64_t。数据类型int64_t、uint64_t仅支持tensor前n个数据计算接口。
<!-- end id8 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

-   使用Tensor高维切分计算接口时，节省地址空间，开发者可以定义一个Tensor，供源操作数与目的操作数同时使用（即地址重叠），相关约束如下：
    -   对于单次repeat（repeatTime=1），且源操作数与目的操作数之间要求100%完全重叠，不支持部分重叠。
    -   对于多次repeat（repeatTime\>1），操作数与目的操作数之间存在依赖的情况下，即第N次迭代的目的操作数是第N+1次的源操作数，不支持地址重叠。
    -   源操作数为LocalTensor单点元素的场景，不支持源操作数和目的操作数地址重叠。

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

<!-- npu="950" id9 -->
-   针对Ascend 950PR/Ascend 950DT，tensor前n个数据计算API中的isSetMask参数不生效，保持默认值即可。
<!-- end id9 -->
-   左操作数及右操作数中，必须有一个为矢量；当前不支持左右操作数同时为标量。
-   本接口传入LocalTensor单点数据作为标量时，idx参数需要传入编译期已知的常量，传入变量时需要声明为constexpr。

## 调用示例<a name="section642mcpsimp"></a>

-   Tensor高维切分计算样例-mask连续模式

    ```cpp
    // dstLocal：输出Tensor
    // src0Local：输入Tensor
    // src1Local：输入Tensor

    uint64_t mask = 128;
    // repeatTime = 4,单次迭代处理128个数，计算512个数需要迭代4次
    // dstBlkStride, srcBlkStride = 1,每个迭代内src0参与计算的数据地址间隔为1个datablock，表示单次迭代内数据连续读取和写入
    // dstRepStride, srcRepStride = 8,相邻迭代间的地址间隔为8个datablock，表示相邻迭代间数据连续读取和写入
    // 标量在后示例
    AscendC::Ands(dstLocal, src0Local, src1Local[0], mask, 4, { 1, 1, 8, 8 });

    // 标量在前示例
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ands<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, {1, 1, 8, 8});
    ```

-   Tensor高维切分计算样例-mask逐bit模式

    ```cpp
    // dstLocal：输出Tensor
    // src0Local：输入Tensor
    // src1Local：输入Tensor

    uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
    // repeatTime = 4,单次迭代处理128个数，计算512个数需要迭代4次
    // dstBlkStride, srcBlkStride = 1,每个迭代内src0参与计算的数据地址间隔为1个datablock，表示单次迭代内数据连续读取和写入
    // dstRepStride, srcRepStride = 8,相邻迭代间的地址间隔为8个datablock，表示相邻迭代间数据连续读取和写入
    // 标量在后示例
    AscendC::Ands(dstLocal, src0Local, src1Local[0], mask, 4, {1, 1, 8, 8});

    // 标量在前示例
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ands<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, mask, 4, {1, 1, 8, 8});
    ```

-   Tensor前n个数据计算样例

    ```cpp
    // 标量在后示例
    AscendC::Ands(dstLocal, src0Local, src1Local[0], 512);

    // 标量在前示例
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ands<BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, 512);
    ```

结果示例如下：

```plain
// 标量在后，src1Local[0]作为标量
输入数据src0Local: [1 2 3 ... 512]
输入数据src1Local: [0]
输出数据dstLocal: [0 0 0 ... 0]

// 标量在前，src0Local[0]作为标量
输入数据src0Local: [0]
输入数据src1Local: [1 2 3 ... 512]
输出数据dstLocal: [0 0 0 ... 0]
```
