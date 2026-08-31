# UnPack<a name="ZH-CN_TOPIC_0000002047032737"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_compression/UnPack_res.md#id1 -->


## 功能说明<a name="section1922292299"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_pack_intf.h"`。

对于无符号整型，将源操作数srcReg中低半部分或高半部分的元素以高位填0扩充位宽的方式写入dstReg。对于有符号整型，将源操作数srcReg中低半部分或高半部分的元素以保持符号位扩充位宽的方式写入dstReg。

**图 1**  UnPack示意图

![UnPack](../../../../figures/reg_unpack_1.png)

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, HighLowPart part = HighLowPart::LOWEST, typename S, typename V>
__simd_callee__ inline void UnPack(S& dstReg, V& srcReg)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明
| 参数名称 | 描述 |
| ------ | ------ |
| T | 目的操作数数据类型。<br> 源操作数和目的操作数的数据类型约束参考见[表 源操作数和目的操作数的数据类型对应表](#table3)。|
| U | 源操作数数据类型。 |
| part | 枚举类型，用于控制读取srcReg的低半部分还是高半部分。<br>• HighLowPart::LOWEST，低位模式，读取srcReg的低半部分。<br>• HighLowPart::HIGHEST，高位模式，读取srcReg的高半部分。<br>注：RegTraitNumTwo只支持LOWEST模式。|
| S | 目的操作数RegTensor类型。 |
| V | 源操作数RegTensor类型。 |

**表 2**  函数参数说明
| 参数名称 | 描述 |
| ------ | ------ |
| dstReg | 目的操作数。<br> 类型为[RegTensor](../register_data_types/RegTensor.md)。|
| srcReg | 源操作数。<br> 类型为[RegTensor](../register_data_types/RegTensor.md)。|

## 返回值说明<a name="section640mcpsimp"></a>

无

## 数据类型

**表 3**  源操作数和目的操作数的数据类型对应表<a id="table3"></a> 
| T数据类型 | U数据类型 |
| ------ | ------ |
| int16_t | int8_t |
| uint16_t | uint8_t |
| int32_t | int16_t |
| uint32_t | uint16_t |
| int64_t | int32_t |
| uint64_t | uint32_t |
## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section932512912207"></a>

```cpp
template<typename T, typename U, int32_t mode>
__simd_vf__ inline void UnPackVF(__ubuf__ T* dstAddr, __ubuf__ U* srcAddr, uint32_t oneDstRepSize, uint16_t repeatTimes, uint32_t oneSrcRepSize)
{
    AscendC::Reg::RegTensor<U> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneSrcRepSize);
        if constexpr (mode == 0) {
            AscendC::Reg::UnPack<T, U, AscendC::Reg::HighLowPart::LOWEST>(dstReg, srcReg);
        } else if constexpr (mode == 1) {
            AscendC::Reg::UnPack<T, U, AscendC::Reg::HighLowPart::HIGHEST>(dstReg, srcReg);
        }
        AscendC::Reg::StoreAlign(dstAddr + i * oneDstRepSize, dstReg, mask);
    }
}
```
