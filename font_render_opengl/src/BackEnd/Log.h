#pragma once
#include <cstdio> // For printf

namespace Misk
{
	class Log {
	public:

        template<typename... Args>
        static void Log_Assert(const char* format, Args... args) {

            std::printf("Assertion Failed here the problem: ");
            std::printf(format, args...); // Forward arguments to printf
            std::printf("\n");

        }

	};
}




#define MK_ASSERT(x,...)  if(!x){ ::Misk::Log::Log_Assert(__VA_ARGS__);__debugbreak();}
