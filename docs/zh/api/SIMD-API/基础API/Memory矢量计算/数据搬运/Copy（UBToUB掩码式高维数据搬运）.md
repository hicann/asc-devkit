# Copy（UBToUB掩码式高维数据搬运）<a name="ZH-CN_TOPIC_0000002575088175"></a>

## 产品支持情况<a name="zh-cn_topic_0000002567699435_section796754519912"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a name="zh-cn_topic_0000002567699435_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

支持Unified Buffer和Unified Buffer之间的数据搬运，数据搬运时格式和内容保持不变，支持mask操作和DataBlock间隔操作。

具体支持的数据通路为（以[逻辑位置TPosition](../../辅助数据结构/TPosition.md)表示）：

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

## 函数原型<a name="zh-cn_topic_0000002567699435_section82039854412"></a>

- mask逐bit模式

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```

- mask连续模式 

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask, const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```

## 参数说明<a name="zh-cn_topic_0000002567699435_section16128134420472"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 源操作数或者目的操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002567699435_section4219135304818)。 |
| isSetMask | 是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数，类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor简介.md)，存储位置为Unified Buffer，目的地址需要32字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor，存储位置为Unified Buffer，源地址需要32字节对齐。 |
| mask[]/mask | 输入 | mask用于控制每次迭代内参与计算的元素。详细设置参考[掩码](../SIMD计算说明/掩码.md)。 |
| repeatTime | 输入 | 重复迭代次数。矢量计算单元，每次读取连续的256字节数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体描述请参考[高维切分](../SIMD计算说明/高维切分.md)。|
| repeatParams | 输入 | 控制操作数地址步长的参数。<br>CopyRepeatParams类型，包含操作数相邻迭代间相同DataBlock的地址步长，操作数同一迭代内不同DataBlock的地址步长等参数。CopyRepeatParams参数说明请参考表3。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表3**  CopyRepeatParams结构体参数定义

| 参数名 | 描述 |
| --- | --- |
| dstStride | 目的操作数同一迭代内相邻DataBlock的地址步长，数据类型为uint16_t，取值范围：dstStride∈[0, 65535]，单位：DataBlock（32B）。 |
| srcStride | 源操作数同一迭代内相邻DataBlock的地址步长，数据类型为uint16_t，取值范围：srcStride∈[0, 65535]，单位：DataBlock（32B） |
| dstRepeatSize | 目的操作数相邻迭代间相同DataBlock的地址步长，数据类型为uint16_t，取值范围：dstRepeatSize∈[0, 4095]，单位：DataBlock（32B）。 |
| srcRepeatSize | 源操作数相邻迭代间相同DataBlock的地址步长，数据类型为uint16_t，取值范围：srcRepeatSize∈[0, 4095]，单位：DataBlock（32B）。 |

## 数据类型<a name="zh-cn_topic_0000002567699435_section4219135304818"></a>

源操作数和目的操作数支持的数据类型保持一致。

<!-- npu="950" id10 -->

- Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。

<!-- end id10 -->

<!-- npu="A3" id11 -->

- Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

<!-- end id11 -->

<!-- npu="910b" id12 -->

- Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

<!-- end id12 -->

<!-- npu="310b" id13 -->

- Atlas 200I/500 A2 推理产品，支持的数据类型为：int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

<!-- end id13 -->

<!-- npu="x90" id14 -->

- Kirin X90，支持的数据类型为：int16_t、uint16_t、half、int32_t、uint32_t、float。

<!-- end id14 -->

<!-- npu="9030" id15 -->

- Kirin 9030，支持的数据类型为：int16_t、uint16_t、half、int32_t、uint32_t、float。

<!-- end id15 -->

## 返回值说明

无

## 约束说明<a name="zh-cn_topic_0000002567699435_section2045914466492"></a>

- 位于Unified Buffer的地址必须32字节对齐。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#通用地址重叠约束)。
- CopyRepeatParams结构体参数的值需在取值范围内：

    **表4**  CopyRepeatParams结构体参数取值范围

    | 参数名 | 取值范围 |
    | --- | --- |
    | dstStride | [0, 65535] |
    | srcStride | [0, 65535] |
    | dstRepeatSize | [0, 4095] |
    | srcRepeatSize | [0, 4095] |

<!-- npu="A3,910b,950" id20 -->
- 当参数repeatTime取值为0时，该接口的行为如下：
    <!-- npu="A3,910b" id18 -->
  - 针对如下型号，当参数repeatTime取值为0时，不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
     <!-- npu="A3" id16 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
     <!-- end id16 -->
     <!-- npu="910b" id17 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
     <!-- end id17 -->
    <!-- end id18 -->
    <!-- npu="950" id19 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../Reg矢量计算/Reg矢量计算.md)API实现兼容，当参数repeatTime取值为0时，不保证该接口被视为NOP（空操作）。
    <!-- end id19 -->
<!-- end id20 -->

- Copy和矢量计算API一样，支持和掩码操作API配合使用。但Counter模式配合高维切分计算API时，和[通用的Counter模式](../SIMD计算说明/掩码.md#接口内设置mask)有一定差异。具体差异如下：
    - 通用的Counter模式：Mask代表**整个矢量计算参与计算的元素个数，迭代次数不生效。**
    - Copy高维切分计算API的Counter模式：Mask代表**每次Repeat中处理的元素个数，迭代次数生效。**
    图1中Copy高维切分计算API的Counter模式时，迭代次数生效，每次迭代参与计算的DataBlock个数可通过Mask计算得出：

    $$\text{N}=\text{ceil}(\text{Mask}*\text{sizeof(T)}/\text{datablockSize})$$

    **图1**  Copy高维切分计算API的Counter模式  
    ![](../../../../figures/repeat-times-12.png)

## 调用示例<a name="zh-cn_topic_0000002567699435_section088124295117"></a>

```cpp
    if constexpr (scenarioNum == 1) {
        // 场景1：[1, 512]数据搬运，mask连续模式，源和目的空间一样大。
        // mask=64表示每次迭代处理64个int32元素。
        // repeatTime=8表示迭代8次，共处理512个元素。
        // srcStride=1, dstStride=1：同一迭代内DataBlock无间隙。
        // srcRepeatSize=8, dstRepeatSize=8：相邻迭代间步长为8个block（每个block 8个int32元素）。
        uint64_t mask = 64;
        AscendC::Copy(dstLocal, srcLocal, mask, 8, {1, 1, 8, 8});
    } else if constexpr (scenarioNum == 2) {
        // 场景2：从[18, 64]的数据里搬运[18, 8]的数据量。
        // 源数据shape: [18, 64]，共1152个元素。
        // 目的数据shape: [18, 8]，共144个元素。
        // 每次迭代处理一行中的8个元素，共18次迭代。
        // mask=8：每次迭代处理8个元素。
        // repeatTime=18：迭代18次（对应18行）。
        // srcStride=1, dstStride=1：同一迭代内无间隙。
        // srcRepeatSize=8：源每次迭代后跳过64个元素（跳到下一行起始位置）。
        // dstRepeatSize=1：目的每次迭代后跳过8个元素（紧凑排列下一行）。
        uint64_t mask = 8;
        AscendC::Copy(dstLocal, srcLocal, mask, 18, {1, 1, 1, 8});
    } else if constexpr (scenarioNum == 3) {
        // 场景3：从[18, 64]搬运[18, 8]数据，Counter模式。
        // 源数据shape: [18, 64]，共1152个元素。
        // 目的数据shape: [18, 8]，共144个元素。
        // Counter模式下，Mask代表每次Repeat中处理的元素个数。
        // mask=144：每次迭代处理144个元素。
        // repeatTime=1：迭代1次（对应18行）。
        // srcStride=8：源数据两个DataBlock地址步长为8（每次选择每一行的起始DataBlock）。
        // dstStride=1：目的数据连续存放无间隙。
        // srcRepeatSize=8, dstRepeatSize=8：repeatTime=1场景下该参数无效。
        AscendC::SetMaskCount();
        AscendC::SetVectorMask<int32_t, AscendC::MaskMode::COUNTER>(144);
        AscendC::Copy<int32_t, false>(dstLocal, srcLocal, AscendC::MASK_PLACEHOLDER, 1, {1, 8, 8, 8});
        AscendC::SetMaskNorm();
        AscendC::ResetMask();
    } else {
        uint64_t mask = 64;
        AscendC::Copy(dstLocal, srcLocal, mask, 8, {1, 1, 8, 8});
    }
```

完整样例请参考[Copy样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/copy_ub2ub)。
