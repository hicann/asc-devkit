# DataCopy（GM与UB切片数据搬运）<a name="ZH-CN_TOPIC_0000002568770166"></a>

## 产品支持情况<a name="zh-cn_topic_0000002565968945_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
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

## 功能说明<a name="zh-cn_topic_0000002565968945_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

该接口为软仿接口，从易用性角度出发进行设计，支持数据的切片搬运，提取多维Tensor数据的子集进行搬运。

具体支持的数据通路为（以[逻辑位置TPosition](../../辅助数据结构/TPosition.md)表示）：

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id8 -->
    - CO2 -> GM（仅Atlas 推理系列产品AI Core支持）
    <!-- end id8 -->

## 函数原型<a name="zh-cn_topic_0000002565968945_section82039854412"></a>

- Global Memory -> Unified Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

- Unified Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T> &dst, const LocalTensor<T> &src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

> [!NOTE]说明 
> 各原型支持的数据类型，请参考[数据类型](#zh-cn_topic_0000002565968945_section4219135304818)。

## 参数说明<a name="zh-cn_topic_0000002565968945_section16128134420472"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :--- | :--- |
| T | 源操作数或者目的操作数的数据类型。支持的数据类型请参考[数据类型](#zh-cn_topic_0000002565968945_section4219135304818)。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| dst | 输出 | 目的操作数。<br>&bull;类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor简介.md)时，存储位置为Unified Buffer，目的地址需要32字节对齐。<br>&bull;类型为[GlobalTensor](../../数据结构/GlobalTensor/GlobalTensor简介.md)时，存储位置为Global Memory，目的地址需要1字节对齐。 |
| src | 输入 | 源操作数。<br>&bull;类型为GlobalTensor时，存储位置为Global Memory，源地址需要1字节对齐。<br>&bull;类型为LocalTensor时，存储位置为Unified Buffer，源地址需要32字节对齐。 |
| dstSliceInfo | 输入 | 目的操作数切片信息，类型为SliceInfo。通过该参数可以配置切片的起始和终止元素个数、间隔、长度等信息。<br>SliceInfo参数说明请参考[表3](#table_slice_3)。 |
| srcSliceInfo | 输入 | 源操作数切片信息，类型为SliceInfo。通过该参数可以配置切片的起始和终止元素个数、间隔、长度等信息。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_data_copy.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>SliceInfo参数说明请参考[表3](#table_slice_3)。 |
| dimValue | 输入 | 操作数维度信息，默认值为1。 |

**表3**  SliceInfo结构体参数定义<a name="table_slice_3"></a>

| 参数名 | 描述 |
| :--- | :--- |
| startIndex | 切片的起始元素位置，数据类型为uint32_t，单位是元素个数。 |
| endIndex | 切片的终止元素位置，数据类型为uint32_t，单位是元素个数。 |
| stride | 切片的间隔元素个数，数据类型为uint32_t。 |
| burstLen | 横向切片，每一片数据的长度，仅在dimValue = 1时生效，超出1维的情况下，必须配置为1，不支持配置成其他值。数据类型为uint32_t，单位：datablock（32B）。<br>例如，srcSliceInfo的List为 {{16, 70, 7, 3, 87},  {0, 2, 1, 1, 3}}，{16, 70, 7, 3, 87}表示第一维的切片信息，burstLen设置为3，表示一个切片数据段大小为3个datablock；{0, 2, 1, 1, 3}为第二维的切片信息，burstLen仅能设置为1。 |
| shapeValue | 当前维度的原始长度。数据类型为uint32_t，单位为元素个数。 |

如[图1](#fig1196816115710)所示，以float数据类型为例，切片搬运接口能从shape为3\*87的源操作中，选取四个不连续的横向切片搬运到目的操作数中，下面结合图示对SliceInfo结构体参数进行解析：

**图1**  参数解析示意图<a name="fig1196816115710"></a>  
![](../../../../figures/slice_datacopy_parameter_analysis_diagram.png "参数解析示意图")

- dimValue为2，表示操作数有2维。
- srcSliceInfo为 {{16, 70, 7, 3, 87},  {0, 2, 1, 1, 3}}
    - {16, 70, 7, 3, 87}是针对单独一行，即从一维的角度来配置，每个元素代表一个数：

        **startIndex**  = 16，表示有效数据段从第16个数开始；

        **endIndex**  = 70，表示有效数据段到第70个数结束；

        **stride**  = 7，单位为元素个数，表示相邻的2个切片数据段间隔的元素个数，为7个0的间距；

        **burstLen**  = 3，单位为32B，表示在这一个有效数据段中，一个切片数据段大小为3个datablock；

        **shapeValue**  = 87，表示单独一行的长度，单位为元素个数，即8 \* 10 + 7 = 87个元素。

    - {0, 2, 1, 1, 3}是针对多行，即从二维的角度来配置，每个元素代表一行：

        **startIndex**  = 0，表示有效数据段从第0行开始；

        **endIndex**  = 2，表示有效数据段到第2行结束；

        **stride**  = 1，表示相邻的2个切片数据段中间隔元素为1行；

        **burstLen**  = 1，在dimValue \> 1时必须填为1；

        **shapeValue**  = 3，表明一共有3行。

- dstSliceInfo为{{0, 47, 0, 3, 48}, {0, 1, 0, 1, 2}}
    - {0, 47, 0, 3, 48}是针对单独一行，即从一维的角度来配置，每个元素代表一个数：

        **startIndex**  = 0，表示有效数据段从第0个数开始；

        **endIndex**  = 47，表示有效数据段到第47个数结束；

        **stride**  = 0，单位为元素个数，表示相邻的2个切片数据段间隔的元素个数，为0表示两个切片数据段没有间距；

        **burstLen**  = 3，单位为32B，表示在这一个有效数据段中，一个切片数据段大小为3个datablock；

        **shapeValue**  = 48，表示单独一行的长度，单位为元素个数，即8 \* 6 = 48个元素。

    - {0, 1, 0, 1, 2} 是针对多行，即从二维的角度来配置，每个元素代表1行：

        **startIndex** = 0，表示有效数据段从第0行开始；

        **endIndex** = 1，表示有效数据段到第1行结束；

        **stride** = 0，表示相邻的2个切片数据段没有间隔；

        **burstLen**  = 1，在dimValue \> 1时必须填为1；

        **shapeValue** = 2，表示一共有2行。

## 数据类型<a name="zh-cn_topic_0000002565968945_section4219135304818"></a>

源操作数和目的操作数支持的数据类型保持一致，Global Memory -> Unified Buffer和Unified Buffer -> Global Memory两个数据通路的数据类型支持情况相同，具体如下：

<!-- npu="950" id9 -->

- Ascend 950PR/Ascend 950DT：bool、int8_t、uint8_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。

<!-- end id9 -->

<!-- npu="A3" id10 -->

- Atlas A3 训练系列产品/Atlas A3 推理系列产品：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

<!-- end id10 -->

<!-- npu="910b" id11 -->

- Atlas A2 训练系列产品/Atlas A2 推理系列产品：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。

<!-- end id11 -->

<!-- npu="310p" id12 -->

- Atlas 推理系列产品AI Core：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。

<!-- end id12 -->

## 返回值说明

无

## 约束说明<a name="zh-cn_topic_0000002565968945_section2045914466492"></a>

- 该接口为软仿接口，从易用性角度出发进行设计。
- 位于Global Memory的地址必须1字节对齐，位于Unified Buffer的地址必须32字节对齐。
- burstLen，仅在dimValue = 1时生效，超出1维的情况下，必须配置为1，不支持配置成其他值。
- 切片数据搬运中的横向burstLen大小设置，需要用户自己通过计算：横向切片元素个数\* sizeof\(T\)/32字节。横向切片元素个数\* sizeof\(T\)的大小必须是32字节的倍数。
- SliceInfo结构体的成员变量startIndex、endIndex、stride、burstLen的取值须满足：
$$endIndex - startIndex + 1 = (N - 1) \times stride + N \times burstLen \times \frac{32}{sizeof(T)}, \quad N \in正整数$$
- SliceInfo结构体的成员变量startIndex、endIndex、shapeValue的取值须满足：
$$startIndex < endIndex \le shapeValue$$
- 切片数据搬运中的SliceInfo结构体数组大小和dimValue需要保持一致，并且不超过8。
- 切片数据搬运中的srcSliceInfo结构体数组大小的和dstSliceInfo的结构体数组大小需要保持一致，两者的结构体中的burstLen需要相等（srcSliceInfo\[i\].burstLen = dstSliceInfo\[i\].burstLen）。
- 切片数据搬运对参数有一定要求，建议使用者参考调用示例，并在CPU上仿真结果无误后，再到NPU侧执行。

## 调用示例<a name="zh-cn_topic_0000002565968945_section088124295117"></a>

```cpp
// 配置切片搬运参数。
AscendC::SliceInfo srcSliceInfoIn[] = {{16, 70, 7, 3, 87}, {0, 2, 1, 1, 3}};
AscendC::SliceInfo dstSliceInfoIn[] = {{0, 47, 0, 3, 48}, {0, 1, 0, 1, 2}};
uint32_t dimValueIn = 2;
dimValue = dimValueIn;
for (uint32_t i = 0; i < dimValueIn; i++) {
    srcSliceInfo[i].startIndex = srcSliceInfoIn[i].startIndex;
    srcSliceInfo[i].endIndex = srcSliceInfoIn[i].endIndex;
    srcSliceInfo[i].stride = srcSliceInfoIn[i].stride;
    srcSliceInfo[i].burstLen = srcSliceInfoIn[i].burstLen;
    srcSliceInfo[i].shapeValue = srcSliceInfoIn[i].shapeValue;
    dstSliceInfo[i].startIndex = dstSliceInfoIn[i].startIndex;
    dstSliceInfo[i].endIndex = dstSliceInfoIn[i].endIndex;
    dstSliceInfo[i].stride = dstSliceInfoIn[i].stride;
    dstSliceInfo[i].burstLen = dstSliceInfoIn[i].burstLen;
    dstSliceInfo[i].shapeValue = dstSliceInfoIn[i].shapeValue;
}

// 先后进行两个数据通路的切片搬运。
// Global Memory -> Unified Buffer: MTE2流水搬运。
AscendC::DataCopy(srcLocal, srcGlobal, dstSliceInfo, srcSliceInfo, dimValue);
// 等待MTE2搬运完成，MTE3可以开始。
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
// Unified Buffer -> Global Memory: MTE3流水搬运。
AscendC::DataCopy(dstGlobal, srcLocal, dstSliceInfo, dstSliceInfo, dimValue);
```

完整样例请参考[切片数据搬运样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2ub_slice)。srcSliceInfo、dstSliceInfo参数解析与结果示例请参考[图1](#fig1196816115710)。
