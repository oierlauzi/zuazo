#pragma once

#include <string>

namespace Zuazo::Utils{
class FileBase{
public:
	FileBase()=default;
	FileBase(const std::string& file);
	FileBase(const FileBase& other)=default;
	virtual ~FileBase()=default;
protected:
	std::string		m_directory;
};

inline FileBase::FileBase(const std::string& file) :
		m_directory(file)
{
}

}
