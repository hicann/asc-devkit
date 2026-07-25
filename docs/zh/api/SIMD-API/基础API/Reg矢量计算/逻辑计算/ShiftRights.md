# ShiftRights<a name="ZH-CN_TOPIC_0000001956827109"></a>

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

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_vec_binary_scalar_intf.h"`。

`ShiftRights`指令根据mask对源操作数srcReg，按照scalarValue进行右移操作，将结果写入目的操作数dstReg。根据源操作数的数据类型，右移操作分为以下两种情况：

- **数据类型为无符号类型：执行逻辑右移。**

  逻辑右移会将二进制数整体向右移动指定的位数，最低位被丢弃，最高位用0填充。例如，二进制数1010101010101010（uint16\_t类型）逻辑右移1位后，结果为0101010101010101。
- **数据类型为有符号类型：执行算术右移。**

  算术右移会将二进制数整体向右移动指定的位数，最低位被丢弃，最高位复制符号位。例如，二进制数1010101010101010（int16\_t类型）算术右移1位后，结果为1101010101010101；算术右移3位后，结果为1111010101010101。

$$
dstReg_i = srcReg_i \gg scalarValue
$$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void ShiftRights(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 矢量目的操作数和源操作数的数据类型。 |
| U | 标量源操作数的数据类型。 |
| mode | [MaskMergeMode](../辅助数据类型/MaskMergeMode.md)枚举类型。选择MERGING模式或ZEROING模式。<br>• ZEROING模式下，mask未筛选的元素在dstReg中置零。<br>• MERGING模式当前不支持。 |
| S | 矢量目的操作数和源操作数的`RegTensor`类型，例如`RegTensor<uint16_t>`，由编译器自动推导，用户不需要填写。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dstReg | 输出 | 目的操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcReg | 输入 | 源操作数。<br>类型为[RegTensor](../寄存器数据类型/RegTensor.md)。<br>数据类型需要与目的操作数保持一致。 |
| scalarValue | 输入 | 源操作数。<br>类型为一个标量。<br>不支持设置为负数，负数行为未定义。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型

**表3**  数据类型支持情况

| dstReg   | srcReg   | scalarValue |
| :------- | :------- | :---------- |
| int8_t   | int8_t   | int16_t     |
| uint8_t  | uint8_t  | int16_t     |
| int16_t  | int16_t  | int16_t     |
| uint16_t | uint16_t | int16_t     |
| int32_t  | int32_t  | int16_t     |
| uint32_t | uint32_t | int16_t     |
| int64_t  | int64_t  | int16_t     |
| uint64_t | uint64_t | int16_t     |

## 约束说明<a name="section633mcpsimp"></a>

- 对于逻辑位移（无符号数据类型），如果位移量（scalarValue）大于数据类型位宽，则输出为0。
- 对于算术位移（有符号数据类型），如果srcReg小于0，位移量大于数据类型位宽，则输出-1；如果srcReg大于0，位移量大于数据类型位宽，则输出0。
- scalarValue不支持设置为负数，负数行为未定义。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T, typename U>
__simd_vf__ inline void ShiftRightsVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, U scalarValue, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::ShiftRights(dstReg, srcReg, scalarValue, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
