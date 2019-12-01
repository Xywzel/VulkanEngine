#include "FileOutStream.h"

#include <iostream>

FileOutStream::FileOutStream(std::string filename)
	: stream(filename, std::ios_base::binary|std::ios_base::trunc)
{
}

FileOutStream& FileOutStream::operator<<(std::vector<bool> data)
{
	size_t size = data.size();
	(*this) << size;
	size_t padd = (8 - size % 8) % 8;
	if(padd > 0)
		data.resize(size + padd, false);
	
	for(size_t i = 0; i < data.size();)
	{
		unsigned char a = 0;
		for(unsigned char mask = 1; mask > 0 && i < data.size(); ++i, mask <<= 1)
			if(data.at(i))
				a |= mask;
		(*this) << a;
	}
	return (*this);
}

bool FileOutStream::ok()
{
	return stream.good();
}
