#pragma once

#include "Interfaces.h"
#include "BMPFile.h"
#include "DDSFile.h"

#include <map>
#include <memory>
#include <string>

class Converter : public IConverter {
public:
	Converter();
	~Converter();

	void convert(const std::string& location);

private:
	std::map<std::string, std::shared_ptr<IFiletype>> m_filetypes;

	std::shared_ptr<IFiletype> createFiletype(const std::string& location);
};

