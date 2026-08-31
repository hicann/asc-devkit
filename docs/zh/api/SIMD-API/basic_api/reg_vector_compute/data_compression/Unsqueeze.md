# Unsqueeze<a name="ZH-CN_TOPIC_0000002009161778"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/data_compression/Unsqueeze_res.md#id1 -->


## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_gather_mask_intf.h"`。

将dstReg中数据根据mask进行解压缩。解压缩方式：dstReg中第0个元素置为0，dstReg中的第i个元素等于mask中从第0个到第(i-1)个元素中1的数量。mask最高位被忽略，不参与统计。

具体算法如图1所示，dstReg的首位为0。对于后续元素，与dstReg[i-1]对应的有效mask位为1时，dstReg[i]的值为dstReg[i-1] + 1；对应的有效mask位为0时，dstReg[i]的值为dstReg[i-1]。

**图 1**  Unsqueeze示意图

![Unsqueeze](../../../../figures/reg_unsqueeze.png)

## 定义原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U>
__simd_callee__ inline void Unsqueeze(U& dstReg, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| T | 目的操作数的数据类型。 |
| U | 目的操作数的RegTensor类型，由编译器自动推导，用户不需要填写。 |

**表2**  函数参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| dstReg | 输入/输出 | 源操作数和目的操作数。<br>类型为[RegTensor](../register_data_types/RegTensor.md)。 |
| mask | 输入 | mask用于提供dstReg解压缩信息。<br>类型为[MaskReg](../register_data_types/MaskReg.md)。 |

## 返回值说明

无

## 数据类型

支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t。

## 约束说明<a name="section177921451558"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void UnsqueezeVF(__ubuf__ T* dstAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Unsqueeze(dstReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
