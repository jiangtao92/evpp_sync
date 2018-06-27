#ifndef __TIMER_H_
#define __TIMER_H_

#include <chrono>

namespace jpp {

	class Timer {

	public:
		Timer() { reset(); }

		template <class T = std::chrono::milliseconds>
		int64_t elapse() { return chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - begin_).count(); }

		void reset() { begin_ = std::chrono::high_resolution_clock::now(); };

	private:
		std::chrono::high_resolution_clock::time_point begin_;
	};
}

#endif
