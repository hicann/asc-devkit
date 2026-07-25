# Select（灵活标量位置）<a name="ZH-CN_TOPIC_0000002171712357"></a>

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

给定两个源操作数src0和src1，根据selMask（用于选择的Mask掩码）的比特位值选取元素，得到目的操作数dst。选择的规则为：当selMask的比特位是1时，从src0中选取，比特位是0时从src1选取。

对于tensor高维切分计算接口，支持根据mask参数对上述选取结果，再次进行过滤，有效位填入最终的dst，无效位则保持dst原始值。例如：src0为\[1,2,3,4,5,6,7,8\]，src1为\[9,10,11,12,13,14,15,16\]，selMask为\[0,0,0,0,1,1,1,1\]，mask为\[1,1,1,1,0,0,0,0\]，dst原始值为\[-1,-2,-3,-4,-5,-6,-7,-8\]，则根据selMask的比特位选取后的结果dst\_temp为：\[9,10,11,12,5,6,7,8\]，然后再根据mask进行过滤，dst的最终输出结果为\[9,10,11,12,-5,-6,-7,-8\]。

本选择功能支持三种模式：

-   模式0：根据selMask在两个tensor中选取元素。selMask中有效数据的个数存在限制，具体取决于源操作数的数据类型。在每一轮迭代中，根据selMask的有效位数据进行选择操作，每一轮迭代采用的selMask，均为相同数值，即selMask的有效数值。
-   模式1：根据selMask在1个tensor和1个scalar标量中选取元素，selMask无有效数据限制。多轮迭代时，每轮迭代连续使用selMask的不同部分。
-   模式2：根据selMask在两个tensor中选取元素，selMask无有效数据限制。多轮迭代时，每轮迭代连续使用selMask的不同部分。

针对模式1，提供灵活标量位置的接口。

## 函数原型<a name="section620mcpsimp"></a>

-   tensor前n个数据计算

    ```cpp
    template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
    __aicore__ inline void Select(const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint32_t count)
    ```

-   tensor高维切分计算
    -   mask逐bit模式

        ```cpp
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Select(const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

    -   mask连续模式

        ```cpp
        template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
        __aicore__ inline void Select(const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
        ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 含义 |
| :--- | :--- |
| T0 | 源操作数和目的操作数的数据类型。<br>特别地，对于灵活标量位置接口，为预留参数，暂未启用，为后续的功能扩展做保留，需要指定时，传入默认值BinaryDefaultType即可。 |
| T1 | selMask的数据类型。 |
| isSetMask | 预留参数，保持默认值即可。如需使用在接口外部设置mask的功能，可以调用不传入mask参数的接口来实现。 |
| selMode | 同[表2 参数说明](#table8955841508)中的selMode参数说明。 |
| config | 类型为BinaryConfig，当标量为LocalTensor单点元素类型时生效，用于指定单点元素操作数位置。默认值DEFAULT_BINARY_CONFIG，表示右操作数为标量。<br>struct BinaryConfig {<br>&nbsp;&nbsp;&nbsp;&nbsp;int8_t scalarTensorIndex = 1; // 用于指定标量为LocalTensor单点元素时标量的位置，0表示左操作数，1表示右操作数<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| T2 | LocalTensor类型，根据输入参数dst自动推导相应的数据类型，开发者无需配置该参数，保证dst满足数据类型的约束即可。 |
| T3 | LocalTensor类型或标量类型，根据输入参数src0自动推导相应的数据类型，开发者无需配置该参数，保证src0满足数据类型的约束即可。 |
| T4 | LocalTensor类型或标量类型，根据输入参数src1自动推导相应的数据类型，开发者无需配置该参数，保证src1满足数据类型的约束即可。 |

**表2**  参数说明

<a name="table8955841508"></a>
| 参数名称 | 输入/输出 | 含义 |
| :--- | :--- | :--- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐 |
| selMask | 输入 | 选取mask。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。<br>Ascend 950PR/Ascend 950DT，支持的数据类型为：uint8_t/uint16_t/uint32_t/uint64_t。<br>每个比特位表示1个元素的选取，当selMask的比特位为1时，从src0中选取元素；比特位为0时，从src1中选取元素。<br>selMode为模式0时，在每一轮迭代中，根据selMask的有效位数据进行选择操作，每一轮迭代采用的selMask，均为相同数值，即selMask的有效数值。selMode为模式1/2时，多次迭代对selMask连续消耗。<br>- 模式0：根据selMask在两个tensor中选取元素，selMask有位数限制，不管迭代多少次，每次迭代都只根据截取后的固定位数的selMask进行选择。当源操作数的数据类型为8位时，selMask前256比特位有效；当源操作数的数据类型为16位时，selMask前128比特位有效；源操作数的数据类型为32位时，selMask前64比特位有效；源操作数的数据类型为64位时，selMask前32比特位有效。<br>- 模式1：根据selMask在1个tensor和1个scalar标量中选取元素。支持多次迭代，选取方式为，根据selMask的设置值，如果selMask比特值为1，则选择src0内的同位置数值，如果selMask比特值为0，则选择标量值。selMask连续存放，当源操作数的数据类型为8位时，一次比较获取selMask256bit长度的数据；当源操作数的数据类型为16位时，一次比较获取selMask128bit长度的数据；源操作数的数据类型为32位时，一次比较获取selMask64bit长度的数据；源操作数的数据类型为64位时，一次比较获取selMask32bit长度的数据。<br>- 模式2：根据selMask在两个tensor中选取元素。支持多次迭代，选取方式为，根据selMask的设置值，如果selMask比特值为1，则选择src0内的同位置数值，如果selMask比特值为0，则选择src1内的同位置数值。selMask连续存放，当源操作数的数据类型为8位时，一次比较获取selMask256bit长度的数据；当源操作数的数据类型为16位时，一次比较获取selMask128bit长度的数据；源操作数的数据类型为32位时，一次比较获取selMask64bit长度的数据；源操作数的数据类型为64位时，一次比较获取selMask32bit长度的数据。 |
| src0/src1 | 输入 | 灵活标量位置接口中源操作数。<br>支持类型为LocalTensor或标量。数据类型需要与目的操作数保持一致。<br> 类型为LocalTensor时，支持当作矢量操作数或标量单点元素，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| selMode | 输入 | 指令模式，SELMODE类型，取值如下：<br>enum class SELMODE : uint8_t {<br>&nbsp;&nbsp;&nbsp;&nbsp;VSEL_CMPMASK_SPR = 0,<br>&nbsp;&nbsp;&nbsp;&nbsp;VSEL_TENSOR_SCALAR_MODE,<br>&nbsp;&nbsp;&nbsp;&nbsp;VSEL_TENSOR_TENSOR_MODE,<br>};<br>- 模式0：取值为VSEL_CMPMASK_SPR。根据selMask在两个tensor中选取元素。selMask中有效数据的个数存在限制，具体取决于源操作数的数据类型。在每一轮迭代中，根据selMask的有效位数据进行选择操作，每一轮迭代采用的selMask，均为相同数值，即selMask的有效数值。<br>- 模式1：取值为VSEL_TENSOR_SCALAR_MODE。根据selMask在1个tensor和1个scalar标量中选取元素，selMask无有效数据限制。多轮迭代时，每轮迭代连续使用selMask的不同部分。<br>- 模式2：取值为VSEL_TENSOR_TENSOR_MODE。根据selMask在两个tensor中选取元素，selMask无有效数据限制。多轮迭代时，每轮迭代连续使用selMask的不同部分。 |
| mask/mask[] | 输入 | mask用于控制每次迭代内参与计算的元素。详细设置参考[掩码](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体描述请参考[高维切分API](../SIMD计算说明/高维切分.md)。 |
| repeatParams | 输入 | 控制操作数地址步长的参数。[BinaryRepeatParams](../../辅助数据结构/BinaryRepeatParams.md)类型，包含操作数相邻迭代间相同datablock的地址步长，操作数同一迭代内不同datablock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。 |
| count | 输入 | 参与计算的元素个数。 |

## 数据类型

<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT，源操作数dst和目的操作数src支持的数据类型为：int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/float/int32_t/uint32_t/complex32/int64_t/uint64_t/complex64。其中，int8\_t/uint8\_t/uint64\_t/int64\_t/complex32/complex64数据类型仅支持tensor前n个数据计算接口。
<!-- end id8 -->

## 返回值说明<a name="section198548421851"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

-   调用灵活标量位置接口且源操作数为LocalTensor单点元素的场景，不支持源操作数和目的操作数地址重叠。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

-   左操作数及右操作数中，必须有一个为矢量；当前不支持左右操作数同时为标量。
-   本接口传入LocalTensor单点数据作为标量时，idx参数需要传入编译期已知的常量，传入变量时需要声明为constexpr。
-   模式1场景使用灵活标量位置接口时需要填写模板参数config避免接口匹配到其他模式。

## 调用示例<a name="section642mcpsimp"></a>

-   Select-tensor前n个数据计算样例（模式1）

    ```cpp
    // 灵活标量位置，src1Local[0]作为标量
    static constexpr AscendC::BinaryConfig config = { 1 };
    AscendC::Select<BinaryDefaultType, uint8_t, config>(dstLocal, maskLocal, src0Local, src1Local[0], AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, dataSize);
    
    // 灵活标量位置，src0Local[0]作为标量
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Select<BinaryDefaultType, uint8_t, config>(dstLocal, maskLocal, src0Local[0], src1Local, AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, dataSize);
    ```
