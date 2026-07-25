# SetLoopModePara<a name="ZH-CN_TOPIC_0000002414107833"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas 训练系列产品：不支持
<!-- end id14 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

DataCopy、DataCopyPad过程中通过该接口使能loop mode并且设置loop mode的参数，在数据搬运结束后通过[ResetLoopModePara](ResetLoopModePara.md)重置loop mode的参数。

具体支持的数据通路为（以[逻辑位置TPosition](../../辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetLoopModePara(const LoopModeParams& loopParams, DataCopyMVType type)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| loopParams | 输入 | 循环模式参数LoopModeParams类型，定义如下，具体参数说明请参考[表2](#table_setloop_2)。<br><pre>struct LoopModeParams {<br>        loop1Size = 0;<br>        loop2Size = 0;<br>        loop1SrcStride = 0;<br>        loop1DstStride = 0;<br>        loop2SrcStride = 0;<br>        loop2DstStride = 0 ;<br>};</pre> |
| type | 输入 | 数据搬运模式。DataCopyMVType为枚举类型，定义如下，具体参数说明请参考[表3](#table1166074612214)。<br><pre>enum class DataCopyMVType : uint8_t {<br>    UB_TO_OUT = 0,<br>    OUT_TO_UB = 1,<br>};</pre> |

**表2**  LoopModeParams结构体参数说明<a name="table_setloop_2"></a>

| 参数名 | 描述 |
| --- | --- |
| loop1Size | 用于设置内层循环的循环次数，数据类型为uint32_t，取值范围为[0, 2^21)。 |
| loop2Size | 用于设置外层循环的循环次数，数据类型为uint32_t，取值范围为[0, 2^21)。 |
| loop1SrcStride | 用于设置内层循环中相邻迭代源操作数的数据块间的间隔，单位为字节，数据类型为uint64_t。<br>&bull;当数据搬运模式是UB_TO_OUT的时候取值范围为[0, 2^21)，并且loop1SrcStride必须32字节对齐。<br>&bull;当数据搬运模式是OUT_TO_UB的时候取值范围为[0, 2^40)。 |
| loop1DstStride | 用于设置内层循环中相邻迭代目的操作数的数据块间的间隔，单位为字节，数据类型为uint64_t。<br>&bull;当数据搬运模式是UB_TO_OUT的时候取值范围为[0, 2^40)。<br>&bull;当数据搬运模式是OUT_TO_UB的时候取值范围为[0, 2^21)，并且loop1DstStride必须32字节对齐。 |
| loop2SrcStride | 用于设置外层循环中相邻迭代源操作数的数据块间的间隔，单位为字节，数据类型为uint64_t。<br>&bull;当数据搬运模式是UB_TO_OUT的时候取值范围为[0, 2^21)，并且loop2SrcStride必须32字节对齐。<br>&bull;当数据搬运模式是OUT_TO_UB的时候取值范围为[0, 2^40)。 |
| loop2DstStride | 用于设置外层循环中相邻迭代目的操作数的数据块间的间隔，单位为字节，数据类型为uint64_t。<br>&bull;当数据搬运模式是UB_TO_OUT的时候取值范围为[0, 2^40)。<br>&bull;当数据搬运模式是OUT_TO_UB的时候取值范围为[0, 2^21)，并且loop2DstStride必须32字节对齐。 |

**表3**  DataCopyMVType结构体参数说明 <a name="table1166074612214"></a>

| 参数名 | 描述 |
| --- | --- |
| UB_TO_OUT | 从UB搬运到GM的通路。 |
| OUT_TO_UB | 从GM搬运到UB的通路。 |

下面的样例呈现了SetLoopModePara的使用方法。

- 样例中在数据类型为int8\_t的场景下，数据块大小为384，配置DataCopyPad的数据搬运模式为Compact模式，blockLen = 48，blockCount = 2，表明每个连续传输数据块包含48字节，且连续传输数据块有两个，srcStride = 0，dstStride = 0，isPad = false，表明源操作数相邻数据块之间没有间隔且不需要填充用户自定义的数据；
- 再设置SetLoopModePara中LoopModeParams的参数：loop1Size = 2，loop2Size = 2，loop1SrcStride = 96，loop2SrcStride =192，loop1DstStride = 128，loop2DstStride = 288，DataCopyMVType为OUT\_TO\_UB，表明内层循环和外层循环的次数分别为2次，内层循环和外层循环中相邻迭代源操作数的数据块间隔分别为96字节和192字节，内层循环和外层循环中相邻迭代目的操作数的数据块间隔分别为128字节和288字节，通路是从GM搬运到UB；
- 使用以上配置，调用SetLoopModePara再调用DataCopyPad就可以开启DataCopyPad的loop模式完成数据类型为int8\_t的数据块大小为384的数据搬运。详细图解如下：

**图1**  源操作数搬运场景示例<a name="fig6671114911311"></a>  
![](../../../../figures/source_operand_transfer_scenario.png "源操作数搬运场景示例")

**图2**  目的操作数搬运场景示例<a name="fig226181311513"></a>  
![](../../../../figures/destination_operand_transfer_scenario.png "目的操作数搬运场景示例")

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 源操作数和目的操作数的起始地址需要保证32字节对齐。
- 目的操作数的数据不能重叠，如果有重叠，硬件层面不会报错或者告警，同时也不能保证重叠数据的正确性；但是不同迭代可以交织，例如内层循环中相邻迭代目的操作数的数据块间的间隔可以小于相邻连续目的操作数的数据块的间隔。
- 需要在每次使能loop mode并且设置loop mode的参数后通过ResetLoopModePara进行寄存器的复位，否则会影响到下一次对应通路的搬运的使用，引发异常。

## 调用示例<a name="section1227835243314"></a>

- SetLoopModePara使能loop mode，Compact模式

    ```cpp
    // 场景4：使用SetLoopModePara使能loop mode进行数据搬运（仅支持Ascend 950PR/Ascend 950DT）。
    // Compact模式：每次内层循环搬运80B后填充16B使其96字节对齐。
    // padding值设为-1。
    AscendC::DataCopyExtParams copyParams{BLOCK_COUNT, BLOCK_LEN, 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams{true, 0, 0, -1};

    AscendC::LoopModeParams loopParam2Ub {LOOP1_SIZE, LOOP2_SIZE, LOOP1_SRC_STRIDE, LOOP1_DST_STRIDE, LOOP2_SRC_STRIDE, LOOP2_DST_STRIDE};
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);
    AscendC::DataCopyPad<int8_t, AscendC::PaddingMode::Compact>(srcLocal, srcGlobal, copyParams, padParams);
    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

- SetLoopModePara使能loop mode，Normal模式

    ```cpp
    // 场景5：使用SetLoopModePara使能loop mode进行数据搬运（仅支持Ascend 950PR/Ascend 950DT）。
    // Normal模式：每个block搬运40B后填充24B使其64字节对齐。
    // padding值设为-1。
    AscendC::DataCopyExtParams copyParams{BLOCK_COUNT, BLOCK_LEN, 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParams{true, 0, 0, -1};

    AscendC::LoopModeParams loopParam2Ub {LOOP1_SIZE, LOOP2_SIZE, LOOP1_SRC_STRIDE, LOOP1_DST_STRIDE, LOOP2_SRC_STRIDE, LOOP2_DST_STRIDE};
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);
    AscendC::DataCopyPad<int8_t>(srcLocal, srcGlobal, copyParams, padParams);
    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

- 五维数据搬运

    ```cpp
    // 场景6：五维数据搬运GM[2, 4, 3, 128, 126]int8 -> UB[512, 128]int8。
    // Normal模式，使用loop mode。
    // 搬运规格：[2, 2, 2, 64, 126]，每个126字节补2字节padding到128字节。
    // 最终UB连续存放为[512, 128]。

    // Stride参数说明：
    // - loop1SrcStride=128*126：第2维stride，每次loop1源地址跳过128*126字节
    // - loop1DstStride=64*128：每次loop1目的地址跳过64*128字节（64行×128字节/行）
    // - loop2SrcStride=3*128*126：第1维stride，每次loop2源地址跳过3*128*126字节
    // - loop2DstStride=2*64*128：每次loop2目的地址跳过2*64*128字节

    AscendC::LoopModeParams loopParam2Ub {
        2,  // loop1Size=2（第2维搬运2次）
        2,  // loop2Size=2（第1维搬运2次）
        128 * 126,  // loop1SrcStride（第2维stride）
        64 * 128,   // loop1DstStride
        3 * 128 * 126,  // loop2SrcStride（第1维stride）
        2 * 64 * 128    // loop2DstStride
    };
    AscendC::SetLoopModePara(loopParam2Ub, AscendC::DataCopyMVType::OUT_TO_UB);

    AscendC::DataCopyExtParams copyParams{
        64,  // blockCount=64（第3维搬运64次）
        126,  // blockLen=126（第4维搬运126字节，padding到128）
        0, 0, 0  // srcStride=0, dstStride=0, rsv=0
    };
    AscendC::DataCopyPadExtParams<int8_t> padParams{true, 0, 0, 0};

    // 第0维for循环搬运2次。
    for (uint32_t dim0 = 0; dim0 < 2; dim0++) {
        constexpr uint32_t dim0SrcStride = 4 * 3 * 128 * 126;  // 第0维源stride
        constexpr uint32_t dim0DstStride = 2 * 2 * 64 * 128;   // 第0维目的stride
        uint32_t srcOffset = dim0 * dim0SrcStride;
        uint32_t dstOffset = dim0 * dim0DstStride;

        AscendC::DataCopyPad<int8_t>(srcLocal[dstOffset], srcGlobal[srcOffset], copyParams, padParams);
    }

    AscendC::ResetLoopModePara(AscendC::DataCopyMVType::OUT_TO_UB);
    ```

完整样例请参考[DataCopyPad样例场景4、5、6](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm)。
