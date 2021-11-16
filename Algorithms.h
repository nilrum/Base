//
// Created by user on 07.11.2021.
//

#ifndef PROPERTYEXAMPLE_ALGORITHMS_H
#define PROPERTYEXAMPLE_ALGORITHMS_H

#include <algorithm>
#include <cmath>
#include <stdarg.h>

template<typename T>
std::vector<T> Split(const T& value, typename T::value_type delim)
{
    size_t begin = 0;
    size_t count = 0;
    std::vector<T> rez;
    for(auto i : value)
    {
        if(i == delim)
        {
            rez.emplace_back(value, begin, count);
            begin = begin + count + 1;//+1 пропускаем сам символ разделитель
            count = 0;
        }
        else
            count++;
    }
    if(count) rez.emplace_back(value, begin, count);
    return rez;
}

template<typename T>
T Trim(const T& value)
{
    auto isspace = [](char ch) { return std::isspace(static_cast<unsigned char>(ch)); };
    auto begin = std::find_if_not(value.begin(), value.end(), isspace);
    auto end = value.end();
    if(begin != end)
        end = std::find_if_not(value.rbegin(), value.rend(), isspace).base();
    return TString(begin, end);
}

template<typename T>
std::vector<T> SplitTrim(const T& value, typename T::value_type delim)
{
    std::vector<T> rez = Split(value, delim);
    for(size_t i = 0; i < rez.size(); i++)
        rez[i] = Trim(rez[i]);
    return rez;
}

template<typename T>
T ToUpperCase(const T& value)
{
    T res = value;
    std::transform(value.begin(), value.end(), res.begin(), ::toupper);
    return res;
}

template<typename T>
T ToLowerCase(const T& value)
{
    T res = value;
    std::transform(value.begin(), value.end(), res.begin(), ::tolower);
    return res;
}

template<typename T>
T TrimBefore(const T& value, typename T::value_type delim, size_t maxCount = 0)
{
    if(value.empty()) return value;
    auto v = std::find(value.rbegin(), value.rend(), delim);
    if(v != value.rend())
        return T{".."} + T{value.begin() + (value.rend() - v - 1), value.end()};//-1 чтобы знак тоже входил
    if(maxCount == 0)
        return value;
    return T{value.begin() + value.size() - maxCount, value.end()};
}

inline bool TryStrToInt(const TString& str, int& value)
{
    try{
        value = std::stoi(str);
        return true;
    }
    catch (...)
    {
        return false;
    }
}



struct TDoubleCheck{

    static bool Equal(const double& one, const double& two)
    {
        if(one > 0. && two > 0. || one < 0. && two < 0.)
            return std::fabs(one - two) < 0.00001;
        else
            return std::fabs(one + two) < 0.00001;
    }

    static bool Less(const double& one, const double& two)
    {
        if(one > 0. && two > 0. || one < 0. && two < 0.)
            return std::fabs(two) - std::fabs(one) > 0.00001;
        else
            return std::fabs(two + one) > 0.00001;
    }

    static bool Great(const double& one, const double& two)
    {
        return Less(two, one);
    }

    static bool LessEq(const double& one, const double& two)
    {
        return Less(one, two) || Equal(one, two);
    }

    static bool GreatEq(const double& one, const double& two)
    {
        return Great(one, two) || Equal(one, two);
    }

    struct TDoubleLess// : public std::binary_function<double, double, bool>
    {
        bool operator()(const double& x, const double& y) const
        { return Less(x, y); }
    };

    struct TDoubleLessEq// : public std::binary_function<double, double, bool>
    {
        bool operator()(const double& x, const double& y) const
        { return LessEq(x, y); }
    };

    struct TDoubleGreater// : public std::binary_function<double, double, bool>
    {
        bool operator()(const double& x, const double& y) const
        { return Great(x, y); }
    };
    struct TDoubleGreaterEq// : public std::binary_function<double, double, bool>
    {
        bool operator()(const double& x, const double& y) const
        { return GreatEq(x, y); }
    };
};


template<typename TVec>
bool RemoveVal(TVec& vec, const typename TVec::value_type& val)
{
    typename TVec::iterator it = std::find(vec.begin(), vec.end(), val);
    if(it == vec.end()) return false;
    vec.erase(it);
    return true;
}

template<typename TVec>
bool RemoveValFor(TVec& vec, const typename TVec::value_type& val)
{
    for(auto it = vec.begin(); it != vec.end(); it++)
        if(*it == val)
        {
            vec.erase(it);
            return true;
        }
    return false;
}


template<typename TArray>
struct TIndexerVector{
    typename TArray::reference operator() (TArray& array, int index)
    {
        return array[index];
    }
    bool contains(TArray& array, int index) const
    {
        return index < array.size();
    }
};
//Возвращает в зависимости от шаблонного параметра isCol
// либо значение столбца, либо значение строки под индексом index из двумерного массива array
template<typename TArray, bool isCol>
struct TIndexerMatrix{
    typename TArray::value_type::reference operator() (TArray& array, int index)
    {
        if constexpr(isCol)
            return array[indFix][index];
        else
            return array[index][indFix];
    }
    bool contains(TArray& array, int index) const
    {
        if constexpr(isCol)
            return indFix < array.size() && index < array[indFix].size();
        else
            return index < array.size() && indFix < array[index].size();
    }
    size_t indFix = 0;
};
template<typename TArray, typename TIndexer = TIndexerVector<TArray> >
void TakeAndMove(TArray& array, int opos, int npos, TIndexer get = TIndexer())
{
    if(get.contains(array, opos) == false)
        return;
    auto el = get(array, opos);  //сохраняем элемент, который двигаем
    if(el == nullptr) return;
    int chg = opos > npos ? -1 : 1;
    for(int i = opos; i != npos; i += chg)
        get(array, i) = get(array, i + chg);
    get(array, npos) = el;//присваиваем перемещенный элемент
}

inline const char* PtrText(const char* value) { return value; }
inline const char* PtrText(const TString& value) { return STR(value); }

#define STDFORMAT(FRMT, ...) StdFormat(std::snprintf(nullptr, 0, PtrText(FRMT), __VA_ARGS__) + 1, PtrText(FRMT), __VA_ARGS__)

TString Transliteration(TString utf8text);

inline std::string StdFormat(int sizeBuf, const char* frmt,  ...)
{
    va_list args;
    va_start(args, frmt);
    std::vector<char> buf(sizeBuf);
    std::vsnprintf(&buf[0], buf.size(), frmt, args);
    return &buf[0];
}


class TFormatDouble{
private:
    int maxEndCount;//максимум знаков после запятой
    int maxBegCount;//максимум пять знаков перед запятой
    int beginCheck;//для 5ти знаков будет максимальное 1000000
    int endCheck;
    bool isUseUtf8 = true;
    static constexpr uint8_t textPows[8][8] = {
            {0xC3, 0x97, '1', '0', 0xC2, 0xB2, 0},        // * 10 ^ 2
            {0xC3, 0x97, '1', '0', 0xC2, 0xB3, 0},        // 3
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB4, 0},  //4
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB5, 0},  //5
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB6, 0},  //6
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB7, 0},  //7
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB8, 0},  //8
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB9, 0}   //9
    };
public:
    TFormatDouble(int maxEnd = 3, int maxBeg = 6){ Set(maxEnd, maxBeg); }

    void Set(int maxEnd, int maxBeg = 6)
    {
        maxEndCount = maxEnd;
        maxBegCount = std::max(2, maxBeg);

        beginCheck = int(std::pow(10, maxBegCount));
        endCheck = int(std::pow(10, maxEndCount));
    }
    TString Format(double value, double eps)
    {
        if(eps >= 1) return Round(value);
        int begVal = int(value);
        int endVal = int((value - begVal + 0.0000001) * endCheck);
        int chk = 10;
        int count = maxEndCount;
        while(endVal % chk == 0 && count > 0)
        {
            count--;
            chk *= 10;
        }
        return STDFORMAT("%.*f", count, value);
    }

    TString Round(double value) const
    {
        if(std::isnan(value))
            return "nan";
        if(value < beginCheck)
        {
            int begVal = int(value);
            int endVal = int((value - begVal + 0.0000001) * endCheck);

            if(endVal)
            {
                int endCheckVal = endCheck;
                while (endCheckVal > 1)
                {
                    endCheckVal = endCheckVal / 10;
                    /*if ((endVal / endCheckVal) % 10 < 5)
                    {
                        endCheckVal = endCheckVal * 10;
                        break;
                    }*/
                }
                endVal = endVal / endCheckVal;
            }

            if(endVal == 0)
                return std::to_string(begVal);
            else
                return std::to_string(begVal) + "." + std::to_string(std::abs(endVal));
        }
        else
        {
            double begin = beginCheck;
            double val = value / begin;
            int pow = maxBegCount;
            while (val >= 10)
            {
                pow++;
                begin = begin * 10;
                val = value / begin;
            }
            if(isUseUtf8 && pow < 10)
                return Round(val) + TString((char*)textPows[pow - 2]);
            else
                return Round(val) + "*10^" + std::to_string(pow);
        }
    }

    static TString FormatDouble(double value, int maxEnd, int maxBeg = 6)
    {
        return TFormatDouble(maxEnd, maxBeg).Round(value);
    }

};

#define AND_OR_ENUM(TYPE)\
    constexpr bool operator & (TYPE lhs, TYPE rhs) { return static_cast<int>(lhs) & static_cast<int>(rhs); }\
    constexpr TYPE operator & (TYPE lhs, int rhs) { return static_cast<TYPE>(static_cast<int>(lhs) & rhs); }\
    constexpr TYPE operator | (TYPE lhs, TYPE rhs) { return static_cast<TYPE>(static_cast<int>(lhs) | static_cast<int>(rhs)); }\
    constexpr int operator ~(TYPE lhs) { return ~static_cast<int>(lhs); }\
    constexpr int operator *(TYPE lhs) { return static_cast<int>(lhs); }

#define FOR_ENUM(BEGIN, LAST, ...) \
    for(int i = static_cast<int>(BEGIN); i < static_cast<int>(LAST); i++)\
    {\
        auto e = static_cast<decltype(BEGIN)>(i);\
        __VA_ARGS__;\
    }

#define PROXY(FUNC) \
    [this](auto && ...a) { FUNC(std::forward<decltype(a)>(a)...); }

#define PROXY_C(FUNC, LBD_ARGS) \
    [LBD_ARGS](auto && ...a) { FUNC(); }
#endif //PROPERTYEXAMPLE_ALGORITHMS_H
