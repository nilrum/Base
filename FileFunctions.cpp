//
// Created by user on 07.11.2021.
//

#include "FileFunctions.h"

#include "Types.h"
#ifdef _WIN32
#include <Windows.h>
#endif

std::shared_ptr<FILE> OpenFile(const std::string& path, bool isRead)
{
    FILE* f = nullptr;
#ifndef _WIN32
    f = std::fopen(path.c_str(), (isRead) ? "rb" : "wb");
#else
    std::wstring pathW;
    pathW.resize(path.size());

    int newSize = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), path.size(), nullptr, 0);
    pathW.resize(newSize);
    MultiByteToWideChar(CP_UTF8, 0, path.c_str(), path.size(), pathW.data(), path.size());
    _wfopen_s(&f, pathW.c_str(), (isRead) ? L"rb" : L"wb");
#endif
    if(f != nullptr)
        return std::shared_ptr<FILE>(f, [](FILE* file){ if(file != nullptr) std::fclose(file); });
    else
        return std::shared_ptr<FILE>();
}

std::shared_ptr<FILE> OpenFile(const std::string& path, TOpenFileMode mode)
{
    FILE* f = nullptr;
#ifndef _WIN32
    f = std::fopen(path.c_str(), mode == TOpenFileMode::Read ? "rb" :( mode == TOpenFileMode::Write ? "wb" : "ab" ));
#else
    std::wstring pathW = WStringFromUtf8(path);
    _wfopen_s(&f, pathW.c_str(), mode == TOpenFileMode::Read ? L"rb" :( mode == TOpenFileMode::Write ? L"wb" : L"ab" ));
#endif
    if(f != nullptr)
        return std::shared_ptr<FILE>(f, [](FILE* file)
        {
            if(file != nullptr)
                std::fclose(file);
        });
    else
        return std::shared_ptr<FILE>();
}

TResult WriteSizeStringInFile(const TString& value, const TPtrFile& file)
{
    auto res = WriteFile<size_t>(value.size(), file);
    if(res.IsError()) return res;
    return std::fwrite(STR(value), sizeof(TString::value_type), value.size(), file.get()) == value.size() ? TResult() : TFileResult::ErrorWrite;
}


TResult ReadSizeStringFromFile(TString& value, const TPtrFile& file)
{
    size_t size = 0;
    auto res = ReadFile(size, file);
    if(size == 0)
    {
        value = TString();
        return TResult();
    }
    value.resize(size);
    return std::fread(&value[0], sizeof(TString::value_type), value.size(), file.get()) == 1 ? TResult() : TFileResult::ErrorRead;
}

static const char* trans[] = {"A", "B", "V", "G", "D", "E", "ZH", "Z", "I", "I", "K", "L", "M", "N", "O",
                              "P", "R", "S", "T", "U", "F", "H", "TS", "CH", "SH", "SCH", "", "Y", "", "E", "YU", "YA",
                              "a", "b", "v", "g", "d", "e", "zh", "z", "i", "i", "k", "l", "m", "n", "o",
                              "p", "r", "s", "t", "u", "f", "h", "ts", "ch", "sh", "sch", "", "y", "", "e", "yu", "ya",
};

TString Transliteration(TString utf8text)
{
    TString res;
    for(auto it = utf8text.begin(); it != utf8text.end(); it++)
    {
        unsigned char a = *it;
        if (a < 0xC0)//???????????? ???? ??????????????
        {
            res.push_back(a);
        }
        else
        {
            unsigned short unic = (a & 0x3F) << 6;
            it++;
            a = *it;
            unic = unic | (0x7F & a);//???????????????? ???????????? ?? ??????????????
            if(unic >= 0x410 && unic <= 0x044F)//?? = 0x0410 - ?? = 0x042F, ?? = 0x0430 - ?? = 0x044F
                res.insert(res.length(), trans[unic - 0x410]);
            else if(unic == 0x401) res.insert(res.length(), "EO");//?? ?????????? ????????????????
            else if(unic == 0x451) res.insert(res.length(), "eo");//??
        }
    }
    return res;
}

TString Merge(const TVecString& values, typename TString::value_type delim)
{
    TString res;
    for(const auto& r : values)
    {
        if(res.empty() == false) res.push_back(delim);
        res.insert(res.end(), r.begin(), r.end());
    }
    return res;
}

std::wstring WStringFromUtf8(const TString &value)
{
    std::wstring res;
#ifdef _WIN32

    res.resize(value.size());

    int newSize = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), value.size(), nullptr, 0);
    res.resize(newSize);
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(), value.size(), res.data(), value.size());
#endif
    return res;
}

TResult FileLines(TVecString &lines, const TString &path)
{
    auto file = OpenFile(path);
    if(file == nullptr) return TFileResult::ErrorOpen;
    TString line;
    auto c = std::fgetc(file.get());
    while(c != EOF)
    {
        if(c == '\n' || c == '\r')//???????? ???????????? ?????????? ????????????
        {
            if(line.empty() == false)
            {
                lines.push_back(line);
                line.clear();
            }
        }
        else
            line.push_back(c);

        c = std::fgetc(file.get());
    }
    if(line.empty() == false)
        lines.push_back(line);
    return TResult();
}
