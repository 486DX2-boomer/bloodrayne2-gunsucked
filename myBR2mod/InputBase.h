#pragma once

class InputBase {
public:
	InputBase() {};
	virtual ~InputBase() {};
	virtual void checkAndExecute() {};
	// virtual void reset() {};
};