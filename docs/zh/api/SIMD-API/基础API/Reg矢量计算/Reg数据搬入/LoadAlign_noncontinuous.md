# 非连续对齐搬入（LoadAlign）<a name="ZH-CN_TOPIC_0000001928912110"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`。

LoadAlign能够实现数据从Unified Buffer（UB）非连续搬运至[RegTensor](../寄存器数据类型/RegTensor.md)，有两类接口：

- 普通搬运接口：完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。
- PostUpdate扩展搬运接口：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。

## 函数原型<a name="section620mcpsimp"></a>

- 普通搬运接口

    ```cpp
    template <typename T = DefaultType, DataCopyMode dataMode, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr, uint32_t dataBlockStride, MaskReg& mask)
    ```

- PostUpdate扩展搬运接口

    ```cpp
    template <typename T = DefaultType, DataCopyMode dataMode, PostLiteral postMode, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T*& srcAddr, uint32_t dataBlockStride, uint32_t repeatStride, MaskReg& mask)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
|-----|-----|
| T | 源操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| dataMode | 非连续对齐场景下的搬运模式，DataCopyMode类型。[表3 DataCopyMode参数说明](#表3-DataCopyMode参数说明)。 |
| postMode | 用于控制是否使能UB地址会自动更新功能（post update），PostLiteral类型。具体参数说明请参考[PostLiteral](../辅助数据类型/PostLiteral.md)。 |
| U | 目的操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstReg | 输出 | 目的操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcAddr | 输入/输出 | 源操作数，UB起始地址，需要32字节对齐。 |
| dataBlockStride | 输入 | 单条指令一次搬运8个DataBlock，该参数表示相邻DataBlock间的间隔（前面一个DataBlock的头与后面DataBlock的头的间隔），单位：DataBlock（32B）。<br>注：当dataBlockStride=0时，表示重复搬入第一个DataBlock。 |
| repeatStride | 输入 | 地址更新步长，单位：DataBlock（32B）。repeatStride * sizeof(T)需要32字节对齐。根据postMode的取值有两种情况：<br>&bull; POST_MODE_NORMAL：实际搬运的UB起始地址为srcAddr + repeatStride * 32B / sizeof(T)，搬运后地址不会更新。<br>&bull; POST_MODE_UPDATE：实际搬运UB起始地址为srcAddr，搬运后执行地址更新srcAddr += repeatStride * 32B / sizeof(T)。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。<br>&bull; 因该接口以DataBlock为数据搬运单元，当DataBlock中的任意一个元素被mask筛选成有效元素时，该DataBlock中所有数据都会搬入至RegTensor。<br>&bull; 当DataBlock中所有元素都被mask筛选成无效元素时，该DataBlock中的数据不会搬入到RegTensor，对应位置的元素设置为0，即使UB越界也不会报错。 |

<a id="表3-DataCopyMode参数说明"></a>
**表 3**  DataCopyMode参数说明

| 参数名 | 含义 |
|-----|-----|
| DATA_BLOCK_COPY | DataBlock搬运模式，即以DataBlock（32B）为单位进行搬运。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。支持的数据类型为：b8、b16、b32。

## 返回值说明<a name="section1575141714439"></a>

无

## 约束说明<a name="section11585101304320"></a>

- 位于Unified Buffer的地址必须32字节对齐。
- RegTensor模板参数regTrait只支持RegTraitNumOne。
- repeatStride * 32B / sizeof(T)需要32字节对齐。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template <typename T>
__simd_vf__ inline void Compute(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign<T, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg, srcAddr, 1, i * 8, mask);
        AscendC::Reg::StoreAlign<T, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, srcReg, 1, i * 8, mask);
    }
}
```

