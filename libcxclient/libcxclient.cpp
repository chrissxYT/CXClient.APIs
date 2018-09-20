#include "libcxclient.h"

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

i16 std::int16(u8 b[2])
{
	return (b[0] << 8) | (b[1]);
}
u16 std::uint16(u8 b[2])
{
	return (b[0] << 8) | b[1];
}
i32 std::int32(u8 b[4])
{
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3]);
}
u32 std::uint32(u8 b[4])
{
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3]);
}
i64 std::int64(u8 b[8])
{
	return ((i64)b[0] << 56) | ((i64)b[1] << 48) | ((i64)b[2] << 40) |
		((i64)b[3] << 32) | (b[4] << 24) | (b[5] << 16) | (b[6] << 8) | (b[7]);
}
u64 std::uint64(u8 b[8])
{
	return ((u64)b[0] << 56) | ((u64)b[1] << 48) | ((u64)b[2] << 40) |
		((u64)b[3] << 32) | (b[4] << 24) | (b[5] << 16) | (b[6] << 8) | (b[7]);
}

eapi_mod::eapi_mod() {}

eapi_mod::eapi_mod(char *name, bool_t enabled, map<char*, uint8_t*> *values)
{
	this->name = name;
	this->enabled = enabled;
	this->values = values;
}

eapi_mod::~eapi_mod()
{
	for (pair<char*, uint8_t*> k : *values)
	{
		free(k.first);
		free(k.second);
	}
	free(name);
	delete values;
}

eapi_info::eapi_info() {}

eapi_info::eapi_info(bool_t running)
{
	this->running = running;
}

eapi_info::~eapi_info()
{
	for (eapi_mod *m : mods)
	{
		delete m;
	}
}

vector<uint8_t*> split(uint8_t *arr, streampos len, uint8_t separator)
{
	vector<uint8_t*> a;
	vector<uint8_t> b;
	for (streampos i = 0; i < len; i += 1)
	{
		if (arr[i] == separator)
		{
			a.push_back(malloc(&b[0], b.size()));
			b = vector<uint8_t>();
		}
		else
		{
			b.push_back(arr[i]);
		}
	}
	return a;
}



map<char*, uint8_t*> *read_values(path &mod_dir)
{
	map<char*, uint8_t*> *values = new map<char*, uint8_t*>;
	for (path p : directory_iterator(mod_dir))
	{
		ifstream s(p.string().c_str(), ios::binary);
		streampos b = s.tellg();
		s.seekg(ios::end);
		streampos e = s.tellg();
		s.seekg(ios::beg);
		streampos l = e - b;
		uint8_t *bfr = (uint8_t*)malloc(l);
		s.read((char*)bfr, l);
		s.close();
		values->operator[](malloc((char*)p.filename().string().c_str())) = bfr;
	}
	return values;
}

vector<eapi_mod*> read_mods(char *enabled_file)
{
	vector<eapi_mod*> mods;
	ifstream s(enabled_file, ios::binary);
	streampos b = s.tellg();
	s.seekg(ios::end);
	streampos e = s.tellg();
	s.seekg(ios::beg);
	streampos len = e - b;
	uint8_t *bytes = (uint8_t*)malloc(len);
	s.read((char*)bytes, len);
	s.close();
	vector<uint8_t*> splt = split(bytes, len, 11);
	free(bytes);
	for (uint8_t *v : splt)
	{
		size_t last_idx = sizeof(v) - 1;
		bool_t enabled = v[last_idx];
		v[last_idx] = 0;
		eapi_mod *mod = new eapi_mod(malloc((char*)v), enabled, read_values(path(enabled_file).parent_path().append("/").append((char*)v)));
		mods.push_back(mod);
		free(v);
	}
	return mods;
}

bool_t std::fexists(string &file)
{
	return ifstream(file).good();
}

char *std::malloc(char *src)
{
	size_t len = strlen(src);
	char *c = (char*)malloc(len);
	memcpy(c, src, len + 1);
	return c;
}

uint8_t *std::malloc(uint8_t *src, size_t len)
{
	uint8_t *c = (uint8_t*)malloc(len);
	memcpy(c, src, len);
	return c;
}

eapi_info eapi::parse_eapi()
{
	string eapi_root = string(dot_minecraft_path).append("/cxclient_eapi");
	string mods_root = string(eapi_root).append("/mods");
	string enabled_file = string(mods_root).append("/enabled");
	string running_file = string(eapi_root).append("/running");
	eapi_info info(fexists(running_file));
	info.mods = read_mods((char*)enabled_file.c_str());
	return info;
}

void cxclient::add_addon(string &path)
{
	fcopy(path, string(dot_minecraft_path).append("/cxclient_addons")
		.append(dot_minecraft_path.substr(
			max<size_t>(dot_minecraft_path.find_last_of('/'),
				dot_minecraft_path.find_last_of('\\')))));
}

int std::fcopy(string &file1, string &file2)
{
	FILE *f1 = fopen(file1.c_str(), "r");
	FILE *f2 = fopen(file2.c_str(), "w");
	int c = 0;
	int i = 0;
	while ((c = fgetc(f1)) != EOF)
	{
		fputc(c, f2);
		i++;
	}
	fclose(f1);
	fclose(f2);
	return i;
}
