# WriteSpmBuffer<a name="ZH-CN_TOPIC_0000001560560012"></a>

## 产品支持情况<a name="section1550532418810"></a>

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
- Atlas 训练系列产品：支持
<!-- end id7 -->


## 功能说明<a name="section618mcpsimp"></a>

将需要溢出暂存的数据拷贝到SPM Buffer中。

## 函数原型<a name="section620mcpsimp"></a>

-   适用于连续和不连续的数据暂存：

    ```
    template <typename T>
    __aicore__ inline void WriteSpmBuffer(const LocalTensor<T>& writeBuffer, const DataCopyParams& copyParams, int32_t writeOffset = 0)
    ```

-   适用于连续的数据暂存：

    ```
    template <typename T>
    __aicore__ inline void WriteSpmBuffer(const LocalTensor<T>& writeBuffer, const int32_t writeSize, int32_t writeOffset = 0)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  接口参数说明

<a name="table1794522316251"></a>
<table><thead align="left"><tr id="row19456238252"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p119458239258"><a name="p119458239258"></a><a name="p119458239258"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p9945152332514"><a name="p9945152332514"></a><a name="p9945152332514"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p1594552312513"><a name="p1594552312513"></a><a name="p1594552312513"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row1694552372511"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1094516239250"><a name="p1094516239250"></a><a name="p1094516239250"></a>writeBuffer</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p8945112312514"><a name="p8945112312514"></a><a name="p8945112312514"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p169454239253"><a name="p169454239253"></a><a name="p169454239253"></a>需要溢出暂存的Local内存。</p>
</td>
</tr>
<tr id="row524916295111"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p92491829141111"><a name="p92491829141111"></a><a name="p92491829141111"></a>copyParams</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p6249142931114"><a name="p6249142931114"></a><a name="p6249142931114"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p18111121482617"><a name="p18111121482617"></a><a name="p18111121482617"></a>搬运参数，DataCopyParams类型，DataCopyParams结构定义请参考<a href="#table9182515919">表2</a>。</p>
</td>
</tr>
<tr id="row138643616158"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p1986436201513"><a name="p1986436201513"></a><a name="p1986436201513"></a>writeSize</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p208648611152"><a name="p208648611152"></a><a name="p208648611152"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p198121859191513"><a name="p198121859191513"></a><a name="p198121859191513"></a>拷贝的元素个数。</p>
</td>
</tr>
<tr id="row126421762566"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p2642463560"><a name="p2642463560"></a><a name="p2642463560"></a>writeOffset</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p18642364562"><a name="p18642364562"></a><a name="p18642364562"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p146421267562"><a name="p146421267562"></a><a name="p146421267562"></a>拷贝到SPM Buffer的偏移，单位为字节。</p>
</td>
</tr>
</tbody>
</table>

**表2**  DataCopyParams结构体参数定义

<a name="table9182515919"></a>
<table><thead align="left"><tr id="row151816516917"><th class="cellrowborder" valign="top" width="15%" id="mcps1.2.3.1.1"><p id="p18182513916"><a name="p18182513916"></a><a name="p18182513916"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="85%" id="mcps1.2.3.1.2"><p id="p41815515920"><a name="p41815515920"></a><a name="p41815515920"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row1818105113916"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p17780347142614"><a name="p17780347142614"></a><a name="p17780347142614"></a>blockCount</p>
</td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p478014752618"><a name="p478014752618"></a><a name="p478014752618"></a>待搬运的连续传输数据块个数。uint16_t类型，取值范围：blockCount∈[1, 4095]。</p>
</td>
</tr>
<tr id="row2968131992515"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p878011470264"><a name="p878011470264"></a><a name="p878011470264"></a>blockLen</p>
</td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p11780174752617"><a name="p11780174752617"></a><a name="p11780174752617"></a>待搬运的每个连续传输数据块长度，单位为DataBlock（32字节）。uint16_t类型，取值范围：blockLen∈[1, 65535]。</p>
</td>
</tr>
<tr id="row1589112062510"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p378018478265"><a name="p378018478265"></a><a name="p378018478265"></a>srcGap</p>
</td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p19780547162614"><a name="p19780547162614"></a><a name="p19780547162614"></a>源操作数相邻连续数据块的间隔（前面一个数据块的尾与后面数据块的头的间隔），单位为DataBlock（32字节）。uint16_t类型，srcGap不要超出该数据类型的取值范围。</p>
</td>
</tr>
<tr id="row3593192082512"><td class="cellrowborder" valign="top" width="15%" headers="mcps1.2.3.1.1 "><p id="p18780347152610"><a name="p18780347152610"></a><a name="p18780347152610"></a>dstGap</p>
</td>
<td class="cellrowborder" valign="top" width="85%" headers="mcps1.2.3.1.2 "><p id="p18780947162613"><a name="p18780947162613"></a><a name="p18780947162613"></a>目的操作数相邻连续数据块间的间隔（前面一个数据块的尾与后面数据块的头的间隔），单位为DataBlock（32字节）。uint16_t类型，dstGap不要超出该数据类型的取值范围。</p>
</td>
</tr>
</tbody>
</table>

## 约束说明<a name="section633mcpsimp"></a>

-   暂存拷贝到L1时注意writeSize和writeOffset保证32字节对齐。
-   拷贝的内存不要超出初始化的SPM Buffer大小，否则会存在溢出踩踏等问题。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 调用示例<a name="section642mcpsimp"></a>

-   使用DataCopyParams搬运

    ```
    AscendC::TPipe pipe;
    int dataSize = 32; // 假设T为half类型，从ub上申请一块内存32 * sizeof(half)字节
    int offset = 32; // 拷贝到spmBuffer时偏移32字节
    AscendC::DataCopyParams copyParams{1, 2, 0, 0}; // 从ub上搬运一个连续传输数据块，一个数据块的长度为2个datablock，一个datablock为32bytes
    // writeLocal为SPM Buffer上的half类型的LocalTensor
    pipe.WriteSpmBuffer(writeLocal, copyParams, offset); // 将ub上的连续传输数据块搬运到SPM Buffer
    pipe.ReadSpmBuffer(writeLocal, copyParams, offset); // 将暂存在SPM Buffer的数据读回到local数据
    ...
    // 当ub内存足够时，将暂存在SPM Buffer的数据块搬运回GM上，dstGlobal为half类型的GlobalTensor
    AscendC::DataCopy(dstGlobal, writeLocal, copyParams);
    ```

-   使用writeSize连续搬运

    ```
    AscendC::TPipe pipe;
    int dataSize = 32; // 假设T为half类型，从ub上申请一块内存32 * sizeof(half)字节
    int offset = 32; // 拷贝到spmBuffer时偏移32字节
    ;
    // writeLocal为SPM Buffer上的half类型的LocalTensor
    pipe.WriteSpmBuffer(writeLocal, dataSize, offset); // 将ub上的连续传输数据块搬运到SPM Buffer
    pipe.ReadSpmBuffer(writeLocal, dataSize, offset); // 将暂存在SPM Buffer的数据读回到local数据
    ...
    // 当ub内存足够时，将暂存在SPM Buffer的数据块搬运回GM上，dstGlobal为half类型的GlobalTensor
    AscendC::DataCopy(dstGlobal, writeLocal, dataSize);
    ```

