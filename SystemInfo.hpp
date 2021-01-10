#include <string>
namespace dbr
{
	namespace sys
	{
		// total RAM in MiB
		std::uint64_t totalRAM();

		// friendly name of CPU
		std::string cpuModel();

		namespace os
		{
			// user-friendly name of the OS
			std::string name();

			std::string version();

			std::string architecture();
		}

		namespace video
		{
			std::string vendor();

			// user-friendly video card name
			std::string name();

			// driver version
			std::string driver();
		}
		namespace utils {
			std::string GetSystemVersion();
		}
	}
}
