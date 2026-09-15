# GlobalTensor Overview<a name="ZH-CN_TOPIC_0000002132084952"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:55:52.515Z -->

**GlobalTensor** is used to store global data in Global Memory (external storage).

The public member functions of **GlobalTensor** are as follows. Type **T** supports basic data types and the [TensorTrait](../../aux_data_structures/TensorTrait/TensorTrait.md) type, but must comply with the data type support of the instruction that uses this **GlobalTensor**.

```cpp
template <typename T> class GlobalTensor : public BaseGlobalTensor<T> {
public:
    // PrimT is used to extract the LiteType basic data type from TensorTrait when T is passed as the TensorTrait type.
    using PrimType = PrimT<T>;
    // Constructor.
    __aicore__ inline GlobalTensor<T>() {}
    // Initialize GlobalTensor.
    __aicore__ inline void SetGlobalBuffer(__gm__ PrimType* buffer, uint64_t bufferSize); 
    __aicore__ inline void SetGlobalBuffer(__gm__ PrimType* buffer);
    // Obtain the address of the global data.
    __aicore__ inline const __gm__ PrimType* GetPhyAddr() const;
    __aicore__ inline __gm__ PrimType* GetPhyAddr(const uint64_t offset) const;
    // Obtain the value at the corresponding offset of GlobalTensor.
    __aicore__ inline __inout_pipe__(S) PrimType GetValue(const uint64_t offset) const; 
    // Obtain the reference to the element at a certain index.
    __aicore__ inline __inout_pipe__(S) __gm__ PrimType& operator()(const uint64_t offset) const;
    // Set the value at the corresponding offset of GlobalTensor.
    __aicore__ inline void SetValue(const uint64_t offset, PrimType value);
    // Obtain the number of elements in GlobalTensor.
    __aicore__ inline uint64_t GetSize() const;
    // Return the GlobalTensor at the specified offset.
    __aicore__ inline GlobalTensor operator[](const uint64_t offset) const; 
    // Set the shape information of GlobalTensor.
    __aicore__ inline void SetShapeInfo(const ShapeInfo& shapeInfo);
    // Obtain the shape information of GlobalTensor.
    __aicore__ inline ShapeInfo GetShapeInfo() const;
    // Set the mode for writing GlobalTensor to the L2 cache.
    template<CacheRwMode rwMode = CacheRwMode::RW>
    __aicore__ inline void SetL2CacheHint(CacheMode mode);
    // Reinterpret the current GlobalTensor as a new type specified by the user.
    template <typename CAST_T>
    __aicore__ inline GlobalTensor<CAST_T> ReinterpretCast() const;
    ...
};
```
