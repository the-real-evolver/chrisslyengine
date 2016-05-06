#ifndef HARDWAREVERTEXBUFFERBASE_H_
#define HARDWAREVERTEXBUFFERBASE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::HardwareVertexBufferBase

    (C) 2016 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class HardwareVertexBufferBase
{
public:
    /// construct from vertex count and vertex size
    HardwareVertexBufferBase(unsigned int numVertices, unsigned int bytesPerVertex);
    /// destructor
    virtual ~HardwareVertexBufferBase();
    /// lock the entire buffer
    virtual void* Lock();
    /// unlock the buffer
    virtual void Unlock();
    /// gets the size in bytes of a single vertex in this buffer
    unsigned int GetBytesPerVertex() const;
    /// get the number of vertices in this buffer
    unsigned int GetNumVertices() const;
    /// set the number of vertices in this buffer
    void SetNumVertices(unsigned int numVertices);

private:
    /// default constructor
    HardwareVertexBufferBase();

    unsigned int numVertices;
    unsigned int bytesPerVertex;
    void* vertexBuffer;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
