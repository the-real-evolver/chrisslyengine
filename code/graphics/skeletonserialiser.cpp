//------------------------------------------------------------------------------
//  skeletonserialiser.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "skeletonserialiser.h"
#include "skeleton.h"
#include "fswrapper.h"
#include "debug.h"
#include "stb_c_lexer.h"

using namespace chrissly::core;
using namespace chrissly::graphics;

/// defines the state of the serialiser
enum skeleton_parser_state
{
    parse_root,
    parse_skeleton,
    parse_bone,
    parse_animation,
    parse_track,
    parse_keyframe
};

static skeleton_parser_state parser_state = parse_root;
static char text_buffer[65536U] = {'\0'};
static stb_lexer lexer = {};

#define CLEX_PARSE_ERROR 257

//------------------------------------------------------------------------------
/**
*/
static int
parse_signed_int()
{
    if (0 == stb_c_lexer_get_token(&lexer)) {return 0;}
    if ('-' == lexer.token)
    {
        if (0 == stb_c_lexer_get_token(&lexer)) {return 0;}
        return -(int)lexer.int_number;
    }
    return (int)lexer.int_number;
}

//------------------------------------------------------------------------------
/**
*/
static float
parse_float()
{
    if (0 == stb_c_lexer_get_token(&lexer)) {return 0.0f;}
    if ('-' == lexer.token)
    {
        if (0 == stb_c_lexer_get_token(&lexer)) {return 0.0f;}
        return -(float)lexer.real_number;
    }
    return (float)lexer.real_number;
}

//------------------------------------------------------------------------------
/**
*/
static Matrix4
parse_matrix4()
{
    Matrix4 m;
    m[0U][0U] = parse_float(); m[0U][1U] = parse_float(); m[0U][2U] = parse_float(); m[0U][3U] = parse_float();
    m[1U][0U] = parse_float(); m[1U][1U] = parse_float(); m[1U][2U] = parse_float(); m[1U][3U] = parse_float();
    m[2U][0U] = parse_float(); m[2U][1U] = parse_float(); m[2U][2U] = parse_float(); m[2U][3U] = parse_float();
    m[3U][0U] = parse_float(); m[3U][1U] = parse_float(); m[3U][2U] = parse_float(); m[3U][3U] = parse_float();
    return m;
}

//------------------------------------------------------------------------------
/**
*/
void
ce_graphics_import_skeleton(char const* const file_path, Mesh* const mesh)
{
    FileHandle fd = FSWrapper::Open(file_path, ReadAccess, Buffer, 0777);
    unsigned int file_size = FSWrapper::GetFileSize(fd);
    void* file_buffer = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, file_size);
    CE_ASSERT(file_buffer != NULL, "ce_graphics_import_skeleton(): failed to allocate '%i' bytes\n", file_size);
    FSWrapper::Read(fd, file_buffer, file_size);
    FSWrapper::Close(fd);

    memset(&lexer, 0, sizeof(lexer));
    memset(&text_buffer, 0, sizeof(text_buffer));
    stb_c_lexer_init(&lexer, (char*)file_buffer, (char*)((uintptr_t)file_buffer + file_size), text_buffer, sizeof(text_buffer));
    parser_state = parse_root;

    Matrix4 matrix = Matrix4::ZERO, inverse_model_matrix = Matrix4::ZERO;
    float time = 0.0f;

    Skeleton* skeleton = NULL;
    int bone_index = -1, parent_bone_index = -1, num_keyframes = 0;

    String animation_name;
    Animation* animation = NULL;
    BoneAnimationTrack* bone_track = NULL;

    while (stb_c_lexer_get_token(&lexer) != 0)
    {
        if (CLEX_PARSE_ERROR == lexer.token)
        {
            break;
        }

        switch (parser_state)
        {
            case parse_root:
                if (0 == strcmp(lexer.string, "skeleton"))
                {
                    if (0 == stb_c_lexer_get_token(&lexer)) {return;}
                    if ('{' == lexer.token)
                    {
                        parser_state = parse_skeleton;
                    }
                }
                else if (0 == strcmp(lexer.string, "animation"))
                {
                    if (0 == stb_c_lexer_get_token(&lexer)) {return;}
                    animation_name.Set(lexer.string, lexer.string_len);
                    if (0 == stb_c_lexer_get_token(&lexer)) {return;}
                    if ('{' == lexer.token)
                    {
                        parser_state = parse_animation;
                    }
                }
                break;
            case parse_skeleton:
                if ('}' == lexer.token)
                {
                    parser_state = parse_root;
                }
                else if (0 == strcmp(lexer.string, "num_bones"))
                {
                    skeleton = CE_NEW Skeleton(parse_signed_int());
                    mesh->SetSkeleton(skeleton);
                }
                else if (0 == strcmp(lexer.string, "bone"))
                {
                    if (0 == stb_c_lexer_get_token(&lexer)) {return;} // bone name
                    if (0 == stb_c_lexer_get_token(&lexer)) {return;}
                    if ('{' == lexer.token)
                    {
                        parser_state = parse_bone;
                    }
                }
                break;
            case parse_bone:
                if ('}' == lexer.token)
                {
                    skeleton->SetBone(bone_index, parent_bone_index, matrix, inverse_model_matrix);
                    parser_state = parse_skeleton;
                }
                else if (0 == strcmp(lexer.string, "index"))
                {
                    bone_index = parse_signed_int();
                }
                else if (0 == strcmp(lexer.string, "parent"))
                {
                    parent_bone_index = parse_signed_int();
                }
                else if (0 == strcmp(lexer.string, "local_matrix"))
                {
                    matrix = parse_matrix4();
                }
                else if (0 == strcmp(lexer.string, "inv_model_matrix"))
                {
                    inverse_model_matrix = parse_matrix4();
                }
                break;
            case parse_animation:
                if ('}' == lexer.token)
                {
                    parser_state = parse_root;
                }
                else if (0 == strcmp(lexer.string, "length"))
                {
                    animation = mesh->CreateAnimation(animation_name.C_Str(), parse_float());
                }
                else if (0 == strcmp(lexer.string, "num_keyframes"))
                {
                    num_keyframes = parse_signed_int();
                }
                else if (0 == strcmp(lexer.string, "track"))
                {
                    if (0 == stb_c_lexer_get_token(&lexer)) {return;} // track name
                    if (0 == stb_c_lexer_get_token(&lexer)) {return;}
                    if ('{' == lexer.token)
                    {
                        bone_track = animation->CreateBoneTrack((unsigned int)num_keyframes);
                        parser_state = parse_track;
                    }
                }
                break;
            case parse_track:
                if ('}' == lexer.token)
                {
                    parser_state = parse_animation;
                }
                else if (0 == strcmp(lexer.string, "key"))
                {
                    if (0 == stb_c_lexer_get_token(&lexer)) {return;}
                    if ('{' == lexer.token)
                    {
                        parser_state = parse_keyframe;
                    }
                }
                break;
            case parse_keyframe:
                if ('}' == lexer.token)
                {
                    bone_track->CreateKeyFrame(time, matrix);
                    parser_state = parse_track;
                }
                else if (0 == strcmp(lexer.string, "time"))
                {
                    time = parse_float();
                }
                else if (0 == strcmp(lexer.string, "local_matrix"))
                {
                    matrix = parse_matrix4();
                }
                break;
        }
    }

    CE_FREE(file_buffer);
}
