#pragma once

#include <string>
#include <vector>

// File class is meant for quick and singular read or write of binary data
// For continuous write or reading in parts, use FileStreams
class File
{
public:
	File(const std::string& filename);
	// Read file from start to end and return as char buffer
	// On failure, error is logged and emty buffer returned
	const std::vector<char> read() const;
	// Write character buffer to file, replacing existing content
	// If file doesn't exist, it is created
	// If operation fails, error is logged and the file should not be modified
	void write(const std::vector<char>& data) const;
private:
	std::string filename;
};
