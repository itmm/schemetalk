class Arith_Equals: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Arith_Equals::eval(Node_Ptr invocation, Node_Ptr state) const {
	double value, to;
	parse_two_args_cmd(
		invocation, "value:", value,
		"compared-to:", to, state
	);
	return value == to ? bool_true : bool_false;
}

class Arith_Not_Equals: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Arith_Not_Equals::eval(Node_Ptr invocation, Node_Ptr state) const {
	double value, to;
	parse_two_args_cmd(
		invocation, "value:", value,
		"compared-to:", to, state
	);
	return value != to ? bool_true : bool_false;
}

class Arith_Greater: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Arith_Greater::eval(Node_Ptr invocation, Node_Ptr state) const {
	double value, to;
	parse_two_args_cmd(
		invocation, "value:", value,
		"compared-to:", to, state
	);
	return value > to ? bool_true : bool_false;
}

class Arith_Greater_Or_Equals: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Arith_Greater_Or_Equals::eval(Node_Ptr invocation, Node_Ptr state) const {
	double value, to;
	parse_two_args_cmd(
		invocation, "value:", value,
		"compared-to:", to, state
	);
	return value >= to ? bool_true : bool_false;
}

class Arith_Smaller: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Arith_Smaller::eval(Node_Ptr invocation, Node_Ptr state) const {
	double value, to;
	parse_two_args_cmd(
		invocation, "value:", value,
		"compared-to:", to, state
	);
	return value < to ? bool_true : bool_false;
}

class Arith_Smaller_Or_Equals: public Command {
public:
	[[nodiscard]] Node_Ptr eval(Node_Ptr invocation, Node_Ptr state) const override;
};

Node_Ptr Arith_Smaller_Or_Equals::eval(Node_Ptr invocation, Node_Ptr state) const {
	double value, to;
	parse_two_args_cmd(
		invocation, "value:", value,
		"compared-to:", to, state
	);
	return value <= to ? bool_true : bool_false;
}

void add_arith(const Node_Ptr &state) {
	Map *s { state->as_map() };
	s->push(std::make_shared<Arith_Equals>(), "equals?");
	s->push(std::make_shared<Arith_Not_Equals>(), "not-equals?");
	s->push(std::make_shared<Arith_Greater>(), "greater?");
	s->push(std::make_shared<Arith_Greater_Or_Equals>(), "greater-or-equals?");
	s->push(std::make_shared<Arith_Smaller>(), "smaller?");
	s->push(std::make_shared<Arith_Smaller_Or_Equals>(), "smaller-or-equals?");
}
