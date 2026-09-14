# TilingData Structure Definition<a name="ZH-CN_TOPIC_0000002078653614"></a>

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T16:33:13.090Z pushedAt=2026-09-11T07:47:32.021Z -->

## Description<a name="zh-cn_topic_0000001576606633_section212607105720"></a>

Defines a **TilingData** class and adds the required member variables (**TilingData** fields) to store the required **TilingData** parameters. After the **TilingData** class is defined, the class provides the following APIs by inheriting the **TilingDef** class (the base class used to store and process the member variables of user-defined Tiling structures):

-   **set\_\{field\_name\}** API: Used to set the field value of the **TilingData** class, where **field\_name** is the field name added when defining the **TilingData** class.
-   **get\_\{field\_name\}** API: Used to obtain the field value whose field name is **field\_name**.
-   **SaveToBuffer** API: Completes the serialization and saving of **TilingData**.
-   **GetDataSize** API: Obtains the length of **TilingData**.
-   **CheckAlignAndGenPlaceHolder**: This is an internal associated API used by the framework to check whether the member variables in the Tiling structure meet the byte alignment requirements and to pad the unaligned variables. Developers do not need to pay attention to it.
-   **SetDataPtr** API: This is a reserved API, and developers do not need to pay attention to it.

## Prototype<a name="zh-cn_topic_0000001576606633_section129451113125413"></a>

-   Define a **TilingData** class.

    ```
    BEGIN_TILING_DATA_DEF(class_name)
    ```

-   Add a **TilingData** field of a common data type.

    ```
    TILING_DATA_FIELD_DEF(data_type, field_name)
    ```

-   Add a **TilingData** field of an array type, where the element data type of the array is a common data type.

    ```
    TILING_DATA_FIELD_DEF_ARR(arr_type, arr_size, field_name)
    ```

-   Add a **TilingData** field of a structure type.

    ```
    TILING_DATA_FIELD_DEF_STRUCT(struct_type, field_name)
    ```

-   End the definition.

    ```
    END_TILING_DATA_DEF
    ```

## Parameters<a name="zh-cn_topic_0000001576606633_section552316288018"></a>

**Table 1** **BEGIN\_TILING\_DATA\_DEF** parameter description

<a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p10223674448"><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p10223674448"></a><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p645511218169"><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p645511218169"></a><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p1922337124411"><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p1922337124411"></a><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p2340183613156"><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p2340183613156"></a><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p2340183613156"></a>class_name</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p143401361158"><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p143401361158"></a><a name="zh-cn_topic_0000001576606633_zh-cn_topic_0000001389733241_p143401361158"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576606633_p10538192216213"><a name="zh-cn_topic_0000001576606633_p10538192216213"></a><a name="zh-cn_topic_0000001576606633_p10538192216213"></a>User-defined tiling structure name, consistent with C++ variable naming requirements.</p></td>
</tr>
</tbody>
</table>

**Table 2** **TILING\_DATA\_FIELD\_DEF** parameter description

<a name="zh-cn_topic_0000001576606633_table396666731"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001576606633_row296610614315"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001576606633_p1496666238"><a name="zh-cn_topic_0000001576606633_p1496666238"></a><a name="zh-cn_topic_0000001576606633_p1496666238"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001576606633_p59661261731"><a name="zh-cn_topic_0000001576606633_p59661261731"></a><a name="zh-cn_topic_0000001576606633_p59661261731"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001576606633_p5966136534"><a name="zh-cn_topic_0000001576606633_p5966136534"></a><a name="zh-cn_topic_0000001576606633_p5966136534"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001576606633_row109661268319"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576606633_p59661461233"><a name="zh-cn_topic_0000001576606633_p59661461233"></a><a name="zh-cn_topic_0000001576606633_p59661461233"></a>data_type</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576606633_p496626537"><a name="zh-cn_topic_0000001576606633_p496626537"></a><a name="zh-cn_topic_0000001576606633_p496626537"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576606633_p424615489310"><a name="zh-cn_topic_0000001576606633_p424615489310"></a><a name="zh-cn_topic_0000001576606633_p424615489310"></a>Data type of the field.</p></td>
</tr>
<tr id="zh-cn_topic_0000001576606633_row185910339316"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576606633_p1559115331134"><a name="zh-cn_topic_0000001576606633_p1559115331134"></a><a name="zh-cn_topic_0000001576606633_p1559115331134"></a>field_name</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576606633_p195916331037"><a name="zh-cn_topic_0000001576606633_p195916331037"></a><a name="zh-cn_topic_0000001576606633_p195916331037"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576606633_p15913331135"><a name="zh-cn_topic_0000001576606633_p15913331135"></a><a name="zh-cn_topic_0000001576606633_p15913331135"></a>Field name, consistent with C++ variable naming requirements.</p></td>
</tr>
</tbody>
</table>

**Table 3** **TILING\_DATA\_FIELD\_DEF\_ARR** parameter description

<a name="zh-cn_topic_0000001576606633_table164915116419"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001576606633_row13490116418"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001576606633_p124919111447"><a name="zh-cn_topic_0000001576606633_p124919111447"></a><a name="zh-cn_topic_0000001576606633_p124919111447"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001576606633_p34913111848"><a name="zh-cn_topic_0000001576606633_p34913111848"></a><a name="zh-cn_topic_0000001576606633_p34913111848"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001576606633_p124913111748"><a name="zh-cn_topic_0000001576606633_p124913111748"></a><a name="zh-cn_topic_0000001576606633_p124913111748"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001576606633_row44919112419"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576606633_p44701128172914"><a name="zh-cn_topic_0000001576606633_p44701128172914"></a><a name="zh-cn_topic_0000001576606633_p44701128172914"></a>arr_type</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576606633_p2491111848"><a name="zh-cn_topic_0000001576606633_p2491111848"></a><a name="zh-cn_topic_0000001576606633_p2491111848"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576606633_p74291753113019"><a name="zh-cn_topic_0000001576606633_p74291753113019"></a><a name="zh-cn_topic_0000001576606633_p74291753113019"></a>Data type of array elements.</p></td>
</tr>
<tr id="zh-cn_topic_0000001576606633_row84971116417"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576606633_p8491711042"><a name="zh-cn_topic_0000001576606633_p8491711042"></a><a name="zh-cn_topic_0000001576606633_p8491711042"></a>arr_size</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576606633_p1249181116413"><a name="zh-cn_topic_0000001576606633_p1249181116413"></a><a name="zh-cn_topic_0000001576606633_p1249181116413"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576606633_p169783181544"><a name="zh-cn_topic_0000001576606633_p169783181544"></a><a name="zh-cn_topic_0000001576606633_p169783181544"></a>Number of array elements.</p></td>
</tr>
<tr id="zh-cn_topic_0000001576606633_row9483162022920"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576606633_p1448419208292"><a name="zh-cn_topic_0000001576606633_p1448419208292"></a><a name="zh-cn_topic_0000001576606633_p1448419208292"></a>field_name</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576606633_p146498486302"><a name="zh-cn_topic_0000001576606633_p146498486302"></a><a name="zh-cn_topic_0000001576606633_p146498486302"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576606633_p1048422011297"><a name="zh-cn_topic_0000001576606633_p1048422011297"></a><a name="zh-cn_topic_0000001576606633_p1048422011297"></a>Field name, consistent with C++ variable naming requirements.</p></td>
</tr>
</tbody>
</table>

**Table 4** **TILING\_DATA\_FIELD\_DEF\_STRUCT** parameter description

<a name="zh-cn_topic_0000001576606633_table69741814053"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001576606633_row1197414141052"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001576606633_p4974814859"><a name="zh-cn_topic_0000001576606633_p4974814859"></a><a name="zh-cn_topic_0000001576606633_p4974814859"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001576606633_p0974414053"><a name="zh-cn_topic_0000001576606633_p0974414053"></a><a name="zh-cn_topic_0000001576606633_p0974414053"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001576606633_p15974914157"><a name="zh-cn_topic_0000001576606633_p15974914157"></a><a name="zh-cn_topic_0000001576606633_p15974914157"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001576606633_row2097412141353"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576606633_p19938194863116"><a name="zh-cn_topic_0000001576606633_p19938194863116"></a><a name="zh-cn_topic_0000001576606633_p19938194863116"></a>struct_type</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576606633_p4974714459"><a name="zh-cn_topic_0000001576606633_p4974714459"></a><a name="zh-cn_topic_0000001576606633_p4974714459"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576606633_p199749141554"><a name="zh-cn_topic_0000001576606633_p199749141554"></a><a name="zh-cn_topic_0000001576606633_p199749141554"></a>Structure type.</p></td>
</tr>
<tr id="zh-cn_topic_0000001576606633_row79741141253"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576606633_p20562560312"><a name="zh-cn_topic_0000001576606633_p20562560312"></a><a name="zh-cn_topic_0000001576606633_p20562560312"></a>field_name</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576606633_p597471415519"><a name="zh-cn_topic_0000001576606633_p597471415519"></a><a name="zh-cn_topic_0000001576606633_p597471415519"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576606633_p1097411419512"><a name="zh-cn_topic_0000001576606633_p1097411419512"></a><a name="zh-cn_topic_0000001576606633_p1097411419512"></a>Field name, consistent with C++ variable naming requirements.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001576606633_section65498832"></a>

-   The header file register/tilingdata\_base.h must be included when using it.
-   The variables defined in TILING\_DATA\_FIELD\_DEF and TILING\_DATA\_FIELD\_DEF\_ARR support only the int8\_t, uint8\_t, int16\_t, uint16\_t, int32\_t, uint32\_t, int64\_t, uint64\_t, and  float data types.
-   In TILING\_DATA\_FIELD\_DEF\_STRUCT, struct\_type supports only tiling structures defined by BEGIN\_TILING\_DATA\_DEF and does not support structure types defined directly using C++ syntax.
-   To set parameter values and use tiling data on the host side, use the set\_xxx and get\_xxx APIs (replace xxx with the field name). For details, see the example.
-   Tiling data members must meet the byte alignment requirement, that is, the offset of the current data member dataVar in the structure must satisfy offset % sizeof\(dataVar\) == 0.
-   The tiling structure is a global attribute. Note that the structure name must be used as a globally unique identifier. If different operators register tiling structures with the same name but different structures, undefined behavior occurs.
-   When registering an intermediate structure, if the intermediate structure name is struct\_name, the first parameter is fixed to struct\_name\#Op.
-   When setting the field value defined by TILING\_DATA\_FIELD\_DEF\_ARR, note that set\_\{field\_name\} only passes in the array pointer and assigns values according to the array length defined in the macro. Therefore, you must ensure that the length of the array pointed to by the passed-in array pointer is not less than the array length defined in the macro to avoid out-of-bounds access.

## Example<a name="zh-cn_topic_0000001576606633_section97001499599"></a>

```
#include "register/tilingdata_base.h"

// Define the tilingdata class.
namespace optiling {
BEGIN_TILING_DATA_DEF(Matmul)
  TILING_DATA_FIELD_DEF(uint16_t, mmVar);
  TILING_DATA_FIELD_DEF_ARR(uint16_t, 3, mmArr);
END_TILING_DATA_DEF;
// Register the intermediate structure. The first parameter is fixed as struct_name#Op, and the second parameter is struct_name. For example, if struct_name is Matmul, the first parameter is MatmulOp and the second parameter is Matmul.
REGISTER_TILING_DATA_CLASS(MatmulOp, Matmul)      // Register the intermediate structure.

BEGIN_TILING_DATA_DEF(AddCustomTilingData)        // Register a tiling class, using the tiling name as the input parameter.
  TILING_DATA_FIELD_DEF(uint32_t, blkDim);        // Add a tiling variable-type field, which participates in calculating the number of cores.
  TILING_DATA_FIELD_DEF(uint32_t, totalSize);     // Add a tiling variable-type field, which is the total amount of data to be computed.
  TILING_DATA_FIELD_DEF(uint32_t, splitTile);     // Add a tiling variable-type field, which is the data block computed by each core.
  TILING_DATA_FIELD_DEF_ARR(uint16_t, 3, arrSample);    // Add a tiling array-type field.
  TILING_DATA_FIELD_DEF_STRUCT(Matmul, mm);             // Add a tiling struct-type field.
END_TILING_DATA_DEF;                                    // End of definition.
// Register the operator tilingdata class to the corresponding AddCustom operator.
REGISTER_TILING_DATA_CLASS(AddCustom, AddCustomTilingData) 
}

// Set parameter values and use tiling parameters on the host side.
static void TilingAddInit(AddCustomTilingData *tiling, uint32_t numBlocks)
{
  // Set parameter values.
  tiling->set_blkDim(numBlocks);                  // Set the value of the general data type variable numBlocks.
  uint16_t arr[] = {10,2,8,2,3,4,5,2,1,2,4,4,5,};
  tiling->set_arrSample(arr);                    // Set the value of the general data type array variable arrSample. Only the first three elements of arr are copied, consistent with arr_size in TILING_DATA_FIELD_DEF_ARR.
  tiling->mm.set_mmVar(1);                       // Set the value of the nested struct general data type variable mmVar.
  tiling->mm.set_mmArr(arr);                     // Set the value of the nested struct general data type array mmArr.
  
  // Use parameter values.
  uint32_t useNumBlocks = tiling->get_blkDim();    // Obtain the general data type variable numBlocks.
  uint32_t* arrPoint = tiling->get_arrSample();   // Obtain the general data type array variable arrSample.
  useNumBlocks = tiling->mm.get_mmVar();           // Obtain the nested struct general data type variable mmVar.
  arrPoint = tiling->mm.get_mmArr();              // Obtain the nested struct general data type array mmArr.
}
```

