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
    String(const char* cStr);
    /// destructor
    ~String();
    /// assignment operator
    void operator=(const String& rhs);
    /// assign from const char*
    void operator=(const char* cStr);
    /// return size of the string
    unsigned int Size();
    /// returns a pointer to an array that contains a null-terminated sequence of characters
    const char* C_Str() const;

private:
    /// set as char ptr
    void Set(const char* ptr);
    /// free allocated memory
    void Delete();

    char* data;
    unsigned int size;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
