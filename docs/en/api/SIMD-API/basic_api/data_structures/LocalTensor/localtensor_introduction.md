# LocalTensor Introduction<a name="ZH-CN_TOPIC_0000002364805793"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T13:07:46.067Z -->

**LocalTensor** represents the data in the L1/L0A/L0B/L0C buffers and the Unified Buffer within the Local Memory (internal storage) of the AI Core.

## Header Files to Include<a name="zh-cn_topic_0000002213064918_section78885814919"></a>

```cpp
#include "kernel_operator.h"
```

## Prototype<a name="section1380144323919"></a>

```cpp
template <typename T> class LocalTensor : public BaseLocalTensor<T> {
public:
    // PrimT is used to extract the LiteType base data type from TensorTrait when T is passed as a TensorTrait type.
    using PrimType = PrimT<T>;
    __aicore__ inline LocalTensor<T>() {};
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    ~LocalTensor();
    explicit LocalTensor<T>(TBuffAddr& address);
    LocalTensor<T>(const LocalTensor<T>& other);
    LocalTensor<T> operator = (const LocalTensor<T>& other); // The two LocalTensor instances before and after = share the same internal storage.
    PrimType* GetPhyAddr(const uint32_t offset) const;
    PrimType* GetPhyAddr() const;
    __inout_pipe__(S) PrimType GetValue(const uint32_t offset) const;
    __inout_pipe__(S) PrimType& operator()(const uint32_t offset) const;
    template <typename CAST_T> __aicore__ inline LocalTensor<CAST_T> ReinterpretCast() const;
    template <typename T1> __inout_pipe__(S) void SetValue(const uint32_t index, const T1 value) const;
    LocalTensor operator[](const uint32_t offset) const;
    template <typename T1> void SetAddrWithOffset(LocalTensor<T1> &src, uint32_t offset);
    inline void Print();
    inline void Print(uint32_t len);
    int32_t ToFile(const std::string& fileName) const;
#else
    __aicore__ inline uint64_t GetPhyAddr() const;
    __aicore__ inline uint64_t GetPhyAddr(const uint32_t offset) const;
    __aicore__ inline __inout_pipe__(S) PrimType GetValue(const uint32_t index) const;
    __aicore__ inline __inout_pipe__(S) __ubuf__ PrimType& operator()(const uint32_t offset) const;
    template <typename CAST_T> __aicore__ inline LocalTensor<CAST_T> ReinterpretCast() const;
    template <typename T1> __aicore__ inline __inout_pipe__(S)
        void SetValue(const uint32_t index, const T1 value) const;
    __aicore__ inline LocalTensor operator[](const uint32_t offset) const;
    template <typename T1>
    [[deprecated("NOTICE: SetAddrWithOffset has been deprecated and will be removed in the next version. "
        "Please do not use it!")]]
    __aicore__ inline void SetAddrWithOffset(LocalTensor<T1> &src, uint32_t offset);
#endif
    __aicore__ inline LocalTensor<T>(AscendC::TPosition pos, uint32_t addr, uint32_t tieSize);
    __aicore__ inline int32_t GetPosition() const;
    __aicore__ inline void SetSize(const uint32_t size);
    __aicore__ inline uint32_t GetSize() const;
    [[deprecated("NOTICE: GetLength has been deprecated and will be removed in the next version. Please do not use "
                 "it!")]]
    __aicore__ inline uint32_t GetLength() const;
    __aicore__ inline void SetBufferLen(uint32_t dataLen);
    __aicore__ inline void SetUserTag(const TTagType tag);
    __aicore__ inline TTagType GetUserTag() const;
    ...
    __aicore__ inline void SetShapeInfo(const ShapeInfo& shapeInfo);
    __aicore__ inline ShapeInfo GetShapeInfo() const;
    ...
};
```

## Template Parameters<a name="section116801320102618"></a>

**Table 1** Template parameter description

<a name="table13588175515344"></a>
<table><thead align="left"><tr id="row1160915519346"><th class="cellrowborder" valign="top" width="21.8%" id="mcps1.2.3.1.1"><p id="p9609105553412"><a name="p9609105553412"></a><a name="p9609105553412"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="78.2%" id="mcps1.2.3.1.2"><p id="p156091955143419"><a name="p156091955143419"></a><a name="p156091955143419"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1545073919457"><td class="cellrowborder" valign="top" width="21.8%" headers="mcps1.2.3.1.1 "><p id="p1745103924512"><a name="p1745103924512"></a><a name="p1745103924512"></a>T</p></td>
<td class="cellrowborder" valign="top" width="78.2%" headers="mcps1.2.3.1.2 "><p id="zh-cn_topic_0000001441184464_p1742518920145"><a name="zh-cn_topic_0000001441184464_p1742518920145"></a><a name="zh-cn_topic_0000001441184464_p1742518920145"></a>Type **T** can support basic data types as well as the <a href="../../aux_data_structures/TensorTrait/TensorTrait.md">TensorTrait</a> type, but must comply with the data type support of the **LocalTensor** API being used.</p>
<p id="p2740141254519"><a name="p2740141254519"></a><a name="p2740141254519"></a>In particular, for constructors that return a Tensor object based on a specified logical position/address/length, type **T** supports only basic data types.</p></td>
</tr>
</tbody>
</table>
