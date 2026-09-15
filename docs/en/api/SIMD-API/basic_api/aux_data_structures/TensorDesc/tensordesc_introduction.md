# TensorDesc Introduction<a name="ZH-CN_TOPIC_0000002306498702"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T11:53:57.442Z -->

**TensorDesc** is used to store the Tensor description information obtained by [ListTensorDesc](../ListTensorDesc.md).**GetDesc()** based on the **index**.

## Prototype<a name="section10580930144614"></a>

```cpp
template<class T> class TensorDesc {
    TensorDesc();
    ~TensorDesc();
    void SetShapeAddr(uint64_t* shapePtr);
    uint64_t GetDim();
    uint64_t GetIndex();
    uint64_t GetShape(uint32_t offset);
    T* GetDataPtr();
    GlobalTensor<T> GetDataObj();
}
```

## Template Parameters<a name="section116801320102618"></a>

**Table 1** Template parameter description

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.2.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.2.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row260915573419"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p45051858102513"><a name="p45051858102513"></a><a name="p45051858102513"></a>**T**</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="p823866165711"><a name="p823866165711"></a><a name="p823866165711"></a>Tensor data type.</p></td>
</tr>
</tbody>
</table>

