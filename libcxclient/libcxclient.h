#pragma once

#include <string>
#include <map>
#include <vector>
#include <filesystem>
#include <fstream>

namespace std
{
	bool fexists(string &file);
	//Copies file1 to file2 and returns the number of written bytes.
	_off_t fcpy(string &file1, string &file2);
	_off_t fsize(string &file);
	struct stat fstat(string &file);

	char *c_str(vector<uint8_t> &raw);

	int16_t int16(uint8_t bytes[2]);
	uint16_t uint16(uint8_t bytes[2]);
	int32_t int32(uint8_t bytes[4]);
	uint32_t uint32(uint8_t bytes[4]);
	int64_t int64(uint8_t bytes[8]);
	uint64_t uint64(uint8_t bytes[8]);
}

using namespace std;
using namespace std::filesystem;

namespace cxclient
{
#ifdef _WIN32
	const string mc_path = string(getenv("APPDATA")).append("\\.minecraft");
#else
	const string mc_path = string(getenv("HOME")).append("/.minecraft");
#endif
	namespace eapi
	{
		//A single mod in the CXClient.
		struct eapi_mod
		{
		public:
			//The eAPI name of the mod. (usually malloced)
			string name;
			//False if the mod is disabled, any other value if it's enabled.
			//(the byte written by CXClient, which is only 0 or 1 at the moment)
			bool enabled;
			//All the mods variables that are exported by it.
			//Key/first is the name the mod specified. (often different from it's name in the CXClient code)
			//Value/second are the bytes saved in the file. (usually some kind of int, which can be resolved with std::int32, std::int16, ...)
			map<string, uint8_t*> values;
			eapi_mod();
			eapi_mod(vector<uint8_t> &raw_name, bool enabled, map<string, uint8_t*> values);
			~eapi_mod();
		};
		//All the info the eAPI can give you.
		class eapi_info
		{
		public:
			//False if no CXClient is running, any other value if it is.
			//(it is std::fexists which is std::ifstream::good)
			bool running;
			//All the mods loaded by the CXClient with all their properties.
			vector<eapi_mod> mods;
			//Initializes a new eapi_info.
			//running is the default/0.
			//mods is vector<eapi_mod*>()
			eapi_info();
			//Initializes a new eapi_info from the given values.
			eapi_info(bool running, vector<eapi_mod> mods);
			//Deconstructs the given eapi_info.
			//Also deletes all eapi_mods in the mods variable.
			//All malloced RAM is freed by this.
			~eapi_info();
		};
		//Parses the eAPI information from the .minecraft-path.
		//All required RAM allocations are done by malloc and new.
		//Remember to delete the eapi_info by calling the deconstructor.
		eapi_info parse_eapi();
	}
	void add_addon(std::string &path);
}

using namespace cxclient;
using namespace cxclient::eapi;
