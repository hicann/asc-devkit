# 离散搬入（GatherB）<a name="ZH-CN_TOPIC_0000001955999929"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/GatherB_discrete_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`。

该指令会根据索引值index将源操作数按DataBlock（32B）收集到目的操作数dstReg中。收集过程如图1所示：

**图 1**  GatherB功能说明

![图1 GatherB功能说明](../../../../figures/reg_gatherb.png)

其中，index中仅前8个元素有效，每个元素对应一个DataBlock。例如，第一个元素为96（3 * 32），表示选取DataBlock3写入dstReg中对应的位置。

以下Python代码用于等价描述GatherB的数据选择过程。其中，**baseAddr列表中的每个元素表示一个DataBlock**，index中的元素表示相对于baseAddr的字节偏移。

```python
def gather_b(base_addr, index):
    data_block_size = 32
    dst_reg = []
    for offset in index[:8]:
        data_block_id = offset // data_block_size
        dst_reg.append(base_addr[data_block_id])
    return dst_reg
```

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U, typename S>
__simd_callee__ inline void GatherB(U& dstReg, __ubuf__ T* baseAddr, S& index, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
|-----|-----|
| T | 目的操作数和源操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 目的操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |
| S | 索引值的RegTensor类型，例如RegTensor\<uint32_t>，由编译器自动推导，用户不需要手动填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstReg | 输出 | 目的操作数，类型为[RegTensor](../register_data_types/RegTensor.md)。|
| baseAddr | 输入 | 源操作数，UB中的基地址，需要32字节对齐。 |
| index | 输入 | 索引值，dstReg中的每个DataBlock在Unified Buffer（UB）中相对于baseAddr的位置，仅前8个元素有效。单位：字节。类型为[RegTensor](../register_data_types/RegTensor.md)。索引值必须32B对齐，即一个索引值对应1个DataBlock。index中的值可以重复。例如：<br>baseAddr: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock7, ... , DataBlock32, ...]。<br>index: [0\*32, 1\*32, 2\*32, 3\*32, 4\*32, 5\*32, 6\*32, 32\*32]<br>dstReg: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock32]。 |
| mask | 输入 | DataBlock搬运的有效指示，按b32格式解释。一个DataBlock对应4bit，仅每4bit中的最低位有效。由于index仅前8个元素有效，因此mask仅使用前8个b32元素对应的bit 0、4、8、12、16、20、24、28，分别控制dstReg中DataBlock0至DataBlock7是否更新，其余bit无效。详细说明请参考[MaskReg](../register_data_types/MaskReg.md)。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。支持的数据类型为：b8、b16、b32、b64。

索引值支持的数据类型为：uint32_t。

## 返回值说明

无

## 约束说明<a name="section177921451558"></a>

- 位于UB的地址必须32字节对齐。
- 源操作数和目的操作数数据类型必须相同。
- index索引值必须32字节对齐，即一个索引值对应1个DataBlock。
- index索引值对应的数据必须在UB有效地址范围内。
- RegTensor模板参数regTrait只支持RegTraitNumOne。
- index仅前8个元素有效。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template <typename T>
__simd_vf__ inline void GatherBVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ uint32_t* indexAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::RegTensor<uint32_t> indexReg;
    AscendC::Reg::MaskReg calMask = AscendC::Reg::CreateMask<uint32_t, AscendC::Reg::MaskPattern::VL8>();
    AscendC::Reg::MaskReg storeMask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign(indexReg, indexAddr + i * oneRepeatSize);
        AscendC::Reg::GatherB(dstReg, srcAddr, indexReg, calMask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, storeMask);
    }
}
```
