// BasicActions.h

#ifndef _BASICACTIONS_h
#define _BASICACTIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DataStructure.h"

namespace Macros
{
	namespace BasicActions
	{
		void QButton1(std::vector<MacroAction> &queue);
		void QButton2(std::vector<MacroAction> &queue);
		void QButton3(std::vector<MacroAction> &queue);
		void QButton4(std::vector<MacroAction> &queue);
		void QButton5(std::vector<MacroAction> &queue);


		void QPositiveX(std::vector<MacroAction> &queue);
		void QNegativeX(std::vector<MacroAction> &queue);
		void QPositiveY(std::vector<MacroAction> &queue);
		void QNegativeY(std::vector<MacroAction> &queue);
		void QPositiveZ(std::vector<MacroAction> &queue);
		void QNegativeZ(std::vector<MacroAction> &queue);
		void QPositiveR(std::vector<MacroAction> &queue);
		void QNegativeR(std::vector<MacroAction> &queue);

	}
}

#endif
