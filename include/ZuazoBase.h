#pragma once

namespace Zuazo{
/*
 * Class which defines a periodical event
 */
class ZuazoBase{
public:
	ZuazoBase()=default;
	ZuazoBase(const ZuazoBase& other)=default;
	virtual ~ZuazoBase()=default;

	virtual void						open();
	virtual void						close();
	bool								isOpen() const;
private:
	bool								m_isOpen=false;
};

inline void ZuazoBase::open(){
	m_isOpen=true;
}

inline void ZuazoBase::close(){
	m_isOpen=false;
}

inline bool ZuazoBase::isOpen() const{
	return m_isOpen;
}



}
