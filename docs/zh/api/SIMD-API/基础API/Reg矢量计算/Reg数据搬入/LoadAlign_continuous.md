# 连续对齐搬入（LoadAlign）<a name="ZH-CN_TOPIC_0000001955990973"></a>

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

LoadAlign能够实现数据从Unified Buffer（UB）连续搬运至[RegTensor](../寄存器数据类型/RegTensor.md)。支持单搬入模式和双搬入模式：

- 单搬入模式：从UB读取VL数据量，搬入到一个RegTensor（VL）中。
- 双搬入模式：从UB读取2*VL数据量，交错搬运，将偶数索引和奇数索引的元素分别搬入两个RegTensor（2\*VL）中。

连续对齐搬运有三类接口：普通搬运接口、PostUpdate扩展搬运接口、使用[AddrReg](../寄存器数据类型/AddrReg.md)寄存器存储偏移量接口。

RegTensor的模板参数regTrait支持RegTraitNumOne及RegTraitNumTwo，具体支持情况如下：

| RegTensor模板参数regTrait | 普通搬运接口 | PostUpdate扩展搬运接口 | 使用AddrReg寄存器存储偏移量接口 |
|-----|-----|-----|-----|
| RegTraitNumOne（单搬入/双搬入） | 完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。 | &bull; POST_MODE_NORMAL模式：不支持。<br>&bull; POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。 | 在每次迭代中，需要先调用CreateAddrReg手动设定地址偏移量，再调用搬运指令。 |
| RegTraitNumTwo（单搬入） | 完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。 | &bull; POST_MODE_NORMAL模式：不支持。<br>&bull; POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。 | 不支持 |
| RegTraitNumTwo（双搬入） | 不支持 | 不支持 | 不支持 |

数据搬入时，可以通过模板参数配置搬运的数据分布模式，能够实现broadcast、上采样、下采样、解压缩等功能，分布模式具体介绍请参考[表3 LoadDist参数说明（单搬入模式）](#表3-LoadDist参数说明（单搬入模式）)、[表6 LoadDist参数说明（双搬入模式）](#表6-LoadDist参数说明（双搬入模式）)。

[图1 连续对齐搬入分布模式图示](#fig-loadalign-dist)展示了DIST_NORM、DIST_BRC_B16、DIST_UNPACK_B16分布模式。

**图 1**  数据搬入分布模式图示<a id="fig-loadalign-dist"></a>  

![](../../../../figures/reg_loadalign_1.png)

连续对齐搬入/搬出的三类搬运接口在迭代内的调用方式请参考：[关键特性说明](#关键特性说明)。

## 函数原型<a name="section620mcpsimp"></a>

- 单搬入模式：普通搬运接口

    ```cpp
    template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr)
    ```

- 单搬入模式：PostUpdate扩展搬运接口

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode, LoadDist dist = LoadDist::DIST_NORM, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
    ```

- 单搬入模式：使用[AddrReg](../寄存器数据类型/AddrReg.md)存储偏移量接口，需要配合[CreateAddrReg](../寄存器数据类型/AddrReg.md)接口使用。

    ```cpp
    template <typename T = DefaultType, LoadDist dist = LoadDist::DIST_NORM, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg, __ubuf__ T* srcAddr, AddrReg offset)
    ```

- 双搬入模式：普通搬运接口

    ```cpp
    template <typename T = DefaultType, LoadDist dist, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr)
    ```

- 双搬入模式：PostUpdate扩展搬运接口

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode, LoadDist dist, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T*& srcAddr, int32_t postUpdateStride)
    ```

- 双搬入模式：使用[AddrReg](../寄存器数据类型/AddrReg.md)存储偏移量接口，需要配合[CreateAddrReg](../寄存器数据类型/AddrReg.md)接口使用。

    ```cpp
    template <typename T = DefaultType, LoadDist dist, typename U>
    __simd_callee__ inline void LoadAlign(U& dstReg0, U& dstReg1, __ubuf__ T* srcAddr, AddrReg offset)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明（单搬入模式）

| 参数名 | 描述 |
|-----|-----|
| T | 源操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| postMode | 用于控制是否使能UB地址会自动更新功能（post update），PostLiteral类型。具体参数说明请参考[PostLiteral](../辅助数据类型/PostLiteral.md)。 |
| dist | 数据分布模式，LoadDist类型。具体参数说明请参考[表3 LoadDist参数说明（单搬入模式）](#表3-LoadDist参数说明（单搬入模式）)。 |
| U | 目的操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |

**表 2**  参数说明（单搬入模式）

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstReg | 输出 | 目的操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcAddr | 输入/输出 | 源操作数，UB起始地址。 |
| postUpdateStride | 输入 | 地址更新步长，int32_t类型，单位：元素个数。postUpdateStride * sizeof(T)的对齐约束请参考[表3 LoadDist参数说明（单搬入模式）](#表3-LoadDist参数说明（单搬入模式）)。根据postMode的取值有两种情况：<br>&bull; POST_MODE_NORMAL：不支持。<br>&bull; POST_MODE_UPDATE：实际搬运UB起始地址为srcAddr，搬运后执行地址更新 srcAddr += postUpdateStride。 |
| offset | 输入 | 地址偏移寄存器，AddrReg类型。需要在搬运前调用CreateAddrReg设定地址偏移量，实际搬运UB地址为srcAddr + 寄存器中存储的偏移量。AddrReg寄存器中存储的偏移量 * sizeof(T)的对齐约束请参考[表3 LoadDist参数说明（单搬入模式）](#表3-LoadDist参数说明（单搬入模式）)。 |

<a id="表3-LoadDist参数说明（单搬入模式）"></a>
**表 3**  LoadDist参数说明（单搬入模式）

| LoadDist取值 | 含义 | 搬运对齐约束（Byte） |
|-----|-----|-----|
| DIST_NORM | 正常模式，搬运VL数据。可参考[图1 连续对齐搬入分布模式图示](#fig-loadalign-dist)。 | 32 |
| DIST_BRC_B8 | 搬运一个b8类型的数据，并Broadcast到所有元素位置。 | 1 |
| DIST_BRC_B16 | 搬运一个b16类型的数据，并Broadcast到所有元素位置。可参考[图1 连续对齐搬入分布模式图示](#fig-loadalign-dist)。 | 2 |
| DIST_BRC_B32 | 搬运一个b32类型的数据，并Broadcast到所有元素位置。 | 4 |
| DIST_US_B8 | 数据2倍上采样，加载数据量为VL/2，每个输入元素重复两次，数据类型为b8。 | min(32, VL/2) |
| DIST_US_B16 | 数据2倍上采样，加载数据量为VL/2，每个输入元素重复两次，数据类型为b16。 | min(32, VL/2) |
| DIST_DS_B8 | 数据2倍下采样，加载数据量为2*VL，数据每隔一个保留，数据类型为b8。 | 32 |
| DIST_DS_B16 | 数据2倍下采样，加载数据量为2*VL，数据每隔一个保留，数据类型为b16。 | 32 |
| DIST_UNPACK_B8 | 解压缩模式，按无符号整型u8加载VL/2数据量，每个元素后会补1个值为0元素，即unpack到VL。<br>例如VL=256B：<br>src: [0x00, 0x01, 0x02, 0x03, ..., 0xFF, ...]。<br>dst: [0x00, 0x00, 0x01, 0x00, 0x02, 0x00, ..., 0x7F, 0x00]。 | min(32, VL/2) |
| DIST_UNPACK_B16 | 解压缩模式，按无符号整型u16加载VL/2数据量，每个元素后会补1个值为0元素，即unpack到VL。可参考[图1 连续对齐搬入分布模式图示](#fig-loadalign-dist)。<br>例如VL=256B：<br>src: [0x0000, 0x0001, 0x0002, 0x0003, ..., 0x007F, ...]。<br>dst: [0x0000, 0x0000, 0x0001, 0x0000, 0x0002, 0x0000, ..., 0x003F, 0x0000]。 | min(32, VL/2) |
| DIST_BLK | 读取一个DataBlock（32B），并广播到VL。 | 32 |
| DIST_E2B_B16 | 加载(VL/DataBlock)个B16的数据，并将每个元素（16bit）广播到一个DataBlock（32B）中。 | VL/16 |
| DIST_E2B_B32 | 加载(VL/DataBlock)个B32的数据，并将每个元素（32bit）广播到一个DataBlock（32B）中。 | VL/8 |
| DIST_UNPACK_B32 | 解压缩模式，按无符号整型u32加载VL/2数据量，每个元素后会补1个值为0元素，即unpack到VL。<br>例如VL=256B：<br>src: [0x00000000, 0x00000001, 0x00000002, ..., 0x000003F, ...]。<br>dst: [0x00000000, 0x00000000, 0x00000001, 0x00000000, ..., 0x0000001F, 0x00000000]。 | min(32, VL/2) |
| DIST_UNPACK4_B8 | 解压缩模式，按无符号整型u8加载VL/4数据量，unpack到VL，每个元素后会补3个值为0的元素。<br>例如VL=256B：<br>src: [0x00, 0x01, 0x02, 0x03, ..., 0xFF, ...]。<br>dst: [0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,..., 0x3F, 0x00, 0x00, 0x00]。 | min(32, VL/4) |

**表 4**  模板参数说明（双搬入模式）

| 参数名 | 描述 |
|-----|-----|
| T | 源操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| postMode | 用于控制是否使能UB地址会自动更新功能（post update），PostLiteral类型。具体参数说明请参考[PostLiteral](../辅助数据类型/PostLiteral.md)。 |
| dist | 搬运模式，LoadDist类型。具体参数说明请参考[表6 LoadDist参数说明（双搬入模式）](#表6-LoadDist参数说明（双搬入模式）)。 |
| U | 目的操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |

**表 5**  参数说明（双搬入模式）

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstReg0 | 输出 | 第一个目的操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| dstReg1 | 输出 | 第二个目的操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| srcAddr | 输入/输出 | 源操作数，UB起始地址。 |
| postUpdateStride | 输入 | 地址更新步长，int32_t类型，单位：元素个数。postUpdateStride * sizeof(T)需要32字节对齐。<br>根据postMode的取值有两种情况：<br>&bull; POST_MODE_NORMAL：不支持。<br>&bull; POST_MODE_UPDATE：实际搬运UB起始地址为srcAddr，搬运后执行地址更新srcAddr += postUpdateStride。 |
| offset | 输入 | 地址偏移寄存器，AddrReg类型。需要在搬运前调用CreateAddrReg设定地址偏移量，实际搬运UB地址为srcAddr + 寄存器中存储的偏移量。 AddrReg寄存器中存储的偏移量 * sizeof(T)需要32字节对齐。 |

<a id="表6-LoadDist参数说明（双搬入模式）"></a>
**表 6**  LoadDist参数说明（双搬入模式）

| LoadDist取值 | 含义 | 搬运对齐约束（Byte） |
|-----|-----|-----|
| DIST_DINTLV_B8 | 双搬入模式，基于元素的交错搬运，从src中读取2*VL数据量，将偶数索引的元素存入dst0，将奇数索引的元素存入dst1，数据类型为b8。 | 32 |
| DIST_DINTLV_B16 | 双搬入模式，基于元素的交错搬运，从src中读取2*VL数据量，将偶数索引的元素存入dst0，将奇数索引的元素存入dst1，数据类型为b16。 | 32 |
| DIST_DINTLV_B32 | 双搬入模式，基于元素的交错搬运，从src中读取2*VL数据量，将偶数索引的元素存入dst0，将奇数索引的元素存入dst1，数据类型为b32。 | 32 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。
- 当RegTensor模板参数regTrait为RegTraitNumOne时，支持的数据类型为：b8、b16、b32、b64。
- 当RegTensor模板参数regTrait为RegTraitNumTwo时，支持的数据类型为：complex32、b64。

## 返回值说明

无

## 约束说明<a name="section11585101304320"></a>

- 位于Unified Buffer的地址约束、postUpdateStride * sizeof(T)对齐约束、AddrReg寄存器中存储的偏移量 * sizeof(T)对齐约束与分布模式LoadDist有关，具体地址约束请参考[表3 LoadDist参数说明（单搬入模式）](#表3-LoadDist参数说明（单搬入模式）)、[表6 LoadDist参数说明（双搬入模式）](#表6-LoadDist参数说明（双搬入模式）)。
- 单搬入模式：b64数据类型只支持LoadDist中的DIST_NORM模式。
- 当RegTensor模板参数RegTrait为RegTraitNumOne和RegTraitNumTwo时，支持情况如下：

    | RegTensor模板参数RegTrait取值 | 支持的接口 | 支持的数据类型 |
    |-----|-----|-----|
    | RegTraitNumOne | 所有单搬入接口和双搬入接口 | b8/b16/b32/b64 |
    | RegTraitNumTwo | &bull; 单搬入 普通搬运接口<br>&bull; 单搬入 PostUpdate扩展搬运接口 | complex32/b64 |

## 关键特性说明<a name="section18316112542110"></a>

**连续对齐搬入搬出三类搬运接口调用方式**

[图2 regTrait为RegTraitNumOne时的三类搬运接口调用方式](#fig-loadalign-regtrait)展示了连续对齐搬入/搬出的三类搬运模式在迭代内使用调用方式：

- 普通搬运接口：完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。
- PostUpdate扩展搬运接口：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。
- 使用AddrReg存储偏移量接口：在每次迭代中，需要先调用CreateAddrReg手动设定地址偏移量，再调用搬运指令。

<a id="fig-loadalign-regtrait"></a>
**图 2**  regTrait为RegTraitNumOne时的三类搬运接口调用方式  
![](../../../../figures/reg_loadstorealign.png)

## 调用示例<a name="section642mcpsimp"></a>

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
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
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

