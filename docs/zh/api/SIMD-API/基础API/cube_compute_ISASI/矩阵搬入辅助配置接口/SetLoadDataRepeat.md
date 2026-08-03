# SetLoadDataRepeat<a name="ZH-CN_TOPIC_0000001788021082"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

用于设置[LoadData（卷积数据搬运）](../矩阵计算的搬入/LoadData_3D.md)接口的repeat参数。设置repeat参数后，可以通过调用一次LoadData（卷积数据搬运）接口完成多个迭代的数据搬运。

## 函数原型<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetLoadDataRepeat(const LoadDataRepeatParam& repeatParams)
```

## 参数说明<a name="section622mcpsimp"></a>

### 接口参数和结构体参数说明

**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| repeatParams | 输入 | 设置[LoadData（卷积数据搬运）](../矩阵计算的搬入/LoadData_3D.md)接口的repeat参数，类型为LoadDataRepeatParam。<br>具体定义请参考：`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>参数说明请参考[表2](#table15780447181917)。 |

<a name="table15780447181917"></a>
**表2** LoadDataRepeatParam结构体参数说明

| 参数名称 | 含义 |
| --- | --- |
| repeatStride | height/width方向上的前一个迭代与后一个迭代起始地址的距离，取值范围：repeatStride∈[0, 65535]，默认值为0。<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; repeatMode为0，repeatStride的单位为16个元素。<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; repeatMode为1，repeatStride的单位和具体型号有关。下文中的data_type指LoadData（卷积数据搬运）v2中源操作数的数据类型。 |
| repeatTime | height/width方向上的迭代次数，取值范围：repeatTime∈[0, 255]。默认值为1。 |
| repeatMode | 控制repeat迭代的方向，取值范围：repeatMode∈[0, 1]。默认值为0。<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; 0：迭代沿height方向；<br>&nbsp;&nbsp;&nbsp;&nbsp;&bull; 1：迭代沿width方向。 |
| dstStride | 输出矩阵K轴方向偏移，以512B分形为单位。不同的型号，dstStride的支持度不同，请参考[dstStride参数支持度说明](#dstStride参数支持度说明)。 |

### dstStride参数支持度说明

<!-- npu="950" id8 -->
- 针对Ascend 950PR/Ascend 950DT，新增dstStride参数，调用该接口必须配置参数dstStride。
<!-- end id8 -->

<!-- npu="910b" id9 -->
- 针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，不支持该参数。
<!-- end id9 -->

<!-- npu="A3" id10 -->
- 针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，不支持该参数。
<!-- end id10 -->

<!-- npu="310b" id11 -->
- 针对Atlas 200I/500 A2 推理产品，不支持该参数。
<!-- end id11 -->

## 返回值说明

无

## 约束说明

- 迭代沿height方向时，repeatStride的单位为16个元素，迭代沿width方向时，repeatStride的单位为32/sizeof\(data\_type\)个元素。
- repeatTime=0表示LoadData（卷积数据搬运）不执行搬运，LoadData（卷积数据搬运）接口将被视为NOP（空操作）。

<!-- npu="950" id12 -->
- 针对Ascend 950PR/Ascend 950DT，调用LoadData（卷积数据搬运）指令时，必须配置本接口中dstStride参数。
<!-- end id12 -->

- 不同芯片型号，repeatStride的单位不同，具体参考如下：
    
    <!-- npu="950" id13 -->
    - Ascend 950PR/Ascend 950DT，repeatStride的单位为32/sizeof(data_type)个元素。
    <!-- end id13 -->

    <!-- npu="910b" id14 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品，repeatStride的单位为32/sizeof(data_type)个元素。
    <!-- end id14 -->

    <!-- npu="A3" id15 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品，repeatStride的单位为32/sizeof(data_type)个元素。
    <!-- end id15 -->

    <!-- npu="310b" id16 -->
    - Atlas 200I/500 A2 推理产品，repeatStride的单位为64/sizeof(data_type)个元素。
    <!-- end id16 -->

## 调用示例<a name="section642mcpsimp"></a>

展示代码示例片段：

```cpp
// LoadData（卷积数据搬运）v2指令完成img2col的过程，可知img2col后A矩阵高度为ho * wo，根据ho和wo的计算公式，代入卷积核宽度、卷积核滑动步长、卷积核膨胀系数等参数可知：A矩阵的高度为CeilAlign(k, fractalShape[0])；img2col后A矩阵宽度为ci * kh * kw，代入kh=1，kw=1，可知A矩阵的宽度为CeilAlign(m, fractalShape[1])。最后，配置loadDataParams.enTranspose = true，将整个A矩阵转置并且将其中每一个分形转置
// 使用load3d接口，实现NZ2ZZ
AscendC::LoadData3DParamsV2<T> loadDataParams;

// 设置loadDataParams相关参数
...

// 使用SetLoadDataRepeat接口，设置LoadData（卷积数据搬运）v2接口的repeat参数
AscendC::LoadDataRepeatParam repeatParams;
repeatParams.repeatTime = 1;  // height/width方向上的迭代次数
repeatParams.repeatStride = 1;  // height/width方向上前后迭代起始位置的距离
repeatParams.repeatMode = 0;  // 迭代方向0: 迭代沿height; 1: 迭代沿width
repeatParams.dstStride = CeilDivision(m, fractalShape[0]);  // 输出矩阵K轴方向偏移
AscendC::SetLoadDataRepeat(repeatParams);

AscendC::LoadData(a2Local, a1Local, loadDataParams);
```
