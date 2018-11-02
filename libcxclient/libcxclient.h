#pragma once

#include <map>
#include <fstream>

#if defined(_WIN32) || defined(_WIN64)
#define WIN 1
#else
#define WIN 0
#endif

#if WIN || defined(__CYGWIN__) || defined(__MINGW32__)
#define WINF 1
#else
#define WINF 0
#endif

#if WIN
typedef _off_t off;
#include <filesystem>
#else
typedef off_t off;
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <experimental/filesystem>
#endif

namespace std
{
	//0 if file doesn't exist, anything else if it does
	bool fexists(string &file);
	//Copies file1 to file2 and returns the number of written bytes.
	off fcpy(string &file1, string &file2);
	//returns the size of the file in bytes
	off fsize(string &file);
	//calls stat on the file and returns the whole struct
	struct stat fstat(string &file);

	//converts uint8 vector to char array
	char *c_str(vector<uint8_t> &raw);

	int16_t int16(uint8_t bytes[2]);
	uint16_t uint16(uint8_t bytes[2]);
	int32_t int32(uint8_t bytes[4]);
	uint32_t uint32(uint8_t bytes[4]);
	int64_t int64(uint8_t bytes[8]);
	uint64_t uint64(uint8_t bytes[8]);
}

using namespace std;

#if WIN
using namespace std::filesystem;
#else
using namespace experimental::filesystem;
#endif

namespace cxclient
{
	const std::string mc_path = string(getenv(
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) ||\
	defined(__MINGW32__)
	"APPDATA"
#else
	"HOME"
#endif
	)).append("/.minecraft");

	namespace eapi
	{
		//A single mod in the CXClient.
		struct mod
		{
		public:
			//The eAPI name of the mod. (usually malloced)
			std::string name;
			//False if the mod is disabled, any other value if it's enabled.
			//(the byte written by CXClient, which is only 0 or 1 at the moment)
			bool enabled;
			//All the mods variables that are exported by it.
			//Key/first is the name the mod specified. (often different from it's name in the CXClient code)
			//Value/second are the bytes saved in the file. (usually some kind of int, which can be resolved with std::int32, std::int16, ...)
			map<std::string, uint8_t*> values;
			mod();
			mod(std::vector<uint8_t> &raw_name, bool enabled, map<string, uint8_t*> values);
			~mod();
		};

		//All the info the eAPI can give you.
		class info
		{
		public:
			//False if no CXClient is running, any other value if it is.
			//(it is std::fexists which is std::ifstream::good)
			bool running;
			//All the mods loaded by the CXClient with all their properties.
			std::vector<mod> mods;
			//Initializes a new eapi_info.
			//running is the default/0.
			//mods is vector<eapi_mod*>()
			info();
			//Initializes a new eapi_info from the given values.
			info(bool running, std::vector<mod> mods);
			//Deconstructs the given eapi_info.
			//Also deletes all eapi_mods in the mods variable.
			//All malloced RAM is freed by this.
			~info();
		};
		//Parses the eAPI information from the .minecraft-path.
		//All required RAM allocations are done by malloc and new.
		//Remember to delete the eapi_info by calling the deconstructor.
		info parse_eapi();
	}
	void add_addon(std::string &path);
}

using namespace cxclient;
using namespace cxclient::eapi;
