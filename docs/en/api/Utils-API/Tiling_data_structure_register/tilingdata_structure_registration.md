# TilingData Structure Registration<a name="ZH-CN_TOPIC_0000002114052953"></a>

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T16:35:51.863Z pushedAt=2026-09-11T07:47:43.663Z -->

## Description<a name="zh-cn_topic_0000001576728165_section212607105720"></a>

Registers the defined **TilingData** structure and binds it to a custom operator. For details, see [Example](#zh-cn_topic_0000001576728165_section97001499599).

## Prototype<a name="zh-cn_topic_0000001576728165_section129451113125413"></a>

```
#define REGISTER_TILING_DATA_CLASS(op_type, class_name)
  class op_type##class_name##Helper {
  public:
    op_type##class_name##Helper() {
      CTilingDataClassFactory::RegisterTilingData(#op_type, op_type##class_name##Helper::CreateTilingDataInstance);
    }
    static std::shared_ptr<TilingDef> CreateTilingDataInstance() {
      return std::make_shared<class_name>();
    }
  };
  op_type##class_name##Helper g_tilingdata_##op_type##class_name##helper;
```

## Parameters<a name="zh-cn_topic_0000001576728165_section552316288018"></a>

**Table 1**  Parameters

<a name="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_p10223674448"><a name="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_p10223674448"></a><a name="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_p645511218169"><a name="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_p645511218169"></a><a name="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_p1922337124411"><a name="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_p1922337124411"></a><a name="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001576728165_zh-cn_topic_0000001389733241_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576728165_p390264532814"><a name="zh-cn_topic_0000001576728165_p390264532814"></a><a name="zh-cn_topic_0000001576728165_p390264532814"></a>op_type</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576728165_p190218452287"><a name="zh-cn_topic_0000001576728165_p190218452287"></a><a name="zh-cn_topic_0000001576728165_p190218452287"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576728165_p1290244542815"><a name="zh-cn_topic_0000001576728165_p1290244542815"></a><a name="zh-cn_topic_0000001576728165_p1290244542815"></a>Registered operator name.</p></td>
</tr>
<tr id="zh-cn_topic_0000001576728165_row487964220282"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0000001576728165_p139022045172817"><a name="zh-cn_topic_0000001576728165_p139022045172817"></a><a name="zh-cn_topic_0000001576728165_p139022045172817"></a>struct_name</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001576728165_p3902845162817"><a name="zh-cn_topic_0000001576728165_p3902845162817"></a><a name="zh-cn_topic_0000001576728165_p3902845162817"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0000001576728165_p790318455287"><a name="zh-cn_topic_0000001576728165_p790318455287"></a><a name="zh-cn_topic_0000001576728165_p790318455287"></a>Tiling structure name, consistent with the C++ variable naming requirements.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001576728165_section65498832"></a>

-   The header file register/tilingdata\_base.h must be included when using it.
-   The intermediate structure and the custom tilingkey structure must follow the op\_type naming rules. For details, see [Example](#zh-cn_topic_0000001576728165_section97001499599).
-   The operator-customized tilingkey structure must ensure that the op\_type default structure is registered.
-   The tiling structure is a global attribute. The structure name must be used as a globally unique identifier. If different operators register tiling structures with the same name but different structures, undefined behavior occurs.

## Example<a name="zh-cn_topic_0000001576728165_section97001499599"></a>

-   Register the operator Tiling structure.

    ```
    #include "register/tilingdata_base.h"
    
    // Define the tilingdata class.
    namespace optiling {
    BEGIN_TILING_DATA_DEF(AddCustomTilingData)    // Register a tiling class, using the tiling name as the input parameter.
      TILING_DATA_FIELD_DEF(uint32_t, blkDim);    // Add a tiling field that participates in computing the number of cores.
      TILING_DATA_FIELD_DEF(uint32_t, totalSize); // Add a tiling field for the total computation data size, that is, the input shape size.
      TILING_DATA_FIELD_DEF(uint32_t, splitTile); // Add a tiling field for the data block processed by each core.
    END_TILING_DATA_DEF;                          // End of definition.
    // Register the operator tilingdata class to the corresponding AddCustom operator.
    REGISTER_TILING_DATA_CLASS(AddCustom, AddCustomTilingData) 
    }
    ```

-   Register an intermediate structure. When a user has a nested structure scenario, the nested structure is called an intermediate structure. Because one operator name can register only one Tiling structure, to enable the framework to detect the intermediate structure information, you need to construct a "virtual operator name" (structure name + Op) and register the intermediate structure through the REGISTER\_TILING\_DATA\_CLASS API. The registration method is as follows:

    ```
    BEGIN_TILING_DATA_DEF(Matmul)
      TILING_DATA_FIELD_DEF(uint16_t, mmVar);
      TILING_DATA_FIELD_DEF_ARR(uint16_t, 3, mmArr);
    END_TILING_DATA_DEF;
    // Register the intermediate structure. The first parameter is fixed as struct_name#Op, and the second parameter is struct_name. For example, if struct_name is Matmul, the first parameter is MatmulOp and the second parameter is Matmul.
    REGISTER_TILING_DATA_CLASS(MatmulOp, Matmul)      // Register the intermediate structure.
    ```

-   Customize tiling\_key to register different Tiling structures.

    ```
    /* In REGISTER_TILING_DATA_CLASS, the first parameter is ${op_type} + '_' + tiling_key. If no matching tiling structure is registered for tiling_key, the default structure is used. As shown in the following two methods, when tiling_key is not specified or is not 1, the tiling structure is AddStruct; when tiling_key equals 1, the tiling structure is AddStructSample1. */
    
    // Taking op_type as Add as an example, the default tiling structure is registered as follows.
    BEGIN_TILING_DATA_DEF(AddStruct)   
      TILING_DATA_FIELD_DEF(uint16_t, mmVar);   
      TILING_DATA_FIELD_DEF_ARR(uint16_t, 3, mmArr); 
    END_TILING_DATA_DEF; 
    REGISTER_TILING_DATA_CLASS(Add, AddStruct) 
    
    // When TilingKey equals 1, the structure is registered as follows.
    BEGIN_TILING_DATA_DEF(AddStructSample1) 
      TILING_DATA_FIELD_DEF(uint16_t, mmVar);   
      TILING_DATA_FIELD_DEF_ARR(uint16_t, 3, mmArr); 
    END_TILING_DATA_DEF; 
    REGISTER_TILING_DATA_CLASS(Add_1, AddStructSample1) 
    ```

