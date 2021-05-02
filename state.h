#pragma once

#include <map>
#include <string>

#include "cmd.h"

class State;

using State_Ptr = std::shared_ptr<State>;

class State {
	State_Ptr parent_;
	std::map<std::string, Command_Ptr> commands_;
public:
	State(State_Ptr parent = State_Ptr { }): parent_ { parent } { }
	Command_Ptr find(const std::string& name);
	void push(const Command_Ptr& command, const std::string& name);
};
