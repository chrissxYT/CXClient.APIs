#pragma once

//if you're using MSIE, emacs or TempleOS to compile uncomment this
//#define BAD_COMPILER

#include <stdint.h>
#include <string>
#include <map>
#include <vector>
#include <filesystem>
#include <fstream>

#ifdef BAD_COMPILER
typedef int bool_t;
#else
typedef bool bool_t;
#endif

namespace std
{
	bool_t fexists(string &file);
	//Copies file1 to file2 and returns the number of written bytes.
	int fcopy(string &file1, string &file2);
	
	//Mallocs the given C string (0 terminated) and returns a ptr to the heap space.
	char *malloc(char *src);
	//Mallocs the given byte array with the given length and returns a ptr to the heap space.
	uint8_t *malloc(uint8_t *src, size_t len);

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
	const string dot_minecraft_path = string(getenv("APPDATA")).append("\\.minecraft");
#else
	const string dot_minecraft_path = string(getenv("HOME")).append("/.minecraft");
#endif
	namespace eapi
	{
		//A single mod in the CXClient.
		class eapi_mod
		{
		public:
			//The eAPI and displayed name of the mod.
			//(usually malloced)
			char *name;
			//0 if the mod is disabled, any other value if it's enabled.
			//(usually 1, it's the byte written by CXClient, which is only 0 or 1 at the moment)
			bool_t enabled;
			//All the mods variables that are exported by it.
			//Key/first is the name the mod specified. (often different from it's name in the CXClient code)
			//Value/second are the bytes saved in the file. (usually some kind of int, ints can be resolved with std::int32, std::int16, ...)
			map<char*, uint8_t*> *values;
			eapi_mod();
			eapi_mod(char *name, bool_t enabled, map<char*, uint8_t*> *values);
			~eapi_mod();
		};

		//All the info the eAPI can give you.
		class eapi_info
		{
		public:
			//0 if no CXClient is running, any other value if it is.
			//(usually 1, it is std::fexists which is std::ifstream::good)
			bool_t running;
			//All the mods loaded by the CXClient with all their properties.
			vector<eapi_mod*> mods;
			//Initializes a new eapi_info.
			//running is the default/0.
			//mods is vector<eapi_mod*>()
			eapi_info();
			//Initializes a new eapi_info.
			//running is the given running.
			//mods is vector<eapi_mod*>()
			eapi_info(bool_t running);
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
