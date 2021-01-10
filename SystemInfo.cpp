#include "SystemInfo.hpp"

#include <sstream>

#ifdef __linux__
	#include <sys/utsname.h>
	#include <unistd.h>
	#include <stdio.h>
#elif _WIN32
	/*	If defined, the following flags inhibit definition
	 *	of the indicated items.
	 */

	#include <GLFW/glfw3.h>
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
		std::uint64_t availRAM() {
#ifdef __linux__
			//Linux memory calculus like (total - free - buffer - cache) / total
			//String needing to fill
			FILE* t = fopen("/proc/meminfo" , "r+");
#elif _WIN32
			MEMORYSTATUSEX statex;

			statex.dwLength = sizeof(statex);

			GlobalMemoryStatusEx(&statex);

			return statex.ullAvailPhys>>20;//right shift by 20 bits means divided by (1024*1024)

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
				return utils::GetSystemVersion();
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
		namespace utils {
#if _WIN32_WINNT > 0x0602 

			std::string GetSystemVersion()
			{
				std::string strOSVersion = "";

				typedef void(__stdcall* NTPROC)(DWORD*, DWORD*, DWORD*);
				HINSTANCE hinst = LoadLibraryA("ntdll.dll");
				DWORD dwMajor, dwMinor, dwBuildNumber;
				NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
				proc(&dwMajor, &dwMinor, &dwBuildNumber);
				if (dwMajor == 6 && dwMinor == 3)//win 8.1
				{
					if (dwBuildNumber == 4026541440)//WinServer2012R2µÄBuildNumberºÅ
					{
						strOSVersion = "Microsoft Windows Server 2012 R2";
					}
					else
					{
						strOSVersion = "Microsoft Windows 8.1";
					}
				}
				else if (dwMajor == 10 && dwMinor == 0)//win 10
				{
					if (dwBuildNumber == 4026546233)//Win10µÄBuildNumberºÅ
					{
						strOSVersion = "Microsoft Windows 10";
					}
					else
					{
						strOSVersion = "Microsoft Windows Server 2016";
					}
				}
				return(strOSVersion);
			}
#else
#include <WinUser.h>
			std::string GetSystemVersion()
			{
				std::string strOSVersion = ("Unknown Microsoft Windows Version");

				OSVERSIONINFOEX os;
				os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
				if (!GetVersionEx((OSVERSIONINFO*)&os)) return(strOSVersion);

				switch (os.dwMajorVersion)
				{
				case 4:
					switch (os.dwMinorVersion)
					{
					case 0:
						if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
							strOSVersion = ("Microsoft Windows NT 4.0");
						else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
							strOSVersion = ("Microsoft Windows 95");
						break;
					case 10:
						strOSVersion = ("Microsoft Windows 98");
						break;
					case 90:
						strOSVersion = ("Microsoft Windows Me");
						break;
					}
					break;
				case 5:
					switch (os.dwMinorVersion)
					{
					case 0:
						strOSVersion = ("Microsoft Windows 2000");
						break;

					case 1:
						strOSVersion = ("Microsoft Windows XP");
						break;

					case 2:
					{
						SYSTEM_INFO info;
						GetSystemInfo(&info);
						if (os.wProductType == VER_NT_WORKSTATION
							&& info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
						{
							strOSVersion = ("Microsoft Windows XP Professional x64 Edition");
						}
						else if (GetSystemMetrics(SM_SERVERR2) == 0)
							strOSVersion = ("Microsoft Windows Server 2003");
						else if (GetSystemMetrics(SM_SERVERR2) != 0)
							strOSVersion = ("Microsoft Windows Server 2003 R2");
					}
					break;
					}
					break;

				case 6:
					switch (os.dwMinorVersion)
					{
					case 0:
						if (os.wProductType == VER_NT_WORKSTATION)
							strOSVersion = ("Microsoft Windows Vista");
						else
							strOSVersion = ("Microsoft Windows Server 2008");
						break;
					case 1:
						if (os.wProductType == VER_NT_WORKSTATION)
							strOSVersion = ("Microsoft Windows 7");
						else
							strOSVersion = ("Microsoft Windows Server 2008 R2");
						break;
					case 2:
						if (os.wProductType == VER_NT_WORKSTATION)
							strOSVersion = ("Microsoft Windows 8");
						else
							strOSVersion = ("Microsoft Windows Server 2012");
						break;
					}
					break;
				}
				return(strOSVersion);
			}
#endif
		}
	}
}
