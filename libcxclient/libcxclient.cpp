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

mod::mod() {}

mod::mod(std::vector<uint8_t> &raw_name, bool enabled,
	std::map<std::string, uint8_t*> values)
{
	this->name = std::string((char*)&raw_name[0]);
	this->enabled = enabled;
	this->values = values;
}

mod::~mod()
{
	for (std::pair<std::string, uint8_t*> k : values)
	{
		free(k.second);
	}
}

info::info() {}

info::info(bool running, std::vector<mod> mods)
{
	this->running = running;
	this->mods = mods;
}

info::~info()
{
	for (mod m : mods)
	{
		m.~mod();
	}
}

std::vector<std::vector<uint8_t>> split(uint8_t *arr, off len, uint8_t sep)
{
	std::vector<std::vector<uint8_t>> a;
	std::vector<uint8_t> b;
	for (off i = 0; i < len; i++)
	{
		if (arr[i] == sep)
		{
			a.push_back(b);
			b = std::vector<uint8_t>();
		}
		else
		{
			b.push_back(arr[i]);
		}
	}
	return a;
}

std::map<std::string, uint8_t*> read_values(path &mod_dir)
{
	std::map<std::string, uint8_t*> values;
	for (path p : directory_iterator(mod_dir))
	{
		std::string str = p.string();
		std::ifstream s(str, std::ios::binary);
		std::streampos l = fsize(str);
		uint8_t *bfr = (uint8_t*)malloc(l);
		s.read((char*)bfr, l);
		values[p.filename().string()] = bfr;
	}
	return values;
}

std::vector<mod> read_mods(std::string &enabled_file)
{
	std::vector<mod> mods;
	std::streampos len = fsize(enabled_file);
	std::ifstream s(enabled_file, std::ios::binary);
	uint8_t *bytes = (uint8_t*)malloc(len);
	s.read((char*)bytes, len);
	std::vector<std::vector<uint8_t>> splt = split(bytes, len, 11);
	free(bytes);
	for (std::vector<uint8_t> v : splt)
	{
		size_t last_idx = v.size() - 1;
		bool enabled = v[last_idx];
		v[last_idx] = 0;
		mods.push_back(mod(v, enabled, read_values(path(
			enabled_file).parent_path().append("/").append(std::c_str(v)))));
	}
	return mods;
}

bool std::fexists(std::string &file)
{
	return ifstream(file).good();
}

info eapi::parse_eapi()
{
	return info
	(
		std::fexists(std::string(mc_path).append("/cxclient_eapi/running")),
		read_mods(std::string(mc_path).append("/cxclient_eapi/mods/enabled"))
	);
}

void cxclient::add_addon(std::string &file)
{
	fcpy(file, std::string(mc_path).append("/cxclient_addons")
		.append(path(file).filename().string()));
}

off std::fcpy(string &file1, string &file2)
{
	FILE *f1 = fopen(file1.c_str(), "r");
	FILE *f2 = fopen(file2.c_str(), "w");
	int c = 0;
	off i = 0;
	while ((c = fgetc(f1)) != EOF)
	{
		fputc(c, f2);
		i++;
	}
	fclose(f1);
	fclose(f2);
	return i;
}

off std::fsize(string &file)
{
	return fstat(file).st_size;
}

struct stat std::fstat(std::string &file)
{
	struct stat s;
	stat(file.c_str(), &s);
	return s;
}

char *std::c_str(std::vector<uint8_t>& raw)
{
	return (char*)&raw[0];
}
