#include "libcxclient.h"

int16_t std::int16(uint8_t b[2])
{
	return (b[0] << 8) | (b[1]);
}
uint16_t std::uint16(uint8_t b[2])
{
	return (b[0] << 8) | b[1];
}
int32_t std::int32(uint8_t b[4])
{
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3]);
}
uint32_t std::uint32(uint8_t b[4])
{
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3]);
}
int64_t std::int64(uint8_t b[8])
{
	return ((int64_t)b[0] << 56) | ((int64_t)b[1] << 48) | ((int64_t)b[2] << 40) |
		((int64_t)b[3] << 32) | (b[4] << 24) | (b[5] << 16) | (b[6] << 8) | (b[7]);
}
uint64_t std::uint64(uint8_t b[8])
{
	return ((uint64_t)b[0] << 56) | ((uint64_t)b[1] << 48) | ((uint64_t)b[2] << 40) |
		((uint64_t)b[3] << 32) | (b[4] << 24) | (b[5] << 16) | (b[6] << 8) | (b[7]);
}

eapi_mod::eapi_mod() {}

eapi_mod::eapi_mod(vector<uint8_t> &raw_name, bool enabled, map<string, uint8_t*> values)
{
	this->name = string((char*)&raw_name[0]);
	this->enabled = enabled;
	this->values = values;
}

eapi_mod::~eapi_mod()
{
	for (pair<string, uint8_t*> k : values)
	{
		free(k.second);
	}
}

eapi_info::eapi_info() {}

eapi_info::eapi_info(bool running, vector<eapi_mod> mods)
{
	this->running = running;
	this->mods = mods;
}

eapi_info::~eapi_info()
{
	for (eapi_mod m : mods)
	{
		m.~eapi_mod();
	}
}

vector<vector<uint8_t>> split(uint8_t *arr, _off_t len, uint8_t separator)
{
	vector<vector<uint8_t>> a;
	vector<uint8_t> b;
	for (_off_t i = 0; i < len; i++)
	{
		if (arr[i] == separator)
		{
			a.push_back(b);
			b = vector<uint8_t>();
		}
		else
		{
			b.push_back(arr[i]);
		}
	}
	return a;
}

map<string, uint8_t*> read_values(path &mod_dir)
{
	map<string, uint8_t*> values;
	for (path p : directory_iterator(mod_dir))
	{
		string str = p.string();
		ifstream s(str, ios::binary);
		streampos l = fsize(str);
		uint8_t *bfr = (uint8_t*)malloc(l);
		s.read((char*)bfr, l);
		values[p.filename().string()] = bfr;
	}
	return values;
}

vector<eapi_mod> read_mods(string &enabled_file)
{
	vector<eapi_mod> mods;
	ifstream s(enabled_file, ios::binary);
	streampos len = fsize(enabled_file);
	uint8_t *bytes = (uint8_t*)malloc(len);
	s.read((char*)bytes, len);
	vector<vector<uint8_t>> splt = split(bytes, len, 11);
	free(bytes);
	for (vector<uint8_t> v : splt)
	{
		size_t last_idx = v.size() - 1;
		bool enabled = v[last_idx];
		v[last_idx] = 0;
		mods.push_back(eapi_mod(v, enabled, read_values(path(
			enabled_file).parent_path().append("/").append(c_str(v)))));
	}
	return mods;
}

bool std::fexists(string &file)
{
	return ifstream(file).good();
}

eapi_info eapi::parse_eapi()
{
	return eapi_info
	(
		fexists(string(mc_path).append("/cxclient_eapi/running")),
		read_mods(string(mc_path).append("/cxclient_eapi/mods/enabled"))
	);
}

void cxclient::add_addon(string &file)
{
	fcpy(file, string(mc_path).append("/cxclient_addons")
		.append(path(file).filename().string()));
}

_off_t std::fcpy(string &file1, string &file2)
{
	FILE *f1 = fopen(file1.c_str(), "r");
	FILE *f2 = fopen(file2.c_str(), "w");
	int c = 0;
	_off_t i = 0;
	while ((c = fgetc(f1)) != EOF)
	{
		fputc(c, f2);
		i++;
	}
	fclose(f1);
	fclose(f2);
	return i;
}

_off_t std::fsize(string &file)
{
	return fstat(file).st_size;
}

struct stat std::fstat(string &file)
{
	struct stat s;
	stat(file.c_str(), &s);
	return s;
}

char *std::c_str(vector<uint8_t>& raw)
{
	return (char*)&raw[0];
}
