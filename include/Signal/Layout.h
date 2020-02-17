#pragma once

#include <set>
#include <string>
#include <string_view>
#include <typeindex>

namespace Zuazo::Signal {

class Layout {
public:

	class PadBase {
		friend Layout;
	public:
		enum Direction {
			INPUT,
			OUTPUT
		};

		const Layout*       getOwner() const;
		Layout*             getOwner();

		const std::type_index& getType() const;
		Direction           getDirection() const;
		const std::string&  getName() const;

	protected:
		PadBase() = default;
		PadBase(	const std::type_index& type, 
					Direction dir, 
					std::string&& name, 
					Layout* owner ); 
		PadBase(const PadBase& other); 
		PadBase(PadBase&& other);
		virtual ~PadBase();

		PadBase&            operator=(const PadBase& other);
		PadBase&            operator=(PadBase&& other);

		void                setType(const std::type_info& type);
		void                setDirection(Direction dir);
		void                setName(std::string&& name);

	private:
		Layout*             m_owner = nullptr;

		std::type_index     m_type = typeid(void);
		Direction           m_direction = {};
		std::string         m_name;
		
	};

	template <typename T>
	class InputPad;

	template <typename T>
	class OutputPad;

	Layout(std::string&& name);
	Layout(const Layout& other) = delete;
	Layout(Layout&& other);
	virtual ~Layout();

	Layout&                 operator=(Layout&& other);

	void                    setName(std::string&& name);
	const std::string&      getName() const;

	const std::set<PadBase*>& getPads();  
	std::set<const PadBase*>  getPads() const;  

	template<typename T>
	OutputPad<T>&			getOutput(const std::string_view& str);
	template<typename T>
	const OutputPad<T>&		getOutput(const std::string_view& str) const;

	template<typename T>
	InputPad<T>&			getInput(const std::string_view& str);
	template<typename T>
	const InputPad<T>&		getInput(const std::string_view& str) const;

protected:
	void                    addPad(PadBase& pad);
	void                    removePad(PadBase& pad);

private:
	//TODO add some form of base layout

	std::string             m_name;
	std::set<PadBase*>      m_pads;


	PadBase&				findPad(const std::string_view& name, PadBase::Direction dir, const std::type_info& type) const;
	template<typename T>
	OutputPad<T>&			findOutput(const std::string_view& name) const;
	template<typename T>
	InputPad<T>&			findInput(const std::string_view& name) const;
};

template <typename T>
class Layout::InputPad : public virtual PadBase {
	friend OutputPad<T>;
public:
	void                                    setSource(OutputPad<T>* src);
	OutputPad<T>*                           getSource() const;
protected:
	InputPad() = default;
	InputPad(const InputPad& other);
	InputPad(InputPad&& other);
	virtual ~InputPad();

	InputPad&                               operator=(const InputPad& other);
	InputPad&                               operator=(InputPad&& other);

	static const T							NO_SIGNAL;

	const T&         						get() const;
private:
	OutputPad<T>*                           m_source = nullptr;
};

template <typename T>
class Layout::OutputPad : public virtual PadBase {
	friend InputPad<T>;
public:
	const std::set<InputPad<T>*>&		getConsumers() const;
protected:
	OutputPad() = default;
	OutputPad(OutputPad&& other);
	OutputPad(const OutputPad& other) = delete;
	virtual ~OutputPad();

	OutputPad&							operator=(const OutputPad& other) = delete;
	OutputPad&							operator=(OutputPad&& other);

	virtual const T&					get() const = 0;

private:
	std::set<InputPad<T>*>				m_consumers;
};

class NoSignal {};
constexpr NoSignal noSignal;

template <typename T>
void operator<<(Layout::InputPad<T>& dst, Layout::OutputPad<T>& src);

template <typename T>
void operator<<(Layout::InputPad<T>& dst, NoSignal);

}

#include "Layout.inl"