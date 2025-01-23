#ifndef STDTEMPLATE_H
#define STDTEMPLATE_H

#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>
#include <exception>

using namespace std;

namespace std
{
//---------------------------------------------------------------------------
// Convert a int  to a zero-filled string
inline std::string ToZeroIntString(unsigned int v, int width)
{
    std::stringstream osstream;
    osstream << std::setw(width) << std::setfill('0') << v;
    return osstream.str();
}

//---------------------------------------------------------------------------
// Convert a hexa to a zero-filled hexa string
inline std::string ToZeroHexaString(unsigned int v, int width)
{
    std::stringstream osstream;
    osstream << std::setw(width) << std::setfill('0') <<std::hex << v;
    return osstream.str();
}

//---------------------------------------------------------------------------
// Convert a number to a string

template<class T>
std::string ToStr(const T &value)
{
    std::ostringstream oss;
    if(!(oss<<std::dec<<value)) throw ("Invalid argument");
    return oss.str();
}


//---------------------------------------------------------------------------
// Convert a string to a number

template<class T>
T ToInt(const std::string &str)
{
    if(str.size()==0) return 0;
    std::istringstream iss(str);
    T result=0;
    if(!(iss>>std::dec>>result)) throw ("Invalid argument");
    return result;
}


//---------------------------------------------------------------------------
// Convert an hex string to a number

template<class T>
T HexToInt(const std::string &str)
{
    if(str.size()==0)return 0;
    std::istringstream iss(str);
    T result=0;
    if(!(iss>>std::hex>>result)) throw ("Invalid argument");
    return result;
}

//---------------------------------------------------------------------------
// Convert a number to an hex string

template<class T>
std::string ToHex(const T &value)
{
    std::ostringstream oss;
    if(!(oss<<std::hex<<value)) throw ("Invalid argument");
    return oss.str();
}

template <class T> T fromstring ( std::string s )
{
    T result;
    std::stringstream str;
    str << s;
    str >> result;
    return result;
}

inline
std::string convertToLowerCase(const std::string & str)
{
    std::string result;
    for (unsigned i=0;i<str.size ();i++)
    {
        result += tolower(str.at(i));
    }
    return result;
}

inline
std::string convertToUpperCase(const std::string & str)
{
    std::string result;
    for (unsigned i=0;i<str.size ();i++)
    {
        result += toupper(str.at(i));
    }
    return result;
}

inline  std::vector<std::string> split(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

inline std::string getFilename(const std::string s)
{
    const size_t last_slash_idx = s.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        return s.substr(last_slash_idx+1, s.size());
    }

    return s;
}

}

#endif // STDTEMPLATE_H
