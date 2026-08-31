# GetValue<a name="ZH-CN_TOPIC_0000002132084956"></a>

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
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/data_structures/GlobalTensor/GetValue_res.md#id1 -->


## 功能说明<a name="section618mcpsimp"></a>

获取GlobalTensor的相应偏移位置的值。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline __inout_pipe__(S) PrimType GetValue(const uint64_t offset) const
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="13.94%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="12.98%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="73.08%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="13.94%" headers="mcps1.2.4.1.1 "><p id="p151479166327"><a name="p151479166327"></a><a name="p151479166327"></a>offset</p>
</td>
<td class="cellrowborder" valign="top" width="12.98%" headers="mcps1.2.4.1.2 "><p id="p158449584436"><a name="p158449584436"></a><a name="p158449584436"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="73.08%" headers="mcps1.2.4.1.3 "><p id="p126017529210"><a name="p126017529210"></a><a name="p126017529210"></a>偏移offset个元素。</p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section640mcpsimp"></a>

返回PrimType类型的立即数。

## 约束说明<a name="section633mcpsimp"></a>

如果GetValue的Global Memory地址内容存在被外部改写的可能，需要先调用[DataCacheCleanAndInvalid](../../cache_control/DataCacheCleanAndInvalid.md)，确保Data Cache与Global Memory的Cache一致性，之后再调用此接口。

## 调用示例<a name="section1956710347119"></a>

```cpp
// 数据(GlobalTensor): [0 100 200  ... ]

AscendC::GlobalTensor<uint32_t> tiling_global;
tiling_global.SetGlobalBuffer(reinterpret_cast<__gm__ uint32_t *>(tiling), 16);
const uint16_t C1 = (uint16_t)tiling_global.GetValue(0);
const uint16_t H = (uint16_t)tiling_global(1);  
// 示例结果如下：
// C1 = 0，H = 100
```
