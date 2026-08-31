# Abs<a name="ZH-CN_TOPIC_0000001956810269"></a>

## 产品支持情况

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/basic_arithmetic/Abs_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径：`"basic_api/reg_compute/kernel_reg_compute_vec_unary_intf.h"`。

- 对实数类型

    对srcReg中的有效元素逐个取绝对值，并将结果写入dstReg对应位置，计算公式如下：

    $$
    dstReg_i = |srcReg_i|
    $$

- 对复数类型

    对srcReg中有效元素逐个取模，并将结果写入dstReg对应位置，计算公式如下：

    $$
    dstReg_i = |srcReg_i| = (\alpha^2 + \beta^2)^{1/2}
    $$

    其中$srcReg_i = \alpha + \beta i$，α为复数的实部，β为复数的虚部。

## 函数原型<a name="section620mcpsimp"></a>

- 实数计算

    ```cpp
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
    __simd_callee__ inline void Abs(U& dstReg, U& srcReg, MaskReg& mask)
    ```

- 复数计算

    ```cpp
    template <typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
    __simd_callee__ inline void Abs(S& dstReg, V& srcReg, MaskReg& mask)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 目的操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 源操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md)枚举类型，选择MERGING模式或ZEROING模式。<br>&bull; ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>&bull; MERGING模式当前不支持。 |
| S | 目的操作数的RegTensor类型，例如RegTensor&lt;half&gt;，由编译器自动推导，用户不需要填写。 |
| V | 源操作数的RegTensor类型，例如RegTensor&lt;half&gt;，由编译器自动推导，用户不需要填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| srcReg | 输入 | 源操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../register_data_types/MaskReg.md)。 |

## 数据类型

**表 3**  数据类型组合

| srcReg | dstReg |
| --- | --- |
| int8_t | int8_t |
| int16_t | int16_t |
| half | half |
| int32_t | int32_t |
| float | float |
| complex32 | half |
| int64_t | int64_t |
| complex64 | float |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 当目的操作数和源操作数数据类型不一致时，目的操作数和源操作数不可重叠。
- 整型数据的计算结果如果超出数据类型的表示范围会采取非饱和截断，比如int8_t类型，srcReg为-128，其绝对值128会被截断成-128。
<!-- npu="950" id8 -->
- 针对Ascend 950PR/Ascend 950DT，仅complex32和complex64类型的接口内部计算涉及Subnormal，处理方式受编译选项--cce-ftz控制（默认值为true）：
    - --cce-ftz=false时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    - --cce-ftz=true时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id8 -->

## 调用示例<a name="section642mcpsimp"></a>

- 实数计算

    ```cpp
    template<typename T>
    __simd_vf__ inline void AbsVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Abs(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

- 复数计算

    ```cpp
    template<typename T, typename U>
    __simd_vf__ inline void AbsVF(__ubuf__ U* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<U> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T>(count);
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::Abs(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```
