#pragma once

#include <fstream>
#include <vector>

class FileInStream
{
	public:
		FileInStream(std::string filename);
		FileInStream& operator>>(std::vector<bool>& data);

		template<typename T>
		FileInStream& operator>>(std::vector<T>& data)
		{
			uint32_t size;
			stream >> size;
			data.resize(size);
			stream.read(reinterpret_cast<char *>(data.data()), sizeof(T)*size);
			return (*this);
		}

		template<typename T>
		FileInStream& operator>>(T& t)
		{
			stream.read(reinterpret_cast<char *>(&t), sizeof(t));
			return (*this);
		}

		bool ok();
	private:
		std::ifstream stream;
};
