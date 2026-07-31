# 连续对齐搬出（StoreAlign）<a name="ZH-CN_TOPIC_0000001929071462"></a>

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

StoreAlign能够实现数据从[RegTensor](../寄存器数据类型/RegTensor.md)连续搬运至Unified Buffer（UB），支持单搬出模式和双搬出模式：

- 单搬出模式：将一个RegTensor（VL）中的VL数据量搬出至UB。
- 双搬出模式：将两个RegTensor(2*VL)的数据交错搬出至UB，偶数索引和奇数索引的元素分别来自两个RegTensor。

连续对齐搬运有三类接口：普通搬运接口、PostUpdate扩展搬运接口、使用AddrReg寄存器存储偏移量接口。

RegTensor的模板参数regTrait支持RegTraitNumOne及RegTraitNumTwo，具体支持情况如下：

| RegTensor模板参数regTrait | 普通搬运接口 | PostUpdate扩展搬运接口 | 使用AddrReg寄存器存储偏移量接口 |
|-----|-----|-----|-----|
| RegTraitNumOne（单搬出） | 完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。 | &bull; POST_MODE_NORMAL模式：不支持。<br>&bull; POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。 | 在每次迭代中，需要先调用CreateAddrReg手动设定地址偏移量，再调用搬运指令。 |
| RegTraitNumOne（双搬出） | 完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。 | 不支持 | 在每次迭代中，需要先调用CreateAddrReg手动设定地址偏移量，再调用搬运指令。 |
| RegTraitNumTwo（单搬出） | 完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。 | &bull; POST_MODE_NORMAL模式：不支持。<br>&bull; POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。 | 不支持 |
| RegTraitNumTwo（双搬出） | 不支持 | 不支持 | 不支持 |

数据搬出时，可以通过模板参数配置搬运的数据分布模式，能够实现压缩、只搬出第一个元素等功能，分布模式具体介绍请参考[表3 StoreDist参数说明（单搬出模式）](#表3-StoreDist参数说明（单搬出模式）)、[表6 StoreDist参数说明（双搬出模式）](#表6-StoreDist参数说明（双搬出模式）)。[图1](#fig-storealign-dist)展示了DIST_NORM、DIST_FIRST_ELEMENT_B16、DIST_PACK_B32分布模式。

**图 1**  数据搬出分布模式图示<a id="fig-storealign-dist"></a>

![图1 数据搬出分布模式图示](../../../../figures/reg_storealign_1.png)

连续对齐搬入/搬出的三类搬运接口在迭代内调用方式请参考：[关键特性说明](#关键特性说明)。

## 函数原型<a name="section620mcpsimp"></a>

- 单搬出模式：普通搬运接口

    ```cpp
    template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, MaskReg& mask)
    ```

- 单搬出模式：PostUpdate扩展搬运接口

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode, StoreDist dist = StoreDist::DIST_NORM, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T*& dstAddr, U& srcReg, int32_t postUpdateStride, MaskReg& mask)
    ```

- 单搬出模式：使用AddrReg存储偏移量，需要配合CreateAddrReg接口使用

    ```cpp
    template <typename T = DefaultType, StoreDist dist = StoreDist::DIST_NORM, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg, AddrReg offset, MaskReg& mask)
    ```

- 双搬出模式：普通搬运接口

    ```cpp
    template <typename T = DefaultType, StoreDist dist, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, MaskReg& mask)
    ```

- 双搬出模式：使用AddrReg存储偏移量，需要配合CreateAddrReg接口使用   

    ```cpp
    template <typename T = DefaultType, StoreDist dist, typename U>
    __simd_callee__ inline void StoreAlign(__ubuf__ T* dstAddr, U& srcReg0, U& srcReg1, AddrReg offset, MaskReg& mask)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明（单搬出模式）

| 参数名 | 描述 |
|-----|-----|
| T | 目的操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。  |
| postMode | 用于控制是否使能UB地址会自动更新功能（post update），PostLiteral类型。具体参数说明请参考[PostLiteral](../辅助数据类型/PostLiteral.md)。 |
| dist | 搬运模式，StoreDist类型。具体参数说明请参考[表3 StoreDist参数说明（单搬出模式）](#表3-StoreDist参数说明（单搬出模式）)。 |
| U | 源操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |

**表 2**  参数说明（单搬出模式）

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstAddr | 输入/输出 | 目的操作数，UB起始地址。 |
| srcReg | 输入 | 源操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| postUpdateStride | 输入 | 地址更新步长，int32_t类型，单位：元素个数。postUpdateStride * sizeof(T)的对齐约束请参考[表3 StoreDist参数说明（单搬出模式）](#表3-StoreDist参数说明（单搬出模式）)。根据postMode的取值有两种情况：<br>&bull; POST_MODE_NORMAL：不支持。<br>&bull; POST_MODE_UPDATE：实际搬运UB起始地址为dstAddr，搬运后执行地址更新dstAddr += postUpdateStride。 |
| offset | 输入 | 地址偏移寄存器，AddrReg类型。需要在搬运前调用CreateAddrReg设定地址偏移量，实际搬运UB地址为dstAddr + 寄存器中存储的偏移量。AddrReg寄存器中存储的偏移量 * sizeof(T)的对齐约束请参考[表3 StoreDist参数说明（单搬出模式）](#表3-StoreDist参数说明（单搬出模式）)。|
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

<a id="表3-StoreDist参数说明（单搬出模式）"></a>
**表 3**  StoreDist参数说明（单搬出模式）

| StoreDist | 含义 | 对齐约束（Byte） |
|-----|-----|-----|
| DIST_NORM_B8 | 正常模式，搬运数据量为VL，数据类型为b8。 | 32 |
| DIST_NORM_B16 | 正常模式，搬运数据量为VL，数据类型为b16。 | 32 |
| DIST_NORM_B32 | 正常模式，搬运数据量为VL，数据类型为b32。 | 32 |
| DIST_FIRST_ELEMENT_B8 | 忽略mask，向dst中搬运src第一个元素，数据类型为b8。 | 1 |
| DIST_FIRST_ELEMENT_B16 | 忽略mask，向dst中搬运src第一个元素，数据类型为b16。可参考[图1 数据搬出分布模式图示](#fig-storealign-dist)。 | 2 |
| DIST_FIRST_ELEMENT_B32 | 忽略mask，向dst中搬运src第一个元素，数据类型为b32。 | 4 |
| DIST_PACK_B16 | 压缩模式，数据类型为b16，根据mask，将src中有效元素的低半部分bit数据连续存储于dst中。<br>例：数据类型为uint16_t：<br>src: [0x3210, 0x7654, 0xBA98, 0xFEDC, ..., 0xFEDC, 0xBA98, 0x7654, 0x3210]。<br>dst: [0x5410, 0xDC98, ... 0x98DC, 0x1054]。 | min(32, VL/2) |
| DIST_PACK_B32 | 压缩模式，数据类型为b32，根据mask，将src中有效元素的低半部分bit数据连续存储于dst中。可参考[图1 数据搬出分布模式图示](#fig-storealign-dist)。 | min(32, VL/2) |
| DIST_PACK_B64 | 压缩模式，数据类型为b64，根据mask，将src中有效元素的低半部分bit数据连续存储于dst中。 | min(32, VL/2) |
| DIST_PACK4_B32 | 压缩模式，数据类型为b32，根据mask，将src中有效元素的低8bit（四分之一）数据连续存储于dst中。 | min(32, VL/4) |
| DIST_NORM | 正常模式，搬运VL数据，支持数据类型b8/b16/b32，系统会根据模板参数T自动选择DIST_NORM_B8、DIST_NORM_B16、DIST_NORM_B32。可参考[图1 数据搬出分布模式图示](#fig-storealign-dist)。 | 32 |

**表 4**  模板参数说明（双搬出模式）

| 参数名 | 描述 |
|-----|-----|
| T | 目的操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| dist | 搬运模式，StoreDist类型。具体参数说明请参考[表6 StoreDist参数说明（双搬出模式）](#表6-StoreDist参数说明（双搬出模式）)。 |
| U | 源操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |

**表 5**  参数说明（双搬出模式）

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstAddr | 输入/输出 | 目的操作数，UB起始地址。 |
| srcReg0 | 输入 | 第一个源操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcReg1 | 输入 | 第二个源操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| offset | 输入 | 地址偏移量，AddrReg类型。需要在搬运前调用CreateAddrReg设定地址偏移量，实际搬运UB地址为dstAddr + offset存储的偏移量，搬运后地址不会更新。AddrReg寄存器中存储的偏移量*sizeof(T)需要32字节对齐。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

<a id="表6-StoreDist参数说明（双搬出模式）"></a>
**表 6**  StoreDist参数说明（双搬出模式）

| StoreDist | 含义 | 对齐约束(Byte) |
|-----|-----|-----|
| DIST_INTLV_B8 | 双搬出模式，数据类型为b8，忽略mask，将src0，src1中的元素交错存储于dst中，dst长度需要为2*VL。<br>例：数据类型为uint8_t：<br>src0: [0, 2, 4, 6, ... 254, 0, 2, 4, ..., 252, 254]。<br>src1: [1, 3, 5, 7, ..., 255, 1, 3, 5, ..., 253, 255]。<br>dst: [0, 1, 2, 3, ..., 254, 255, 0, 1, 2, 3, ..., 253, 254, 255]。 | 32 |
| DIST_INTLV_B16 | 双搬出模式，数据类型为b16，忽略mask，将src0，src1中的元素交错存储于dst中，dst长度需要为2*VL。 | 32 |
| DIST_INTLV_B32 | 双搬出模式，数据类型为b32，忽略mask，将src0，src1中的元素交错存储于dst中，dst长度需要为2*VL。 | 32 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。
- 当RegTensor模板参数regTrait为RegTraitNumOne时，支持的数据类型为：b8、b16、b32、b64。
- 当RegTensor模板参数regTrait为RegTraitNumTwo时，支持的数据类型为：complex32、b64。

## 返回值说明

无

## 约束说明<a name="section587915597213"></a>

- 位于Unified Buffer的地址约束、postUpdateStride * sizeof(T)的对齐约束、AddrReg寄存器中存储的偏移量 \* sizeof(T)的对齐约束与分布模式StoreDist有关，具体地址约束请参考[表3 StoreDist参数说明（单搬出模式）](#表3-StoreDist参数说明（单搬出模式）)、[表6 StoreDist参数说明（双搬出模式）](#表6-StoreDist参数说明（双搬出模式）)。

- 单搬出模式：b64数据类型只支持StoreDist中的DIST_NORM和DIST_PACK_B64模式。
- 当RegTensor模板参数RegTrait为RegTraitNumOne和RegTraitNumTwo时，支持情况如下：
    | RegTensor模板参数RegTrait取值 | 支持的接口 | 支持的数据类型 |
    |-----|-----|-----|
    | RegTraitNumOne | 所有单搬出接口和双搬出接口 | b8/b16/b32/b64 |
    | RegTraitNumTwo | &bull; 单搬出 普通搬运接口<br>&bull; 单搬出 PostUpdate扩展搬运接口 | complex32/b64 |

## 关键特性说明

**连续对齐搬入搬出三类搬运接口调用方式**

图2展示了连续对齐搬入/搬出的三类搬运模式在迭代内使用调用方式：

- 普通搬运接口：完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。
- PostUpdate扩展搬运接口：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。
- 使用AddrReg存储偏移量接口：在每次迭代中，需要先调用CreateAddrReg手动设定地址偏移量，再调用搬运指令。

**图 2**  regTrait为RegTraitNumOne时的三类搬运模式调用方式<a id="fig-storealign-regtrait"></a>

![](../../../../figures/reg_loadstorealign.png)

## 调用示例<a name="section5497339111410"></a>

- 单搬入/单搬出模式：普通搬运接口

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode01(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dstSize, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            mask = AscendC::Reg::UpdateMask<T>(dstSize);
            AscendC::Reg::LoadAlign(dstReg, srcAddr + i * oneRepeatSize);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```

- 单搬入/单搬出模式：PostUpdate扩展搬运接口

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode02(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t dstSize, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            mask = AscendC::Reg::UpdateMask<T>(dstSize);
            AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstReg, srcAddr, oneRepeatSize);
            AscendC::Reg::StoreAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, dstReg, oneRepeatSize, mask);
        }
    }
    ```

- 单搬入/单搬出模式：使用AddrReg存储偏移量

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode03(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
        AscendC::Reg::AddrReg aReg;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
            AscendC::Reg::LoadAlign(dstReg, srcAddr, aReg);
            AscendC::Reg::StoreAlign(dstAddr, dstReg, aReg, mask);
        }
    }
    ```

- 双搬入/双搬出模式：普通搬运接口

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode04(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<uint8_t, AscendC::Reg::MaskPattern::ALL>();
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<T, AscendC::Reg::LoadDist::DIST_DINTLV_B8>(srcReg0, srcReg1, srcAddr + i * oneRepeatSize);
            AscendC::Reg::StoreAlign<T, AscendC::Reg::StoreDist::DIST_INTLV_B8>(dstAddr + i * oneRepeatSize, srcReg0, srcReg1, mask);
        }
    }
    ```

- 双搬入/双搬出模式：使用AddrReg存储偏移量，需要配合CreateAddrReg接口使用

    ```cpp
    template <typename T>
    __simd_vf__ inline void ComputeMode05(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
        AscendC::Reg::AddrReg aReg;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
            AscendC::Reg::LoadAlign<T, AscendC::Reg::LoadDist::DIST_DINTLV_B8>(srcReg0, srcReg1, srcAddr, aReg);
            AscendC::Reg::StoreAlign<T, AscendC::Reg::StoreDist::DIST_INTLV_B8>(dstAddr, srcReg0, srcReg1, aReg, mask);
        }
    }
    ```

