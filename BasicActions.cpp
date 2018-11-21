// 
// 
// 

#include "BasicActions.h"


namespace Macros
{
	namespace BasicActions
	{
		void QButton1(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.press(0);
			queue.push_back(press);

			MacroAction release(300);
			release.release(0);
			queue.push_back(release);
		}

		void QButton2(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.press(1);
			queue.push_back(press);

			MacroAction release(300);
			release.release(1);
			queue.push_back(release);
		}

		void QButton3(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.press(2);
			queue.push_back(press);

			MacroAction release(300);
			release.release(2);
			queue.push_back(release);
		}

		void QButton4(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.press(3);
			queue.push_back(press);

			MacroAction release(300);
			release.release(3);
			queue.push_back(release);
		}

		void QButton5(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.press(4);
			queue.push_back(press);

			MacroAction release(300);
			release.release(4);
			queue.push_back(release);
		}

		void QPositiveX(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.move('X', 1.0f);
			queue.push_back(press);

			MacroAction release(300);
			release.move('X', 0.0f);
			queue.push_back(release);
		}

		void QNegativeX(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.move('X', -1.0f);
			queue.push_back(press);

			MacroAction release(300);
			release.move('X', 0.0f);
			queue.push_back(release);
		}

		void QPositiveY(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.move('Y', 1.0f);
			queue.push_back(press);

			MacroAction release(300);
			release.move('Y', 0.0f);
			queue.push_back(release);
		}

		void QNegativeY(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.move('Y', -1.0f);
			queue.push_back(press);

			MacroAction release(300);
			release.move('Y', 0.0f);
			queue.push_back(release);
		}

		void QPositiveZ(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.move('Z', 1.0f);
			queue.push_back(press);

			MacroAction release(300);
			release.move('Z', 0.0f);
			queue.push_back(release);
		}

		void QNegativeZ(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.move('Z', -1.0f);
			queue.push_back(press);

			MacroAction release(300);
			release.move('Z', 0.0f);
			queue.push_back(release);
		}

		void QPositiveR(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.move('R', 1.0f);
			queue.push_back(press);

			MacroAction release(300);
			release.move('R', 0.0f);
			queue.push_back(release);
		}

		void QNegativeR(std::vector<MacroAction> &queue)
		{
			MacroAction press(100);
			press.move('R', -1.0f);
			queue.push_back(press);

			MacroAction release(300);
			release.move('R', 0.0f);
			queue.push_back(release);
		}
	}

} // Macros
