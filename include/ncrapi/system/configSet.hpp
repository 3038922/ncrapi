#pragma once
#include "../userDisplay/userDisplay.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
namespace ncrapi
{
#define MAX_BUF_LEN 1024
#define MAX_KEY_LEN 64
#define MAX_VAL_LEN 256
class Config
{
  private:
    std::vector<std::pair<std::string, std::string>> *_data;

    std::string _filePath;
    void Trim(char s[])
    {
        for (int n = strlen(s) - 1; n >= 0; n--)
        {
            if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
                break;
            s[n + 1] = '\0';
        }
    }

  public:
    Config(std::vector<std::pair<std::string, std::string>> *data, const std::string &filePath) : _data(data), _filePath(filePath)
    {
        FILE *file = fopen(_filePath.c_str(), "r");
        if (file == NULL)
        {
            std::cerr << "file open error" << std::endl;
        }
        else
        {
            char buf[MAX_BUF_LEN];
            int text_comment = 0;
            while (fgets(buf, MAX_BUF_LEN, file) != NULL)
            {

                Trim(buf);
                // to skip text comment with flags /* ... */
                if (buf[0] != '#' && (buf[0] != '/' || buf[1] != '/'))
                {
                    if (strstr(buf, "/*") != NULL)
                    {
                        text_comment = 1;
                        continue;
                    }
                    else if (strstr(buf, "*/") != NULL)
                    {
                        text_comment = 0;
                        continue;
                    }
                }
                if (text_comment == 1)
                {
                    continue;
                }

                int buf_len = strlen(buf);
                // ignore and skip the line with first chracter '#', '=' or '/'
                if (buf_len <= 1 || buf[0] == '#' || buf[0] == '=' || buf[0] == '/')
                {
                    continue;
                }
                buf[buf_len - 1] = '\0';

                char _paramk[MAX_KEY_LEN] = {0}, _paramv[MAX_VAL_LEN] = {0};
                int _kv = 0, _klen = 0, _vlen = 0;
                int i = 0;
                for (i = 0; i < buf_len; ++i)
                {
                    if (buf[i] == ' ')
                        continue;
                    // scan param key name
                    if (_kv == 0 && buf[i] != '=')
                    {
                        if (_klen >= MAX_KEY_LEN)
                            break;
                        _paramk[_klen++] = buf[i];
                        continue;
                    }
                    else if (buf[i] == '=')
                    {
                        _kv = 1;
                        continue;
                    }
                    // scan param key value
                    if (_vlen >= MAX_VAL_LEN || buf[i] == '#')
                        break;
                    _paramv[_vlen++] = buf[i];
                }
                if (strcmp(_paramk, "") == 0 || strcmp(_paramv, "") == 0)
                    continue;
                _data->push_back(std::make_pair(_paramk, _paramv));
            }
            fclose(file);
        }
    }

    // template <class T>
    // static std::string T_as_string(const T &t);
    // template <class T>
    // static T string_as_T(const std::string &s);
};

} // namespace ncrapi
//extern ncrapi::Config *config;
//
// /* static */
// template <class T>
// std::string Config::T_as_string(const T &t)
// {
//     // Convert from a T to a string
//     // Type T must support << operator
//     std::ostringstream ost;
//     ost << t;
//     return ost.str();
// }
//
// /* static */
// template <class T>
// T Config::string_as_T(const std::string &s)
// {
//     // Convert from a string to a T
//     // Type T must support >> operator
//     T t;
//     std::istringstream ist(s);
//     ist >> t;
//     return t;
// }
//
// /* static */
// template <>
// inline std::string Config::string_as_T<std::string>(const std::string &s)
// {
//     // Convert from a string to a string
//     // In other words, do nothing
//     return s;
// }
//
// /* static */
// template <>
// inline bool Config::string_as_T<bool>(const std::string &s)
// {
//     // Convert from a string to a bool
//     // Interpret "false", "F", "no", "n", "0" as false
//     // Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true
//     bool b = true;
//     std::string sup = s;
//     for (std::string::iterator p = sup.begin(); p != sup.end(); ++p)
//         *p = toupper(*p); // make string all caps
//     if (sup == std::string("FALSE") || sup == std::string("F") ||
//         sup == std::string("NO") || sup == std::string("N") ||
//         sup == std::string("0") || sup == std::string("NONE"))
//         b = false;
//     return b;
// }
