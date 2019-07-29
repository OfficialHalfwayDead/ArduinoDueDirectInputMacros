#include "RealScenarios.h"
#include <Joystick.h>
#include <LiquidCrystal.h>
#include <math.h>
#include <DueTimer.h>
#include <random>
#include "DataStructure.h"
#include "BasicActions.h"


LiquidCrystal lcd(6, 7, 8, 9, 10, 11);


Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
	12, 1,                  // Button Count, Hat Switch Count
	true, true, true,     // X and Y, but no Z Axis
	false, false, true,   // No Rx, Ry, or Rz
	false, false,          // No rudder or throttle
	false, false, false);

const int buttonPin = 28;

byte dots[8] = {
	B00000,
	B00000,
	B00000,
	B00000,
	B00000,
	B10101,
	B00000,
};

volatile byte task = 0b00000000;

const byte macro_start = 0b00000001;
const byte macro_action = 0b00000010;
const byte macro_running = 0b00000100;
//const byte loop_end = 0b00001000;
//const byte experiment_start = 0b00010000;
//const byte experiment_pause = 0b00100000;
//const byte joystick_set = 0b01000000;

const uint8_t jump = 0;
const uint8_t boost = 1;
const uint8_t handbrake = 2;
const uint8_t start_button = 3;
const uint8_t end_button = 4;
const char steer = 'X';
const char yaw = 'X';
const char pitch = 'Y'; 
const char accel = 'Z';
const char roll = 'R';

bool cursor = false;
bool cursor_change = false;

int queue_ms;
int run_ms;

bool loop_macro = true;
volatile bool end_loop = false;


const byte numChars = 32;
char command[numChars] = "";   // an array to store the current command
char time_mod[numChars] = "";   // an array to store the current modifier


std::vector<MacroAction> action_queue = std::vector<MacroAction>();
std::vector<int> time_queue = std::vector<int>();

void setup() {

	Joystick.begin(false);
	Joystick.setXAxis(512);
	Joystick.setYAxis(512);
	Joystick.setZAxis(512);
	Joystick.setRzAxis(512);
	Joystick.sendState();

	lcd.createChar(0, dots);
	lcd.begin(16, 2);
	lcd.home();
	lcd.print("Ready...");

	

	SerialUSB.begin(115200);    // Initialize Native USB port
	while (!SerialUSB);  // Wait until connection is established

	SerialUSB.println("Ready...");

	randomSeed(analogRead(1)); // don't connect this pin to anything to generate random noise

	noInterrupts();
	pinMode(buttonPin, INPUT);
	attachInterrupt(digitalPinToInterrupt(buttonPin), button_pressed, RISING);
	interrupts();


}

void loop() 
{

	if (task) 
	{
		if (task & macro_action) 
		{
			set_next_state();
		}
		else if (task & macro_start) 
		{
			queue_actions();
		}
		else if (task & macro_running) 
		{
			check_if_done();
		}
	}
	else 
	{
		read_command();
	}
}



void read_command() 
{
	static int current = 0;
	int i = 0;

	if (SerialUSB.available())
	{
		if (current == 0)
		{
			while (SerialUSB.available() > 0 && i < numChars - 1) {
				command[i++] = SerialUSB.read();
			}
			command[i] = '\0'; // terminate the string
			lcd.clear();
			lcd.print(command);
			lcd.setCursor(0, 1);
			lcd.print(time_mod);
			SerialUSB.print("Accepted new command: ");
			SerialUSB.println(command);
		}
		else if (current == 1)
		{
			while (SerialUSB.available() > 0 && i < numChars - 1) {
				time_mod[i++] = SerialUSB.read();
			}
			time_mod[i] = '\0'; // terminate the string
			lcd.clear();
			lcd.print(command);
			lcd.setCursor(0, 1);
			lcd.print(time_mod);
			SerialUSB.print("Accepted new time modifier: ");
			SerialUSB.println(time_mod);
		}
		current = (current + 1) % 2;
	}
}

void check_if_done()
{
	if (time_queue.empty())
	{
		Timer0.stop();
		lcd.noCursor();
		lcd.clear();
		lcd.print(command);
		lcd.setCursor(0, 1);
		lcd.print(time_mod);
		if (loop_macro)
		{
			if (end_loop)
			{
				SerialUSB.print("Done with: ");
				SerialUSB.println(command);
				end_loop = false; // for the next execution
			}
			else
			{
				task |= macro_start;
			}
		}
		else
		{
			SerialUSB.print("Done with: ");
			SerialUSB.println(command);
		}

		task ^= macro_running;
	}
	else if(cursor_change)
	{
		cursor_change = false;
		if (cursor)
			lcd.cursor();
		else
			lcd.noCursor();
	}
}

void button_pressed() {
	if (!(task & (macro_running | macro_action | macro_start))) { // only start macro if nothing is going on
		task |= macro_start;
	}
	else
	{
		end_loop = true;
	}
}


void QFastAerial(std::vector<MacroAction> &queue)
{
	MacroAction firstJump(50);
	firstJump.press(jump);
	firstJump.move(pitch, -1.0f);
	queue.push_back(firstJump);
	
	MacroAction startBoost(92);
	startBoost.press(boost);
	//startBoost.move(accel, 1.0f); // unnecessary
	queue.push_back(startBoost);

	MacroAction releaseJump(108);
	releaseJump.release(jump);
	queue.push_back(releaseJump);

	MacroAction secondJump(9);
	secondJump.move(pitch, -0.99f);
	secondJump.press(jump);
	queue.push_back(secondJump);

	MacroAction fullPitch(9);
	fullPitch.move(pitch, -1.0f);
	//fullPitch.release(jump); // unnecessary
	queue.push_back(fullPitch);

	MacroAction counterPitch(187);
	counterPitch.move(pitch, 1.0f);
	queue.push_back(counterPitch);

	MacroAction releasePitch(355);
	releasePitch.move(pitch, 0.0f);
	queue.push_back(releasePitch);

	MacroAction stopBoost(1400);
	stopBoost.release(boost);
	queue.push_back(stopBoost);

}

void QBoomer(std::vector<MacroAction> &queue)
{
	MacroAction setup(50);
	setup.press(start_button);
	//startBoost.move(accel, 1.0f); // unnecessary
	queue.push_back(setup);

	MacroAction startBoost(500);
	startBoost.release(start_button);
	startBoost.press(boost);
	//startBoost.move(accel, 1.0f); // unnecessary
	queue.push_back(startBoost);

	MacroAction firstJump(658);
	firstJump.press(jump);
	firstJump.move(pitch, -1.0f);
	queue.push_back(firstJump);

	MacroAction releaseJump(125);
	releaseJump.release(jump);
	queue.push_back(releaseJump);

	MacroAction dodge(217);
	dodge.move(pitch, 1.0f);
	dodge.press(jump);
	queue.push_back(dodge);

	MacroAction stopBoost(200);
	stopBoost.release(boost);
	stopBoost.release(jump);
	stopBoost.move(pitch, 0.0f);
	queue.push_back(stopBoost);

}

void QFlipcancel(std::vector<MacroAction> &queue)
{
	MacroAction setup(50);
	setup.press(start_button);
	//startBoost.move(accel, 1.0f); // unnecessary
	queue.push_back(setup);

	MacroAction startBoost(500);
	startBoost.release(start_button);
	startBoost.press(boost);
	//startBoost.move(accel, 1.0f); // unnecessary
	queue.push_back(startBoost);

	MacroAction firstJump(658);
	firstJump.press(jump);
	firstJump.move(pitch, -1.0f);
	queue.push_back(firstJump);

	MacroAction releaseJump(125);
	releaseJump.release(jump);
	queue.push_back(releaseJump);

	MacroAction dodge(17);
	dodge.move(pitch, 1.0f);
	dodge.press(jump);
	queue.push_back(dodge);

	MacroAction cancel(650);
	cancel.move(pitch, -1.0f);
	queue.push_back(cancel);

	MacroAction stopBoost(200);
	stopBoost.release(boost);
	stopBoost.release(jump);
	stopBoost.move(pitch, 0.0f);
	queue.push_back(stopBoost);

}

void queue_actions()
{
	int(*time_mod_func)(int);
	time_mod_func = ident;
	if (strcasecmp(time_mod, "125 Hz") == 0 || strcasecmp(time_mod, "125Hz") == 0 || strcasecmp(time_mod, "125") == 0 || strcasecmp(time_mod, "8 ms") == 0 || strcasecmp(time_mod, "8ms") == 0 || strcasecmp(time_mod, "8") == 0)
	{
		time_mod_func = polling_8;
	}
	else if (strcasecmp(time_mod, "250 Hz") == 0 || strcasecmp(time_mod, "250Hz") == 0 || strcasecmp(time_mod, "250") == 0 || strcasecmp(time_mod, "4 ms") == 0 || strcasecmp(time_mod, "4ms") == 0 || strcasecmp(time_mod, "4") == 0)
	{
		time_mod_func = polling_4;
	}
	else if (strcasecmp(time_mod, "500 Hz") == 0 || strcasecmp(time_mod, "500Hz") == 0 || strcasecmp(time_mod, "500") == 0 || strcasecmp(time_mod, "2 ms") == 0 || strcasecmp(time_mod, "2ms") == 0 || strcasecmp(time_mod, "2") == 0)
	{
		time_mod_func = polling_2;
	}
	else if (strcasecmp(time_mod, "100 Hz") == 0 || strcasecmp(time_mod, "100Hz") == 0 || strcasecmp(time_mod, "100") == 0 || strcasecmp(time_mod, "10 ms") == 0 || strcasecmp(time_mod, "10ms") == 0 || strcasecmp(time_mod, "10") == 0)
	{
		time_mod_func = polling_10;
	}
	else if (strcasecmp(time_mod, "DS4") == 0 || strcasecmp(time_mod, "wireless") == 0 || strcasecmp(time_mod, "DS4 wireless") == 0 || strcasecmp(time_mod, "DS4 Bluetooth") == 0)
	{
		time_mod_func = wireless_DS4;
	}
	else if (strcasecmp(time_mod, "ident") == 0 || strcasecmp(time_mod, "1000Hz") == 0 || strcasecmp(time_mod, "1000") == 0 || strcasecmp(time_mod, "1 ms") == 0 || strcasecmp(time_mod, "1ms") == 0 || strcasecmp(time_mod, "1") == 0)
	{
		time_mod_func = ident;
	}
	else
	{
		SerialUSB.println("Unknown time modifier: " + String(time_mod));
		task ^= macro_start;
		return;
	}

	if (strcasecmp(command, "Button1") == 0 || strcasecmp(command, "Button 1") == 0 || strcasecmp(command, "Jump") == 0)
	{
		Macros::BasicActions::QButton1(action_queue);
	}
	else if (strcasecmp(command, "Button2") == 0 || strcasecmp(command, "Button 2") == 0 || strcasecmp(command, "Boost") == 0)
	{
		Macros::BasicActions::QButton2(action_queue);
	}
	else if (strcasecmp(command, "Button3") == 0 || strcasecmp(command, "Button 3") == 0 || strcasecmp(command, "Handbrake") == 0 || strcasecmp(command, "Powerslide") == 0)
	{
		Macros::BasicActions::QButton3(action_queue);
	}
	else if (strcasecmp(command, "Button4") == 0 || strcasecmp(command, "Button 4") == 0 || strcasecmp(command, "Start button") == 0 || strcasecmp(command, "StartButton") == 0)
	{
		Macros::BasicActions::QButton4(action_queue);
	}
	else if (strcasecmp(command, "Button5") == 0 || strcasecmp(command, "Button 5") == 0 || strcasecmp(command, "End button") == 0 || strcasecmp(command, "EndButton") == 0)
	{
		Macros::BasicActions::QButton5(action_queue);
	}
	else if (strcasecmp(command, "X+") == 0 || strcasecmp(command, "Steer+") == 0 || strcasecmp(command, "Yaw+") == 0)
	{
		Macros::BasicActions::QPositiveX(action_queue);
	}
	else if (strcasecmp(command, "Y+") == 0 || strcasecmp(command, "Pitch+") == 0)
	{
		Macros::BasicActions::QPositiveY(action_queue);
	}
	else if (strcasecmp(command, "Z+") == 0 || strcasecmp(command, "Accel+") == 0 || strcasecmp(command, "Throttle+") == 0 || strcasecmp(command, "Accelerate+") == 0)
	{
		Macros::BasicActions::QPositiveZ(action_queue);
	}
	else if (strcasecmp(command, "R+") == 0 || strcasecmp(command, "Roll+") == 0 || strcasecmp(command, "Airroll+") == 0)
	{
		Macros::BasicActions::QPositiveR(action_queue);
	}
	else if (strcasecmp(command, "X-") == 0 || strcasecmp(command, "Steer-") == 0 || strcasecmp(command, "Yaw-") == 0)
	{
		Macros::BasicActions::QNegativeX(action_queue);
	}
	else if (strcasecmp(command, "Y-") == 0 || strcasecmp(command, "Pitch-") == 0)
	{
		Macros::BasicActions::QNegativeY(action_queue);
	}
	else if (strcasecmp(command, "Z-") == 0 || strcasecmp(command, "Accel-") == 0 || strcasecmp(command, "Throttle-") == 0 || strcasecmp(command, "Accelerate-") == 0)
	{
		Macros::BasicActions::QNegativeZ(action_queue);
	}
	else if (strcasecmp(command, "R-") == 0 || strcasecmp(command, "Roll-") == 0 || strcasecmp(command, "Airroll-") == 0)
	{
		Macros::BasicActions::QNegativeR(action_queue);
	}
	else if (strcasecmp(command, "fast aerial") == 0 || strcasecmp(command, "fastaerial") == 0)
	{
		QFastAerial(action_queue);
	}
	else if (strcasecmp(command, "boomer") == 0 || strcasecmp(command, "shot") == 0)
	{
		QBoomer(action_queue);
	}
	else if (strcasecmp(command, "flipcancel") == 0 || strcasecmp(command, "cancel") == 0)
	{
		QFlipcancel(action_queue);
	}
	else
	{
		SerialUSB.println("Unknown command: " + String(command));
		task ^= macro_start;
		return;
	}


	MacroAction backToDefault(1000);
	for (int i = 0; i < 12; i++)
	{
		backToDefault.release(i);
	}
	backToDefault.move('X', 0.0f);
	backToDefault.move('Y', 0.0f);
	backToDefault.move('Z', 0.0f);
	backToDefault.move('R', 0.0f);
	action_queue.push_back(backToDefault);

	queue_ms = 100 + random(40); // to cause variation in how polling windows line up, sample from lowest common denominator of all polling rates
	int prev_time = queue_ms;
	for (auto it : action_queue)
	{
		queue_ms += it.get_delay();
		time_queue.push_back(time_mod_func(queue_ms));
		//SerialUSB.println(time_queue.back());

	}
	run_ms = 0;

	lcd.setCursor(15, 1);
	cursor_change = true;
	cursor = true;
	if (!loop_macro)
	{
		SerialUSB.print("Running: ");
		SerialUSB.println(command);
	}
	Timer0.attachInterrupt(send_joystick_data).setPeriod(1000).start();
	task |= macro_running;
	task |= macro_action;
	task ^= macro_start;
}




void set_next_state()
{
	if (action_queue.empty())
	{
		task ^= macro_action;
		return;
	}
	MacroAction next_action = action_queue.front();
	action_queue.erase(action_queue.begin());
	std::vector<ButtonAction> bt_actions = next_action.get_bt_actions();
	std::vector<AxisAction> ax_actions = next_action.get_ax_actions();
	while (!bt_actions.empty())
	{
		ButtonAction action = bt_actions.back();
		bt_actions.pop_back();
		Joystick.setButton(action.button,action.state);
	}
	while (!ax_actions.empty())
	{
		AxisAction action = ax_actions.back();
		ax_actions.pop_back();
		if (action.name == 'X')
		{
			Joystick.setXAxis(action.position);
		}
		else if (action.name == 'Y')
		{
			Joystick.setYAxis(action.position);
		}
		else if (action.name == 'Z')
		{
			Joystick.setZAxis(action.position);
		}
		else if (action.name == 'R')
		{
			Joystick.setRzAxis(action.position);
		}
	}

	cursor_change = true;
	cursor = !cursor;
	task ^= macro_action;
}


inline int polling_rate_mod(int time, int window)
{
	if (time % window != 0)
	{
		return time + (window - (time % window));
	}
	return time;
}

int polling_10(int time)
{
	return polling_rate_mod(time, 10);
}

int polling_8(int time)
{
	return polling_rate_mod(time, 8);
}

int polling_4(int time)
{
	return polling_rate_mod(time, 4);
}

int polling_2(int time)
{
	return polling_rate_mod(time, 2);
}

int ident(int time)
{
	return time;
}

int wireless_DS4(int time)
{
	int sample = random(1000);
	if (sample < 923)
		return time;
	else if (sample <= 945)
		return time + 1;
	else if (sample < 959)
		return time + 2;
	else if (sample < 963)
		return time + 3;
	else if (sample <= 967)
		return time + 4;
	else if (sample < 971)
		return time + 5;
	else if (sample < 973)
		return time + 6;
	else if (sample < 977)
		return time + 7;
	else if (sample < 983)
		return time + 8;
	else if (sample < 987)
		return time + 9;
	else if (sample < 990)
		return time + 10;
	else if (sample < 996)
		return time + 11;
	else if (sample < 997)
		return time + 12;
	else if (sample < 998)
		return time + 13;
	else
		return time + 14;

}

void send_joystick_data()
{
	if (!time_queue.empty())
	{
		if (run_ms >= time_queue.front())
		{
			Joystick.sendState();
			time_queue.erase(time_queue.begin());
			task |= macro_action;
		}
	}
	
	run_ms++;
}
