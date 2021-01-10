#include <string>
namespace dbr
{
	namespace sys
	{
		// total RAM in MiB
		std::uint64_t totalRAM();
		// available memmory in MiB
		std::uint64_t availRAM();

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

class SystemInfo {
public:
	std::string getSystemInfoString();
	SystemInfo();

private:
	uint64_t totalRAM;
	std::string cpuModel;
	std::string OSName;
	std::string OSVersion;
	std::string Arch;
	std::string Vendor;
	std::string GCName;//graphics card name
	std::string DriverString;
};