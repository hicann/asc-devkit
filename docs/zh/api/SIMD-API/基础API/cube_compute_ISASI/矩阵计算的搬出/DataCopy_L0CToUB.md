# DataCopy（L0C Buffer到UB数据搬运）<a name="ZH-CN_TOPIC_0000002382867725"></a>

## 产品支持情况<a name="section1550532418810"></a>

>**说明：**
> 本接口搬运功能仅在Atlas 推理系列产品AI Core产品型号的L0C Buffer-\>UB（CO1 -\> CO2）通路支持。其他型号和其他通路支持接口调用但功能不生效，功能等同于基础数据搬运。

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section474617392321"></a>

对数据搬运能力进行增强，相比于基础数据搬运接口，增加了L0C Buffer-\>Unified Buffer（UB）通路的随路计算。

## 函数原型<a name="section1954364615315"></a>

- Local Memory -\> Local Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```

- Local Memory -> Local Memory，支持源操作数和目的操作数类型不一致。

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```

>**说明：**
> 各原型支持的具体数据通路和数据类型，请参考[数据类型](#section8664371012)。

## 参数说明<a name="section1592117358563"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T、U | 操作数的数据类型。支持的数据类型请参考[数据类型](#section8664371012)。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。 |
| src | 输入 | 源操作数，类型为LocalTensor。 |
| intriParams | 输入 | 搬运参数。DataCopyParams类型。 |
| enhancedParams | 输入 | 增强信息参数。DataCopyEnhancedParams类型。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。 |

**表3** DataCopyEnhancedParams结构参数说明

| 参数名称 | 含义 |
| ---------- | ---------- |
| blockMode | 数据搬运基本分形，BlockMode枚举类型，支持以下配置：<br>&nbsp;&nbsp;&bull; BLOCK_MODE_NORMAL：表示传输单位为32字节。当前暂不支持。<br>&nbsp;&nbsp;&bull; BLOCK_MODE_MATRIX：表示传输单位为一个16 \* 16的cube分形。<br>&nbsp;&nbsp;&bull; BLOCK_MODE_VECTOR：表示传输单位为一个1 \* 16的cube分形。<br>&nbsp;&nbsp;&bull; BLOCK_MODE_SMALL_CHANNEL：表示传输单位为一个16 \* 4的cube分形。当前暂不支持。<br>&nbsp;&nbsp;&bull; BLOCK_MODE_DEPTHWISE：表示传输单位为一个16 \* 16的cube分形，提供随路channel-split功能。当前暂不支持。<br>每种模式下对应的blockLen等参数单位见[表4](#table13396838183618)。 |
| deqScale | 随路精度转换辅助参数，即量化模式，支持的量化模式取值和对应的数据类型等信息请参考[表5](#table168091348673)。其中DEQ、DEQ8、DEQ16模式，需要传入deqValue量化系数，设置deqValue的对应比特位；VDEQ、VDEQ8、VDEQ16模式，需要传入包含16个元素（deqValue）的量化参数向量，设置deqTensorAddr的对应比特位，同时保证DEQADDR中存储的反量化参数向量的每个元素（deqValue）都符合预期和使用限制。<br>VDEQ模式下，反量化参数向量长度为32字节（16个half元素）；其他模式下，反量化参数向量长度为128字节（16个64bit的反量化元素）。 |
| deqValue | 量化系数。 deqValue的配置方式请参考[deqValue配置方式](#table54451538192912)。 |
| deqTensorAddr | UB中存储反量化参数向量的起始地址。deqScale为VDEQ/VDEQ8/VDEQ16模式时，需要传入反量化运算时的参数向量的地址。该地址要满足32字节对齐。<br>对于VDEQ模式，该地址指向32字节大小的反量化参数向量，其中每个元素大小为16bit（half）。<br>对于VDEQ8、VDEQ16模式，反量化参数向量中的每个元素大小都为64bit。搬运时会搬运blockCount个连续传输数据块，每个数据块的长度为blockLen。每个数据块对应一个128字节的反量化向量。对于同一个数据块，反量化参数向量中的16个元素会被连续复用。不同的数据块，对应不同的反量化参数向量，地址会相应的偏移12。例如：假设对应起始地址为A，第一个数据块的12反量化参数向量起始地址为A，第二个数据块的12反量化参数向量起始地址为A + 12。<br>同一个反量化参数向量的每一个元素的MCB标志位必须一致。 |
| sidStoreMode | 用于deqScale为DEQ8/VDEQ8时配置存储模式，控制反量化结果如何存储在dst地址中。配置效果参考[sidStoreMode配置示意图](#fig5416115192414)。<br>&nbsp;&nbsp;&bull; 0：dst的数据存储在每个DataBlock的前半段，即每32字节的高16字节；<br>&nbsp;&nbsp;&bull; 1：dst的数据存储在每个DataBlock的后半段，即每32字节的低16字节；<br>&nbsp;&nbsp;&bull; 2：dst的数据存储在完整的DataBlock中，即整个32字节。 |
| isRelu | 配置是否可以随路做线性整流操作。配置deqValue的情况下，如果该参数被置为true，那么会刷新deqValue的ReLU标志位为1；如果被置为false，则不会做修改。配置deqTensorAddr的情况下，反量化参数向量元素中的ReLU标志位不生效，以isRelu为准。<br>仅配置isRelu，不配置量化参数，即deqValue配置为DEQ_NONE场景，支持src和dst的数据类型组合如下：{half，half}，{float，float}，{int32_t，int32_t}，{float，half}；同时配置isRelu和量化参数的场景，支持的数据类型组合参考[表5](#table168091348673)。 |
| padMode | 预留参数，当前暂不支持。 |

**表4** 不同blockMode对应的参数单位<a id="table13396838183618"></a>

| blockMode | src | dst | 数据类型 | blockLen单位 | srcStride单位 | dstStride单位 |
| ---------- | ---------- | ---------- | ---------- | ---------- | ---------- | ---------- |
| BLOCK_MODE_MATRIX | CO1 | CO2 | half、int16_t、uint16_t | 512B | 512B | 32B |
| BLOCK_MODE_MATRIX | CO1 | CO2 | float、int32_t、uint32_t | 1024B | 1024B | 32B |
| BLOCK_MODE_VECTOR | CO1 | CO2 | half、int16_t、uint16_t | 32B | 512B | 32B |
| BLOCK_MODE_VECTOR | CO1 | CO2 | float、int32_t、uint32_t | 64B | 1024B | 32B |

**表5** deqScale参数列表<a id="table168091348673"></a>

| 量化模式 | src.dtype | dst.dtype | 配合使用的参数 |
| ---------- | ---------- | ---------- | ---------- |
| DEQ | int32_t | half | deqValue中的变量M |
| DEQ | half | half | deqValue中的变量M |
| DEQ8 | int32_t | int8_t | &bull; deqValue<br>&nbsp;&nbsp;&bull;变量M<br>&nbsp;&nbsp;&bull;变量N<br>&nbsp;&nbsp;&bull; MCB标志位<br>&nbsp;&nbsp;&bull; Offset<br>&nbsp;&nbsp;&bull; Sign标志位<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| DEQ8 | int32_t | uint8_t | &bull; deqValue<br>&nbsp;&nbsp;&bull;变量M<br>&nbsp;&nbsp;&bull;变量N<br>&nbsp;&nbsp;&bull; MCB标志位<br>&nbsp;&nbsp;&bull; Offset<br>&nbsp;&nbsp;&bull; Sign标志位<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| DEQ16 | int32_t | half | &bull; deqValue<br>&nbsp;&nbsp;&bull;变量M<br>&nbsp;&nbsp;&bull;变量N<br>&nbsp;&nbsp;&bull; MCB标志位<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| DEQ16 | int32_t | int16_t | &bull; deqValue<br>&nbsp;&nbsp;&bull;变量N<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ | int32_t | half | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ8 | int32_t | int8_t | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ8 | int32_t | uint8_t | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ16 | int32_t | half | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ16 | int32_t | int16_t | deqTensorAddr地址存储的反量化参数向量中的元素deqValue支持配置的参数分别对应DEQ/DEQ8/DEQ16的说明。<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU标志位<br>&nbsp;&nbsp;&bull; isRelu |

**表6** deqValue配置方式<a id="table54451538192912"></a>

| 模式 | 比特位数 | 变量名 | 作用介绍 |
| ---------- | ---------- | ---------- | ---------- |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 0~31 | M | 32位数视为float，作为反量化计算所需要乘的值。src为int32_t，dst为int16_t的场景下，变量M不生效。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 32~35 | N | 4位比特位，表示范围为[1, 16]（b'0000对应表示1，b'1111对应表示16）。<br>当模式为DEQ8、VDEQ8时，MCB标志位置为1时，将输入的值进行右移N比特位。当模式为DEQ16、VDEQ16并且dst数据类型为int16_t时，直接进行N位的右移，不受MCB标志位控制。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 36 | MCB标志位 | Mode Control Bit。如果置为0，输入的int32_t会被直接转换为float。如果置为1，输入的int32_t会先右移N比特位，转变成int16_t，然后转换为float。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 37~45 | Offset | 9bit的整型数据，在进行反量化src * M的计算结果后与Offset进行相加。仅在DEQ8、VDEQ8模式中会用到。如果不使用offset，请置为0。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 46 | Sign标志位 | 如果置为1，表明反量化结果是signed(int8)；如果置为0，表明反量化结果是unsigned(uint8)。仅在DEQ8、VDEQ8模式中会用到。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 47 | ReLU标志位 | 如果置为1，对最终结果进行ReLU计算；如果置为0，不进行额外计算。<br>&nbsp;&nbsp;&bull;对于int32_t->int8_t，配置ReLU时，offset必须配置成-128；<br>&nbsp;&nbsp;&bull;对于int32_t->uint8_t，配置ReLU时，offset必须配置成0。 |
| DEQ8、VDEQ8、DEQ16、VDEQ16 | 48~63 | - | 预留 |
| DEQ、VDEQ | 0 ~ 15 | M | 这16位数被视为half，作为反量化计算需要乘的值。 |

**图1** sidStoreMode配置示意图<a id="fig5416115192414"></a>  

![](../../../../figures/sidStoreMode配置示意图.png "sidStoreMode配置示意图")

## 数据类型<a id="section8664371012"></a>

源操作数和目的操作数的数据类型保持一致时，支持的数据类型为：half、float、int32_t、uint32_t。

**表7** 源操作数和目的操作数的数据类型不一致时的支持情况

| 源操作数的数据类型 | 目的操作数的数据类型 |
| ---------- | ---------- |
| float | half |
| int32_t | int8_t、uint8_t、int16_t、half。 |

## 返回值说明<a name="section16867144513189"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 开发者需要保证DataCopyEnhancedParams中的isRelu参数配置和量化系数deqValue/量化参数向量deqTensorAddr的ReLU标志位配置一致：都开启或都不开启。
- 如果有随路精度转换，操作数的blockLen单位需要减半。

## 调用示例<a name="section1491211256313"></a>

- Local Memory -\> Local Memory

    ```cpp
    // srcLocal、dstLocal为half类型的LocalTensor
    // 使用传入DataCopyParams参数的搬运接口，支持连续和非连续搬运
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // 连续数据块个数为1
    intriParams.blockLen = 512 * sizeof(half) / 32; // 连续数据块长度，单位为DataBlock，此处长度为512个half元素
    intriParams.srcGap = 0; // 源操作数做连续搬运
    intriParams.dstGap = 0; // 目的操作数连续排布
    DataCopyEnhancedParams enhancedParams;
    enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX; // 传输单位为16*16的cube分形
    enhancedParams.deqScale = DeqScale::DEQ_NONE; // 不做量化
    enhancedParams.deqValue = 0;
    enhancedParams.deqTensorAddr = 0;
    enhancedParams.sidStoreMode = 0; // 仅deqScale为DEQ8/VDEQ8时生效
    enhancedParams.isRelu = false; // 不支持ReLU
    enhancedParams.padMode = pad_t::PAD_NONE; // 预留参数
    // 功能等同基础数据搬运
    AscendC::DataCopy(dstLocal, srcLocal, intriParams, enhancedParams);
    ```

- Local Memory -\> Local Memory，源操作数与目的操作数类型不一致

    ```cpp
    // 以下示例为L0C Buffer -> UB通路的数据搬运，仅在Atlas 推理系列产品AI Core场景下支持。srcLocal为int32_t类型的L0C Buffer上的LocalTensor，dstLocal为int8_t类型的UB上的LocalTensor
    // 使用传入DataCopyParams参数的搬运接口，支持连续和非连续搬运
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // 连续数据块个数为1
    intriParams.blockLen = 512 * sizeof(half) / 32; // 连续数据块长度，单位为DataBlock，此处长度为512个half元素
    intriParams.srcGap = 0; // 源操作数做连续搬运
    intriParams.dstGap = 0; // 目的操作数连续排布
    DataCopyEnhancedParams enhancedParams;
    enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX; // 传输单位为16*16的cube分形
    enhancedParams.deqScale = DeqScale::DEQ8; // int32_t -> int8_t量化模式，配合deqValues使用
    enhancedParams.deqValue = 70369809530880; // float 1 = uint64_t 1065353216, when s322s8 use 1065353216 | (1 << 46)
    enhancedParams.deqTensorAddr = 0; // 仅VDEQ模式下使用
    enhancedParams.sidStoreMode = 2; // dstLocal的数据存储在完整的DataBlock中
    enhancedParams.isRelu = false; // 不支持ReLU
    enhancedParams.padMode = pad_t::PAD_NONE; // 预留参数
    // 此时dstLocal = srcLocal
    AscendC::DataCopy(dstLocal, srcLocal, intriParams, enhancedParams);
    ```
