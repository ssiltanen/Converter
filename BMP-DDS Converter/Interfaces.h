#pragma once

#include <string>

class IFiletype {
public:
	virtual ~IFiletype() {};

};


class IConverter {
public:
	virtual ~IConverter() {};

	virtual void convert(const std::string& filename) = 0;
};