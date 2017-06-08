#ifndef GPUPROGRAMPARAMS_H_
#define GPUPROGRAMPARAMS_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::GpuProgramParameters

    (C) 2012 Christian Bleicher
*/
#include "hashtable.h"
#include "matrix4.h"
#include "quaternion.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
    /// enumeration of the types of constant we may encounter in programs
    enum GpuConstantType
    {
        GCT_FLOAT1 = 1,
        GCT_FLOAT2 = 2,
        GCT_FLOAT3 = 3,
        GCT_FLOAT4 = 4,
        GCT_SAMPLER1D = 5,
        GCT_SAMPLER2D = 6,
        GCT_SAMPLER3D = 7,
        GCT_SAMPLERCUBE = 8,
        GCT_SAMPLER1DSHADOW = 9,
        GCT_SAMPLER2DSHADOW = 10,
        GCT_SAMPLER2DARRAY = 11,
        GCT_MATRIX_2X2 = 12,
        GCT_MATRIX_2X3 = 13,
        GCT_MATRIX_2X4 = 14,
        GCT_MATRIX_3X2 = 15,
        GCT_MATRIX_3X3 = 16,
        GCT_MATRIX_3X4 = 17,
        GCT_MATRIX_4X2 = 18,
        GCT_MATRIX_4X3 = 19,
        GCT_MATRIX_4X4 = 20,
        GCT_INT1 = 21,
        GCT_INT2 = 22,
        GCT_INT3 = 23,
        GCT_INT4 = 24,
        GCT_UNKNOWN = 99
    };

    /// information about predefined program constants
    struct GpuConstantDefinition
    {
        /// constructor
        GpuConstantDefinition() : constType(GCT_UNKNOWN), location(-1), buffer(NULL), size(0U), arraySize(0U) {}
        /// data type
        GpuConstantType constType;
        /// uniform location
        int location;
        /// memoryblock holding the data
        void* buffer;
        /// size of the memoryblock that holds the data in bytes
        unsigned int size;
        /// size of the array
        unsigned int arraySize;
    };

    /// struct collecting together the information for named constants
    struct GpuNamedConstants
    {
        /// constructor
        GpuNamedConstants();
        /// destructor
        ~GpuNamedConstants();
        /// map of parameter names to GpuConstantDefinition
        ce_hash_table map;
    };

    /// collects together the program parameters used for a GpuProgram
    class GpuProgramParameters
    {
    public:
        enum AutoConstantType
        {
            /// the current world matrix
            ACT_WORLD_MATRIX = 0,
            /// the current view matrix
            ACT_VIEW_MATRIX,
            /// the current projection matrix
            ACT_PROJECTION_MATRIX,
            /// the current world, view & projection matrices concatenated
            ACT_WORLDVIEWPROJ_MATRIX,
            /// provides the current transform matrix of the texture unit
            ACT_TEXTURE_MATRIX,
            /// specifies the morph weight when blending between two vertices
            ACT_MORPH_WEIGHT,
            /// the  number of AutoConstantType elements
            ACT_COUNT
        };

        /// default constructor
        GpuProgramParameters();
        /// destructor
        ~GpuProgramParameters();

        /// sets a single value constant floating-point parameter to the program
        void SetNamedConstant(const char* const name, float val);
        /// sets a single value constant integer parameter to the program
        void SetNamedConstant(const char* const name, int val);
        /// sets a Vector3 parameter to the program
        void SetNamedConstant(const char* const name, const core::Vector3& vec);
        /// sets a list of Vector3 parameters to the program
        void SetNamedConstant(const char* const name, const core::Vector3* const vec, unsigned int numEntries);
        /// sets a Quaternion (Vector4) parameter to the program
        void SetNamedConstant(const char* const name, const core::Quaternion& q);
        /// sets a Matrix4 parameter to the program
        void SetNamedConstant(const char* const name, const core::Matrix4& m);
        /// sets a list of Matrix4 parameters to the program
        void SetNamedConstant(const char* const name, const core::Matrix4* const m, unsigned int numEntries);
        /// sets a single matrix constant which will automatically be updated by the system
        void SetAutoConstant(AutoConstantType autoType, const core::Matrix4& m);
        /// sets a single float constant which will automatically be updated by the system
        void SetAutoConstant(AutoConstantType autoType, float val);

        /// internal method for providing a link to a name->definition map for parameters
        void _SetNamedConstants(GpuNamedConstants* const constantMap);
        /// internal method for providing a link to a auto constant definition for parameters
        void _SetAutoConstant(AutoConstantType autoType, GpuConstantDefinition* const def);

        /// gets the AutoConstantType enum value if the given string matches an auto type, otherwise returns ACT_COUNT
        static AutoConstantType AutoConstantTypeFromString(const char* const name);

    private:
        GpuNamedConstants* constantDefs;
        GpuConstantDefinition* autoConstants[ACT_COUNT];
    };

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
