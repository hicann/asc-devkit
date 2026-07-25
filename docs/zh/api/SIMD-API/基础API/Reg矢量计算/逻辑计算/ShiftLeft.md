# ShiftLeft<a name="ZH-CN_TOPIC_0000002011932045"></a>

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

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"`。

`ShiftLeft`指令根据mask对源操作数srcReg0，按照srcReg1对应元素进行左移操作，将结果写入目的操作数dstReg。根据源操作数的数据类型，左移操作分为以下两种情况：

- **数据类型为无符号类型：执行逻辑左移。**

  逻辑左移会将二进制数整体向左移动指定的位数，最高位被丢弃，最低位用0填充。例如，二进制数1010101010101010（uint16\_t类型）逻辑左移1位后，结果为0101010101010100。
- **数据类型为有符号类型：执行算术左移。**

  算术左移会将二进制数整体向左移动指定的位数，次高位被丢弃，最低位用0填充。例如，二进制数1010101010101010（int16\_t类型）算术左移1位后，结果为1101010101010100；算术左移3位后，结果为1101010101010000。

$$
dstReg_i = srcReg0_i \ll srcReg1_i
$$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void ShiftLeft(S& dstReg, S& srcReg0, V& srcReg1, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 源操作数数据类型。 |
| U | 左移参数数据类型。 |
| mode | [MaskMergeMode](../辅助数据类型/MaskMergeMode.md)枚举类型。选择MERGING模式或ZEROING模式。<br>• ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>• MERGING模式当前不支持。 |
| S | 目的操作数和源操作数的`RegTensor`类型，例如`RegTensor<uint32_t>`，由编译器自动推导，用户不需要填写。 |
| V | 左移参数的`RegTensor`类型，例如`RegTensor<int32_t>`，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcReg0 | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。<br>源操作数的数据类型需要与目的操作数保持一致。 |
| srcReg1 | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。<br>不支持设置为负数，负数行为未定义。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型

**表3**  数据类型支持情况

| dstReg   | srcReg0  | srcReg1 |
| :------- | :------- | :------ |
| int8_t   | int8_t   | int8_t  |
| uint8_t  | uint8_t  | int8_t  |
| int16_t  | int16_t  | int16_t |
| uint16_t | uint16_t | int16_t |
| int32_t  | int32_t  | int32_t |
| uint32_t | uint32_t | int32_t |
| int64_t  | int64_t  | int64_t |
| uint64_t | uint64_t | int64_t |

## 约束说明<a name="section633mcpsimp"></a>

- 对于逻辑位移（无符号数据类型），如果位移量（srcReg1）大于数据类型位宽，则输出为0。
- 对于算术位移（有符号数据类型），如果位移量大于数据类型位宽，则输出0。
- srcReg1不支持设置为负数，负数行为未定义。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
// 示例：对src0Addr中的数据，按src1Addr对应元素的值进行左移，结果写入dstAddr
// 输入：src0Addr指向待移位的数据（如uint16_t类型[1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 9, 3, 4, 6, 5, 7, ...]）
//       src1Addr指向每个元素左移的位数（如int16_t类型[1, 2, 3, 4, 1, 2, 3, 4, 2, 3, 1, 2, 3, 1, 2, 1, ...]）
// 输出：dstAddr存储左移结果（如[2, 8, 24, 64, 10, 24, 56, 128, 4, 16, 18, 12, 32, 12, 20, 14, ...]，即1<<1=2, 2<<2=8, 3<<3=24, 4<<4=64...）
template<typename T, typename U>
__simd_vf__ inline void ShiftLeftVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ U* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;  // 源操作数，待移位的数据
    AscendC::Reg::RegTensor<U> srcReg1;  // 源操作数，每个元素左移的位数
    AscendC::Reg::RegTensor<T> dstReg;   // 目的操作数，存储左移结果
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);  // 根据实际元素个数更新mask
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);  // 加载待移位的数据
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);  // 加载每个元素左移的位数
        AscendC::Reg::ShiftLeft(dstReg, srcReg0, srcReg1, mask);  // 执行左移：dstReg[i] = srcReg0[i] << srcReg1[i]
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);  // 将左移结果存回dstAddr
    }
}
```
