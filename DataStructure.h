// DataStructure.h

#ifndef _DATASTRUCTURE_h
#define _DATASTRUCTURE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ArduinoSTL.h>

struct ButtonAction
{
	uint8_t button;
	uint8_t state;
};

struct AxisAction
{
	char name;
	int16_t position;
};

struct MacroAction
{
private:
	int delay;
	std::vector<ButtonAction> bt_actions;
	std::vector<AxisAction> ax_actions;
public:
	MacroAction(int delay);
	int get_delay();
	void press(uint8_t button);
	void release(uint8_t button);
	void move(char axis, float position);
	std::vector<ButtonAction> get_bt_actions();
	std::vector<AxisAction> get_ax_actions();
};

#endif

