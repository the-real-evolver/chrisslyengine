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

/// enum describing buffer usage
enum Usage
{
    /**
        Static buffer which the application rarely modifies once created.
        Modifying the contents of this buffer will involve a performance hit.
    */
    HBU_STATIC = 1,
    /**
        Indicates the application would like to modify this buffer with the
        CPU fairly often. Buffers created with this flag will typically end
        up in main memory rather than video memory.
    */
    HBU_DYNAMIC = 2
};

class HardwareVertexBufferBase
{
public:
    /// construct from vertex count and vertex size
    HardwareVertexBufferBase(unsigned int numVertices, unsigned int bytesPerVertex, Usage usage);
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

protected:
    /// default constructor
    HardwareVertexBufferBase();

    unsigned int numVertices;
    unsigned int bytesPerVertex;
    void* vertexBuffer;
    Usage usage;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
