#pragma once 

#include "Layout.h"
#include "ProcessorLayout.h"
#include "Input.h"
#include "Output.h"

namespace Zuazo::Signal {

template <typename T>
class FailOver 
	: public Layout
	, public ProcessorLayout<T, T>
{
public:
	explicit FailOver(std::string name);
	FailOver(const FailOver& other) = delete;
	~FailOver() = default;

	FailOver&								operator=(const FailOver& other) = delete;

	PadProxy<Input<T>>&						getBackupInput();
	const PadProxy<Input<T>>&				getBackupInput() const;

	static constexpr std::string_view		BACKUP_INPUT_NAME = "backup";

private:
	Input<T>								m_backupInput;
	Input<T>								m_input;
	Output<T>								m_output;

	static Output<T>::PullCallback			makePullCallback(Input<T>& backupInput, Input<T>& input);
};
	
}

#include "FailOver.inl"