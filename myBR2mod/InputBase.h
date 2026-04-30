#pragma once

class InputBase {
public:
	InputBase() {};
	virtual ~InputBase() {};
	virtual void checkAndExecute() {};
	// for when the input should not fire until it sees a new press
	// ie, prevent lock on or attack inputs during gameplay from leaking into photo mode when it's toggled.
	// from firing immediately. default is no operation- really only needed for gamepad buttons.
	virtual void requireFreshPress() {};
	// virtual void reset() {};
};