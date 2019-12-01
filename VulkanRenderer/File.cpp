#include "File.h"

#include <fstream>
#include <iostream>

File::File(const std::string & filename)
	: filename(filename)
{
}

const std::vector<char> File::read() const
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary | std::ios::in);
	std::vector<char> data;

	if (!file.is_open())
	{
		std::cout << "failed to open file " << filename << " for reading!" << std::endl;
		return data;
	}

	size_t fileSize = size_t(file.tellg());
	data.resize(fileSize);
	return data;
}

void File::write(const std::vector<char>& data) const
{
	std::ofstream file(filename, std::ios::binary | std::ios::trunc | std::ios::out);
	if (!file.is_open())
	{
		std::cout << "failed to open file " << filename << " for writing!" << std::endl;
		return;
	}
	file.write(data.data(), data.size());
}
