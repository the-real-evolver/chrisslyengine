//------------------------------------------------------------------------------
//  androidfswrapper.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "androidfswrapper.h"
#include "core/chrisslystring.h"
#include "core/debug.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <regex.h>

namespace chrissly
{

static AAssetManager* AssetManager = NULL;
static char* InternalDataPath = NULL;

//------------------------------------------------------------------------------
/**
*/
core::FileHandle
AndroidFSWrapper::Open(const char* const fileName, core::AccessMode mode, core::AccessPattern pattern, int permission)
{
    core::String name(fileName);
    name.SubstituteString("\\\\", "/");
    name.SubstituteString("\\", "/");
    core::FileHandle fileHandle;
    if (strstr(fileName, InternalDataPath) != NULL)
    {
        fileHandle.fileHandle = fopen(name.C_Str(), AndroidFSWrapper::Get(mode));
        CE_ASSERT(fileHandle.fileHandle != NULL, "FSWrapper::Open(): can't open file '%s'\n", name.C_Str());
    }
    else
    {
        fileHandle.assetHandle = AAssetManager_open(AssetManager, name.C_Str(), AndroidFSWrapper::Get(pattern));
        CE_ASSERT(fileHandle.assetHandle != NULL, "FSWrapper::Open(): can't open file '%s'\n", name.C_Str());
    }
    return fileHandle;
}

//------------------------------------------------------------------------------
/**
*/
void
AndroidFSWrapper::Close(core::FileHandle fileHandle)
{
    if (fileHandle.fileHandle != NULL)
    {
        int result = fclose(fileHandle.fileHandle);
        CE_ASSERT(0 == result, "FSWrapper::Close(): failed to close file '%p'\n", fileHandle.fileHandle);
    }
    else if (fileHandle.assetHandle != NULL)
    {
        AAsset_close(fileHandle.assetHandle);
    }
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
AndroidFSWrapper::GetFileSize(core::FileHandle fileHandle)
{
    unsigned int fileSize = 0U;
    if (fileHandle.fileHandle != NULL)
    {
        if (fseek(fileHandle.fileHandle, 0, SEEK_END) >= 0) fileSize = (unsigned int)ftell(fileHandle.fileHandle);
        fseek(fileHandle.fileHandle, 0, SEEK_SET);
    }
    else if (fileHandle.assetHandle != NULL)
    {
        fileSize = (unsigned int)AAsset_getLength(fileHandle.assetHandle);
    }
    return fileSize;
}

//------------------------------------------------------------------------------
/**
*/
int
AndroidFSWrapper::Read(core::FileHandle fileHandle, void* const buf, unsigned int numBytes)
{
    CE_ASSERT(numBytes > 0U, "FSWrapper::Read(): requested number of bytes to read is '%u', has to be greater than zero", numBytes);
    int bytesRead = 0;
    if (fileHandle.fileHandle != NULL)
    {
        bytesRead = (int)fread(buf, 1U, (size_t)numBytes, fileHandle.fileHandle);
    }
    else if (fileHandle.assetHandle != NULL)
    {
        bytesRead = (int)AAsset_read(fileHandle.assetHandle, buf, (size_t)numBytes);
    }
    CE_ASSERT((bytesRead != 0 && (int)numBytes == bytesRead) || bytesRead == 0, "FSWrapper::Read(): could not read the requested number of bytes (requested: '%u' read: '%d')\n", numBytes, bytesRead);
    return bytesRead;
}

//------------------------------------------------------------------------------
/**
*/
void
AndroidFSWrapper::Seek(core::FileHandle fileHandle, int offset, core::SeekOrigin whence)
{
    if (fileHandle.fileHandle != NULL)
    {
        int result = fseek(fileHandle.fileHandle, (long)offset, AndroidFSWrapper::Get(whence));
        CE_ASSERT(result == 0, "FSWrapper::Seek(): failed to seek file '%p'\n", fileHandle.fileHandle);
    }
    else if (fileHandle.assetHandle != NULL)
    {
        off_t result = AAsset_seek(fileHandle.assetHandle, (off_t)offset, AndroidFSWrapper::Get(whence));
        CE_ASSERT(result != -1, "FSWrapper::Seek(): failed to seek file '%p'\n", fileHandle.assetHandle);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AndroidFSWrapper::Write(core::FileHandle fileHandle, const void *buf, unsigned int numBytes)
{
    CE_ASSERT(numBytes > 0U, "FSWrapper::Write(): requested number of bytes to write is '%u', has to be greater than zero", numBytes);
    if (fileHandle.fileHandle != NULL)
    {
        size_t bytesWritten = fwrite(buf, 1U, (size_t)numBytes, fileHandle.fileHandle);
        CE_ASSERT((unsigned int)bytesWritten == numBytes, "FSWrapper::Write(): failed to write to file '%p', bytes supplied '%u', bytes written '%u'\n", fileHandle.fileHandle, numBytes, bytesWritten);
    }
    else if (fileHandle.assetHandle != NULL)
    {
        CE_ASSERT(false, "FSWrapper::Write(): AAssets can't be modified at runtime. They are read-only\n");
    }
}

//------------------------------------------------------------------------------
/**
*/
bool
AndroidFSWrapper::FileExists(const char* const fileName)
{
    core::String name(fileName);
    name.SubstituteString("\\\\", "/");
    name.SubstituteString("\\", "/");
    core::FileHandle fileHandle;
    fileHandle.fileHandle = fopen(name.C_Str(), "r");
    if (fileHandle.fileHandle != NULL)
    {
        fclose(fileHandle.fileHandle);
        return true;
    }
    fileHandle.assetHandle = AAssetManager_open(AssetManager, name.C_Str(), AASSET_MODE_UNKNOWN);
    if (NULL != fileHandle.assetHandle)
    {
        AAsset_close(fileHandle.assetHandle);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
AndroidFSWrapper::RemoveFile(const char* const fileName)
{
    core::String name(fileName);
    name.SubstituteString("\\\\", "/");
    name.SubstituteString("\\", "/");
    if (strstr(fileName, InternalDataPath) != NULL)
    {
        return (0 == remove(name.C_Str()));
    }
    else
    {
        CE_ASSERT(false, "FSWrapper::RemoveFile(): AAssets can't be removed. They are read-only\n");
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
int
AndroidFSWrapper::ListFiles(const char* const path, const char* const pattern, unsigned int maxNumFiles, char filesOut[][CE_MAX_PATH])
{
    CE_ASSERT(path != NULL && pattern != NULL, "FSWrapper::ListFiles(): invalid pointer passed\n");

    core::String name(path);
    name.SubstituteString("\\\\", "/");
    name.SubstituteString("\\", "/");

    // mimic the behaviour of the WinAPI FindFirstFile() function
    core::String patternRegex(pattern);
    patternRegex.SubstituteString("*", ".*");
    patternRegex.SubstituteString("?", ".");
    regex_t wildcard;
    if (regcomp(&wildcard, patternRegex.C_Str(), REG_BASIC) != 0) return 0;

    unsigned int fileIndex = 0U;
    if (strstr(path, InternalDataPath) != NULL)
    {
        DIR* dir;
        struct dirent* entry;
        dir = opendir(name.C_Str());
        if (NULL == dir) return 0;
        while ((entry = readdir(dir)) != NULL && fileIndex < maxNumFiles)
        {
            if (0 == regexec(&wildcard, entry->d_name, 0U, NULL, 0))
            {
                strncpy(filesOut[fileIndex], entry->d_name, CE_MAX_PATH);
                ++fileIndex;
            }
        }
        closedir(dir);
    }
    else
    {
        AAssetDir* dir = AAssetManager_openDir(AssetManager, name.C_Str());
        const char* assetName = NULL;
        while ((assetName = AAssetDir_getNextFileName(dir)) != NULL && fileIndex < maxNumFiles)
        {
            if (0 == regexec(&wildcard, assetName, 0U, NULL, 0))
            {
                strncpy(filesOut[fileIndex], assetName, CE_MAX_PATH);
                ++fileIndex;
            }
        }
        AAssetDir_close(dir);
    }

    regfree(&wildcard);

    return (int)fileIndex;
}

//------------------------------------------------------------------------------
/**
*/
const char*
AndroidFSWrapper::GetAppDataDirectory()
{
    return InternalDataPath;
}

//------------------------------------------------------------------------------
/**
*/
void
AndroidFSWrapper::_Initialise(ANativeActivity* const nativeActivity)
{
    AssetManager = nativeActivity->assetManager;
    InternalDataPath = (char*)nativeActivity->internalDataPath;
    CE_LOG("AndroidFSWrapper::_Initialise()\n");
}

//------------------------------------------------------------------------------
/**
*/
const char*
AndroidFSWrapper::Get(core::AccessMode mode)
{
    switch (mode)
    {
        case core::READ_ACCESS:         return "rb";
        case core::WRITE_ACCESS:        return "wb";
        case core::READ_WRITE_ACCESS:   return "wb+";
        case core::APPEND_ACCESS:       return "ab+";
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal AccessMode '%i'\n", mode);
    }

    return NULL;
}

//------------------------------------------------------------------------------
/**
*/
int
AndroidFSWrapper::Get(core::SeekOrigin origin)
{
    switch (origin)
    {
        case core::BEGIN:   return SEEK_SET;
        case core::CURRENT: return SEEK_CUR;
        case core::END:     return SEEK_END;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal SeekOrigin '%i'\n", origin);
    }

    return 0;
}

//------------------------------------------------------------------------------
/**
*/
int
AndroidFSWrapper::Get(core::AccessPattern pattern)
{
    switch (pattern)
    {
        case core::UNKNOWN:   return AASSET_MODE_UNKNOWN;
        case core::RANDOM:    return AASSET_MODE_RANDOM;
        case core::STREAMING: return AASSET_MODE_STREAMING;
        case core::BUFFER:    return AASSET_MODE_BUFFER;
        default: CE_ASSERT(false, "FSWrapper::Get(): illegal AccessPattern '%i'\n", pattern);
    }

    return -1;
}

} // namespace chrissly
