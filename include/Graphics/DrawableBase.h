#pragma once

namespace Zuazo::Graphics{
class DrawableBase{
public:
	DrawableBase()=default;
	DrawableBase(const DrawableBase& other)=default;
	virtual ~DrawableBase()=default;

	virtual void draw() const=0;
};
}
