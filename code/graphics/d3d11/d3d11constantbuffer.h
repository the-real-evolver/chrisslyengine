#ifndef D3D11CONSTANTBUFFER_H_
#define D3D11CONSTANTBUFFER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::D3D11ConstantBuffer

    (C) 2016 Christian Bleicher
*/
#include "dynamicarray.h"
#include "d3d11config.h"

//------------------------------------------------------------------------------
namespace chrissly
{

class D3D11ConstantBuffer
{
    friend class D3D11GpuProgram;
public:
    /// constructor with size, slot index and the number of constants
    D3D11ConstantBuffer(UINT byteWidth, UINT slot, unsigned int numConstants);
    /// destructor
    ~D3D11ConstantBuffer();
    /// binds the buffer to the internal slot
    void Bind();
    /// updates the buffer from the constant definitions
    void UpdateConstants();

private:
    /// private default constructor
    D3D11ConstantBuffer();

    ID3D11Buffer* buffer;
    UINT slot;
    mutable ce_dynamic_array constants;
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
