# Reg矢量计算接口边界值汇总<a name="ZH-CN_TOPIC_0000002565891916"></a>

## 概述<a name="section3674181114910"></a>

本节汇总介绍了Reg矢量计算接口在边界值输入下的两类输出结果，以下内容针对如下型号生效：

- Ascend 950PR/Ascend 950DT：

Reg矢量计算接口在边界值输入下，有两类输出结果：

- INF/NAN模式（非饱和模式）

    计算结果是inf、-inf、nan的三类场景，按原样输出。

- 饱和模式

    通过接口[SetCtrlSpr](../SIMD-API/basic_api/special_register_access/SetCtrlSpr_ISASI.md)进行设置，饱和模式下：inf和-inf会被饱和为MAX和MIN进行输出，nan会被饱和为0进行输出。

> [!NOTE]说明
>
>- float数据类型输入仅支持INF/NAN模式（即使配置饱和模式，输出结果也与INF/NAN模式输出结果一致）；half、int16_t及int32_t类型输入，默认输出INF/NAN模式的结果。
>
>- **绝大部分场景下，INF/NAN模式的输出结果与IEEE754标准结果一致。如存在不一致的情况，会在对应接口处备注说明。**

## 基础算术

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

**表1** Exp特殊值/边界值输入的计算结果说明

<a name="table1088820617487"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | 0.0 | 0.0 |
| half | 65504.0（MAX） | inf | 65504.0 |
| half | 0.0 | 1.0 | 1.0 |
| half | -0.0 | 1.0 | 1.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | inf | inf |
| float | 0.0 | 1.0 | 1.0 |
| float | -0.0 | 1.0 | 1.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | 0.0 | 0.0 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表2** Ln特殊值/边界值输入的计算结果说明

<a name="table10972164142410"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | nan | 0.0 |
| half | 65504.0（MAX） | 11.094 | 11.094 |
| half | 0.0 | -inf | -65504.0 |
| half | -0.0 | -inf | -65504.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38（MIN） | nan | nan |
| float | 3.4028235e+38（MAX） | 88.723 | 88.723 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表3** Log特殊值/边界值输入的计算结果说明

<a name="table58051753113819"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | nan | 0.0 |
| half | 65504.0（MAX） | 11.094 | 11.094 |
| half | 0.0 | -inf | -65504.0 |
| half | -0.0 | -inf | -65504.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38（MIN） | nan | nan |
| float | 3.4028235e+38（MAX） | 88.723 | 88.723 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表4** Log2特殊值/边界值输入的计算结果说明

<a name="table1234181619537"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | nan | 0.0 |
| half | 65504.0（MAX） | 16 | 16 |
| half | 0.0 | -inf | -65504.0 |
| half | -0.0 | -inf | -65504.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38（MIN） | nan | nan |
| float | 3.4028235e+38（MAX） | 128 | 128 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表5** Log10特殊值/边界值输入的计算结果说明

<a name="table546135512543"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | nan | 0.0 |
| half | 65504.0（MAX） | 4.8164 | 4.8164 |
| half | 0.0 | -inf | -65504.0 |
| half | -0.0 | -inf | -65504.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38（MIN） | nan | nan |
| float | 3.4028235e+38（MAX） | 38.5318 | 38.53 |
| float | 0.0 | -inf | -inf |
| float | -0.0 | -inf | -inf |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表6** Neg特殊值/边界值输入的计算结果说明

<a name="table81271921555"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | 65504.0（MAX） | 65504.0（MAX） |
| half | 65504.0（MAX） | -65504.0（MIN） | -65504.0（MIN） |
| half | 0.0 | -0.0 | -0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | 0.0 |
| half | inf | -inf | -65504.0（MIN） |
| half | -inf | inf | 65504.0（MAX） |
| float | -3.4028235e+38（MIN） | 3.4028235e+38（MAX） | 3.4028235e+38（MAX） |
| float | 3.4028235e+38（MAX） | -3.4028235e+38（MIN） | -3.4028235e+38（MIN） |
| float | 0.0 | -0.0 | -0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | inf | -inf | -inf |
| float | -inf | inf | inf |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表7** Abs特殊值/边界值输入的计算结果说明

<a name="table1975185932512"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | 65504.0 | 65504.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | inf | 65504.0 |
| float | -3.4028235e+38（MIN） | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38（MAX） | 3.40282E+38 | 3.40282E+38 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | inf | inf |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表8** Sqrt特殊值/边界值输入的计算结果说明

<a name="table1648943062718"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | nan | 0.0 |
| half | 65504.0（MAX） | 256.0 | 256.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | -0.0 | -0.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | nan | 0.0 |
| float | -3.4028235e+38（MIN） | nan | nan |
| float | 3.4028235e+38（MAX） | 1.84467E+19 | 1.84467E+19 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | -0.0 | -0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表9** Relu特殊值/边界值输入的计算结果说明

<a name="table135522523284"></a>

| src/dst的数据类型 | src的元素取值 | dst的元素取值（INF/NAN模式） | dst的元素取值（饱和模式） |
| --- | --- | --- | --- |
| half | -65504.0（MIN） | 0.0 | 0.0 |
| half | 65504.0（MAX） | 65504.0 | 65504.0 |
| half | 0.0 | 0.0 | 0.0 |
| half | -0.0 | 0.0 | 0.0 |
| half | nan | nan | 0.0 |
| half | inf | inf | 65504.0 |
| half | -inf | 0.0 | 0.0 |
| float | -3.4028235e+38（MIN） | 0.0 | 0.0 |
| float | 3.4028235e+38（MAX） | 3.40282E+38 | 3.40282E+38 |
| float | 0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | inf | inf | inf |
| float | -inf | 0.0 | 0.0 |
| int32_t | -2147483648（MIN） | 0 | 0 |
| int32_t | 2147483647（MAX） | 2147483647 | 2147483647 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表10** Prelu特殊值/边界值输入的计算结果说明

<a name="table16370150182115"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | 65504.0（MAX） | MAX/MIN/inf/-inf/0.0/-0.0/nan | 65504.0 | 65504.0 |
| half | -65504.0（MIN） | MAX/inf | -inf | -65504.0 |
| half | -65504.0（MIN） | MIN/-inf | inf | 65504.0 |
| half | -65504.0（MIN） | 0.0 | -0.0 | -0.0 |
| half | -65504.0（MIN） | -0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | 0.0 | MAX/MIN/inf/-inf/0.0/-0.0/nan | 0.0 | 0.0 |
| half | -0.0 | MAX/0.0 | -0.0 | -0.0 |
| half | -0.0 | MIN/-0.0 | 0.0 | 0.0 |
| half | -0.0 | inf/-inf/nan | nan | 0.0 |
| half | nan | MAX/MIN/inf/-inf/0.0/-0.0/nan | nan | 0.0 |
| half | inf | MAX/MIN/inf/-inf/0.0/-0.0/nan | inf | 65504.0 |
| half | -inf | MAX/inf | -inf | -65504.0 |
| half | -inf | MIN/-inf | inf | 65504.0 |
| half | -inf | 0.0/-0.0/nan | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | MAX/inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | MIN/-inf | inf | inf |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | -0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | MAX/MIN/inf/-inf/0.0/-0.0/nan | 3.40282E+38 | 3.40282E+38 |
| float | 0.0 | MAX/MIN/inf/-inf/0.0/-0.0/nan | 0.0 | 0.0 |
| float | -0.0 | MAX/0.0 | -0.0 | -0.0 |
| float | -0.0 | MIN/-0.0 | 0.0 | 0.0 |
| float | -0.0 | inf/-inf/nan | nan | 0.0 |
| float | nan | MAX/MIN/inf/-inf/0.0/-0.0/nan | nan | nan |
| float | inf | MAX/MIN/inf/-inf/0.0/-0.0/nan | inf | inf |
| float | -inf | MAX/inf | -inf | -inf |
| float | -inf | MIN/-inf | inf | inf |
| float | -inf | 0.0/-0.0/nan | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表11** Add特殊值/边界值输入的计算结果说明

<a name="table071981613"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/-inf | -inf | -65504.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | 0.0 | 0.0 |
| half | -65504.0（MIN） | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf | inf | 65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX）/inf | inf | 65504.0 |
| half | 65504.0（MAX） | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf | inf | 65504.0 |
| half | 0.0 | -inf | -inf | -65504.0 |
| half | -0.0 | -0.0 | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf | inf | 65504.0 |
| half | -0.0 | -inf | -inf | -65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | nan | 0.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/-inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0/-0.0 | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0 | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0 | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | nan | nan |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | -32768 |
| int16_t | -32768（MIN） | 32767（MAX） | -1 | -1 |
| int16_t | 32767（MAX） | 32767（MAX） | -2 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | -2147483648 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | -1 | -1 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | -2 | 2147483647 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表12** Adds特殊值/边界值输入的计算结果说明

<a name="table1415167829"></a>

| src/dst的数据类型 | src0的元素取值 | scalar的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/-inf | -inf | -65504.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | 0.0 | 0.0 |
| half | -65504.0（MIN） | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf | inf | 65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX）/inf | inf | 65504.0 |
| half | 65504.0（MAX） | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf | inf | 65504.0 |
| half | 0.0 | -inf | -inf | -65504.0 |
| half | -0.0 | -0.0 | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf | inf | 65504.0 |
| half | -0.0 | -inf | -inf | -65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | nan | 0.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38 (MIN)/-inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38 (MAX) | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0/-0.0 | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38 (MAX)/inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0 | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0 | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | nan | nan |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | -32768 |
| int16_t | -32768（MIN） | 32767（MAX） | -1 | -1 |
| int16_t | 32767（MAX） | 32767（MAX） | -2 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | -2147483648 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | -1 | -1 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | -2 | 2147483647 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表13** AddC特殊值/边界值输入的计算结果说明

<a name="table145171041144910"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| uint32_t | 4294967295（MAX） | 4294967295（MAX） | 4294967295（MAX） | 4294967295（MAX） |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | 0 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | -1 | -1 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | -2 | -2 |


**表14** Sub特殊值/边界值输入的计算结果说明

<a name="table163421214360"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | 0.0 | 0.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | -inf | -65504.0 |
| half | -65504.0（MIN） | 0.0/-0.0 | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf | -inf | -65504.0 |
| half | -65504.0（MIN） | -inf | inf | 65504.0 |
| half | 65504.0（MAX） | -65504.0（MIN） | inf | 65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX） | 0.0 | 0.0 |
| half | 65504.0（MAX） | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | inf | -inf | -65504.0 |
| half | 65504.0（MAX） | -inf | inf | 65504.0 |
| half | 0.0/-0.0 | -65504.0（MIN） | 65504.0 | 65504.0 |
| half | 0.0/-0.0 | 65504.0（MAX） | -65504.0 | -65504.0 |
| half | 0.0/-0.0 | nan | nan | 0.0 |
| half | 0.0/-0.0 | inf | -inf | -65504.0 |
| half | 0.0/-0.0 | -inf | inf | 65504.0 |
| half | 0.0 | -0.0/0.0 | 0.0 | 0.0 |
| half | -0.0 | -0.0/0.0 | -0.0 | -0.0 |
| half | nan | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | nan | 0.0 |
| half | inf | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/-inf | inf | 65504.0 |
| half | inf | nan/inf | nan | 0.0 |
| half | -inf | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/inf | -inf | -65504.0 |
| half | -inf | nan/-inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN） | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX） | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0/-0.0 | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38（MIN） | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX） | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0 | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | -inf | -inf |
| float | 3.4028235e+38 (MAX) | -inf | inf | inf |
| float | 0.0/-0.0 | -3.4028235e+38（MIN） | 3.40282E+38 | 3.40282E+38 |
| float | 0.0/-0.0 | 3.4028235e+38（MAX） | -3.40282E+38 | -3.40282E+38 |
| float | 0.0/-0.0 | nan | nan | nan |
| float | 0.0/-0.0 | inf | -inf | -inf |
| float | 0.0/-0.0 | -inf | inf | inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | -0.0 | 0.0/-0.0 | -0.0 | -0.0 |
| float | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/-inf | inf | inf |
| float | inf | nan/inf | nan | nan |
| float | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/inf | -inf | -inf |
| float | -inf | nan/-inf | nan | nan |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | 0 |
| int16_t | -32768（MIN） | 32767（MAX） | 1 | -32768 |
| int16_t | 32767（MAX） | -32768（MIN） | -1 | 32767 |
| int16_t | 32767（MAX） | 32767（MAX） | 0 | 0 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | 0 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | 1 | -2147483648 |
| int32_t | 2147483647（MAX） | -2147483648（MIN） | -1 | 2147483647 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 0 | 0 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表15** SubC特殊值/边界值输入的计算结果说明

<a name="table425565151818"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| uint32_t | 4294967295（MAX） | 4294967295（MAX） | 0 | 0 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | 0 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | 1 | 1 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 0 | 0 |
| int32_t | 2147483647（MAX） | -2147483648（MIN） | -1 | -1 |


**表16** Mul特殊值/边界值输入的计算结果说明

<a name="table168220376319"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | inf | 65504.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | -inf | -65504.0 |
| half | -65504.0（MIN） | 0.0 | -0.0 | -0.0 |
| half | -65504.0（MIN） | -0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf | -inf | -65504.0 |
| half | -65504.0（MIN） | -inf | inf | 65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX） | inf | 65504.0 |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | -0.0 | -0.0 | -0.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | inf | inf | 65504.0 |
| half | 65504.0（MAX） | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan/inf/-inf | nan | 0.0 |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | 0.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | -inf | -65504.0 |
| half | -inf | -inf | inf | 65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN） | inf | inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX） | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX） | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | -0.0 | -0.0 | -0.0 |
| float | 0.0 | nan/inf/-inf | nan | nan |
| float | -0.0 | -0.0 | 0.0 | 0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | inf | inf |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | 32767 |
| int16_t | -32768（MIN） | 32767（MAX） | -32768 | -32768 |
| int16_t | 32767（MAX） | 32767（MAX） | 1 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | 2147483647 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | -2147483648 | -2147483648 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 1 | 2147483647 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表17** Mull特殊值/边界值输入的计算结果说明

<a name="table889992553813"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst0的元素取值 | dst1的元素取值 |
| --- | --- | --- | --- | --- |
| uint32_t | 4294967295（MAX） | 4294967295（MAX） | 1 | 4294967294 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | 1073741824 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | -2147483648（MIN） | -1073741824 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 1 | 1073741823 |


**表18** Div特殊值/边界值输入的计算结果说明

<a name="table1325925915217"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | 1 | 1 |
| half | -65504.0（MIN） | 65504.0（MAX） | -1 | -1 |
| half | -65504.0（MIN） | 0.0 | -inf | -65504.0 |
| half | -65504.0（MIN） | 0.0 | inf | 65504.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf | 0.0 | 0.0 |
| half | -65504.0（MIN） | -inf | 0.0 | 0.0 |
| half | 65504.0（MAX） | -65504.0（MIN） | -1 | -1 |
| half | 65504.0（MAX） | 65504.0（MAX） | 1 | 1 |
| half | 65504.0（MAX） | 0.0 | inf | 65504.0 |
| half | 65504.0（MAX） | 0.0 | -inf | -65504.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | inf | 0.0 | 0.0 |
| half | 65504.0（MAX） | -inf | 0.0 | 0.0 |
| half | 0.0 | -65504.0（MIN）/-inf | 0.0 | 0.0 |
| half | 0.0 | 65504.0（MAX）/inf | 0.0 | 0.0 |
| half | 0.0 | 0.0/-0.0/nan | nan | 0.0 |
| half | -0.0 | -65504.0（MIN）/-inf | 0.0 | 0.0 |
| half | -0.0 | 65504.0（MAX）/inf | 0.0 | 0.0 |
| half | -0.0 | 0.0/-0.0/nan | nan | 0.0 |
| half | nan | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | nan | 0.0 |
| half | inf | -65504.0（MIN）/-0.0 | -inf | -65504.0 |
| half | inf | 65504.0（MAX）/0.0 | inf | 65504.0 |
| half | inf | nan/inf/-inf | nan | 0.0 |
| half | -inf | -65504.0（MIN）/-0.0 | inf | 65504.0 |
| half | -inf | 65504.0（MAX）/0.0 | -inf | -65504.0 |
| half | -inf | nan/inf/-inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN） | 1 | 1 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX） | -1 | -1 |
| float | -3.4028235e+38 (MIN) | 0.0 | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | inf | inf |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | -inf | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38（MIN） | -1 | -1 |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX） | 1 | 1 |
| float | 3.4028235e+38 (MAX) | 0.0 | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0 | -inf | -inf |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | -inf | 0.0 | 0.0 |
| float | 0.0 | -3.4028235e+38（MIN）/-inf | 0.0 | 0.0 |
| float | 0.0 | 3.4028235e+38（MAX）/inf | 0.0 | 0.0 |
| float | 0.0 | 0.0/-0.0/nan | nan | nan |
| float | -0.0 | -3.4028235e+38（MIN）/-inf | 0.0 | 0.0 |
| float | -0.0 | 3.4028235e+38（MAX）/inf | 0.0 | 0.0 |
| float | -0.0 | 0.0/-0.0/nan | nan | nan |
| float | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38（MIN）/-0.0 | -inf | -inf |
| float | inf | 3.4028235e+38（MAX）/0.0 | inf | inf |
| float | inf | nan/inf/-inf | nan | nan |
| float | -inf | -3.4028235e+38（MIN）/-0.0 | inf | inf |
| float | -inf | 3.4028235e+38（MAX）/0.0 | -inf | -inf |
| float | -inf | nan/inf/-inf | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表19** Max特殊值/边界值输入的计算结果说明

<a name="table7822641411"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/-inf | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | 65504.0 | 65504.0 |
| half | -65504.0（MIN） | 0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | 0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf | inf | 65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX）/0.0/-0.0/-inf | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | inf | inf | 65504.0 |
| half | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf | inf | 65504.0 |
| half | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf | inf | 65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf/-inf | inf | 65504.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN）/-inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX） | 3.40282E+38 | 3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX）/0.0/-0.0/-inf | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | inf | inf |
| float | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf/-inf | inf | inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN） | -32768 | -32768 |
| int16_t | -32768（MIN） | 32767（MAX） | 32767 | 32767 |
| int16_t | 32767（MAX） | 32767（MAX） | 32767 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | -2147483648 | -2147483648 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | 2147483647 | 2147483647 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 2147483647 | 2147483647 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表20** Min特殊值/边界值输入的计算结果说明

<a name="table242810341242"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/inf | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | -inf | -inf | -65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX）/inf | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0/inf | 0.0 | 0.0 |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | -inf | -inf | -65504.0 |
| half | -0.0 | -0.0/inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | -inf | -inf | -65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | -inf | -65504.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -inf | -inf | -inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX）/inf | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/inf | 0.0 | 0.0 |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0/inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN）/32767（MAX） | -32768 | -32768 |
| int16_t | 32767（MAX） | 32767（MAX） | 32767 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN）/2147483647（MAX） | -2147483648 | -2147483648 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 2147483647 | 2147483647 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表21** Muls特殊值/边界值输入的计算结果说明

<a name="table8397142191119"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | inf | 65504.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | -inf | -65504.0 |
| half | -65504.0（MIN） | 0.0 | -0.0 | -0.0 |
| half | -65504.0（MIN） | -0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf | -inf | -65504.0 |
| half | -65504.0（MIN） | -inf | inf | 65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX） | inf | 65504.0 |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | -0.0 | -0.0 | -0.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | inf | inf | 65504.0 |
| half | 65504.0（MAX） | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | -0.0 | -0.0 | -0.0 |
| half | 0.0 | nan/inf/-inf | nan | 0.0 |
| half | -0.0 | -0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | 0.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | -inf | -65504.0 |
| half | -inf | -inf | inf | 65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN） | inf | inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX） | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | -0.0 | -0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX） | inf | inf |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | nan/inf/-inf | nan | nan |
| float | -0.0 | 0.0 | -0.0 | -0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | inf | inf |
| int16_t | -32768（MIN） | -32768（MIN） | 0 | 32767 |
| int16_t | -32768（MIN） | 32767（MAX） | -32768 | -32768 |
| int16_t | 32767（MAX） | 32767（MAX） | 1 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | 0 | 2147483647 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | -2147483648 | -2147483648 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 1 | 2147483647 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表22** Maxs特殊值/边界值输入的计算结果说明

<a name="table149066456147"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/-inf | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | 65504.0 | 65504.0 |
| half | -65504.0（MIN） | 0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | 0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf | inf | 65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX）/0.0/-0.0/-inf | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | inf | inf | 65504.0 |
| half | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf | inf | 65504.0 |
| half | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf | inf | 65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf/-inf | inf | 65504.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN）/-inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX） | 3.40282E+38 | 3.40282E+38 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX）/0.0/-0.0/-inf | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | inf | inf |
| float | 0.0 | 0.0/-0.0/-inf | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf | inf | inf |
| float | -0.0 | -0.0/-inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf/-inf | inf | inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN） | -32768 | -32768 |
| int16_t | -32768（MIN） | 32767（MAX） | 32767 | 32767 |
| int16_t | 32767（MAX） | 32767（MAX） | 32767 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN） | -2147483648 | -2147483648 |
| int32_t | -2147483648（MIN） | 2147483647（MAX） | 2147483647 | 2147483647 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 2147483647 | 2147483647 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表23** Mins特殊值/边界值输入的计算结果说明

<a name="table195381845171710"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/inf | -65504.0 | -65504.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | -inf | -inf | -65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX）/inf | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | 0.0 | 0.0 | 0.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | -inf | -inf | -65504.0 |
| half | 0.0 | 0.0/inf | 0.0 | 0.0 |
| half | 0.0 | 0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | -inf | -inf | -65504.0 |
| half | -0.0 | -0.0/inf | -0.0 | -0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | -inf | -inf | -65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | inf | 65504.0 |
| half | inf | -inf | -inf | -65504.0 |
| half | -inf | -inf | -inf | -65504.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/inf | -3.40282E+38 | -3.40282E+38 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -inf | -inf | -inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX）/inf | 3.4e+38 | 3.4e+38 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0 | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf | -inf | -inf |
| float | 0.0 | 0.0/inf | 0.0 | 0.0 |
| float | 0.0 | 0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | -inf | -inf | -inf |
| float | -0.0 | -0.0/inf | -0.0 | -0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | inf | inf |
| float | inf | -inf | -inf | -inf |
| float | -inf | -inf | -inf | -inf |
| int16_t | -32768（MIN） | -32768（MIN）/32767（MAX） | -32768 | -32768 |
| int16_t | 32767（MAX） | 32767（MAX） | 32767 | 32767 |
| int32_t | -2147483648（MIN） | -2147483648（MIN）/2147483647（MAX） | -2147483648 | -2147483648 |
| int32_t | 2147483647（MAX） | 2147483647（MAX） | 2147483647 | 2147483647 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表24** LeakyRelu特殊值/边界值输入的计算结果说明

<a name="table163011385209"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | inf | 65504.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | -inf | -65504.0 |
| half | -65504.0（MIN） | 0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | 0.0 | 0.0 | 0.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf | -inf | -65504.0 |
| half | -65504.0（MIN） | -inf | inf | 65504.0 |
| half | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | 65504.0 | 65504.0 |
| half | 0.0 | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | 0.0 | 0.0 |
| half | -0.0 | -65504.0（MIN）/-0.0 | 0.0 | 0.0 |
| half | -0.0 | 65504.0（MAX）/0.0 | 0.0 | 0.0 |
| half | -0.0 | nan/inf/-inf | nan | 0.0 |
| half | nan | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | nan | 0.0 |
| half | inf | -65504.0（MIN）/65504.0（MAX）/0.0/-0.0/nan/inf/-inf | inf | 65504.0 |
| half | -inf | -65504.0（MIN）/-inf | inf | 65504.0 |
| half | -inf | 65504.0（MAX）/inf | -inf | -65504.0 |
| half | -inf | 0.0/-0.0/nan | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN） | inf | inf |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX） | -inf | -inf |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 0.0 | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf | -inf | -inf |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | 3.4e+38 | 3.4e+38 |
| float | 0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | 0.0 | 0.0 |
| float | -0.0 | -3.4028235e+38（MIN）/-0.0 | 0.0 | 0.0 |
| float | -0.0 | 3.4028235e+38（MAX）/0.0 | 0.0 | 0.0 |
| float | -0.0 | nan/inf/-inf | nan | nan |
| float | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | nan | nan |
| float | inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/0.0/-0.0/nan/inf/-inf | inf | inf |
| float | -inf | -3.4028235e+38（MIN）/-inf | inf | inf |
| float | -inf | 3.4028235e+38（MAX）/inf | -inf | -inf |
| float | -inf | 0.0/-0.0/nan | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

## 复合计算

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

**表25** Axpy特殊值/边界值输入的计算结果说明

<a name="table1819218568247"></a>

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

| src/dst的数据类型 | src取值/scalarValue取值 | dst取值 | dst取值<br>（INF/NAN模式） | dst取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +0.0 | +0 | +0.0 |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | -0.0 | +0.0 | +0 |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | +inf | +inf | MAX |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | -inf | -inf | MIN |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | MAX | MAX | MAX |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | MIN | MIN | MIN |
| half | +0/+0<br>-0/-0<br>+0/MAX<br>-0/MIN | nan | nan | +0.0 |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | +0.0 | +0 | +0.0 |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | -0.0 | -0.0 | -0.0 |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | +inf | +inf | MAX |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | -inf | -inf | MIN |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | MAX | MAX | MAX |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | MIN | MIN | MIN |
| half | +0/MIN<br>-0/MAX<br>+0/-0<br>-0/+0 | nan | nan | +0.0 |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | +0.0 | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | -0.0 | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | +inf | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | -inf | nan | +0.0 |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | MAX | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | MIN | +inf | MAX |
| half | MAX/+inf<br>MIN/-inf<br>+inf/+inf<br>-inf/-inf | nan | nan | +0.0 |
| half | MAX/MAX<br>MIN/MIN | +0.0 | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | -0.0 | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | +inf | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | -inf | -inf | MIN |
| half | MAX/MAX<br>MIN/MIN | MAX | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | MIN | +inf | MAX |
| half | MAX/MAX<br>MIN/MIN | nan | nan | +0.0 |
| half | MAX/MIN<br>MIN/MAX | +0.0 | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | -0.0 | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | +inf | +inf | MAX |
| half | MAX/MIN<br>MIN/MAX | -inf | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | MAX | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | MIN | -inf | MIN |
| half | MAX/MIN<br>MIN/MAX | nan | nan | +0.0 |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | +0.0 | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | -0.0 | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | +inf | nan | +0.0 |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | -inf | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | MAX | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | MIN | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf<br>+inf/-inf | nan | nan | +0.0 |
| half | nan/任意值<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | +0.0 | nan | +0.0 |
| half | nan/任意值<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | -0.0 | nan | +0.0 |
| half | nan/任意值<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | +inf | nan | +0.0 |
| half | nan/任意值<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | -inf | nan | +0.0 |
| half | nan/任意值<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | MAX | nan | +0.0 |
| half | nan/任意值<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | MIN | nan | +0.0 |
| half | nan/任意值<br>+0/+inf<br>+0/-inf<br>+0/nan<br>-0/+inf<br>-0/-inf<br>-0/nan | nan | nan | +0.0 |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | +0.0 | +0 | +0.0 |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | -0.0 | +0.0 | +0 |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | +inf | +inf | +inf |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | -inf | -inf | -inf |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | MAX | MAX | MAX |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | MIN | MIN | MIN |
| float | -0/MIN<br>+0/+0<br>+0/MAX<br>-0/-0 | nan | nan | nan |
| float | -0/MAX<br>+0/-0<br>+0/MIN | +0.0 | +0 | +0.0 |
| float | -0/MAX<br>+0/-0<br>+0/MIN | -0.0 | -0.0 | -0.0 |
| float | -0/MAX<br>+0/-0<br>+0/MIN | +inf | +inf | +inf |
| float | -0/MAX<br>+0/-0<br>+0/MIN | -inf | -inf | -inf |
| float | -0/MAX<br>+0/-0<br>+0/MIN | MAX | MAX | MAX |
| float | -0/MAX<br>+0/-0<br>+0/MIN | MIN | MIN | MIN |
| float | -0/MAX<br>+0/-0<br>+0/MIN | nan | nan | nan |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | +0.0 | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | -0.0 | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | +inf | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | -inf | nan | nan |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | MAX | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | MIN | +inf | +inf |
| float | +inf/+inf<br>+inf/MAX<br>-inf/MIN<br>-inf/-inf | nan | nan | nan |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | +0.0 | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | -0.0 | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | +inf | nan | nan |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | -inf | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | MAX | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | MIN | -inf | -inf |
| float | +inf/-inf<br>+inf/MIN<br>-inf/MAX | nan | nan | nan |
| float | MAX/MAX<br>MIN/MIN | +0.0 | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | -0.0 | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | +inf | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | -inf | -inf | -inf |
| float | MAX/MAX<br>MIN/MIN | MAX | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | MIN | +inf | +inf |
| float | MAX/MAX<br>MIN/MIN | nan | nan | nan |
| float | MAX/MIN | +0.0 | -inf | -inf |
| float | MAX/MIN | -0.0 | -inf | -inf |
| float | MAX/MIN | +inf | +inf | +inf |
| float | MAX/MIN | -inf | -inf | -inf |
| float | MAX/MIN | MAX | -inf | -inf |
| float | MAX/MIN | MIN | -inf | -inf |
| float | MAX/MIN | nan | nan | nan |
| float | nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | +0.0 | nan | nan |
| float | nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | -0.0 | nan | nan |
| float | nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | +inf | nan | nan |
| float | nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | -inf | nan | nan |
| float | nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | MAX | nan | nan |
| float | nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | MIN | nan | nan |
| float | nan/任意值<br>+0/+inf<br>+0/-inf<br>-0/+inf<br>-0/-inf | nan | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表26** AbsSub特殊值/边界值输入的计算结果说明

<a name="table13182948124817"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half | -65504.0（MIN） | -65504.0（MIN） | 0.0 | 0.0 |
| half | -65504.0（MIN） | 65504.0（MAX） | inf | 65504.0 |
| half | -65504.0（MIN） | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | -65504.0（MIN） | nan | nan | 0.0 |
| half | -65504.0（MIN） | inf/-inf | inf | 65504.0 |
| half | 65504.0（MAX） | 65504.0（MAX） | 0.0 | 0.0 |
| half | 65504.0（MAX） | 0.0/-0.0 | 65504.0 | 65504.0 |
| half | 65504.0（MAX） | nan | nan | 0.0 |
| half | 65504.0（MAX） | -inf/inf | inf | 65504.0 |
| half | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | 0.0 | nan | nan | 0.0 |
| half | 0.0 | inf/-inf | inf | 65504.0 |
| half | -0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| half | -0.0 | nan | nan | 0.0 |
| half | -0.0 | inf/-inf | inf | 65504.0 |
| half | nan | nan/inf/-inf | nan | 0.0 |
| half | inf | inf | nan | 0.0 |
| half | inf | -inf | inf | 65504.0 |
| half | -inf | -inf | nan | 0.0 |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN） | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX） | inf | inf |
| float | -3.4028235e+38 (MIN) | 0.0/-0.0 | MAX | MAX |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | inf/-inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX） | 0.0 | 0.0 |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0 | 3.40282E+38 | 3.40282E+38 |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | -inf/inf | inf | inf |
| float | 0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | 0.0 | nan | nan | nan |
| float | 0.0 | inf/-inf | inf | inf |
| float | -0.0 | 0.0/-0.0 | 0.0 | 0.0 |
| float | -0.0 | nan | nan | nan |
| float | -0.0 | inf | inf | inf |
| float | -0.0 | -inf | -inf | -inf |
| float | nan | nan/inf/-inf | nan | nan |
| float | inf | inf | nan | nan |
| float | inf | -inf | inf | inf |
| float | -inf | -inf | nan | nan |
| int64_t | -9223372036854775808（MIN） | -9223372036854775808（MIN） | 0 | 0 |
| int64_t | -9223372036854775808（MIN） | 9223372036854775808（MAX） | 1 | 1 |
| int64_t | 9223372036854775808（MAX） | 9223372036854775808（MAX） | 0 | 0 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表27** ExpSub特殊值/边界值输入的计算结果说明

<a name="table9246161118410"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| half/float | -65504.0（MIN） | -65504.0（MIN） | 1 | 1 |
| half/float | -65504.0（MIN） | 65504.0（MAX）/0.0/-0.0/inf | 0.0 | 0.0 |
| half/float | -65504.0（MIN） | nan | nan | nan |
| half/float | -65504.0（MIN） | -inf | -inf | -inf |
| half/float | 65504.0（MAX） | 65504.0（MAX） | 1 | 1 |
| half/float | 65504.0（MAX） | 0.0/-0.0/-3.4028235e+38（MIN）/-inf | inf | inf |
| half/float | 65504.0（MAX） | nan | nan | nan |
| half/float | 65504.0（MAX） | inf | 0.0 | 0.0 |
| half/float | 0.0/-0.0 | 3.4028235e+38（MAX）/inf | 0.0 | 0.0 |
| half/float | 0.0/-0.0 | 0.0/-0.0 | 1 | 1 |
| half/float | 0.0/-0.0 | nan | nan | nan |
| half/float | 0.0/-0.0 | -3.4028235e+38（MIN）/-inf | inf | inf |
| half/float | nan | nan/inf/-inf/0/-0/max/min | nan | nan |
| half/float | inf | -inf/0/-0/max/min | inf | inf |
| half/float | inf | nan/inf | nan | nan |
| half/float | -inf | inf/-inf/0/-0/max/min | 0.0 | 0.0 |
| half/float | -inf | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -3.4028235e+38（MIN） | 1 | 1 |
| float | -3.4028235e+38 (MIN) | 3.4028235e+38（MAX）/0.0/-0.0/inf | 0.0 | 0.0 |
| float | -3.4028235e+38 (MIN) | nan | nan | nan |
| float | -3.4028235e+38 (MIN) | -inf | inf | inf |
| float | 3.4028235e+38 (MAX) | 3.4028235e+38（MAX） | 1 | 1 |
| float | 3.4028235e+38 (MAX) | 0.0/-0.0/-3.4028235e+38（MIN）/-inf | inf | inf |
| float | 3.4028235e+38 (MAX) | nan | nan | nan |
| float | 3.4028235e+38 (MAX) | inf | 0.0 | 0.0 |
| float | 0.0/-0.0 | 3.4028235e+38（MAX）/inf | 0.0 | 0.0 |
| float | 0.0/-0.0 | 0.0/-0.0 | 1 | 1 |
| float | 0.0/-0.0 | nan | nan | nan |
| float | 0.0/-0.0 | -3.4028235e+38（MIN）/-inf | inf | inf |
| float | nan | nan/inf/-inf/0/-0/max/min | nan | nan |
| float | inf | inf/nan | nan | nan |
| float | inf | -inf/0/-0/max/min | 0.0 | 0.0 |
| float | -inf | inf/-inf/0/-0/max/min | 0.0 | 0.0 |
| float | -inf | nan | nan | nan |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表28** MulDstAdd特殊值/边界值输入的计算结果说明

<a name="table0741155811341"></a>

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

| src/dst的数据类型 | src0取值/src1取值 | dst取值 | dst取值<br>（INF/NAN模式） | dst取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| float | +0/+0<br>-0/+0 | +0/-0/MAX/MIN | +0.0 | +0 |
| float | +0/+0<br>-0/+0 | nan/+inf/-inf | nan | nan |
| float | +0/-0 | +0/MAX | +0.0 | +0 |
| float | +0/-0 | -0/MIN | -0.0 | -0.0 |
| float | +0/-0 | nan/+inf/-inf | nan | nan |
| float | -0/-0 | +0/MAX | -0.0 | -0.0 |
| float | -0/-0 | -0/MIN | +0.0 | +0 |
| float | -0/-0 | nan/+inf/-inf | nan | nan |
| float | -0/MAX<br>+0/MAX | +0/-0/MAX/MIN | MAX | MAX |
| float | -0/MAX<br>+0/MAX | nan/+inf/-inf | nan | nan |
| float | -0/MIN<br>+0/MIN | +0/-0/MAX/MIN | MIN | MIN |
| float | -0/MIN<br>+0/MIN | nan/+inf/-inf | nan | nan |
| float | +0/inf<br>-0/inf | +0/-0/MAX/MIN | inf | inf |
| float | +0/inf<br>-0/inf | nan/+inf/-inf | nan | nan |
| float | +0/-inf<br>-0/-inf | +0/-0/MAX/MIN | -inf | -inf |
| float | +0/-inf<br>-0/-inf | nan/+inf/-inf | nan | nan |
| float | MAX/+inf | +0/-0/MAX/MIN/+inf | inf | inf |
| float | MAX/+inf | -inf/nan | nan | nan |
| float | MIN/+inf | +0/-0/MAX/MIN/-inf | inf | inf |
| float | MIN/+inf | +inf/nan | nan | nan |
| float | MAX/-inf | +0/-0/MAX/MIN/-inf | -inf | -inf |
| float | MAX/-inf | +inf/nan | nan | nan |
| float | MIN/-inf | +0/-0/MAX/MIN/+inf | -inf | -inf |
| float | MIN/-inf | -inf/nan | nan | nan |
| float | MAX/+0 | MAX/+inf | inf | inf |
| float | MAX/+0 | +0/-0 | +0.0 | +0 |
| float | MAX/+0 | MIN/-inf | -inf | -inf |
| float | MAX/+0 | nan | nan | nan |
| float | MIN/+0 | MAX/+inf | -inf | -inf |
| float | MIN/+0 | +0/-0 | +0.0 | +0 |
| float | MIN/+0 | MIN/-inf | inf | inf |
| float | MIN/+0 | nan | nan | nan |
| float | MAX/-0 | MAX/+inf | inf | inf |
| float | MAX/-0 | MIN/-inf | -inf | -inf |
| float | MAX/-0 | +0.0 | +0 | +0.0 |
| float | MAX/-0 | -0.0 | -0.0 | -0.0 |
| float | MAX/-0 | nan | nan | nan |
| float | MIN/-0 | MAX/+inf | inf | inf |
| float | MIN/-0 | MIN/-inf | -inf | -inf |
| float | MIN/-0 | +0.0 | -0.0 | -0.0 |
| float | MIN/-0 | -0.0 | +0.0 | +0 |
| float | MIN/-0 | nan | nan | nan |
| float | MAX/MAX | MAX/+inf | inf | inf |
| float | MAX/MAX | MIN/-inf | -inf | -inf |
| float | MAX/MAX | +0/-0 | MAX | MAX |
| float | MAX/MAX | nan | nan | nan |
| float | MIN/MIN | MAX/+inf | -inf | -inf |
| float | MIN/MIN | MIN/-inf | inf | inf |
| float | MIN/MIN | +0/-0 | MAX | MAX |
| float | MIN/MIN | nan | nan | nan |
| float | MAX/MIN | MAX/+inf | inf | inf |
| float | MAX/MIN | MIN/-inf | -inf | -inf |
| float | MAX/MIN | +0/-0 | MIN | MIN |
| float | MAX/MIN | nan | nan | nan |
| float | MIN/MAX | MAX/+inf | -inf | -inf |
| float | MIN/MAX | MIN/-inf | inf | inf |
| float | MIN/MAX | +0/-0 | MAX | MAX |
| float | MIN/MAX | nan | nan | nan |
| float | inf/inf | MAX/+inf | inf | inf |
| float | inf/inf | MIN/-inf/0/-0/-inf/nan | nan | nan |
| float | -inf/inf | MIN/-inf | inf | inf |
| float | -inf/inf | MAX/+inf/0/-0/-inf/nan | nan | nan |
| float | inf/-inf | MIN/-inf | -inf | -inf |
| float | inf/-inf | MAX/+inf/0/-0/-inf/nan | nan | nan |
| float | -inf/-inf | MAX/+inf | -inf | -inf |
| float | -inf/-inf | MIN/-inf/0/-0/-inf/nan | nan | nan |
| float | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | MAX/+inf | inf | inf |
| float | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | MIN/-inf | -inf | -inf |
| float | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | nan/+0/-0 | nan | nan |
| float | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | MAX/+inf | -inf | -inf |
| float | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | MIN/-inf | inf | inf |
| float | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | nan/+0/-0 | nan | nan |
| float | nan/任意值 | +0/-0/MAX/MIN/nan/+inf/-inf | nan | nan |
| half | +0/+0<br>-0/+0 | +0/-0/MAX/MIN | +0.0 | +0 |
| half | +0/+0<br>-0/+0 | nan/+inf/-inf | nan | +0.0 |
| half | +0/-0 | +0/MAX | +0.0 | +0 |
| half | +0/-0 | -0/MIN | -0.0 | -0.0 |
| half | +0/-0 | nan/+inf/-inf | nan | +0.0 |
| half | -0/-0 | +0/MAX | -0.0 | -0.0 |
| half | -0/-0 | -0/MIN | +0.0 | +0 |
| half | -0/-0 | nan/+inf/-inf | nan | +0.0 |
| half | -0/MAX<br>+0/MAX | +0/-0/MAX/MIN | MAX | MAX |
| half | -0/MAX<br>+0/MAX | nan/+inf/-inf | nan | +0.0 |
| half | -0/MIN<br>+0/MIN | +0/-0/MAX/MIN | MIN | MIN |
| half | -0/MIN<br>+0/MIN | nan/+inf/-inf | nan | +0.0 |
| half | +0/inf<br>-0/inf | +0/-0/MAX/MIN | inf | MAX |
| half | +0/inf<br>-0/inf | nan/+inf/-inf | nan | +0.0 |
| half | +0/-inf<br>-0/-inf | +0/-0/MAX/MIN | -inf | MIN |
| half | +0/-inf<br>-0/-inf | nan/+inf/-inf | nan | +0.0 |
| half | MAX/+inf | +0/-0/MAX/MIN/+inf | inf | MAX |
| half | MAX/+inf | -inf/nan | nan | +0.0 |
| half | MIN/+inf | +0/-0/MAX/MIN/-inf | inf | MAX |
| half | MIN/+inf | +inf/nan | nan | +0.0 |
| half | MAX/-inf | +0/-0/MAX/MIN/-inf | -inf | MIN |
| half | MAX/-inf | +inf/nan | nan | +0.0 |
| half | MIN/-inf | +0/-0/MAX/MIN/+inf | -inf | MIN |
| half | MIN/-inf | -inf/nan | nan | +0.0 |
| half | MAX/+0 | MAX/+inf | inf | MAX |
| half | MAX/+0 | +0/-0 | +0.0 | +0 |
| half | MAX/+0 | MIN/-inf | -inf | MIN |
| half | MAX/+0 | nan | nan | +0.0 |
| half | MIN/+0 | MAX/+inf | -inf | MIN |
| half | MIN/+0 | +0/-0 | +0.0 | +0 |
| half | MIN/+0 | MIN/-inf | inf | MAX |
| half | MIN/+0 | nan | nan | +0.0 |
| half | MAX/-0 | MAX/+inf | inf | MAX |
| half | MAX/-0 | MIN/-inf | -inf | MIN |
| half | MAX/-0 | +0.0 | +0 | +0.0 |
| half | MAX/-0 | -0.0 | -0.0 | -0.0 |
| half | MAX/-0 | nan | nan | +0.0 |
| half | MIN/-0 | MAX/+inf | inf | MAX |
| half | MIN/-0 | MIN/-inf | -inf | MIN |
| half | MIN/-0 | +0.0 | -0.0 | -0.0 |
| half | MIN/-0 | -0.0 | +0.0 | +0 |
| half | MIN/-0 | nan | nan | +0.0 |
| half | MAX/MAX | MAX/+inf | inf | MAX |
| half | MAX/MAX | MIN/-inf | -inf | MIN |
| half | MAX/MAX | +0/-0 | MAX | MAX |
| half | MAX/MAX | nan | nan | +0.0 |
| half | MIN/MIN | MAX/+inf | -inf | MIN |
| half | MIN/MIN | MIN/-inf | inf | MAX |
| half | MIN/MIN | +0/-0 | MAX | MAX |
| half | MIN/MIN | nan | nan | +0.0 |
| half | MAX/MIN | MAX/+inf | inf | MAX |
| half | MAX/MIN | MIN/-inf | -inf | MIN |
| half | MAX/MIN | +0/-0 | MIN | MIN |
| half | MAX/MIN | nan | nan | +0.0 |
| half | MIN/MAX | MAX/+inf | -inf | MIN |
| half | MIN/MAX | MIN/-inf | inf | MAX |
| half | MIN/MAX | +0/-0 | MAX | MAX |
| half | MIN/MAX | nan | nan | +0.0 |
| half | inf/inf | MAX/+inf | inf | MAX |
| half | inf/inf | MIN/-inf/0/-0/-inf/nan | nan | +0.0 |
| half | -inf/inf | MIN/-inf | inf | MAX |
| half | -inf/inf | MAX/+inf/0/-0/-inf/nan | nan | +0.0 |
| half | inf/-inf | MIN/-inf | -inf | MIN |
| half | inf/-inf | MAX/+inf/0/-0/-inf/nan | nan | +0.0 |
| half | -inf/-inf | MAX/+inf | -inf | MIN |
| half | -inf/-inf | MIN/-inf/0/-0/-inf/nan | nan | +0.0 |
| half | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | MAX/+inf | inf | MAX |
| half | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | MIN/-inf | -inf | MIN |
| half | inf/+0<br>inf/-0<br>inf/MAX<br>inf/MIN | nan/+0/-0 | nan | +0.0 |
| half | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | MAX/+inf | -inf | MIN |
| half | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | MIN/-inf | inf | MAX |
| half | -inf/+0<br>-inf/-0<br>-inf/MAX<br>-inf/MIN | nan/+0/-0 | nan | +0.0 |
| half | nan/任意值 | +0/-0/MAX/MIN/nan/+inf/-inf | nan | +0.0 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表29** MulAddDst特殊值/边界值输入的计算结果说明

<a name="table31201353184118"></a>

注：下表中的MAX/MIN对应操作数对应数据类型下的最大值和最小值。

| src/dst的数据类型 | src0取值/src1取值 | dst取值 | dst取值<br>（INF/NAN模式） | dst取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | MAX | MAX | MAX |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | MIN | MIN | MIN |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | nan | nan | nan |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | -0/+0 | 0.0 | 0.0 |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | +inf | inf | inf |
| float | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | -inf | -inf | -inf |
| float | +0/inf    -0/-inf<br>+0/-inf   -0/inf<br>+0/nan   -0/nan | +0/MAX/-0/MIN/nan/+inf/-inf | nan | nan |
| float | MAX/MAX<br>MIN/MIN | +0/MAX/-0/MIN/inf | inf | inf |
| float | MAX/MAX<br>MIN/MIN | -inf | -inf | -inf |
| float | MAX/MAX<br>MIN/MIN | nan | nan | nan |
| float | MAX/MIN | +0/MAX/-0/MIN/-inf | -inf | -inf |
| float | MAX/MIN | inf | inf | inf |
| float | MAX/MIN | nan | nan | nan |
| float | MAX/+inf<br>MIN/-inf | +0/-0/MAX/MIN/+inf | inf | inf |
| float | MAX/+inf<br>MIN/-inf | -inf/nan | nan | nan |
| float | MAX/-inf<br>MIN/+inf | +0/-0/MAX/MIN/-inf | -inf | -inf |
| float | MAX/-inf<br>MIN/+inf | +inf/nan | nan | nan |
| float | inf/inf | +0/-0/MAX/MIN/+inf | inf | inf |
| float | inf/inf | nan/-inf | nan | nan |
| float | inf/-inf | +0/-0/MAX/MIN/-inf | -inf | -inf |
| float | inf/-inf | nan/+inf | nan | nan |
| float | -inf/-inf | +0/-0/MAX/MIN/+inf | -inf | -inf |
| float | -inf/-inf | nan/-inf | nan | nan |
| float | nan/任意值 | +0/-0/MAX/MIN/nan/+inf/-inf | nan | nan |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | MAX | MAX | MAX |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | MIN | MIN | MIN |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | nan | nan | 0.0 |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | -0/+0 | 0.0 | 0.0 |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | +inf | inf | MAX |
| half | +0/+0<br>+0/-0<br>+0/MAX<br>+0/MIN<br>-0/-0<br>-0/MAX<br>-0/MIN | -inf | -inf | MIN |
| half | +0/inf    -0/-inf<br>+0/-inf   -0/inf<br>+0/nan   -0/nan | +0/MAX/-0/MIN/nan/+inf/-inf | nan | 0.0 |
| half | MAX/MAX<br>MIN/MIN | +0/MAX/-0/MIN/inf | inf | MAX |
| half | MAX/MAX<br>MIN/MIN | -inf | -inf | MIN |
| half | MAX/MAX<br>MIN/MIN | nan | nan | 0.0 |
| half | MAX/MIN | +0/MAX/-0/MIN/-inf | -inf | MIN |
| half | MAX/MIN | inf | inf | MAX |
| half | MAX/MIN | nan | nan | 0.0 |
| half | MAX/+inf<br>MIN/-inf | +0/-0/MAX/MIN/+inf | inf | MAX |
| half | MAX/+inf<br>MIN/-inf | -inf/nan | nan | 0.0 |
| half | MAX/-inf<br>MIN/+inf | +0/-0/MAX/MIN/-inf | -inf | MIN |
| half | MAX/-inf<br>MIN/+inf | +inf/nan | nan | 0.0 |
| half | inf/inf | +0/-0/MAX/MIN/+inf | inf | MAX |
| half | inf/inf | nan/-inf | nan | 0.0 |
| half | inf/-inf | +0/-0/MAX/MIN/-inf | -inf | MIN |
| half | inf/-inf | nan/+inf | nan | 0.0 |
| half | -inf/-inf | +0/-0/MAX/MIN/+inf | -inf | MIN |
| half | -inf/-inf | nan/-inf | nan | 0.0 |
| half | nan/任意值 | +0/-0/MAX/MIN/nan/+inf/-inf | nan | 0.0 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表30** MulsCast特殊值/边界值输入的计算结果说明

<a name="table122760369423"></a>

| src/dst的数据类型 | src0的元素取值 | src1的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| float/half | MIN | nan | nan | 0.0 |
| float/half | MIN | -0.0 | 0.0 | 0.0 |
| float/half | MIN | 0.0 | -0.0 | -0.0 |
| float/half | MIN | -inf/MIN | inf | MAX |
| float/half | MIN | inf/MAX | -inf | MIN |
| float/half | MAX | nan | nan | 0.0 |
| float/half | MAX | -0.0 | -0.0 | -0.0 |
| float/half | MAX | 0.0 | 0.0 | 0.0 |
| float/half | MAX | -inf/MIN | -inf | MIN |
| float/half | MAX | inf/MAX | inf | MAX |
| float/half | 0.0 | 0.0 | 0.0 | 0.0 |
| float/half | 0.0 | inf/-inf/nan | nan | 0.0 |
| float/half | 0.0 | -0.0 | -0.0 | -0.0 |
| float/half | -0.0 | 0.0 | -0.0 | -0.0 |
| float/half | -0.0 | inf/-inf/nan | nan | 0.0 |
| float/half | -0.0 | -0.0 | 0.0 | 0.0 |
| float/half | nan | nan/inf/-inf/0/-0/max/min | nan | 0.0 |
| float/half | inf | -inf/0/-0/max/min | inf | MAX |
| float/half | inf | nan/inf | nan | 0.0 |
| float/half | -inf | -inf/MIN | inf | MAX |
| float/half | -inf | inf/MAX | -inf | MIN |
| float/half | -inf | 0/-0/nan | nan | 0.0 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

## 比较与选择

**表31** Compare特殊值/边界值输入的计算结果

<a name="table1961220798"></a>

| src数据类型/src1数据类型 | cmpMode比较模式 | src0数据类型 | src1数据类型 | dst输出数据（INF/NAN模式） | dst输出数据（饱和模式） |
| --- | --- | --- | --- | --- | --- |
| half/half | CMPMODE::EQ | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::EQ | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GE | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::GE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GT | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GT | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GT | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LE | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::LT | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LT | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LT | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::NE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/1/1/1 | 0/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/1/1/1 | 1/0/1/1/1/1/1 |
| half/half | CMPMODE::NE | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/1 | 1/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/0/1 | 1/1/1/1/1/0/1 |
| half/half | CMPMODE::NE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/0 | 1/1/1/1/1/1/0 |
| float/float | CMPMODE::EQ | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::EQ | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GE | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::GE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GT | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GT | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GT | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LE | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::LT | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LT | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LT | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::NE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/1/1/1 | 0/1/1/1/1/1/1 |
| float/float | CMPMODE::NE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/1/1/1 | 1/0/1/1/1/1/1 |
| float/float | CMPMODE::NE | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| float/float | CMPMODE::NE | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| float/float | CMPMODE::NE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/1 | 1/1/1/1/1/1/1 |
| float/float | CMPMODE::NE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/0/1 | 1/1/1/1/1/0/1 |
| float/float | CMPMODE::NE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/0 | 1/1/1/1/1/1/0 |
| int32_t/int32_t | CMPMODE::EQ | -2147483648（MIN） | -2147483648（MIN）/2147483647（MAX） | 1/0 | 1/0 |
| int32_t/int32_t | CMPMODE::EQ | 2147483647（MAX） | -2147483648（MIN）/2147483647（MAX） | 0/1 | 0/1 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表32** Compares特殊值/边界值输入的计算结果

<a name="table43881347182311"></a>

| src数据类型/src1数据类型 | cmpMode比较模式 | src0数据类型 | src1数据类型 | dst输出数据（INF/NAN模式） | dst输出数据（饱和模式） |
| --- | --- | --- | --- | --- | --- |
| half/half | CMPMODE::EQ | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| half/half | CMPMODE::EQ | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::EQ | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::EQ | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GE | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::GE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| half/half | CMPMODE::GT | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| half/half | CMPMODE::GT | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::GT | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| half/half | CMPMODE::GT | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LE | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| half/half | CMPMODE::LT | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| half/half | CMPMODE::LT | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| half/half | CMPMODE::LT | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| half/half | CMPMODE::LT | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| half/half | CMPMODE::LT | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| half/half | CMPMODE::NE | -65504.0（MIN） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/1/1/1 | 0/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | 65504.0（MAX） | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/1/1/1 | 1/0/1/1/1/1/1 |
| half/half | CMPMODE::NE | +0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | -0.0 | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| half/half | CMPMODE::NE | nan | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/1 | 1/1/1/1/1/1/1 |
| half/half | CMPMODE::NE | +inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/0/1 | 1/1/1/1/1/0/1 |
| half/half | CMPMODE::NE | -inf | -65504.0（MIN）/65504.0（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/0 | 1/1/1/1/1/1/0 |
| float/float | CMPMODE::EQ | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/0 | 1/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/0/0 | 0/1/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/1/1/0/0/0 | 0/0/1/1/0/0/0 |
| float/float | CMPMODE::EQ | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::EQ | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::EQ | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GE | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::GE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/1 | 0/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/0/0/1 | 1/0/1/1/0/0/1 |
| float/float | CMPMODE::GT | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/0/0/0/0/1 | 1/0/0/0/0/0/1 |
| float/float | CMPMODE::GT | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::GT | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/0/1 | 1/1/1/1/0/0/1 |
| float/float | CMPMODE::GT | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LE | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/1 | 1/1/1/1/0/1/1 |
| float/float | CMPMODE::LT | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/0/1/0 | 0/1/1/1/0/1/0 |
| float/float | CMPMODE::LT | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/1/0 | 0/0/0/0/0/1/0 |
| float/float | CMPMODE::LT | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/0/0/0/1/0 | 0/1/0/0/0/1/0 |
| float/float | CMPMODE::LT | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/0/0/0/0/0/0 | 0/0/0/0/0/0/0 |
| float/float | CMPMODE::LT | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/0/1/0 | 1/1/1/1/0/1/0 |
| float/float | CMPMODE::NE | -3.4028235e+38（MIN） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 0/1/1/1/1/1/1 | 0/1/1/1/1/1/1 |
| float/float | CMPMODE::NE | 3.4028235e+38（MAX） | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/0/1/1/1/1/1 | 1/0/1/1/1/1/1 |
| float/float | CMPMODE::NE | +0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| float/float | CMPMODE::NE | -0.0 | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/0/0/1/1/1 | 1/1/0/0/1/1/1 |
| float/float | CMPMODE::NE | nan | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/1 | 1/1/1/1/1/1/1 |
| float/float | CMPMODE::NE | +inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/0/1 | 1/1/1/1/1/0/1 |
| float/float | CMPMODE::NE | -inf | -3.4028235e+38（MIN）/3.4028235e+38（MAX）/+0/-0/nan/+inf/-inf | 1/1/1/1/1/1/0 | 1/1/1/1/1/1/0 |
| int32_t/int32_t | CMPMODE::EQ | -2147483648（MIN） | -2147483648（MIN）/2147483647（MAX） | 1/0 | 1/0 |
| int32_t/int32_t | CMPMODE::EQ | 2147483647（MAX） | -2147483648（MIN）/2147483647（MAX） | 0/1 | 0/1 |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

## 类型转换

**表33** Truncate特殊值/边界值输入的计算结果说明

<a name="table1730814818583"></a>

| src/dst的数据类型 | scalar的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | inf | inf | inf |
| half | -inf | -inf | -inf |
| half | 0/-0 | 0.0 | 0.0 |
| half | nan | nan | nan |
| half | MAX | MAX | MAX |
| half | MIN | MIN | MIN |
| float | inf | inf | inf |
| float | -inf | -inf | -inf |
| float | 0/-0 | 0.0 | 0.0 |
| float | nan | nan | nan |
| float | MAX | MAX | MAX |
| float | MIN | MIN | MIN |
| bfloat16 | inf | inf | inf |
| bfloat16 | -inf | -inf | -inf |
| bfloat16 | 0/-0 | 0.0 | 0.0 |
| bfloat16 | nan | nan | nan |
| bfloat16 | MAX | MAX | MAX |
| bfloat16 | MIN | MIN | MIN |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表34** Cast特殊值/边界值输入的计算结果

<a name="table1225292414418"></a>

| src数据类型/dst数据类型 | src输入数据 | dst输出数据（INF/NAN模式） | dst输出数据（饱和模式） |
| --- | --- | --- | --- |
| int4b_t/half，int4b_t/int16_t，int4b_t/bfloat16_t | MIN | -8 | -8 |
| int4b_t/half，int4b_t/int16_t，int4b_t/bfloat16_t | MAX | 7 | 7 |
| int8_t/int16_t，int8_t/half，<br>int8_t/int32_t | MIN | -128 | -128 |
| int8_t/int16_t，int8_t/half，<br>int8_t/int32_t | MAX | 127 | 127 |
| uint8_t/uint16_t，uint8_t/half，uint8_t/uint32_t，int16_t/uint8_t，int32_t/uint8_t，uint16_t/uint8_t，<br>uint32_t/uint8_t | 0.0 | 0.0 | 0.0 |
| uint8_t/uint16_t，uint8_t/half，uint8_t/uint32_t，int16_t/uint8_t，int32_t/uint8_t，uint16_t/uint8_t，<br>uint32_t/uint8_t | MAX | 255 | 255 |
| int16_t/uint32_t | MIN | 0.0 | 0.0 |
| int16_t/uint32_t | MAX | 32767 | 32767 |
| int16_t/int4b_t | MIN | 0.0 | -8 |
| int16_t/int4b_t | MAX | -1 | 7 |
| int16_t/int32_t，int16_t/float<br>int16_t/half | MIN | -32768 | -32768 |
| int16_t/int32_t，int16_t/float<br>int16_t/half | MAX | 32767 | 32767 |
| uint16_t/uint32_t，int32_t/float，uint32_t/uint16_t | 0.0 | 0.0 | 0.0 |
| uint16_t/uint32_t，int32_t/float，uint32_t/uint16_t | MAX | 65535 | 65535 |
| int32_t/int16_t | MIN | 0.0 | -32768 |
| int32_t/int16_t | MAX | -1 | 32767 |
| int32_t/float | MIN | -2147483648 | -2147483648 |
| int32_t/float | MAX | 2147483647 | 2147483647 |
| int32_t/int64_t | MIN | -2147483648 | -2147483648 |
| int32_t/int64_t | MAX | 2147483647 | 2147483647 |
| uint32_t/int16_t | 0.0 | 0.0 | 0.0 |
| uint32_t/int16_t | MAX | -1 | 32767 |
| int64_t/int32_t | MIN | 0.0 | -2147483648 |
| int64_t/int32_t | MAX | -1 | 2147483647 |
| int64_t/float | MIN | &nbsp;&nbsp; | &nbsp;&nbsp; |
| int64_t/float | MAX | &nbsp;&nbsp; | &nbsp;&nbsp; |
| float/int16_t | 0.0 | 0.0 | 0.0 |
| float/int16_t | MIN | 0.0 | -32768 |
| float/int16_t | MAX | 0.0 | 32767 |
| float/int16_t | inf | 32767 | 32767 |
| float/int16_t | -inf | -32768 | -32768 |
| float/int16_t | nan | 0.0 | 0.0 |
| float/int32_t | 0.0 | 0.0 | 0.0 |
| float/int32_t | MIN | 0.0 | -2147483648 |
| float/int32_t | MAX | 0.0 | 2147483647 |
| float/int32_t | inf | 2147483647 | 2147483647 |
| float/int32_t | -inf | -2147483648 | -2147483648 |
| float/int32_t | nan | 0.0 | 0.0 |
| float/bfloat16_t | 0.0 | 0.0 | 0.0 |
| float/bfloat16_t | MIN | -inf | -3.39e+38 |
| float/bfloat16_t | MAX | inf | 3.39e+38 |
| float/bfloat16_t | inf | inf | 3.39e+38 |
| float/bfloat16_t | -inf | -inf | -3.39e+38 |
| float/bfloat16_t | nan | nan | 0.0 |
| float/hifloat8_t | 0.0 | &nbsp;&nbsp; | &nbsp;&nbsp; |
| float/hifloat8_t | MIN | &nbsp;&nbsp; | &nbsp;&nbsp; |
| float/hifloat8_t | MAX | &nbsp;&nbsp; | &nbsp;&nbsp; |
| float/hifloat8_t | inf | &nbsp;&nbsp; | &nbsp;&nbsp; |
| float/hifloat8_t | -inf | &nbsp;&nbsp; | &nbsp;&nbsp; |
| float/hifloat8_t | nan | &nbsp;&nbsp; | &nbsp;&nbsp; |
| float/half | 0.0 | 0.0 | 0.0 |
| float/half | MIN | -inf | -65504.0 |
| float/half | MAX | inf | 65504.0 |
| float/half | inf | inf | 65504.0 |
| float/half | -inf | -inf | -65504.0 |
| float/half | nan | nan | 0.0 |
| float/fp8_e4m3_t | 0.0 | 0.0 | 0.0 |
| float/fp8_e4m3_t | MIN | nan | -448 |
| float/fp8_e4m3_t | MAX | nan | 448 |
| float/fp8_e4m3_t | inf | nan | 448 |
| float/fp8_e4m3_t | -inf | nan | -448 |
| float/fp8_e4m3_t | nan | nan | &nbsp;&nbsp; |
| float/fp8_e5m2_t | 0.0 | 0.0 | 0.0 |
| float/fp8_e5m2_t | MIN | -inf | -57344 |
| float/fp8_e5m2_t | MAX | inf | 57344 |
| float/fp8_e5m2_t | inf | inf | 57344 |
| float/fp8_e5m2_t | -inf | -inf | -57344 |
| float/fp8_e5m2_t | nan | nan | 0.0 |
| fp8_e5m2_t/float | 0.0 | 0.0 | 0.0 |
| fp8_e5m2_t/float | MIN | -57344 | -57344 |
| fp8_e5m2_t/float | MAX | 57344 | 57344 |
| fp8_e5m2_t/float | inf | inf | inf |
| fp8_e5m2_t/float | -inf | -inf | -inf |
| fp8_e5m2_t/float | nan | nan | nan |
| fp8_e4m3_t/float | 0.0 | 0.0 | 0.0 |
| fp8_e4m3_t/float | MIN | -448 | -448 |
| fp8_e4m3_t/float | MAX | 448 | 448 |
| fp8_e4m3_t/float | inf | inf | inf |
| fp8_e4m3_t/float | -inf | -inf | -inf |
| fp8_e4m3_t/float | nan | nan | nan |
| hifloat8_t/float | 0.0 | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/float | MIN | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/float | MAX | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/float | inf | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/float | -inf | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/float | nan | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/half | 0.0 | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/half | MIN | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/half | MAX | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/half | inf | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/half | -inf | &nbsp;&nbsp; | &nbsp;&nbsp; |
| hifloat8_t/half | nan | &nbsp;&nbsp; | &nbsp;&nbsp; |
| half/hifloat8_t | 0.0 | &nbsp;&nbsp; | &nbsp;&nbsp; |
| half/hifloat8_t | MIN | &nbsp;&nbsp; | &nbsp;&nbsp; |
| half/hifloat8_t | MAX | &nbsp;&nbsp; | &nbsp;&nbsp; |
| half/hifloat8_t | inf | &nbsp;&nbsp; | &nbsp;&nbsp; |
| half/hifloat8_t | -inf | &nbsp;&nbsp; | &nbsp;&nbsp; |
| half/hifloat8_t | nan | &nbsp;&nbsp; | &nbsp;&nbsp; |
| half/int32_t | 0.0 | 0.0 | 0.0 |
| half/int32_t | MIN | -65504.0 | -65504 |
| half/int32_t | MAX | 65504.0 | 65504 |
| half/int32_t | inf | 2147483647 | 2147483647 |
| half/int32_t | -inf | -2147483648 | -2147483648 |
| half/int32_t | nan | 0.0 | 0.0 |
| half/int16_t | 0.0 | 0.0 | 0.0 |
| half/int16_t | MIN | -32768 | -32768 |
| half/int16_t | MAX | 32767 | 32767 |
| half/int16_t | inf | 32767 | 32767 |
| half/int16_t | -inf | -32768 | -32768 |
| half/int16_t | nan | 0.0 | 0.0 |
| half/bfloat16_t | 0.0 | 0.0 | 0.0 |
| half/bfloat16_t | MIN | -65504.0 | -65504.0 |
| half/bfloat16_t | MAX | 65504.0 | 65504.0 |
| half/bfloat16_t | inf | inf | inf |
| half/bfloat16_t | -inf | -inf | -inf |
| half/bfloat16_t | nan | nan | nan |
| half/int8_t | 0.0 | 0.0 | 0.0 |
| half/int8_t | MIN | 32 | -128 |
| half/int8_t | MAX | -32 | 127 |
| half/int8_t | inf | 127 | 127 |
| half/int8_t | -inf | -128 | -128 |
| half/int8_t | nan | 0.0 | 0.0 |
| half/uint8_t | 0.0 | 0.0 | 0.0 |
| half/uint8_t | MIN | 32 | 0.0 |
| half/uint8_t | MAX | 224 | 255 |
| half/uint8_t | inf | 255 | 255 |
| half/uint8_t | -inf | 0.0 | 0.0 |
| half/uint8_t | nan | 0.0 | 0.0 |
| half/float | 0.0 | 0.0 | 0.0 |
| half/float | MIN | -65504.0 | -65504.0 |
| half/float | MAX | 65504.0 | 65504.0 |
| half/float | inf | inf | inf |
| half/float | -inf | -inf | -inf |
| half/float | nan | nan | nan |
| bfloat16_t/int32_t | 0.0 | 0.0 | 0.0 |
| bfloat16_t/int32_t | MIN | 0.0 | -2147483648 |
| bfloat16_t/int32_t | MAX | 0.0 | 2147483647 |
| bfloat16_t/int32_t | inf | 2147483647 | 2147483647 |
| bfloat16_t/int32_t | -inf | -2147483648 | -2147483648 |
| bfloat16_t/int32_t | nan | 0.0 | 0.0 |
| bfloat16_t/half | 0.0 | 0.0 | 0.0 |
| bfloat16_t/half | MIN | -inf | -65504.0 |
| bfloat16_t/half | MAX | inf | 65504.0 |
| bfloat16_t/half | inf | inf | 65504.0 |
| bfloat16_t/half | -inf | -inf | -65504.0 |
| bfloat16_t/half | nan | nan | 0.0 |
| bfloat16_t/float | 0.0 | 0.0 | 0.0 |
| bfloat16_t/float | MIN | -3.39e+38 | -3.39e+38 |
| bfloat16_t/float | MAX | 3.39e+38 | 3.39e+38 |
| bfloat16_t/float | inf | inf | inf |
| bfloat16_t/float | -inf | -inf | -inf |
| bfloat16_t/float | nan | nan | nan |
| bfloat16_t/fp4x2_e2m1_t | 0.0 | 0.0 | 0.0 |
| bfloat16_t/fp4x2_e2m1_t | MIN | -6 | -6 |
| bfloat16_t/fp4x2_e2m1_t | MAX | 6 | 6 |
| bfloat16_t/fp4x2_e2m1_t | inf | 6 | 6 |
| bfloat16_t/fp4x2_e2m1_t | -inf | -6 | -6 |
| bfloat16_t/fp4x2_e2m1_t | nan | 0.0 | 0.0 |
| bfloat16_t/fp4x2_e1m2_t | 0.0 | 0.0 | 0.0 |
| bfloat16_t/fp4x2_e1m2_t | MIN | -1.75 | -1.75 |
| bfloat16_t/fp4x2_e1m2_t | MAX | 1.75 | 1.75 |
| bfloat16_t/fp4x2_e1m2_t | inf | 1.75 | 1.75 |
| bfloat16_t/fp4x2_e1m2_t | -inf | -1.75 | -1.75 |
| bfloat16_t/fp4x2_e1m2_t | nan | 0.0 | 0.0 |
| bfloat16_t/fp8_e8m0_t | 0.0 | 0.0 | 0.0 |
| bfloat16_t/fp8_e8m0_t | MIN | 1.70141e+40 | 1.70141e+40 |
| bfloat16_t/fp8_e8m0_t | MAX | 1.70141e+40 | 1.70141e+40 |
| bfloat16_t/fp8_e8m0_t | inf | nan | nan |
| bfloat16_t/fp8_e8m0_t | -inf | nan | nan |
| bfloat16_t/fp8_e8m0_t | nan | nan | nan |
| fp4x2_e2m1_t/bfloat16_t | MIN | -6 | -6 |
| fp4x2_e2m1_t/bfloat16_t | MAX | 6 | 6 |
| fp4x2_e1m2_t/bfloat16_t | MIN | -1.75 | -1.75 |
| fp4x2_e1m2_t/bfloat16_t | MAX | 1.75 | 1.75 |
| fp8_e8m0_t/bfloat16_t | MIN | 1.57772e-30 | 1.57772e-30 |
| fp8_e8m0_t/bfloat16_t | MAX | 1.70141e+40 | 1.70141e+40 |
| fp8_e8m0_t/bfloat16_t | nan | nan | nan |


注：dst的元素取值（INF/NAN模式）列中涉及到浮点类型转换的结果，与IEEE754理论结果一致。

## 归约计算

**表35** Reduce特殊值/边界值输入的计算结果

<a name="table491948105210"></a>

| ReduceType类型 | src/dst的数据类型 | src的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| max | half | -65504.0（MIN） | -65504.0 | -65504.0 |
| max | half | 65504.0（MAX） | 65504.0 | 65504.0 |
| max | half | 0.0 | 0.0 | 0.0 |
| max | half | -0.0 | -0.0 | -0.0 |
| max | half | nan | nan | 0.0 |
| max | half | inf | inf | 65504.0 |
| max | half | -inf | -inf | -65504.0 |
| max | float | -3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| max | float | 3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| max | float | 0.0 | 0.0 | 0.0 |
| max | float | -0.0 | -0.0 | -0.0 |
| max | float | nan | nan | nan |
| max | float | inf | inf | inf |
| max | float | -inf | -inf | -inf |
| min | half | -65504.0（MIN） | -65504.0 | -65504.0 |
| min | half | 65504.0（MAX） | 65504.0 | 65504.0 |
| min | half | 0.0 | 0.0 | 0.0 |
| min | half | -0.0 | -0.0 | -0.0 |
| min | half | nan | nan | 0.0 |
| min | half | inf | inf | 65504.0 |
| min | half | -inf | -inf | -65504.0 |
| min | float | -3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| min | float | 3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| min | float | 0.0 | 0.0 | 0.0 |
| min | float | -0.0 | -0.0 | -0.0 |
| min | float | nan | nan | nan |
| min | float | inf | inf | inf |
| min | float | -inf | -inf | -inf |
| sum | half | -65504.0（MIN） | -inf | -65504.0 |
| sum | half | 65504.0（MAX） | inf | 65504.0 |
| sum | half | 0.0 | 0.0 | 0.0 |
| sum | half | -0.0 | -0.0 | -0.0 |
| sum | half | nan | nan | 0.0 |
| sum | half | inf | inf | 65504.0 |
| sum | half | -inf | -inf | -65504.0 |
| sum | float | -3.4028235e+38（MIN） | -inf | -inf |
| sum | float | 3.4028235e+38（MAX） | inf | inf |
| sum | float | 0.0 | 0.0 | 0.0 |
| sum | float | -0.0 | -0.0 | -0.0 |
| sum | float | nan | nan | nan |
| sum | float | inf | inf | inf |
| sum | float | -inf | -inf | -inf |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表36** ReduceDataBlock特殊值/边界值输入的计算结果

<a name="table1532212314305"></a>

| ReduceType类型 | src/dst的数据类型 | src的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| max | half | -65504.0（MIN） | -65504.0 | -65504.0 |
| max | half | 65504.0（MAX） | 65504.0 | 65504.0 |
| max | half | 0.0 | 0.0 | 0.0 |
| max | half | -0.0 | -0.0 | -0.0 |
| max | half | nan | nan | 0.0 |
| max | half | inf | inf | 65504.0 |
| max | half | -inf | -inf | -65504.0 |
| max | float | -3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| max | float | 3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| max | float | 0.0 | 0.0 | 0.0 |
| max | float | -0.0 | -0.0 | -0.0 |
| max | float | nan | nan | nan |
| max | float | inf | inf | inf |
| max | float | -inf | -inf | -inf |
| min | half | -65504.0（MIN） | -65504.0 | -65504.0 |
| min | half | 65504.0（MAX） | 65504.0 | 65504.0 |
| min | half | 0.0 | 0.0 | 0.0 |
| min | half | -0.0 | -0.0 | -0.0 |
| min | half | nan | nan | 0.0 |
| min | half | inf | inf | 65504.0 |
| min | half | -inf | -inf | -65504.0 |
| min | float | -3.4028235e+38（MIN） | -3.4028235e+38 | -3.4028235e+38 |
| min | float | 3.4028235e+38（MAX） | 3.4028235e+38 | 3.4028235e+38 |
| min | float | 0.0 | 0.0 | 0.0 |
| min | float | -0.0 | -0.0 | -0.0 |
| min | float | nan | nan | nan |
| min | float | inf | inf | inf |
| min | float | -inf | -inf | -inf |
| sum | half | -65504.0（MIN） | -inf | -65504.0 |
| sum | half | 65504.0（MAX） | inf | 65504.0 |
| sum | half | 0.0 | 0.0 | 0.0 |
| sum | half | -0.0 | -0.0 | -0.0 |
| sum | half | nan | nan | 0.0 |
| sum | half | inf | inf | 65504.0 |
| sum | half | -inf | -inf | -65504.0 |
| sum | float | -3.4028235e+38（MIN） | -inf | -inf |
| sum | float | 3.4028235e+38（MAX） | inf | inf |
| sum | float | 0.0 | 0.0 | 0.0 |
| sum | float | -0.0 | -0.0 | -0.0 |
| sum | float | nan | nan | nan |
| sum | float | inf | inf | inf |
| sum | float | -inf | -inf | -inf |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

**表37** PairReduceElem特殊值/边界值输入的计算结果

<a name="table1866163012710"></a>

| ReduceType类型 | src/dst的数据类型 | src的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- | --- |
| Sum | half | -65504.0（MIN） | -inf | -65504.0 |
| Sum | half | 65504.0（MAX） | inf | 65504.0 |
| Sum | half | 0.0 | 0.0 | 0.0 |
| Sum | half | -0.0 | -0.0 | -0.0 |
| Sum | half | nan | nan | 0.0 |
| Sum | half | inf | inf | 65504.0 |
| Sum | half | -inf | -inf | -65504.0 |
| Sum | float | -3.4028235e+38（MIN） | -inf | -inf |
| Sum | float | 3.4028235e+38（MAX） | inf | inf |
| Sum | float | 0.0 | 0.0 | 0.0 |
| Sum | float | -0.0 | -0.0 | -0.0 |
| Sum | float | nan | nan | nan |
| Sum | float | inf | inf | inf |
| Sum | float | -inf | -inf | -inf |


注：dst的元素取值（INF/NAN模式）列结果，与IEEE754理论结果一致。

## 索引操作

**表38** Arange特殊值/边界值输入的计算结果说明

<a name="table10167938144218"></a>

| src/dst的数据类型 | scalar的元素取值 | dst的元素取值<br>（INF/NAN模式） | dst的元素取值<br>（饱和模式） |
| --- | --- | --- | --- |
| half | inf | inf | 65504.0 |
| half | -inf | -inf | -65504.0 |
| half | 0/-0 | 0~127 | 0~127 |
| half | nan | nan | 0.0 |
| half | MAX | 16个65504，其余为inf | 65504.0 |
| half | MIN | 16个-65504，32个-65472，32个-65440，32个-65408，16个-65376 | 16个-65504，32个-65472，32个-65440，32个-65408，16个-65376 |
| float | inf | inf | inf |
| float | -inf | -inf | -inf |
| float | 0/-0 | 0~63 | 0~63 |
| float | nan | nan | nan |
| float | MAX | MAX | MAX |
| float | MIN | MIN | MIN |
| int8_t | 127（MAX） | 127（MAX），-128~126 | 127（MAX），-128~126 |
| int8_t | -128（MIN） | -128（MIN）~127 | -128（MIN）~127 |
| int16_t | 32767（MAX） | 32767（MAX），-32768（MIN）~-32642 | 32767（MAX），-32768（MIN）~-32642 |
| int16_t | -32768（MIN） | -32768（MIN）~-32641 | -32768（MIN）~-32641 |
| int32_t | -2147483648（MIN） | -2147483648（MIN）~-2147483585 | -2147483648（MIN）~-2147483585 |
| int32_t | 2147483647（MAX） | 2147483647（MAX）,-2147483648（MIN）~-2147483586 | 2147483647（MAX）,-2147483648（MIN）~-2147483586 |

