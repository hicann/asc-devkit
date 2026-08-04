# SetL2CacheHint<a name="ZH-CN_TOPIC_0000002130744234"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->


## 功能说明<a name="section618mcpsimp"></a>

设置GlobalTensor是否启用L2 Cache，默认启用L2 Cache。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template<CacheRwMode rwMode = CacheRwMode::RW>
__aicore__ inline void SetL2CacheHint(CacheMode mode);
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

<a name="table4835205712588"></a>
<table><thead align="left"><tr id="row118356578583"><th class="cellrowborder" valign="top" width="27.839999999999996%" id="mcps1.2.3.1.1"><p id="p48354572582"><a name="p48354572582"></a><a name="p48354572582"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="72.16%" id="mcps1.2.3.1.2"><p id="p583535795817"><a name="p583535795817"></a><a name="p583535795817"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row18835145716587"><td class="cellrowborder" valign="top" width="27.839999999999996%" headers="mcps1.2.3.1.1 "><p id="p1383515717581"><a name="p1383515717581"></a><a name="p1383515717581"></a><strong id="b53621266338"><a name="b53621266338"></a><a name="b53621266338"></a>rwMode</strong></p>
</td>
<td class="cellrowborder" valign="top" width="72.16%" headers="mcps1.2.3.1.2 "><p id="p77520541653"><a name="p77520541653"></a><a name="p77520541653"></a>设置L2 Cache读写模式。</p>
<a name="screen774625064615"></a><a name="screen774625064615"></a><pre class="screen" codetype="Cpp" id="screen774625064615">enum CacheRwMode {
READ = 1,
WRITE = 2,
RW = 3
};</pre>
<p id="p18689719202918"><a name="p18689719202918"></a><a name="p18689719202918"></a>预留参数。为后续的功能做保留，开发者暂时无需关注，使用默认值即可。</p>
</td>
</tr>
</tbody>
</table>

**表2**  参数说明

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="12.989999999999998%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="73.07000000000001%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p284425844311"><a name="p284425844311"></a><a name="p284425844311"></a><strong id="b16238121793314"><a name="b16238121793314"></a><a name="b16238121793314"></a>mode</strong></p>
</td>
<td class="cellrowborder" valign="top" width="12.989999999999998%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="73.07000000000001%" headers="mcps1.2.4.1.3 "><p id="p6389114616514"><a name="p6389114616514"></a><a name="p6389114616514"></a>用户指定的L2 Cache模式。</p>
<a name="screen1440994812502"></a><a name="screen1440994812502"></a><pre class="screen" codetype="Cpp" id="screen1440994812502">enum class CacheMode : uint8_t {
CACHE_MODE_DISABLE = 0, // 不启用L2 Cache
CACHE_MODE_NORMAL = 1,  // 启用L2 Cache
CACHE_MODE_PERSISTENT = 4,  // 启用L2 Cache驻留模式
};</pre>
<p id="p595815581177"><a name="p595815581177"></a><a name="p595815581177"></a>当特定GlobalTensor启用L2 Cache后实测性能反而下降时，可考虑手动禁用该GlobalTensor的L2 Cache功能。例如，若某算子仅对特定GlobalTensor执行单次读取操作，将其数据缓存至L2 Cache不仅无法带来性能收益，反而可能因数据频繁搬运至L2 Cache而引入额外的开销，此时建议关闭该GlobalTensor的L2 Cache。</p>
<p id="p595815581177"><a name="p595815581177"></a><a name="p595815581177"></a>通常情况下，L2 Cache可采用CACHE_MODE_NORMAL模式运行。在此模式下，当L2 Cache容量耗尽时会触发数据置换机制，已存入L2 Cache中的数据可能被替换。若需确保特定GlobalTensor的数据始终保留在L2 Cache中，可采用驻留模式。目前该驻留模式功能尚在开发中，暂不支持，计划于Ascend 950PR/Ascend 950DT产品上提供支持。</p>
<p id="p165951277168"><a name="p165951277168"></a><a name="p165951277168"></a>如果不调用该接口，默认为CacheMode::CACHE_MODE_NORMAL，即GlobalTensor会启用L2 Cache。</p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section640mcpsimp"></a>

无。

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section17531157161314"></a>

```cpp
uint64_t dataSize = 256; //设置input_global的大小为256

AscendC::GlobalTensor<int32_t> inputGlobal; // 类型为int32_t
inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ int32_t *>(src_gm), dataSize); // 设置源操作数在Global Memory上的起始地址为src_gm，所占外部存储的大小为256个int32_t
inputGlobal.SetL2CacheHint(AscendC::CacheMode::CACHE_MODE_DISABLE); // 设置GlobalTensor不会写入L2 Cache

AscendC::LocalTensor<int32_t> inputLocal = inQueueX.AllocTensor<int32_t>();    
AscendC::DataCopy(inputLocal, inputGlobal, dataSize); // 将Global Memory上的inputGlobal拷贝到Local Memory的inputLocal上
```
