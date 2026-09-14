# Reserved APIs<a name="ZH-CN_TOPIC_0000001724165365"></a>

<!-- md-trans-meta sourceCommit=b817dec23ab1f7e681e8aef4846330c1cf74e443 translatedAt=2026-09-06T08:35:02.092Z pushedAt=2026-09-10T08:32:13.027Z -->

All APIs listed in this chapter are reserved APIs. They may be changed or deprecated in the future, and are not recommended for developers to use. Developers do not need to pay attention to them.

## Memory Management and Synchronization Control<a name="section89791646101413"></a>

\_\_aicore\_\_ constexpr Hardware GetPhyType\(TPosition pos\);

## Matrix Computation<a name="section124371048171211"></a>

-   template <typename DstT, typename SrcT, const FixpipeConfig& config = CFG\_ROW\_MAJOR\> void Fixpipe\(const LocalTensor<DstT\>& dstLocal, const LocalTensor<SrcT\>& srcLocal, const FixpipeParamsV220& intriParams\);
-   template <typename DstT, typename SrcT, const FixpipeConfig& config = CFG\_ROW\_MAJOR\> void Fixpipe\(const LocalTensor<DstT\>& dstLocal, const LocalTensor<SrcT\>& srcLocal, const LocalTensor<uint64\_t\>& cbufWorkspace, const FixpipeParamsV220& intriParams\);
