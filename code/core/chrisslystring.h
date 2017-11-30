#ifndef CHRISSLYSTRING_H_
#define CHRISSLYSTRING_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::String

    (C) 2013 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace core
{

class String
{
public:
    /// default constructor
    String();
    /// copy constructor
    String(const String& rhs);
    /// construct from const char*
    String(const char* const cStr);
    /// destructor
    ~String();
    /// assignment operator
    void operator = (const String& rhs);
    /// assign from const char*
    void operator = (const char* const cStr);
    /// return size of the string
    unsigned int Size() const;
    /// returns a pointer to an array that contains a null-terminated sequence of characters
    const char* const C_Str() const;
    /// set as char ptr
    void Set(const char* const ptr);
    /// set as char ptr, with explicit length
    void Set(const char* const ptr, unsigned int length);

private:
    /// free allocated memory
    void Delete();

    char* data;
    unsigned int size;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
