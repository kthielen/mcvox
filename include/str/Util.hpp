#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED 

#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>
#include <typeinfo>
#include <iostream>

#if (defined(__GNUG__) && (__GNUC__ >= 3))
#include <cxxabi.h>
//    extern "C" char* cplus_demangle(const char* mangled, int opt);
#endif

namespace str
{

// base64 encoding
std::string base64_encode(std::istream& in);

// replace [a b c b] [b c] [e] -> [a e b]
template <typename Char>
    std::basic_string<Char> replace(const std::basic_string<Char>& src, const std::basic_string<Char>& old_substr, const std::basic_string<Char>& new_substr)
    {
		typedef typename std::basic_string<Char>::size_type SZT;
		std::basic_string<Char> result;
		SZT                     sz     = src.find(old_substr);
		SZT                     lsz    = 0;

		while (sz != std::basic_string<Char>::npos)
		{
	    	result += std::string(src.begin() + lsz, src.begin() + sz);
		    result += new_substr;
	
		    lsz = sz + old_substr.size();
	    	sz  = src.find(old_substr, lsz);
		}

		if (lsz != std::basic_string<Char>::npos)
		   result += std::string(src.begin() + lsz, src.end());

		return result;
    }

// a wrapper for strftime
template <typename Char>
    std::basic_string<Char> format_time(time_t mtime, const std::basic_string<Char>& fpattern)
    {
		char fbuff[1024]; // 1K ought to be enough
		strftime(fbuff, sizeof(fbuff) - 1, fpattern.c_str(), gmtime(&mtime));
		return std::basic_string<Char>(fbuff);
    }

template <typename T>
    std::string to_string(const T& v)
    {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }

template <typename T>
    T from_string(const std::string& s, const T& def_value = T())
    {
        std::stringstream ss(s);
        T v = def_value;
        ss >> v;
        return v;
    }

template <typename Char>
    bool is_whitespace(const Char& c)
    {
        return c == static_cast<Char>(' ')  ||
               c == static_cast<Char>('\n') ||
               c == static_cast<Char>('\r') ||
               c == static_cast<Char>('\t') ||
               c == static_cast<Char>('\v') ||
               c == static_cast<Char>('\0');
    }

template <typename Char>
    bool is_numeric(const Char& c)
    {
        return c >= static_cast<Char>('0') && c <= static_cast<Char>('9');
    }

template <typename Char>
	bool is_numeric(const std::basic_string<Char>& s)
	{
		if (s.size() == 0 || (s.size() == 1 && s[0] == static_cast<Char>('-'))) return false;

		int i = (s[0] == static_cast<Char>('-') ? 1 : 0);
		bool fp = false;

		for (; i < s.size(); ++i)
			if (s[i] == static_cast<Char>('.') && !fp)
				fp = true;
			else if (!is_numeric(s[i]))
				return false;

		return true;
	}

template <typename Char>
    bool is_hex_numeric(const Char& c)
	{
		return is_numeric<Char>(c) || (c >= static_cast<Char>('a') && c <= static_cast<Char>('f')) || (c >= static_cast<Char>('A') && c <= static_cast<Char>('F'));
	}

template <typename Char>
	bool is_hex_numeric(const std::basic_string<Char>& s)
	{
		if (s.size() <= 2 || s[0] != static_cast<Char>('0') || s[1] != static_cast<Char>('x'))
			return false;

		for (int i = 2; i < s.size(); ++i)
			if (!is_hex_numeric(s[i]))
				return false;

		return true;
	}

inline char hex_nybble(char c)
{
    if (c >= 'a' && c <= 'f')
        return 10 + (c - 'a');
    else if (c >= 'A' && c <= 'F')
        return 10 + (c - 'A');
    else if (c >= '0' && c <= '9')
        return (c - '0');
    else
        return 0;
}

template <typename Char>
	unsigned int hex_str_to_int(const std::basic_string<Char>& s)
	{
		unsigned int r = 0;

		for (int i = s.size() - 1; i > 1; --i)
			r += hex_nybble(static_cast<char>(s[i])) << ((s.size() - (i + 1)) * 4);

		return r;
	}

template <typename Char>
	std::basic_string<Char> int_to_hex_str(unsigned int i)
	{
		typedef typename std::basic_string<Char> rstr;
		char nyb[] = "0123456789abcdef";
		rstr r;

		for (int j = (sizeof(i) * 2) - 1; j >= 0; --j)
			r.push_back((Char)(nyb[(i >> (j * 4)) & 0xf]));

		return r;
	}

template <typename Char>
    std::basic_string<Char> lcase(const std::basic_string<Char>& s)
    {
		typedef typename std::basic_string<Char>::const_iterator CIter;
		std::basic_string<Char> r;
	
		for (CIter si = s.begin(); si != s.end(); ++si)
		    r.append(1, static_cast<Char>((*si >= static_cast<Char>('A') && *si <= static_cast<Char>('Z')) ? ((*si - static_cast<Char>('A')) + static_cast<Char>('a')) : *si));
	    
		return r;
    }

template <typename Char>
    std::basic_string<Char> ucase(const std::basic_string<Char>& s)
    {
		typedef typename std::basic_string<Char>::const_iterator CIter;
		std::basic_string<Char> r;
	
		for (CIter si = s.begin(); si != s.end(); ++si)
		    r.append(1, static_cast<Char>((*si >= static_cast<Char>('a') && *si <= static_cast<Char>('z')) ? ((*si - static_cast<Char>('a')) + static_cast<Char>('A')) : *si));
	
		return r;
    }
					  

template <typename Char>
    std::basic_string<Char> trim(const std::basic_string<Char>& s)
    {
        if (s.empty()) return s;

        unsigned int begin = 0;
        unsigned int end   = 0;

        for (int i = 0; i < s.size(); ++i)
        {
            if (!is_whitespace<Char>(s[i]))
            {
                begin = i;
                break;
            }
        }

        // finally, find the last non-whitespace character
        for (int i = s.size() - 1; i >= 0; --i)
        {
            if (!is_whitespace<Char>(s[i]))
            {
                end = i + 1;
                break;
            }
        }

        if (begin == end)
            return "";
        else
            return s.substr(begin, end - begin);
    }

template <typename Char>
    std::basic_string<Char> mustendwith(const std::basic_string<Char>& str, const std::basic_string<Char>& suffix)
    {
        typename std::basic_string<Char>::size_type mp = str.rfind(suffix);

        if (mp == str.size() - suffix.size())
            return str;
        else
            return str + suffix;
    }

typedef std::pair< std::string, std::string > string_pair;

template <typename Char>
    std::pair< std::basic_string<Char>, std::basic_string<Char> > lsplit(const std::basic_string<Char>& str, const std::basic_string<Char>& pivot)
    {
		typename std::basic_string<Char>::size_type mp = str.find(pivot);

		if (mp == std::basic_string<Char>::npos)
			return std::make_pair(str, std::basic_string<Char>());
		else
		    return std::make_pair(str.substr(0, mp), str.substr(mp + pivot.size()));
    }

template <typename Char>
    std::pair< std::basic_string<Char>, std::basic_string<Char> > rsplit(const std::basic_string<Char>& str, const std::basic_string<Char>& pivot)
    {
		typename std::basic_string<Char>::size_type mp = str.rfind(pivot);

		if (mp == std::basic_string<Char>::npos)
			return std::make_pair(std::basic_string<Char>(), str);
		else
		    return std::make_pair(str.substr(0, mp), str.substr(mp + pivot.size()));
    }

template <typename Char>
    std::vector< std::basic_string<Char> > csplit(const std::basic_string<Char>& str, const std::basic_string<Char>& pivot)
    {
        typedef std::basic_string<Char>   Str;
        typedef typename Str::size_type   size_type;
        typedef typename std::vector<Str> StrVec;

        StrVec    ret;
        size_type mp = 0;

        while (mp != Str::npos)
        {
            size_type nmp = str.find(pivot, mp);

            if (nmp == Str::npos)
            {
                ret.push_back(str.substr(mp, str.size() - mp));
                mp = nmp;
            }
            else
            {
                ret.push_back(str.substr(mp, nmp - mp));
                mp = nmp + pivot.size();
            }
        }

        return ret;
    }

typedef std::vector<std::string> StrVec;

template <typename Char>
    std::basic_string<Char> escape(const std::basic_string<Char>& input)
    {
	typedef typename std::basic_string<Char>::const_iterator CIter;
	std::basic_string<Char> result;

        for (CIter c = input.begin(); c != input.end(); ++c)
	{
	    char cb = static_cast<char>(*c);
	    
	    switch (cb)
	    {
	    case '"':
		result.append(1, static_cast<Char>('\\'));
		result.append(1, static_cast<Char>('\"'));
		break;
	    case '\'':
		result.append(1, static_cast<Char>('\\'));
		result.append(1, static_cast<Char>('\''));
		break;
	    case '\n':
		result.append(1, static_cast<Char>('\\'));
		result.append(1, static_cast<Char>('n'));
		break;
	    case '\t':
		result.append(1, static_cast<Char>('\\'));
		result.append(1, static_cast<Char>('t'));
		break;
	    case '\r':
		result.append(1, static_cast<Char>('\\'));
		result.append(1, static_cast<Char>('r'));
		break;
	    case '\\':
		result.append(1, static_cast<Char>('\\'));
		result.append(1, static_cast<Char>('\\'));
		break;
            case '\0':
                result.append(1, static_cast<Char>('\\'));
                result.append(1, static_cast<Char>('0'));
                break;
	    default:
		result.append(1, *c);
	    }
	}

	return result;
    }

template <typename Char>
    std::basic_string<Char> unescape(const std::basic_string<Char>& input)
    {
	typedef typename std::basic_string<Char>::const_iterator CIter;
	
	std::basic_string<Char> result;
        bool esc = false;

	for (CIter c = input.begin(); c != input.end(); ++c)
	{
	    char cb = static_cast<char>(*c);
	    
	    if (cb == '\\' && !esc)
	    {
		esc = true;
	    }
	    else if (esc)
	    {
		switch (cb)
		{
		case 'n':
		    result.append(1, static_cast<Char>('\n'));
		    break;
		case 't':
		    result.append(1, static_cast<Char>('\t'));
		    break;
		case 'r':
		    result.append(1, static_cast<Char>('\r'));
		    break;
                case '0':
                    result.append(1, static_cast<Char>('\0'));
                    break;
                case 'x':
                {
                    char b1 = 0; char b2 = 0;
                    ++c; if (c != input.end()) b1 = *c;
                    ++c; if (c != input.end()) b2 = *c;

                    result.append(1, static_cast<Char>((hex_nybble(b1) << 4) + hex_nybble(b2)));
                }
                break;
		default:
		    result.append(1, *c);
		}

		esc = false;
	    }
	    else
	    {
		result.append(1, *c);
	    }
	}

	return result;
    }

template <typename Char>
    std::basic_string<Char> urlencode(const std::basic_string<Char>& s)
    {
	const Char escapechars[] = { '&', '=', '?', ' ', '\n', ':', '/', '%' };
	const Char subst[2]      = { 0, 0 };
	const Char hsub[4]       = { '%', 0, 0, 0 };
	std::basic_string<Char> result;
	
	for (const Char* esc = escapechars; esc != (sizeof(escapechars) / sizeof(escapechars[0])); ++esc)
	{
	    subst[0] = *esc;
	    hsub[1]  = (*esc >> 4);   if (hsub[1] > 9) hsub[1] = (hsub[1] - 10) + 'a'; else hsub[1] = hsub[1] + '0';
	    hsub[2]  = (*esc & 0xf0); if (hsub[2] > 9) hsub[2] = (hsub[2] - 10) + 'a'; else hsub[2] = hsub[2] + '0';
	    
	    result   = replace(result, subst, hsub);
	}

	return result;
    }

template <typename Char>
    std::basic_string<Char> urldecode(const std::basic_string<Char>& s)
    {
	std::basic_string<Char> result;

	bool in_esc    = false;
	Char esc       = 0;
	bool read_char = false;

	for (typename std::basic_string<Char>::const_iterator si = s.begin(); si != s.end(); ++si)
	{
	    if (!in_esc && *si == '%')
	    {
		in_esc = true;
	    }
	    else if (in_esc && !read_char)
	    {
		esc = *si;
		
		if (esc >= 'a' && esc <= 'f')
		    esc = (10 + (esc - 'a')) << 4;
		else if (esc >= 'A' && esc <= 'F')
		    esc = (10 + (esc - 'A')) << 4;
		else if (esc >= '0' && esc <= '9')
		    esc = (esc - '0') << 4;
		    
		read_char = true;
	    }
	    else if (in_esc)
	    {
		Char c = *si;
		
		if (c >= 'a' && c <= 'f')
		    esc += 10 + (c - 'a');
		else if (c >= 'A' && c <= 'F')
		    esc += 10 + (c - 'A');
		else if (c >= '0' && c <= '9')
		    esc += c - '0';
		
		result.append(1, esc);
		in_esc    = false;
		read_char = false;
	    }
	    else
	    {
		result.append(1, *si);
	    }
	}

	return result;
    }

template <typename ToChar, typename FromChar>
	std::basic_string<ToChar> convbase(const std::basic_string<FromChar>& s) {
		typedef typename std::basic_string<FromChar>::const_iterator CIter;
		
		std::basic_string<ToChar> ret;
		
		for (CIter si = s.begin(); si != s.end(); ++si) {
			ret.append(1, static_cast<ToChar>(*si));
		}

		return ret;
	}

template <typename Char>
    std::basic_string<Char> cxx_demangle(const std::string& cxxname) {
#   if (defined(__GNUG__) && (__GNUC__ >= 3))
		int status = 0;
		char* dname = abi::__cxa_demangle(("_Z" + cxxname).c_str(), 0, 0, &status);
		std::string ret = dname ? dname : "";
		free(dname);
		return convbase<Char>(ret);
#   else
        return convbase<Char>(cxxname);
#   endif
    }

template <typename T>
    std::string raw_type_name() {
        return cxx_demangle<char>(std::string(typeid(T).name()));
    }
    
template <typename T>
    std::string short_type_name() {
        return lcase(rsplit<char>(cxx_demangle<char>(std::string(typeid(T).name())), "::").second);
    }

}

#endif

