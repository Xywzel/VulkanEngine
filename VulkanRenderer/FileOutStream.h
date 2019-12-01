#pragma once

#include <fstream>
#include <vector>

class FileOutStream
{
	public:
		FileOutStream(std::string filename);
		FileOutStream& operator<<(std::vector<bool> data);

		template<typename T>
		FileOutStream& operator<<(std::vector<T>& data)
		{
			stream << uint32_t(data.size());
			stream.write(reinterpret_cast<char *>(data.data()), sizeof(T)*data.size());
			return (*this);
		}

		template<typename T>
		FileOutStream& operator<<(const T& t)
		{
			stream.write(reinterpret_cast<const char *>(&t), sizeof(t));
			return (*this);
		}

		bool ok();
	private:
		std::ofstream stream;
};
