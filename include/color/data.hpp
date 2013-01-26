
#ifndef COLOR_DATA_HPP_INCLUDED
#define COLOR_DATA_HPP_INCLUDED

#include <str/Util.hpp>
#include <iostream>

namespace color {

typedef unsigned int  value;
typedef unsigned char channel;

inline value make(channel r, channel g, channel b, channel a = 0xff) {
	return (r << 24) | (g << 16) | (b << 8) | a;
}

inline value make(value rgb) {
	return (rgb << 8) | 0xff;
}

inline channel red  (value p) { return (p >> 24) & 0xff; }
inline channel green(value p) { return (p >> 16) & 0xff; }
inline channel blue (value p) { return (p >>  8) & 0xff; }
inline channel alpha(value p) { return (p >>  0) & 0xff; }

inline double distsq(value a, value b) {
	double rd = double(red  (b) - red  (a));
	double gd = double(green(b) - green(a));
	double bd = double(blue (b) - blue (a));

	return (rd * rd) + (gd * gd) + (bd * bd);
}

template <typename CT>
	value average(const CT& cs) {
		double r = 0.0, g = 0.0, b = 0.0, a = 0.0;
		unsigned int n = 0;

		for (typename CT::const_iterator c = cs.begin(); c != cs.end(); ++c) {
			value cv = *c;
			r += red  (cv);
			g += green(cv);
			b += blue (cv);
			a += alpha(cv);
			++n;
		}

		r /= double(n);
		g /= double(n);
		b /= double(n);
		a /= double(n);

		return make(channel(r), channel(g), channel(b), channel(a));
	}

template <int N>
	value wavg(const channel red[N], const channel green[N], const channel blue[N], const double w[N]) {
		double rs = 0.0, gs = 0.0, bs = 0.0;
		double s = 0.0;
		for (unsigned int i = 0; i < N; ++i) {
			rs += double(red  [i]) * w[i];
			gs += double(green[i]) * w[i];
			bs += double(blue [i]) * w[i];
			s  += w[i];
		}

		rs /= s;
		gs /= s;
		bs /= s;

		return make(int(rs), int(gs), int(bs));
	}

inline std::string show(value c) {
	return "#" + str::int_to_hex_str<char>((unsigned int)c);
}

template <int N>
	value wavgc(const value cs[N], const double w[N]) {
		channel r[N];
		channel g[N];
		channel b[N];

		for (unsigned int i = 0; i < N; ++i) {
			r[i] = red  (cs[i]);
			g[i] = green(cs[i]);
			b[i] = blue (cs[i]);
		}

		value result = wavg<N>(r, g, b, w);
/*
		std::cout << "[";
		for (unsigned int i = 0; i < N; ++i) {
			if (i > 0) std::cout << "; ";
			std::cout << show(cs[i]);
		}
		std::cout << "] * [";
		for (unsigned int i = 0; i < N; ++i) {
			if (i > 0) std::cout << "; ";
			std::cout << w[i];
		}
		std::cout << "] = " << show(result) << std::endl;
*/
		return result;
	}

}

#endif
