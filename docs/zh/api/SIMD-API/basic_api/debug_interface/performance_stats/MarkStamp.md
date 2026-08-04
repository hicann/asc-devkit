# MarkStamp<a name="ZH-CN_TOPIC_0000002491669476"></a>

## 产品支持情况

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

## 功能说明<a name="section165477552317"></a>

推荐使用[asc\_mark\_stamp](../../../../Utils-API/调测接口/asc_mark_stamp.md)接口进行特定位置的标记，该接口同时适用于C语言和C++语言编程。

用户通过调用接口，用于在算子执行过程中标记特定位置，便于后期通过流水图分析代码执行路径与性能热点。

## 函数原型<a name="section054795512320"></a>

```
template<pipe_t pipe, uint16_t index>
__aicore__ inline void MarkStamp()

template<pipe_t pipe>
__aicore__ inline void MarkStamp(uint16_t index)
```

## 参数说明<a name="section2054811551732"></a>

<a name="table18824164416323"></a>
<table><thead align="left"><tr id="row4824124418325"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p58240441320"><a name="p58240441320"></a><a name="p58240441320"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p2824114412328"><a name="p2824114412328"></a><a name="p2824114412328"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row1882512449324"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p88251944153215"><a name="p88251944153215"></a><a name="p88251944153215"></a>pipe</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p148291124113616"><a name="p148291124113616"></a><a name="p148291124113616"></a>指定打点所在的pipeline类型。</p>
</td>
</tr>
<tr id="row182624413210"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p11826104433211"><a name="p11826104433211"></a><a name="p11826104433211"></a>index</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1826184413328"><a name="p1826184413328"></a><a name="p1826184413328"></a>用户设置的打点的唯一标识id。</p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section15548145517311"></a>

无

## 约束说明<a name="section654818551936"></a>

-   芯片给index预留了12个bit，可使用的取值范围为\[0,256\)。为方便从打点图中找到对应的代码，建议不要重复使用相同的index。
-   如果在循环中增加了一个MarkStamp指令，每次执行到指令时都会输出一个打点，且index是相同的。
-   如果开发者在两个相邻的VF分别打标记，由于编译器可能会对VF A和VF B做融合，MarkStamp1和MarkStamp2则会被优化掉，不会输出打点。

## 调用示例<a name="section354885515310"></a>

```
mte2_opt();
//在算子执行开始处打点
MarkStamp<PIPE_V, 0>();
//执行核心计算
vector_opt();
//在算子执行结束处打点
MarkStamp<PIPE_V, 1>();
mte3_opt();
```
