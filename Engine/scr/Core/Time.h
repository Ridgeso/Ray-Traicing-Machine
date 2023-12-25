#pragma once
#include <chrono>

namespace RT
{
	using namespace std::chrono;

	class Timer
	{
	public:

		Timer()
			: timePoint(high_resolution_clock::now())
		{
		}

		float Ellapsed() const
		{
			return duration_cast<nanoseconds>(high_resolution_clock::now() - timePoint).count() * 1e-6f;
		}

	private:
		std::chrono::time_point<high_resolution_clock> timePoint;
	};


}