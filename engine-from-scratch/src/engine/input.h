#pragma once

typedef enum input_key {
	INPUT_KEY_UP,
	INPUT_KEY_ESCAPE,
	
} Input_Key;

typedef enum key_state {
	KS_UNPRESSED,
	KS_PRESSED,
	KS_HELD
} Key_State;

typedef struct input_state {

	Key_State up;
	Key_State escape;
} Input_State;

void input_update(void);