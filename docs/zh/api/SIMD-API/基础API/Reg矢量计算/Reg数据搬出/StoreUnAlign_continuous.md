# 连续非对齐搬出（StoreUnAlign）<a name="ZH-CN_TOPIC_0000001929071470"></a>

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

为提升对不规则内存地址的处理能力，Reg矢量计算支持在数据搬运过程中对非32字节对齐的地址进行访问，降低非对齐访问带来的性能开销。StoreUnAlign能够实现数据从[RegTensor](../寄存器数据类型/RegTensor.md)连续搬运至非对齐Unified Buffer（UB），利用非对齐寄存器UnalignRegForStore作为临时缓存区，暂存跨对齐边界的数据，从而实现高效的连续非对齐数据传输。

非对齐搬运接口有：PostUpdate扩展搬运接口、使用[AddrReg](../寄存器数据类型/AddrReg.md)寄存器存储偏移量接口、使用[AR寄存器](../比较与选择/Squeeze.md)存储偏移量接口。

RegTensor的模板参数regTrait支持RegTraitNumOne及RegTraitNumTwo，具体支持情况如下：

| RegTensor模板参数regTrait | PostUpdate扩展搬运接口 | 使用AddrReg寄存器存储偏移量接口 | 使用AR寄存器存储偏移量接口 |
|-----|-----|-----|-----|
| RegTraitNumOne | &bull; POST_MODE_NORMAL模式：不支持。<br>&bull; POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。 | 在每次迭代中，需要先调用CreateAddrReg手动设定地址偏移量，再调用搬运指令。 | &bull; POST_MODE_NORMAL模式：完成一次搬运后，UB地址不会自动更新，每次迭代需要手动更新地址。<br>&bull; POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。 |
| RegTraitNumTwo | &bull; POST_MODE_NORMAL模式：不支持。<br>&bull; POST_MODE_UPDATE模式：完成一次搬运后，UB地址会自动更新，每次迭代不需要手动更新地址。 | 不支持 | 不支持 |

非对齐搬出原理请参考[关键特性说明](#关键特性说明)。

## 函数原型<a name="section620mcpsimp"></a>

非对齐寄存器UnalignRegForStore作为临时缓存区，用于暂存跨对齐边界的数据，从而实现高效的连续非对齐数据传输。在写非对齐地址前，应该先调用StoreUnAlign，再调用StoreUnAlignPost进行后处理，处理非32字节对齐的尾块数据。

- PostUpdate扩展搬运接口

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg, uint32_t postUpdateStride)

    template <typename T, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE>
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, int32_t postUpdateStride)
    ```

- 使用AddrReg寄存器存储偏移量接口

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T*& dstAddr, U& srcReg, UnalignRegForStore& ureg, AddrReg& areg)

    template <typename T>
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T*& dstAddr, UnalignRegForStore& ureg, AddrReg& areg)
    ```

- 使用AR寄存器存储偏移量接口：配合[Squeeze](../比较与选择/Squeeze.md)使用，Squeeze会存储有效元素的总字节数到AR特殊寄存器，使用此接口会将AR寄存器中有效元素个数作为存储偏移量。

    ```cpp
    template <typename T = DefaultType, PostLiteral postMode = PostLiteral::POST_MODE_UPDATE, typename U>
    __simd_callee__ inline void StoreUnAlign(__ubuf__ T* dstAddr, U& srcReg, UnalignRegForStore& ureg)

    template <typename T>
    __simd_callee__ inline void StoreUnAlignPost(__ubuf__ T* dstAddr, UnalignRegForStore& ureg)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
|-----|-----|
| T | 目的操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| postMode | 用于控制是否使能UB地址会自动更新功能（post update），PostLiteral类型。具体参数说明请参考[PostLiteral](../辅助数据类型/PostLiteral.md)。 |
| U | 源操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstAddr | 输入/输出 | 目的操作数，UB起始地址，不需要32字节对齐。 |
| srcReg | 输入 | 源操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| ureg | 输入/输出 | 非对齐寄存器，UnalignRegForStore类型，用于存储非32字节对齐的数据，寄存器大小为32字节。 |
| postUpdateStride | 输入 | 地址更新步长，uint32_t类型，单位：元素个数。根据postMode的取值有两种情况：<br>&bull; POST_MODE_NORMAL：不支持。<br>&bull; POST_MODE_UPDATE：实际搬运UB起始地址为dstAddr，搬运后执行地址更新dstAddr += postUpdateStride。 |
| areg | 输入 | 地址偏移寄存器，AddrReg类型，需要在搬运前调用CreateAddrReg设定地址偏移量，实际搬运UB地址为dstAddr + 寄存器中存储的偏移量。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。
- 当RegTensor模板参数regTrait为RegTraitNumOne时，支持的数据类型为：b8、b16、b32、b64。
- 当RegTensor模板参数regTrait为RegTraitNumTwo时，支持的数据类型为：complex32、b64。

## 返回值说明

无

## 约束说明<a name="section1575141714439"></a>

- StoreUnAlign与StoreUnAlignPost接口需要组合使用。
- 对于使用AR寄存器存储偏移量接口：
    - 当Squeeze接口的模板参数store配置为STORE_REG时，必须保证Squeeze接口和StoreUnAlign交替使用。
    - 在调用计算前需要调用ClearSpr接口，将AR寄存器清零。

- 当RegTensor模板参数RegTrait为RegTraitNumOne和RegTraitNumTwo时，支持情况如下：

    | RegTensor模板参数RegTrait取值 | 支持的接口 | 支持的数据类型 |
    |-----|-----|-----|
    | RegTraitNumOne | 所有接口 | b8、b16、b32、b64 |
    | RegTraitNumTwo | PostUpdate扩展搬运接口 | complex32、b64 |

## 关键特性说明

**1、非对齐数据搬出原理**

将源寄存器srcReg中的非对齐数据写入UB地址dstAddr，根据ureg当前状态，分为两种场景：

**场景一：ureg为空**（第一次迭代）

如[图1](#fig-storeunalign-1)所示，从源寄存器srcReg（256B）读取数据，并将其搬运至目标UB地址（dstAddr ~ 304）。处理流程如下：

① 调用**StoreUnAlign**，此时ureg内无有效数据，表示连续非对齐搬出的起始状态，将srcReg中对应UB地址48 ~ 288的数据写入dstAddr。此外，srcReg中对应UB地址288 ~ 304的数据会被写入ureg。

② 调用**StoreUnAlignPost**进行非对齐搬出后处理。将ureg中缓存的数据写入UB地址288 ~ 304。

**图 1**  非对齐数据搬出（ureg为空）<a id="fig-storeunalign-1"></a>

![](../../../../figures/reg_storeunalign_1.png)

**场景二：ureg不为空**（除第一次迭代）

如[图2](#fig-storeunalign-2)所示，从源寄存器srcReg（256B）读取数据，并将其搬运至目标UB地址（dstAddr ~ 304）。处理流程如下：

① 调用**StoreUnAlign**，此时ureg内有有效数据，系统将ureg中UB地址32 ~ dstAddr对应的数据与srcReg中UB地址dstAddr ~ 288对应的数据进行拼接，结果写入UB地址dstAddr。此外，srcReg中对应UB地址288 ~ 304的数据会被写入ureg。

② 调用**StoreUnAlignPost**进行非对齐搬出后处理。将ureg中缓存的数据写入UB地址288 ~ 304。


**图 2**  非对齐数据搬出（ureg不为空）<a id="fig-storeunalign-2"></a>

![](../../../../figures/reg_storeunalign_2.png)

**2、连续非对齐搬入搬出示例**

**图 3**  连续非对齐搬入搬出示例（数据类型uint32_t）<a id="fig-storeunalign-3"></a>

![](../../../../figures/reg_loadstoreunalign.png)

**连续非对齐搬入时，LoadUnAlign会将后续未对齐的数据缓存至ureg，所以下一次搬入不需要再次调用LoadUnAlignPre，只需在迭代开始前调用一次LoadUnAlignPre，从而实现非对齐搬入的性能优化。**

**连续非对齐搬出时，下次迭代的StoreUnAlign会将本次迭代StoreUnAlign缓存至ureg中的数据写入UB，所以本次迭代不需要调用StoreUnAlignPost将ureg数据写入UB，只需在迭代结束后调用一次StoreUnAlignPost，从而实现非对齐搬出的性能优化。**

如[图3 连续非对齐搬入搬出示例](#fig-storeunalign-3)，将UB地址48 \~ 560的uint32\_t数据\[1, 2, 3, ... , 128\]搬入至dstReg，再搬回UB，需要两次搬入搬出操作，即for循环执行两次，初始化和后处理移至for循环外。

postUpdateStride = 256B / sizeof(T)，即每次地址偏移256B，repeatTimes = dataSize / 256B（即迭代次数=总数据量/VL）。

PostUpdate扩展搬运接口POST\_MODE\_UPDATE模式示例片段如下：

```cpp
template <typename T>
__simd_vf__ inline void LoadUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::UnalignRegForLoad ureg1;
    AscendC::Reg::UnalignRegForStore ureg2;
    // 非对齐搬入初始化，只需在迭代开始前调用一次。
    AscendC::Reg::LoadUnAlignPre(ureg1, srcAddr);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadUnAlign(dstReg, ureg1, srcAddr, postUpdateStride);
        AscendC::Reg::StoreUnAlign(dstAddr, dstReg, ureg2, postUpdateStride);
    }
    // 非对齐搬出后处理，只需在迭代结束后调用一次。
    AscendC::Reg::StoreUnAlignPost(dstAddr, ureg2, 0);
}
```

具体的搬运步骤如下：

1.  非对齐搬入初始化：更新ureg1 = [1, 2, 3, 4]。
2.  非对齐搬入：tmpReg = [5, 6, 7, ... , 68]，tmpReg部分数据和ureg1数据写入dstReg = [1, 2, 3, ... , 64]，更新ureg1 = [61, 62, 63, ... , 68]。
3.  非对齐搬出：dstReg部分数据[1, 2, 3, ... , 60]写入UB地址48 ~ 288，更新ureg2 = [61, 62, 63, 64]。
4.  非对齐搬入：tmpReg = [69, 70, 71, ... ,128]，tmpReg数据和ureg1部分数据写入dstReg = [65, 66, 67, ... , 128]。
5.  非对齐搬出：ureg2数据[61, 62, 63, 64]和dstReg部分数据[65, 66, 67, ... ,124]写入UB地址288 ~ 544，更新ureg2 = [125, 126, 127, 128]。
6.  非对齐搬出后处理：将ureg2中缓存的数据[125, 126, 127, 128]写入UB地址544 ~ 560。

## 调用示例<a name="section642mcpsimp"></a>

-  PostUpdate扩展搬运接口
    ```cpp
    template <typename T>
    __simd_vf__ inline void StoreUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t postUpdateStride, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::UnalignRegForLoad ureg0;
        AscendC::Reg::UnalignRegForStore ureg1;
        AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr);
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr, postUpdateStride);
            AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, postUpdateStride);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, 0);
    }
    ```

- 使用AddrReg寄存器存储偏移量接口
    ```cpp
    template <typename T>
    __simd_vf__ inline void StoreUnAlignVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;    
        AscendC::Reg::UnalignRegForLoad ureg0;
        AscendC::Reg::UnalignRegForStore ureg1;
        AscendC::Reg::AddrReg aReg;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
            AscendC::Reg::LoadUnAlignPre(ureg0, srcAddr, aReg);
            AscendC::Reg::LoadUnAlign(srcReg, ureg0, srcAddr, aReg, 0);
            AscendC::Reg::StoreUnAlign(dstAddr, srcReg, ureg1, aReg);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg1, aReg);
    }
    ```

- 使用AR寄存器存储偏移量接口
    ```cpp
    template <typename T>
    __aicore__ inline void SqueezeVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg0;
        AscendC::Reg::RegTensor<T> srcReg1;
        AscendC::Reg::UnalignRegForStore ureg;
        AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::H>();
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg0, srcAddr, oneRepeatSize);
            AscendC::Reg::Squeeze<T, AscendC::Reg::GatherMaskMode::STORE_REG>(srcReg1, srcReg0, mask);
            AscendC::Reg::StoreUnAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dstAddr, srcReg1, ureg);
        }
        AscendC::Reg::StoreUnAlignPost(dstAddr, ureg);
    }
    ```

