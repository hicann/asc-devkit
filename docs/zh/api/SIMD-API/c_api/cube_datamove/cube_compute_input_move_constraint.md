# 矩阵计算输入搬运约束

## 对齐约束

具体可见[存储单元说明](../general_description_and_constraints.md#存储单元说明)。

- 当源地址位于L1 Buffer时，该地址必须32Byte对齐；当源地址位于GM时，该地址必须1Byte对齐。
- 当目的地址位于L0A Buffer/L0B Buffer时，该地址必须512Byte对齐；当目的地址位于L1 Buffer时，该地址必须32Byte对齐。

## 搬运粒度约束

- 2D矩阵搬运以大小为512Byte的分形为单位进行搬运。
- 3D矩阵搬运以大小为512Byte的分形为单位进行搬运。
<!-- npu="910b,A3" id4 -->
- asc_copy_l12l0b_sparse搬运以512Byte为单位存放的压缩权重矩阵到L0B Buffer里，同时搬运以128Byte为单位的索引矩阵到内置的专用buffer空间。
<!-- end id4 -->

## 分形约束

数据搬运（GM-\>L1 Buffer）在不使能随路进行ND到Nz转换的时候，不会改变分形间和分形内排布，如Nz2Nz，在使能ND到Nz转换的情况下，一定会改变分形排布，如ND2Nz。

2D矩阵搬运在不使能转置情况下，只能改变分形间的排布，如Nz2Zz；在使能转置情况下，既能改变分形内的排布，又能改变分形间的排布，如Nz2Zn。

转置搬运一定改变分形内的排布，可以按需改变分形间的排布，如Nz2Zn。

转置搬运在B32场景下，源操作数2个连续的16\*8分形将被合并为1个16\*16的方块矩阵，然后再基于该方块矩阵做转置，因此要求两个连续分形合并为方块矩阵，要求L1 Buffer上的矩阵满足Nz排布（当且仅当Row==16）或Zn排布（当且仅当Col==16）。

3D矩阵搬运主要用于对NC1HWC0格式的feature map完成image to column操作，并将展开后的二维矩阵搬入对应内存位置，因此对于3D矩阵搬运的源操作数中数据必须按照NC1HWC0格式排布，目的操作数数据必须按照目的地址L0A Buffer/L0B Buffer中支持的排布方式排布。

<!-- npu="910b,A3" id6 -->
asc_copy_l12l0b_sparse主要用于搬运以512Byte为单位存放的压缩权重矩阵到L0B Buffer里，同时搬运以128Byte为单位的索引矩阵到内置的专用buffer空间（用于后续asc_mmad_sparse接口进行读取），因此源操作数中的压缩权重矩阵和索引矩阵都需要按照Zn格式排布，其中压缩权重矩阵排布方式为32\*16\*8bit=512Byte，索引矩阵排布方式为32\*16\*2bit=128Byte。
<!-- end id6 -->

矩阵计算过程中，常用分形支持总结如下表：

<!-- npu="910b,A3" id7 -->
### 针对Atlas A2训练系列产品/Atlas A2推理系列产品和Atlas A3训练系列产品/Atlas A3推理系列产品

常用分形支持情况如下，供开发者参考：

- **数据搬运**
    - GM->L1 Buffer
        - ND->Nz：

            ![](../../../figures/datacopy_gm2l1_nd2nz.png)
        - Nz->Nz：

            ![](../../../figures/datacopy_gm2l1_nz2nz.png)
        - Zn->Zn：

            ![](../../../figures/datacopy_gm2l1_zn2zn.png)
- **2D矩阵搬运**
    - GM->L1 Buffer（不支持转置）
        - Nz->Nz：

            ![](../../../figures/load2d_gm2l1_nz2nz.png)
        - Zn->Zn：

            ![](../../../figures/load2d_gm2l1_zn2zn.png)
    - GM->L0A Buffer（不支持转置）L1 Buffer->L0A Buffer（不使能转置）（Nz->Zz）

        ![](../../../figures/load2d_gm2l0a_nz2zz.png)
    - GM->L0B Buffer（不支持转置）L1 Buffer->L0B Buffer（不使能转置）（Zn->Zn）

        ![](../../../figures/load2d_gm2l0b_zn2zn.png)
    - L1 Buffer->L0A Buffer（使能转置，仅支持b16数据类型）（Nz->Zz）

        ![](../../../figures/load2d_l12l0a_nz2zz.png)
    - L1 Buffer->L0B Buffer（使能转置，仅支持b16数据类型）（Zn->Zn）

        ![](../../../figures/load2d_l12l0b_zn2zn.png)
- **转置搬运**
    - L1 Buffer->L0A Buffer（b8）（Nz->Zz）

        ![](../../../figures/loaddatawithtranspose_l12l0a_b8_nz2zz.png)
    - L1 Buffer->L0B Buffer（b4/b8）（Zn->Zn）

        ![](../../../figures/loaddatawithtranspose_l12l0b_b4b8_zn2zn.png)
    - L1 Buffer->L0A Buffer（b16）（Nz->Zz）

        ![](../../../figures/loaddatawithtranspose_l12l0a_b16_nz2zz.png)
    - L1 Buffer->L0B Buffer（b16）（Zn->Zn）

        ![](../../../figures/loaddatawithtranspose_l12l0b_b16_zn2zn.png)
    - L1 Buffer->L0A Buffer（b32）（Nz（<span>当且仅当Row==16</span>）->Zz）

        ![](../../../figures/loaddatawithtranspose_l12l0a_b32_nz2zz.png)
    - L1 Buffer->L0B Buffer（b32）（Zn（<span>当且仅当Col==16</span>）->Zn）

        ![](../../../figures/loaddatawithtranspose_l12l0b_b32_zn2zn.png)
- **3D矩阵搬运**
    - L1 Buffer->L0A Buffer（NC1HWC0->Zz）

        ![](../../../figures/load3d_l12l0a_nc1hwc02zz.png)
    - L1 Buffer->L0B Buffer（NC1HWC0->Zn）

        ![](../../../figures/load3d_l12l0b_nc1hwc02zn.png)
- **asc_copy_l12l0b_sparse**
    - L1 Buffer->L0B Buffer（Zn->Zn）

        ![](../../../figures/loaddatawithsparse_l12l0b_zn2zn.png)

针对一些不常用的分形转换的支持情况，开发者可参考下表：

**表1** 扩展分形转换支持情况

| 搬运指令 | 数据通路 | 分形支持情况 |
| --- | --- | --- |
| 2D分形搬运 | GM->L1 Buffer（不支持转置） | Nz->Zz、Zn->Nn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 2D分形搬运 | GM->L0A Buffer（不支持转置）、<br>L1 Buffer->L0A Buffer（不使能转置） | Nz->Nz、Zn->Nn、Zn->Zn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 2D分形搬运 | GM->L0B Buffer（不支持转置）、<br>L1 Buffer->L0B Buffer（不使能转置） | Nz->Zz、Nz->Nz、Zn->Nn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 2D分形搬运 | L1 Buffer->L0A Buffer（使能转置，仅支持b16数据类型） | Nz->Nz、Zn->Nn、Zn->Zn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 2D分形搬运 | L1 Buffer->L0B Buffer（使能转置，仅支持b16数据类型） | Nz->Zz、Nz->Nz、Zn->Nn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 块转置搬运 | L1 Buffer->L0A Buffer（b8）、<br>L1 Buffer->L0A Buffer（b16） | Nz->Nz、Zn->Nn、Zn->Zn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 块转置搬运 | L1 Buffer->L0B Buffer（b4/b8）、<br>L1 Buffer->L0B Buffer（b16） | Nz->Zz、Nz->Nz、Zn->Nn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 块转置搬运 | L1 Buffer->L0A Buffer（b32） | Zz->Nz、Zz->Zz |
| 块转置搬运 | L1 Buffer->L0B Buffer（b32） | Nn->Zn、Nn->Nn |

<!-- end id7 -->

<!-- npu="950" id8 -->
### 针对Ascend 950PR/Ascend 950DT

数据搬运（GM-\>L1 Buffer）支持使能随路进行DN到Nz转换，一定会改变分形排布。

2D矩阵搬运在不使能转置情况下，不改变分形间排布及分形内的排布，如Nz2Nz；在使能转置情况下，既改变分形内的排布，又改变分形间的排布，如Nz2Zn。

MX矩阵的数据和系数分别搬运：A/B数据使用普通2D重载或`_transpose`变体；`asc_copy_l12l0a_mx`/`asc_copy_l12l0b_mx`只搬运E8M0系数，不提供矩阵数据搬运或转置开关。系数需预先按对应布局准备，系数搬运本身不改变分形内排布。

常用分形支持情况如下，供开发者参考：

- **数据搬运**
    - GM->L1 Buffer
        - ND->Nz：

            ![](../../../figures/datacopy_gm2l1_nd2nz.png)
        - DN->Nz：

            ![](../../../figures/datacopy_gm2l1_dn2nz.png)
        - Nz->Nz：

            ![](../../../figures/datacopy_gm2l1_nz2nz.png)
        - Zn->Zn：

            ![](../../../figures/datacopy_gm2l1_zn2zn.png)
- **2D矩阵搬运**
    - L1 Buffer->L0A Buffer（Nz->Nz）

        ![](../../../figures/load2dv2_l12l0a_nz2nz.png)
    - L1 Buffer->L0A Buffer（使能转置）（Zn->Nz）

        ![](../../../figures/load2dv2_l12l0a_zn2nz.png)
    - L1 Buffer->L0B Buffer（Zn->Zn）

        ![](../../../figures/load2dv2_l12l0b_zn2zn.png)
    - L1 Buffer->L0B Buffer（使能转置）（Nz->Zn）

        ![](../../../figures/load2dv2_l12l0b_nz2zn.png)
- **MX数据与系数的独立搬运**
    - L1 Buffer->L0A Buffer（数据，调用asc_copy_l12l0a，Nz->Nz）

        ![](../../../figures/load2dv2mx_l12l0a_nz2nz.png)
    - L1 Buffer->L0A_MX Buffer（系数，调用asc_copy_l12l0a_mx）

        ![](../../../figures/load2dv2mx_l12l0amx.png)
    - L1 Buffer->L0B Buffer（数据，调用asc_copy_l12l0b，Zn->Zn）

        ![](../../../figures/load2dv2mx_l12l0b_zn2zn.png)
    - L1 Buffer->L0B_MX Buffer（系数，调用asc_copy_l12l0b_mx）

        ![](../../../figures/load2dv2mx_l12l0bmx.png)
- **转置搬运**
    - L1 Buffer->L0B Buffer（b4/b8）（Zn->Zn）

        ![](../../../figures/loaddatawithtranspose_l12l0b_b4b8_zn2zn.png)
    - L1 Buffer->L0B Buffer（b16）（Zn->Zn）

        ![](../../../figures/loaddatawithtranspose_l12l0b_b16_zn2zn.png)
    - L1 Buffer->L0B Buffer（b32）（Zn（<span>当且仅当Col==16</span>）->Zn）

        ![](../../../figures/loaddatawithtranspose_l12l0b_b32_zn2zn.png)
- **3D矩阵搬运**
    - L1 Buffer->L0A Buffer（NC1HWC0->Nz）

        ![](../../../figures/load3d_l12l0a_nc1hwc02nz.png)
    - L1 Buffer->L0B Buffer（NC1HWC0->Zn）

        ![](../../../figures/load3d_l12l0b_nc1hwc02zn.png)

针对一些不常用的分形转换的支持情况，开发者可参考下表：

**表2** 扩展分形转换支持情况

| 搬运指令 | 数据通路 | 分形支持情况 |
| --- | --- | --- |
| 2D矩阵搬运 | GM->L1 Buffer（不支持转置） | Nz->Zz、Zn->Nn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 2D矩阵搬运 | L1 Buffer->L0A Buffer（不使能转置） | Nz->Nz、Zn->Nn、Zn->Zn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 2D矩阵搬运 | L1 Buffer->L0B Buffer（不使能转置） | Nz->Zz、Nz->Nz、Zn->Nn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 2D矩阵搬运 | L1 Buffer->L0A Buffer（使能转置，图中以b16为例） | Nz->Nz、Zn->Nn、Zn->Zn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 2D矩阵搬运 | L1 Buffer->L0B Buffer（使能转置，图中以b16为例） | Nz->Zz、Nz->Nz、Zn->Nn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 转置搬运 | L1 Buffer->L0B Buffer（b4/b8）、<br>L1 Buffer->L0B Buffer（b16） | Nz->Zz、Nz->Nz、Zn->Nn、Zz->Zz、Zz->Nz、Nn->Nn、Nn->Zn |
| 转置搬运 | L1 Buffer->L0B Buffer（b32） | Nn->Zn、Nn->Nn |

<!-- end id8 -->

## 数据类型约束

<!-- npu="910b,A3" id9 -->
针对Atlas A2训练系列产品/Atlas A2推理系列产品和Atlas A3训练系列产品/Atlas A3推理系列产品，数据类型约束如表3所示：

**表3** 数据类型约束

| 接口/重载 | C API指针类型 |
| --- | --- |
| `asc_copy_gm2l1`分形搬运 | half、bfloat16_t、int8_t、uint8_t、int32_t、uint32_t、float |
| `asc_copy_gm2l0a`/`asc_copy_gm2l0b`；`asc_copy_l12l0a`/`asc_copy_l12l0b`的2D重载 | 上述类型及int4b_t；具体搬运位宽和转置限制仍按各接口 |
| `asc_copy_l12l0a_trans` / `asc_copy_l12l0b_trans` | half、bfloat16_t、int8_t、uint8_t、int32_t、uint32_t、float |
| `asc_copy_l12l0b_sparse` | int8_t权重及对应索引 |
| `asc_copy_l12l0a`的3D重载 | half、bfloat16_t、int8_t、uint8_t、int4b_t、int32_t、uint32_t、float |
| `asc_copy_l12l0b`的3D重载 | half、bfloat16_t、int32_t、uint32_t、float |

<!-- end id9 -->

<!-- npu="950" id10 -->
针对Ascend 950PR/Ascend 950DT，数据类型约束如表4所示：

**表4** 数据类型约束

| 接口/重载 | C API指针类型 |
| --- | --- |
| `asc_copy_gm2l1`的2D重载 | int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、fp4x2_e1m2_t、fp4x2_e2m1_t、fp8_e4m3fn_t、fp8_e5m2_t、hifloat8_t；另有void位宽搬运重载 |
| `asc_copy_l12l0a`/`asc_copy_l12l0b`的2D重载及`_transpose` | 上述有类型重载及int4b_t |
| `asc_copy_l12l0b_trans` | int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float、int4b_t、fp4x2_e1m2_t、fp4x2_e2m1_t、fp8_e4m3fn_t、fp8_e5m2_t、hifloat8_t |
| `asc_copy_l12l0a`/`asc_copy_l12l0b`的3D重载 | int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、fp8_e4m3fn_t、fp8_e5m2_t、hifloat8_t |
| `asc_copy_l12l0a_mx`/`asc_copy_l12l0b_mx` | fp8_e8m0_t系数；32B分形，不搬A/B数据 |

<!-- end id10 -->

## 搬运指令总结

<!-- npu="910b,A3" id11 -->
结合上述数据类型与分形等约束条件，我们对数据搬入接口的使用场景进行了归纳总结，现提供如下，供开发者参考。

针对Atlas A2训练系列产品/Atlas A2推理系列产品和Atlas A3训练系列产品/Atlas A3推理系列产品，**从L1 Buffer->L0A Buffer/L0B Buffer通路，常用的搬运指令有2D分形搬运、块转置搬运和asc_copy_l12l0a/asc_copy_l12l0b（3D），可以调用指令总结如表5所示：**

**表5** L1 Buffer->L0A Buffer/L0B Buffer通路调用指令总结

| 是否转置/数据类型 | b4（int4b_t） | b8 | b16 | b32 |
| --- | --- | --- | --- | --- |
| A不转置（L1 Buffer->L0A Buffer不需要使能转置） | asc_copy_l12l0a（3D） | 2D分形搬运、asc_copy_l12l0a（3D）（推荐使用） | 2D分形搬运、asc_copy_l12l0a（3D）（推荐使用） | 2D分形搬运、asc_copy_l12l0a（3D）（推荐使用） |
| A转置（L1 Buffer->L0A Buffer需要使能转置） | 不支持 | 块转置搬运 | 2D分形搬运、块转置搬运、asc_copy_l12l0a（3D）（推荐使用） | 块转置搬运（L1 Buffer上数据排布为Nz排布（当且仅当Row==16））、asc_copy_l12l0a（3D）（推荐使用） |
| B不转置（L1 Buffer->L0B Buffer需要使能转置） | 当前公开C API无对应int4块转置重载 | 块转置搬运 | 2D分形搬运、块转置搬运、asc_copy_l12l0b（3D）（推荐使用） | 块转置搬运（L1 Buffer上数据排布为Zn排布（当且仅当Col==16））、asc_copy_l12l0b（3D）（推荐使用） |
| B转置（L1 Buffer->L0B Buffer不需要使能转置） | - | 2D分形搬运 | 2D分形搬运 | 2D分形搬运 |

> [!NOTE]说明
>
> - 由于2D分形搬运和块转置搬运在搬运过程中只能在一个方向做repeat搬运，涉及多个方向的数据块的搬运就需要使用for循环进行搬运，而asc_copy_l12l0a/asc_copy_l12l0b（3D）可以通过配置m_extension和k_extension来完成多个方向的数据块搬运，不需要使用for循环，因此整体的scalar开销要比2D分形搬运和块转置搬运要少，因此，在满足asc_copy_l12l0a/asc_copy_l12l0b（3D）使用约束条件的场景下，推荐使用asc_copy_l12l0a/asc_copy_l12l0b（3D）指令来实现L1 Buffer->L0A Buffer/L0B Buffer的数据搬运。
<!-- end id11 -->

<!-- npu="950" id12 -->
针对Ascend 950PR/Ascend 950DT，asc_copy_l12l0a/asc_copy_l12l0b（2D）可以支持b4（fp4）/b8/b16/b32数据类型以及转置场景搬运，通过配置m_step和k_step来完成多个方向的数据块搬运，并且相比asc_copy_l12l0a/asc_copy_l12l0b（3D），指令的带宽延时更小，因此不带量化系数的矩阵乘法过程中L1 Buffer->L0A Buffer/L0B Buffer通路，都推荐使用asc_copy_l12l0a/asc_copy_l12l0b（2D）指令来实现数据搬运。而带量化系数的矩阵乘法过程中，需要使用asc_copy_l12l0a_mx/asc_copy_l12l0b_mx（系数搬运）指令来实现数据搬运，其中A/B仍由2D接口搬入；MX接口只完成L1 Buffer->L0A_MX Buffer/L0B_MX Buffer系数搬运，两类接口需分别调用。
<!-- end id12 -->
