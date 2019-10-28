#include "ExternalEvent.h"

namespace Zuazo::Timing {

inline void ExternalEvent::invoke(){
	if(isEnabled()){
		update();
	}
}

}