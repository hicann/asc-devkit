# MaskReg<a name="ZH-CN_TOPIC_0000001955873477"></a>

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

头文件引用路径为"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"。

MaskReg寄存器用于指示在计算过程中哪些元素参与计算，宽度为<a href="./RegTensor.md">RegTensor</a>的八分之一（VL/8）。如下图所示，当操作数类型为b8时，每一个element对应1bit MaskReg；当操作数类型为b16时，每一个element对应2bit MaskReg，且仅2bit中的最低位是有效的；当操作数类型为b32时，每一个element对应4bit MaskReg，且仅4bit中的最低位是有效的。

**图1** MaskReg计算过程<a id="fig1"></a>

![](../../../../figures/MaskReg计算过程.png "MaskReg计算过程")

## 函数原型<a name="section620mcpsimp"></a>
- CreateMask接口
  ```cpp
  template <typename T, MaskPattern mode = MaskPattern::ALL, const RegTrait& regTrait = RegTraitNumOne>
  __simd_callee__ inline MaskReg CreateMask()
  ```
- UpdateMask接口
  ```cpp
  template <typename T, const RegTrait& regTrait = RegTraitNumOne>
  __simd_callee__ inline MaskReg UpdateMask(uint32_t& scalarValue)
  ```

## 参数说明<a name="section622mcpsimp"></a>

<a id="create-mask-params"></a>
**表1**  CreateMask参数说明

| 参数名 | 输入/输出 | 描述 |
| :-- | :------------ | :------------ |
| T | 输入 | 模板参数，支持的数据类型为b8/b16/b32/b64。 |
| mode | 输入 | 创建MaskReg的模式，enum class类型。<br><pre>enum class MaskPattern {<br>    ALL,        // 所有元素设置为有效数据<br>    VL1,        // 最低1个元素设置为有效数据<br>    VL2,        // 最低2个元素设置为有效数据<br>    VL3,        // 最低3个元素设置为有效数据<br>    VL4,        // 最低4个元素设置为有效数据<br>    VL8,        // 最低8个元素设置为有效数据<br>    VL16,       // 最低16个元素设置为有效数据<br>    VL32,       // 最低32个元素设置为有效数据<br>    VL64,       // 最低64个元素设置为有效数据<br>    VL128,      // 最低128个元素设置为有效数据<br>    M3,         // 3的倍数设置为有效数据<br>    M4,         // 4的倍数设置为有效数据<br>    H,          // 最低一半元素设置为有效数据<br>    Q,          // 最低四分之一元素设置为有效数据<br>    ALLF = 15   // 所有元素设置为无效数据<br>};</pre> |
| regTrait | 输入 | 该参数默认值为RegTraitNumOne。 |

<a id="update-mask-params"></a>
**表2**  UpdateMask参数说明

| 参数名 | 输入/输出 | 描述 |
| :-- | :------------ | :------------ |
| T | 输入 | 模板参数，支持的数据类型为b8、b16、b32、b64。 |
| regTrait | 输入 | 该参数默认值为RegTraitNumOne。 |
| scalarValue | 输入/输出 | 矢量计算需要操作的元素的具体数量，生成对应的MaskReg，元素有效范围从0到VL_T（一个位宽为VL的向量寄存器中能够存放VL_T个数据类型为T的元素）。<br>执行完该函数后，scalarValue会减去VL_T。<br>`scalarValue = (scalarValue < VL_T) ? 0 : (scalarValue - VL_T)` |


## 返回值说明<a name="section1575141714439"></a>

MaskReg

## 约束说明<a name="section177921451558"></a>

- MaskReg寄存器数量上限为8。超出限制上限的寄存器数据会写入预留的8KUB内存中，可能会引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若寄存器与预留内存均存在可用空间，将优先复用寄存器。

## 关键特性

### Cast精度转换中的MaskReg

不同数据类型下元素对应的mask位宽不一致，在Cast进行类型转换时，MaskReg根据输入的源操作数进行有效元素筛选。

图[b16到b32类型转换过程](#fig4)和图[b32到b16类型转换过程](#fig5)展示了MaskReg和RegLayout同时作用时b16和b32进行类型转换的过程。

**图4** b16到b32类型转换过程<a id="fig4"></a>

![](../../../../figures/b16到b32类型转换过程.png "b16到b32类型转换过程")

**图5** b32到b16类型转换过程<a id="fig5"></a>

![](../../../../figures/b32到b16类型转换过程.png "b32到b16类型转换过程")

特别的，int4x2_t/fp4x2_e2m1_t/fp4x2_e1m2_t和b16之间的转换，指令会以每2个元素为一对进行读写，大转小时mask有效位以偶数位为准。

图[fp4x2_e2m1_t到bfloat16_t类型转换过程](#fig6)和图[bfloat16_t到fp4x2_e2m1_t类型转换过程](#fig7)展示了MaskReg和RegLayout同时作用时fp4x2_e2m1_t和bfloat16_t之间的转换。

**图6** fp4x2_e2m1_t到bfloat16_t类型转换过程<a id="fig6"></a>

![](../../../../figures/fp4x2_e2m1_t到bfloat16_t转换过程.png "fp4x2_e2m1_t到bfloat16_t类型转换过程")

**图7** bfloat16_t到fp4x2_e2m1_t类型转换过程<a id="fig7"></a>

![](../../../../figures/bfloat16_t到fp4x2_e2m1_t转换过程.png "bfloat16_t到fp4x2_e2m1_t类型转换过程")

### Mask设置方式

在Mask设置中，Reg矢量计算支持多种灵活配置方式，可根据实际计算场景选择固定Mask模式、从UB搬运或从RegTensor搬运等策略，以满足不同计算场景的需求。具体设置方式如下表所示：

**表3**  Mask设置方式

| 编号 | 设置方式 | 涉及接口 | 说明 |
| :-- | :------------ | :------------ | :------------ |
| 1 | 调用接口设置 | CreateMask | 调用CreateMask接口以固定的Pattern设置Mask，每次以循环计算均使用此Mask。详见[方式1](#method1)。 |
| 2 | 调用接口设置 | UpdateMask | 调用UpdateMask设置Mask。详见方式2。<br>&bull; 在循环外调用UpdateMask设置固定的Mask，Mask表示在一次循环计算中前count个元素参与计算，每次以循环计算均使用此Mask。<br>&bull; 在循环内调用UpdateMask设置Mask，表示计算中前count个元素参与计算，每次循环Mask会自动更新。详见[方式2](#method2)。 |
| 3 | 从UB搬运 | LoadAlign | 从UB搬运Mask到MaskReg。详见[方式3](#method3)。 |
| 4 | 从RegTensor搬运 | MaskGenWithRegTensor | 从RegTensor搬运Mask到MaskReg。详见[方式4](#method4)。 |
| 5 | 从掩码寄存器中读取 | MoveMask | 从SetVectorMask设置的掩码寄存器{MASK1,MASK0}中读取Mask值。详见[方式5](#method5)。 |

Reg矢量计算中，掩码的配置方式由MaskReg寄存器决定，仅支持接口内设置Mask。Mask值可通过MaskReg类型参数，直接作为入参传递给矢量计算API。

- CreateMask：将特定模式设置Mask到MaskReg寄存器。模式见[CreateMask参数说明](#create-mask-params)。

  **图2** b8数据类型下CreateMask接口不同MaskReg的模式下元素选取<a id="fig2"></a>

![](../../../../figures/b8数据类型下CreateMask接口不同MaskReg的模式下元素选取.png "b8数据类型下CreateMask接口不同MaskReg的模式下元素选取")

- UpdateMask：根据当前scalarValue的值生成对应长度的有效位掩码，并自动将scalarValue减去当前向量长度以更新剩余待处理元素数量。

  **图3** b16数据类型下UpdateMask接口基于scalarValue的掩码生成<a id="fig3"></a>

![](../../../../figures/b16数据类型下UpdateMask接口基于scalarValue的掩码生成.png "b16数据类型下UpdateMask接口基于scalarValue的掩码生成")

以基础算术API Add为例介绍Mask设置的不同方式。

```cpp
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Add(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
```

- 方式1<a id="method1"></a>
  采用CreateMask接口以固定的Pattern设置Mask，Pattern详见[CreateMask参数说明](#create-mask-params)。在循环外调用CreateMask设置固定的Mask，每次以循环计算均使用此Mask。

  ```cpp
  template <typename T>
  __simd_vf__ inline void SetMaskFixedPatternVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint16_t oneRepeatSize, uint16_t repeatTimes)
  {
    AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
      AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
      AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
      AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
      AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
  }
  ```

- 方式2<a id="method2"></a>

  调用UpdateMask接口设置Mask。

  - 在循环外调用UpdateMask设置固定的Mask，Mask表示在一次循环计算中前count个元素参与计算，每次以循环计算均使用此Mask。

    ```cpp
    template <typename T>
    __simd_vf__ inline void SetMaskUpdateFixedVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
      AscendC::Reg::MaskReg mask;
      mask = AscendC::Reg::UpdateMask<T, AscendC::Reg::RegTraitNumOne>(count);
      for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
    }
    ```
  >[!NOTE]说明
  >
  >当数据类型为int16_t（操作数为16位）时，每次循环内能够处理的元素个数Mask∈[1, 128]。当count>128时，仍按一次repeat（128元素）执行，不支持超过128元素的Mask控制。

  - 在循环内调用UpdateMask设置Mask，表示计算中前count个元素参与计算，每次循环Mask会自动更新。

    ```cpp
    template <typename T>
    __simd_vf__ inline void SetMaskUpdateAutoVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count)
    {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
      AscendC::Reg::MaskReg mask;
      // 一次repeat计算的数据个数
      constexpr uint16_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
      // 总循环次数
      uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
      for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T, AscendC::Reg::RegTraitNumOne>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
    }
    ```

- 方式3<a id="method3"></a>
  
  采用[LoadAlign](../Reg数据搬入/MaskReg搬入（LoadAlign）.md)从UB搬运Mask到MaskReg。在UB中设置Mask。

  ```cpp
  template <typename T>
  __simd_vf__ inline void SetMaskFromUBVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, __ubuf__ T* maskAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
      AscendC::Reg::MaskReg mask;
      AscendC::Reg::LoadAlign(mask, maskAddr);
      for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

- 方式4<a id="method4"></a>
  
  采用[MaskGenWithRegTensor](../Reg数据搬入/MaskReg搬入（MaskGenWithRegTensor）.md)从RegTensor搬运Mask到MaskReg。在RegTensor中设置Mask。

  ```cpp
  template <typename T>
  __simd_vf__ inline void SetMaskFromRegTensorVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint16_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg, maskReg;
      AscendC::Reg::MaskReg mask;
      // RegTensor中设置mask
      T scalarValue = static_cast<T>(0xFFFF);
      AscendC::Reg::Duplicate(maskReg, scalarValue);
      AscendC::Reg::MaskGenWithRegTensor<T, 0>(mask, maskReg);
      for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

- 方式5<a id="method5"></a>
  
  采用[MoveMask](../MaskReg计算/MoveMask.md)，从SetVectorMask设置的掩码寄存器{MASK1,MASK0}中读取Mask值，并按模板参数T对应的数据格式转换后写入返回值MaskReg。b16类型读取完整128bit的{MASK1,MASK0}，将每个bit复制为2bit；b32类型读取64bit的MASK0，并将每个bit复制为4bit。

  ```cpp
  template <typename T>
  __simd_vf__ inline void MoveMaskVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint16_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
      AscendC::Reg::MaskReg mask = AscendC::Reg::MoveMask<T>();
      for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

### NPU架构版本2201与NPU架构版本3510之间Mask机制对比

本节对比[NPU架构版本2201](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)在Normal模式的逐bit计算中Mask机制，与[NPU架构版本3510](../../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)基于Reg矢量计算中Mask机制的差异。

- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品和Atlas A3 训练系列产品/Atlas A3 推理系列产品

  - Mask寄存器长度为256bit。

  - 每一个element对应1bit的Mask，数据类型不同，每次循环内能够处理的元素个数最大值不同。当操作数类型为b16时，一次循环中最多有128个元素参与计算；当操作数类型为b32时，一次循环中最多有64个元素参与计算；当操作数类型为b64时，一次循环中最多有32个元素参与计算。详细见[掩码相关参数](../../Memory矢量计算/SIMD计算说明/掩码.md#sheet2)。

- 针对Ascend 950PR/Ascend 950DT

  - MaskReg寄存器长度为VL/8，即256bit。

  - 当操作数类型为b8时，每一个element对应1bit MaskReg；当操作数类型为b16时，每一个element对应2bit MaskReg，且仅2bit中的最低位是有效的；当操作数类型为b32时，每一个element对应4bit MaskReg，且仅4bit中的最低位是有效的。

**图8** NPU架构版本2201的Memory矢量计算与NPU架构版本3510的Reg矢量计算Mask机制对比<a id="fig8"></a>

![](../../../../figures/Memory矢量计算与Reg矢量计算Mask机制对比.png "NPU架构版本2201的Memory矢量计算与NPU架构版本3510的Reg矢量计算Mask机制对比")

## 调用示例<a name="section642mcpsimp"></a>

```cpp
AscendC::Reg::RegTensor<uint32_t> srcReg;
AscendC::Reg::MaskReg mask0 = AscendC::Reg::CreateMask<uint32_t, AscendC::Reg::MaskPattern::ALL>();
AscendC::Reg::MaskReg mask1;
uint32_t scalarValue = 127;
for (uint16_t i = 0; i < 2; i++) {
    mask1 = AscendC::Reg::UpdateMask<uint32_t>(scalarValue);
    AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg, srcAddr, 0);
    AscendC::Reg::Adds(srcReg, srcReg, 1, mask0);
    AscendC::Reg::StoreAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dst0Addr, srcReg, 0, mask0);
    AscendC::Reg::StoreAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dst1Addr, srcReg, 0, mask1);
}
```
