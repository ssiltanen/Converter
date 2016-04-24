#pragma once

#include <string>

class IFiletype {
public:
	virtual ~IFiletype() {};

	//Initializes filetype with data from given file
	//Pre-condition: - Filelocation is valid
	//				 - datatype is valid
	//				 - File has valid headers and valid structure
	//Post-condition: Valid file data is loaded into datastructure
	virtual void VInitialize(const std::string& location) = 0;
};


class IConverter {
public:
	virtual ~IConverter() {};

	//Loads file into data structure
	//Pre-condition: - Filelocation is valid
	//				 - datatype is valid
	//				 - File has valid headers and valid structure
	//Post-condition: Valid file is loaded into datastructure
	virtual void VLoadFile(const std::string& fileLocation) = 0;

	//Converts file from given format to target format and creates new file as an output
	//Pre-condition: Given filetypes has already been loaded into datastructure
	//Post-condition: Has output only if conversion was valid
	virtual void VConvert(const std::string& fromFormat, const std::string& targetFormat) = 0;
};