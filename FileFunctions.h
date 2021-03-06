//
// Created by user on 07.11.2021.
//

#ifndef PROPERTY2_FILEFUNCTIONS_H
#define PROPERTY2_FILEFUNCTIONS_H

#include "Result.h"

enum class TOpenFileMode{ Read, Write, Append };
using TPtrFile = std::shared_ptr<FILE>;

ENUM_CLASS(TFileResult, Ok, ErrorOpen, ErrorRead, ErrorWrite);

TPtrFile OpenFile(const TString& path, TOpenFileMode mode = TOpenFileMode::Read);

template<typename T>
TResult WriteFile(const T& value, const TPtrFile& file)
{
    return std::fwrite(&value, sizeof(T), 1, file.get()) == 1 ? TResult() : TFileResult::ErrorWrite;
}

template<typename T>
TResult WriteFile(const T& value, size_t index, size_t count, const TPtrFile& file)
{
    return std::fwrite(&value[index], sizeof(value[index]), count, file.get()) == count ? TResult() : TFileResult::ErrorWrite;
}

template<typename T>
TResult ReadFile(T& value, const TPtrFile& file)
{
    return std::fread(&value, sizeof(T), 1, file.get()) == 1 ? TResult() : TFileResult::ErrorRead;
}

template<typename T>
TResult ReadFile(T& value, size_t index, size_t count, const TPtrFile& file)
{
    return std::fread(&value[index], sizeof(value[index]), count, file.get()) == 1 ? TResult() : TFileResult::ErrorRead;
}

TResult WriteSizeStringInFile(const TString& value, const TPtrFile& file);
TResult ReadSizeStringFromFile(TString& value, const TPtrFile& file);
std::wstring WStringFromUtf8(const TString &value);

TString Merge(const TVecString& values, typename TString::value_type delim);

TResult FileLines(TVecString& lines, const TString& path);

#endif //PROPERTY2_FILEFUNCTIONS_H
