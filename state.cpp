#include "state.h"

Command_Ptr State::find(const std::string& name) {
	auto found = commands_.find(name);
	if (found != commands_.end()) {
		return found->second;
	} else if (parent_) {
		return parent_->find(name);
	} else {
		return Command_Ptr { };
	}
}

void State::push(const Command_Ptr& command, const std::string& name) {
	commands_.insert({name, command});
}
