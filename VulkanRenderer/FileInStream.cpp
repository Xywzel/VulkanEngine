#include "FileInStream.h"

#include <iostream>

FileInStream::FileInStream(std::string filename)
	: stream(filename, std::ios_base::binary)
{
}

FileInStream& FileInStream::operator>>(std::vector<bool>& data)
{
	size_t size;
	(*this) >> size;
	size_t padd = (8 - size % 8) % 8;
	data.resize(size + padd, false);

	for(size_t i = 0; i < data.size();)
	{
		unsigned char a;
		(*this) >> a;

		for(unsigned char mask = 1; mask > 0 && i < data.size(); ++i, mask <<= 1)
			data[i] = bool(a & mask);
	}
	data.resize(size);
	return (*this);
}

bool FileInStream::ok()
{
	return stream.good();
}
