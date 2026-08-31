# DumpTensor<a name="ZH-CN_TOPIC_0000001584608650"></a>

## 产品支持情况

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
- Atlas 200I/500 A2 推理产品：支持
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
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor_res.md#id1 -->

## 功能说明<a name="section259105813316"></a>

头文件路径为：`"basic_api/kernel_operator_dump_tensor_intf.h"`。

该接口可以打印Tensor的内容，同时支持打印自定义的标签（仅支持uint32_t数据类型的信息），比如打印当前行号等。
在算子kernel侧实现代码中需要打印Tensor数据的地方，调用DumpTensor接口打印相关内容。样例如下：

```cpp
AscendC::DumpTensor(srcLocal, 5, dataLen);
```
> [!CAUTION]注意
> 该接口主要用于调试分析，开启后会对算子性能产生一定影响，通常在调试阶段使用，生产环境建议关闭。<br>
> 默认情况下，调用该接口就会打印相关内容，开发者可以参考[关闭ASCENDC_DUMP说明](../disable_ascendc_dump.md)，按需关闭该接口功能。

打印示例如下：  
```plain
DumpTensor: desc=5, addr=0, data_type=float16, position=UB, dump_size=32
[19.000000, 4.000000, 38.000000, 50.000000, 39.000000, 67.000000, 84.000000, 98.000000, 21.000000, 36.000000, 18.000000, 46.000000, 10.000000, 92.000000, 26.000000, 38.000000, 39.000000, 9.000000, 82.000000, 37.000000, 35.000000, 65.000000, 97.000000, 59.000000, 89.000000, 63.000000, 70.000000, 57.000000, 35.000000, 3.000000, 16.000000,
42.000000]
DumpTensor: desc=5, addr=100, data_type=float16, position=UB, dump_size=32
[6.000000, 34.000000, 52.000000, 38.000000, 73.000000, 38.000000, 35.000000, 14.000000, 67.000000, 62.000000, 30.000000, 49.000000, 86.000000, 37.000000, 84.000000, 18.000000, 38.000000, 18.000000, 44.000000, 21.000000, 86.000000, 99.000000, 13.000000, 79.000000, 84.000000, 9.000000, 48.000000, 74.000000, 52.000000, 99.000000, 80.000000,
53.000000]
...
DumpTensor: desc=5, addr=0, data_type=float16, position=UB, dump_size=32
[35.000000, 41.000000, 41.000000, 22.000000, 84.000000, 49.000000, 60.000000, 0.000000, 90.000000, 14.000000, 67.000000, 80.000000, 16.000000, 46.000000, 16.000000, 83.000000, 6.000000, 70.000000, 97.000000, 28.000000, 97.000000, 62.000000, 80.000000, 22.000000, 53.000000, 37.000000, 23.000000, 58.000000, 65.000000, 28.000000, 4.000000,
29.000000]
```

## 函数原型<a name="section2067518173415"></a>

- 无Tensor shape的打印

    ```cpp
    template <typename T>
    __aicore__ inline void DumpTensor(const LocalTensor<T> &tensor, uint32_t desc, uint32_t dumpSize)
    template <typename T>
    __aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize)
    ```

- 带Tensor shape的打印

    ```cpp
    template <typename T>
    __aicore__ inline void DumpTensor(const LocalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize, const ShapeInfo& shapeInfo)
    template <typename T>
    __aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize, const ShapeInfo& shapeInfo)
    ```

## 参数说明<a name="section158061867342"></a>

**表1**  模板参数说明

| 参数名称 | 描述 |
| ------ | ------ |
| T | 需要dump的Tensor的数据类型。 |

**表2**  参数说明

| 参数名称 | 输入/输出 | 描述 |
| ------ | ------ | ------ |
| tensor | 输入 | 需要dump的Tensor。<br>•待dump的tensor位于UB/L1 Buffer/L0C Buffer时使用LocalTensor类型的tensor参数输入。<br>•待dump的tensor位于Global Memory时使用GlobalTensor类型的tensor参数输入。|
| desc | 输入 | 用户自定义附加信息（行号或其他自定义数字）。<br>在使用DumpTensor功能时，用户可通过desc参数附加自定义信息，以便在不同场景下区分Dump内容的来源。此功能有助于精准定位具体DumpTensor的输出，提升调试与分析效率。|
| dumpSize | 输入 | 需要dump的元素个数。|
| shapeInfo | 输入 | 传入Tensor的shape信息，可按照shape信息进行打印。<br>•当Shape尺寸大于dumpSize元素个数时，按照ShapeInfo打印元素，不足的Dump数据用"-"展示。<br>•当Shape尺寸小于等于dumpSize元素个数时，按照ShapeInfo打印元素，多出的Dump数据不展示。|

## 数据类型

<!-- npu="950" id101 -->
- Ascend 950PR/Ascend 950DT，T支持的数据类型为：bool、int8_t、uint8_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id101 -->
<!-- npu="A3" id102 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品，T支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id102 -->
<!-- npu="910b" id103 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品，T支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id103 -->
<!-- npu="310b" id104 -->
- Atlas 200I/500 A2 推理产品，T支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id104 -->
<!-- npu="310p" id105 -->
- Atlas 推理系列产品AI Core，T支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id105 -->
<!-- npu="x90" id107 -->
- Kirin X90，T支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id107 -->
<!-- npu="9030" id108 -->
- Kirin 9030，T支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。<!-- end id108 -->

## 约束说明<a name="section794123819592"></a>

- 当前支持打印存储位置为Unified Buffer（UB）/L1 Buffer/L0C Buffer/Global Memory的Tensor信息。
<!-- npu="950" id111 -->
- 在Ascend 950PR/Ascend 950DT下新增BiasTable Buffer和Fixpipe Buffer的Tensor数据打印。
<!-- end id111 -->
<!-- npu="950" id112 -->
- 针对Ascend 950PR/Ascend 950DT，使用该接口打印L1 Buffer、BiasTable Buffer或Fixpipe Buffer数据时，HDK版本需要至少升级到25.7.0以上。
<!-- end id112 -->
<!-- npu="950" id113 -->
- 针对Ascend 950PR/Ascend 950DT，打印Fixpipe Buffer中的Tensor信息场景：
  - Fixpipe Buffer保存的是硬件参数位域，打印结果不一定与L1 Buffer中的原始数据按位相同。前级Quant参数每8字节保留bit[7:0]、bit[31:13]和bit[46:37]，期望值为`input & 0x00007fe0ffffe0ffULL`；前级ReLU参数每4字节保留bit[31:13]，期望值为`word & 0xffffe000U`。
<!-- end id113 --> 
- 操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
- SIMD场景下，单次调用本接口打印的数据总量不可超过打印大小限制，默认为30KB。使用时应注意，如果超出这个限制，则数据不会被打印。您可以通过[aclInit接口](https://hiascend.com/document/redirect/CannCommunityruntimeapiaclinit)中的"simd\_printf\_fifo\_size\_per\_core"字段进行配置，配置范围最小为1KB，最大为64MB。使用时应注意，如果超出这个限制，则数据不会被打印。

## 调用示例<a name="section82241477610"></a>

- 无Tensor shape的打印

    ```cpp
    AscendC::DumpTensor(srcLocal, 5, dataLen);
    ```

- 带Tensor shape的打印

    - Shape等于dumpSize元素个数
        ```cpp
        uint32_t array[] = {static_cast<uint32_t>(8), static_cast<uint32_t>(8)};
        AscendC::ShapeInfo shapeInfo(2, array);       // dim为2， shape为(8,8)
        AscendC::DumpTensor(x, 2, 64, shapeInfo);     // dump x的64个元素，且解析按照shapeInfo的(8,8)排列
        ```
        打印结果如下：
        ```plain
        DumpTensor: desc=2, addr=0x0, data_type=float16, position=UB, dump_size=64
        [[2.048828,0.113037,4.042969,3.505859,4.554688,4.019531,0.598633,2.160156],
        [2.707031,0.117981,1.134766,4.835938,1.190430,3.085938,1.334961,0.406250],
        [2.658203,1.674805,3.791016,0.747070,3.541016,4.546875,0.394043,2.455078],
        [1.161133,2.775391,0.453857,2.857422,2.837891,1.052734,2.654297,1.828125],
        [0.358643,4.765625,3.681641,0.850098,2.250000,2.001953,0.446777,0.830078],
        [2.154297,4.781250,1.773438,0.201294,0.028412,3.285156,0.772949,3.261719],
        [0.532227,2.789062,0.588867,4.316406,0.146606,2.201172,3.775391,2.023438],
        [2.820312,2.835938,2.957031,2.398438,4.449219,0.516113,4.796875,0.786133]]
        ```

    - Shape小于dumpSize元素个数
        ```cpp
        uint32_t array1[] = {static_cast<uint32_t>(7), static_cast<uint32_t>(8)};
        AscendC::ShapeInfo shapeInfo1(2, array1); // dim为2， shape为(7,8)
        AscendC::DumpTensor(x1, 3, 64, shapeInfo1); // 当Shape尺寸小于等于dumpSize元素个数时， 按照ShapeInfo打印元素，多出的Dump数据不展示
        ```
        打印结果如下：
        ```plain
        DumpTensor: desc=3, addr=0x0, data_type=float16, position=UB, dump_size=64
        shape is [7, 8], dumpSize is 64, dumpSize is greater than shapeSize.
        [[2.048828,0.113037,4.042969,3.505859,4.554688,4.019531,0.598633,2.160156],
        [2.707031,0.117981,1.134766,4.835938,1.190430,3.085938,1.334961,0.406250],
        [2.658203,1.674805,3.791016,0.747070,3.541016,4.546875,0.394043,2.455078],
        [1.161133,2.775391,0.453857,2.857422,2.837891,1.052734,2.654297,1.828125],
        [0.358643,4.765625,3.681641,0.850098,2.250000,2.001953,0.446777,0.830078],
        [2.154297,4.781250,1.773438,0.201294,0.028412,3.285156,0.772949,3.261719],
        [0.532227,2.789062,0.588867,4.316406,0.146606,2.201172,3.775391,2.023438]]
        ```
    - Shape大于dumpSize元素个数
        ```cpp
        uint32_t array2[] = {static_cast<uint32_t>(9), static_cast<uint32_t>(8)};
        AscendC::ShapeInfo shapeInfo2(2, array2); // dim为2， shape为(9,8)
        AscendC::DumpTensor(x2, 4, 64, shapeInfo2); // 当Shape尺寸大于dumpSize元素个数时， 按照ShapeInfo打印元素，不足的Dump数据用"-"展示
        ```
        打印结果如下：
        ```plain
        DumpTensor: desc=4, addr=0x0, data_type=float16, position=UB, dump_size=64
        shape is [9, 8], dumpSize is 64, data is not enough.
        [[2.048828,0.113037,4.042969,3.505859,4.554688,4.019531,0.598633,2.160156],
        [2.707031,0.117981,1.134766,4.835938,1.190430,3.085938,1.334961,0.406250],
        [2.658203,1.674805,3.791016,0.747070,3.541016,4.546875,0.394043,2.455078],
        [1.161133,2.775391,0.453857,2.857422,2.837891,1.052734,2.654297,1.828125],
        [0.358643,4.765625,3.681641,0.850098,2.250000,2.001953,0.446777,0.830078],
        [2.154297,4.781250,1.773438,0.201294,0.028412,3.285156,0.772949,3.261719],
        [0.532227,2.789062,0.588867,4.316406,0.146606,2.201172,3.775391,2.023438],
        [2.820312,2.835938,2.957031,2.398438,4.449219,0.516113,4.796875,0.786133],
        [-,-,-,-,-,-,-,-]]
        ```
