
#ifndef GEOM_LINE_HPP_INCLUDED
#define GEOM_LINE_HPP_INCLUDED

#include <string.h>
#include <stdlib.h>

namespace geom {

// an N-dimensional line-iterator suitable for iterating over an integer grid
template <int N>
	class line {
	public:
		line(double p0[N], double p1[N]) : pos(0), count(0) {
			memcpy(p,  p0, sizeof(double) * N);
			memcpy(ep, p1, sizeof(double) * N);

			// determine the step vector
			sub(p1, p0, s);
			double as[N];
			abs(s, as);
			m = max(as);
			scale(s, 1.0 / m);

			count = int(::abs(m)) + 1;
		}

		bool done() const {
			return pos >= count;
		}

		void operator++() {
			add(p, s, p);
			++pos;
		}

		const double* point() const {
			return this->p;
		}
	private:
		double p[N];
		double ep[N];
		double s[N];
		double m;
		int    pos, count;

		void add(const double p0[N], const double p1[N], double out[N]) {
			for (int i = 0; i < N; ++i) {
				out[i] = p0[i] + p1[i];
			}
		}

		void sub(const double p0[N], const double p1[N], double out[N]) {
			for (int i = 0; i < N; ++i) {
				out[i] = p0[i] - p1[i];
			}
		}

		void scale(double p[N], double s) {
			for (int i = 0; i < N; ++i) {
				p[i] *= s;
			}
		}

		void abs(const double in[N], double out[N]) {
			for (int i = 0; i < N; ++i) {
				out[i] = ::abs(in[i]);
			}
		}

		double max(double x[N]) {
			if (N == 0) {
				return 0.0;
			} else {
				double m = x[0];
				for (int i = 1; i < N; ++i) {
					m = std::max(m, x[i]);
				}
				return m;
			}
		}
	};

}

#endif
