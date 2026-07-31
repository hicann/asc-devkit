# Compares（灵活标量位置）<a name="ZH-CN_TOPIC_0000002136591178"></a>

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

提供灵活标量位置的接口，支持标量在前和标量在后两种场景。其中标量输入支持配置LocalTensor单点元素，计算公式如下，idx表示LocalTensor单点元素的位置系数。

$dst_i = Compares(src_i, scalar)$

$dst_i = Compares(scalar, src_i)$

$dst_i = Compares(src_i, scalar_{idx})$

$dst_i = Compares(scalar_{idx}, src_i)$

支持多种比较模式：

-   LT：小于（less than）
-   GT：大于（greater than）

-   GE：大于或等于（greater than or equal to）
-   EQ：等于（equal to）
-   NE：不等于（not equal to）
-   LE：小于或等于（less than or equal to）

## 函数原型<a name="section620mcpsimp"></a>

-   tensor前n个数据计算

    ```cpp
    template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
    __aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, uint32_t count)
    ```

-   tensor高维切分计算
    -   mask逐bit模式

        ```cpp
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    -   mask连续模式

        ```cpp
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T0 | 对于固定标量位置接口，表示源操作数数据类型。<br>特别地，对于灵活标量位置接口，为预留参数，暂未启用，为后续的功能扩展做保留，需要指定时，传入默认值BinaryDefaultType即可。 |
| T1 | 对于固定标量位置接口，表示目的操作数数据类型。<br>特别地，对于灵活标量位置接口，为预留参数，暂未启用，为后续的功能扩展做保留，需要指定时，传入默认值BinaryDefaultType即可。 |
| isSetMask | 是否在接口内部设置mask。<br>- true，表示在接口内部设置mask。<br>- false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符MASK_PLACEHOLDER，用于占位，无实际含义。 |
| config | 类型为BinaryConfig，当标量为LocalTensor单点元素类型时生效，用于指定单点元素操作数位置。默认值DEFAULT_BINARY_CONFIG，表示右操作数为标量。<br>struct BinaryConfig {<br>&nbsp;&nbsp;&nbsp;&nbsp;int8_t scalarTensorIndex = 1; // 用于指定标量为LocalTensor单点元素时标量的位置，0表示左操作数，1表示右操作数<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| T2 | LocalTensor类型，根据输入参数dst自动推导相应的数据类型，开发者无需配置该参数，保证dst满足数据类型的约束即可。 |
| T3 | LocalTensor类型或标量类型，根据输入参数src0自动推导相应的数据类型，开发者无需配置该参数，保证src0满足数据类型的约束即可。 |
| T4 | LocalTensor类型或标量类型，根据输入参数src1自动推导相应的数据类型，开发者无需配置该参数，保证src1满足数据类型的约束即可。 |

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。<br>dst用于存储比较结果，将dst中uint8_t类型的数据按照bit位展开，由左至右依次表征对应位置的src0和src1的比较结果，如果比较后的结果为真，则对应比特位为1，否则为0。 |
| src0/src1 | 输入 | 灵活标量位置接口中源操作数。<br>支持类型为LocalTensor或标量。数据类型需要与目的操作数保持一致。<br> 类型为LocalTensor时，支持当作矢量操作数或标量单点元素，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。|
| cmpMode | 输入 | CMPMODE类型，表示比较模式，包括EQ，NE，GE，LE，GT，LT。<br>- LT:src0小于（less than）src1<br>- GT:src0大于（greater than）src1<br>- GE：src0大于或等于（greater than or equal to）src1<br>- EQ：src0等于（equal to）src1<br>- NE：src0不等于（not equal to）src1<br>- LE：src0小于或等于（less than or equal to）src1 |
| mask/mask[] | 输入 | mask用于控制每次迭代内参与计算的元素。详细设置参考[掩码](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体描述请参考[高维切分API](../SIMD计算说明/高维切分.md)。 |
| repeatParams | 输入 | 控制操作数地址步长的参数。[UnaryRepeatParams](../../辅助数据结构//UnaryRepeatParams.md)类型，包含操作数相邻迭代间相同DataBlock的地址步长，操作数同一迭代内不同DataBlock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |
| count | 输入 | 参与计算的元素个数。**设置count时，需要保证count个元素所占空间256字节对齐。** |

## 数据类型

<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT，目的操作数dst支持的数据类型为：uint8_t。源操作数src支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、float、int32_t、uint32_t、int64_t、uint64_t、double。其中，int8\_t、uint8\_t、uint64\_t、int64\_t、double数据类型仅支持tensor前n个数据计算接口，double数据类型只支持CMPMODE::EQ。
<!-- end id8 -->

## 返回值说明<a name="section128671456102513"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
-   调用灵活标量位置接口且源操作数为LocalTensor单点元素的场景，不支持源操作数和目的操作数地址重叠。

-   dst按照小端顺序排序成二进制结果，对应src中相应位置的数据比较结果。
-   使用tensor前n个数据参与计算的接口，设置count时，需要保证count个元素所占空间256字节对齐。
-   左操作数及右操作数中，必须有一个为矢量；当前不支持左右操作数同时为标量。
-   本接口传入LocalTensor单点数据作为标量时，idx参数需要传入编译期已知的常量，传入变量时需要声明为constexpr。

## 调用示例<a name="section642mcpsimp"></a>

对于灵活标量位置接口，支持直接传入立即数或单点LocalTensor作为标量，并且支持标量在前和在后两种调用方式。

完整的调用样例可参考[Compare类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare)场景四。

-   tensor前n个数据计算接口样例

    ```cpp
    // 标量在后，src1Local[0]作为标量
    AscendC::Compares(dstLocal, src0Local, src1Local[0], AscendC::CMPMODE::LT, srcDataSize);
    
    // 标量在前，src0Local[0]作为标量
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Compares<BinaryDefaultType, BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, AscendC::CMPMODE::LT, srcDataSize);
    ```

-   tensor高维切分计算-mask连续模式

    ```cpp
    uint64_t mask = 256 / sizeof(float); // 256为每个迭代处理的字节数
    int repeat = 4;
    AscendC::UnaryRepeatParams repeatParams = { 1, 1, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    // 标量在后，src1Local[0]作为标量
    AscendC::Compares(dstLocal, src0Local, src1Local[0], AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    
    // 标量在前，src0Local[0]作为标量
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Compares<BinaryDefaultType, BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    ```

-   tensor高维切分计算-mask逐bit模式

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, 0};
    int repeat = 4;
    AscendC::UnaryRepeatParams repeatParams = { 1, 1, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // srcBlkStride, = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    // 标量在后，src1Local[0]作为标量
    AscendC::Compares(dstLocal, src0Local, src1Local[0], AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    
    // 标量在前，src0Local[0]作为标量
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Compares<BinaryDefaultType, BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    ```
