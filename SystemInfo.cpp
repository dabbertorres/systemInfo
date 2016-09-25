#include "SystemInfo.hpp"

#include <sstream>

#ifdef __linux__
	#include <sys/utsname.h>
	#include <unistd.h>
#elif _WIN32
	/*	If defined, the following flags inhibit definition
	 *	of the indicated items.
	 */
	#define NOGDICAPMASKS		// - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
	#define NOVIRTUALKEYCODES	// VK_*
	#define NOWINMESSAGES		// WM_*, EM_*, LB_*, CB_*
	#define NOWINSTYLES			// WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
	#define NOSYSMETRICS		// SM_*
	#define NOMENUS				// MF_*
	#define NOICONS				// IDI_*
	#define NOKEYSTATES			// MK_*
	#define NOSYSCOMMANDS		// SC_*
	#define NORASTEROPS			// Binary and Tertiary raster ops
	#define NOSHOWWINDOW		// SW_*
	#define OEMRESOURCE			// OEM Resource values
	#define NOATOM				// Atom Manager routines
	#define NOCLIPBOARD			// Clipboard routines
	#define NOCOLOR				// Screen colors
	#define NOCTLMGR			// Control and Dialog routines
	#define NODRAWTEXT			// DrawText() and DT_*
	#define NOGDI				// All GDI defines and routines
	#define NOKERNEL			// All KERNEL defines and routines
	#define NOUSER				// All USER defines and routines
	#define NONLS				// All NLS defines and routines
	#define NOMB				// MB_* and MessageBox()
	#define NOMEMMGR			// GMEM_*, LMEM_*, GHND, LHND, associated routines
	#define NOMETAFILE			// typedef METAFILEPICT
	#define NOMINMAX			// Macros min(a,b) and max(a,b)
	#define NOMSG				// typedef MSG and associated routines
	#define NOOPENFILE			// OpenFile(), OemToAnsi, AnsiToOem, and OF_*
	#define NOSCROLL			// SB_* and scrolling routines
	#define NOSERVICE			// All Service Controller routines, SERVICE_ equates, etc.
	#define NOSOUND				// Sound driver routines
	#define NOTEXTMETRIC		// typedef TEXTMETRIC and associated routines
	#define NOWH				// SetWindowsHook and WH_*
	#define NOWINOFFSETS		// GWL_*, GCL_*, associated routines
	#define NOCOMM				// COMM driver routines
	#define NOKANJI             // Kanji support stuff.
	#define NOHELP              // Help engine interface.
	#define NOPROFILER			// Profiler interface.
	#define NODEFERWINDOWPOS	// DeferWindowPos routines
	#define NOMCX				// Modem Configuration Extension
	#include <windows.h>
#elif _OSX
	// some OSX header
#endif

#include <GL/gl.h>

namespace dbr
{
	namespace sys
	{
		std::uint64_t totalRAM()
		{
#ifdef __linux__
			auto numPages = sysconf(_SC_PHYS_PAGES);
			auto pageSize = sysconf(_SC_PAGE_SIZE);

			// convert to MiB from B
			return numPages * pageSize / (1024 * 1024);
#elif _WIN32
			MEMORYSTATUSEX statex;
			statex.dwLength = sizeof(statex);
			GlobalMemoryStatusEx(&statex);

			// convert to MiB from B
			return statex.ullTotalPhys / (1024 * 1024);
#elif _OSX
#endif
		}

		std::string cpuModel()
		{
			int model[12];

#ifdef __linux__
			auto callId = 0x80000002;
			asm volatile ("cpuid" : "=a"(model[0]), "=b"(model[1]), "=c"(model[2]), "=d"(model[3]) : "a"(callId));
			++callId;
			asm volatile ("cpuid" : "=a"(model[4]), "=b"(model[5]), "=c"(model[6]), "=d"(model[7]) : "a"(callId));
			++callId;
			asm volatile ("cpuid" : "=a"(model[8]), "=b"(model[9]), "=c"(model[10]), "=d"(model[11]) : "a"(callId));
#elif _WIN32
			__cpuidex(model + 0, 0x80000002, 0);
			__cpuidex(model + 4, 0x80000003, 0);
			__cpuidex(model + 8, 0x80000004, 0);
#elif _OSX
#endif
			// model is null-terminated
			std::string modelStr{reinterpret_cast<const char*>(model)};

			// remove leading spaces (string is provided right-justified
			modelStr.erase(0, modelStr.find_first_not_of(' '));

			return modelStr;
		}

		namespace os
		{
			std::string name()
			{
#ifdef __linux__
				utsname linuxInfo;

				if(uname(&linuxInfo) == -1)
					return "";

				return static_cast<std::string>(linuxInfo.sysname);
#elif _WIN32
				return "Windows";
#elif _OSX
				return "OSX";
#endif
			}

			std::string version()
			{
#ifdef __linux__
				utsname linuxInfo;

				if(uname(&linuxInfo) == -1)
					return "";

				return static_cast<std::string>(linuxInfo.release);
#elif _WIN32
				OSVERSIONINFOEX osvi;
				ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
				osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
				GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&osvi));

				std::ostringstream oss;
				oss << osvi.dwMajorVersion << '.' << osvi.dwMinorVersion << '.' + osvi.dwBuildNumber;
				oss << " SP" << osvi.wServicePackMajor << '.' << osvi.wServicePackMinor;

				return oss.str();
#elif _OSX
	// get OSX info
#endif
			}

			std::string architecture()
			{
#ifdef __linux__
				utsname info;

				if(uname(&info) == -1)
					return "";

				return static_cast<std::string>(info.machine);
#elif _WIN32
				SYSTEM_INFO sysInfo;
				GetNativeSystemInfo(&sysInfo);

				return std::to_string(sysInfo.wProcessorArchitecture);
#elif _OSX
	// get OSX info
#endif
			}
		}

		namespace video
		{
			std::string vendor()
			{
				return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
			}

			std::string name()
			{
				return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
			}

			std::string driver()
			{
				return reinterpret_cast<const char*>(glGetString(GL_VERSION));
			}
		}
	}
}
