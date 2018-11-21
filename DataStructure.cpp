// 
// 
// 

#include "DataStructure.h"


MacroAction::MacroAction(int delay)
{
	this->delay = delay;
}

int MacroAction::get_delay()
{
	return delay;
}

void MacroAction::press(uint8_t button)
{
	bt_actions.push_back({ button,1 });
}

void MacroAction::release(uint8_t button)
{
	bt_actions.push_back({ button,0 });
}

void MacroAction::move(char axis, float position)
{
	char action_axis;
	if (axis == 'X' || axis == 'x')
	{
		action_axis = 'X';
	}
	else if (axis == 'Y' || axis == 'y')
	{
		action_axis = 'Y';
	}
	else if (axis == 'Z' || axis == 'z')
	{
		action_axis = 'Z';
	}
	else if (axis == 'R' || axis == 'r')
	{
		action_axis = 'R';
	}
	else
	{
		//lcd.setCursor(0, 1); // not defined here
		//lcd.print("err in axis def");
		SerialUSB.print("Command tries to move non-existing axis: ");
		SerialUSB.println(axis);
		return;
	}

	position = fminf(1.0f, position);
	position = fmaxf(-1.0f, position);
	int16_t scaled_pos = (int16_t)((position + 1.0f) * 511.5f);
	ax_actions.push_back({ action_axis,scaled_pos });
}

std::vector<ButtonAction> MacroAction::get_bt_actions()
{
	return bt_actions;
}

std::vector<AxisAction> MacroAction::get_ax_actions()
{
	return ax_actions;
}
