#pragma once

#include <algorithm>
#include <ostream>
#include <functional>
#include <memory>
#include <type_traits>
#include <string>

class PCM_source;
class MediaItem;

template <typename T>
inline T bound_value(T lower, T n, T upper)
{
	return std::max(lower, std::min(n, upper));
}

template<typename T,typename U>
inline T map_value(U valin, U inmin, U inmax, T outmin, T outmax)
{
	static_assert(std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, "Only arithmetic types supported");
	return outmin + ((outmax - outmin) * ((T)valin - (T)inmin)) / ((T)inmax - (T)inmin);
}

inline bool is_point_in_rect(int px, int py, int rx, int ry, int rw, int rh)
{
	return px>=rx && px<rx+rw && py>=ry && py<ry+rh;
}

inline bool is_alphaspacenumeric(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c < 'Z') || c == ' ';
}

template<typename T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	const std::size_t kMul = 0x9ddfea08eb382d69ULL;
	std::size_t a = (hasher(v) ^ seed) * kMul;
	a ^= (a >> 47);
	std::size_t b = (seed ^ a) * kMul;
	b ^= (b >> 47);
	seed = b * kMul;
}

// Get a value from a map style container without inserting an element if the key wasn't present
// and instead return default constructed value of the map value type
template<typename Key, typename Cont>
inline auto get_from_map(Cont& c, const Key& k)
{
	if (c.count(k) > 0)
		return c[k];
	return typename Cont::mapped_type();
}

std::string is_source_audio(PCM_source* src);

class readbgbuf : public std::streambuf
{
public:
	int overflow(int c);
	std::streamsize xsputn(const char *buffer, std::streamsize n);
};

class readbg : public std::ostream
{
	readbgbuf buf;
public:
	readbg() :std::ostream(&buf) { }
};

class IValueConverter
{
public:
	virtual ~IValueConverter() {}
	virtual double fromNormalizedToValue(double x) = 0;
	virtual double toNormalizedFromValue(double x) = 0;
	virtual std::string toStringFromValue(double x) = 0;
	virtual double fromStringToValue(const std::string& x) = 0;
};

class LinearValueConverter : public IValueConverter
{
public:
	LinearValueConverter(double minval, double maxval) :
		m_min_value(minval), m_max_value(maxval) {}
	double fromNormalizedToValue(double x) override
	{ return map_value(x,0.0,1.0,m_min_value,m_max_value); }
	double toNormalizedFromValue(double x) override
	{ return map_value(x,m_min_value,m_max_value,0.0,1.0); }
	std::string toStringFromValue(double x) override
	{ return std::to_string(x); }
	double fromStringToValue(const std::string& x) override
	{
		return bound_value(m_min_value, atof(x.c_str()), m_max_value);
	}
private:
	double m_min_value = 0.0;
	double m_max_value = 1.0;
};

class NoCopyNoMove
{
public:
	NoCopyNoMove(){}
	NoCopyNoMove(const NoCopyNoMove&) = delete;
	NoCopyNoMove& operator=(const NoCopyNoMove&) = delete;
	NoCopyNoMove(NoCopyNoMove&&) = delete;
	NoCopyNoMove& operator=(NoCopyNoMove&&) = delete;
};