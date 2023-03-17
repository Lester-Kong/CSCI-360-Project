// Lester Kong
// Project 1
// CSCI 360

class variable
{
private:
	std::string _name, _type;
	bool initialized = false;
	int _value = 0;
	int _address = 0;
public:
	// Mutator Functions
	void resetVariable();
	void setAddress(int address);
	void setName(std::string name);
	void setType(std::string type);
	void setValue(int value);
	// Accessor Functions
	bool isInitialized();
	int getAddress();
	std::string getName();
	std::string getType();
	int getValue();
};

//Variable Mutator Functions
void variable::resetVariable()
{
	initialized = false;
	_address = 0;
	_name = "";
	_type = "";
	_value = 0;
}
void variable::setAddress(int address)
{
	_address = address;
}
void variable::setName(std::string name)
{
	_name = name;
}
void variable::setType(std::string type)
{
	_type = type;
}
void variable::setValue(int value)
{
	initialized = true;
	_value = value;
}

//Variable Accessor Functions
bool variable::isInitialized()
{
	return initialized;
}
int variable::getAddress()
{
	return _address;
}
std::string variable::getName()
{
	return _name;
}
std::string variable::getType()
{
	return _type;
}
int variable::getValue()
{
	return _value;
}

class array
{
private:
	std::vector<variable> _arrOfVariables;
	std::string _name;
	int _arraySize = 0;
public:
	// Mutator Functions
	void resetArray();
	void setName(std::string name);
	void addVariable(variable var);
	void setArraySize(int arraySize);
	// Accessor Functions
	std::string getName();
	int getArraySize();
	variable getVariable(int index);
	std::vector<variable> getArray();
};

//Function Mutator Functions
void array::resetArray()
{
	_arraySize = 0;
	_name = "";
	_arrOfVariables.clear();
}
void array::setName(std::string name)
{
	_name = name;
}
void array::addVariable(variable var)
{
	_arrOfVariables.push_back(var);
}
void array::setArraySize(int arraySize)
{
	_arraySize = arraySize;
}

//Function Accessor Functions
std::string array::getName()
{
	return _name;
}
int array::getArraySize()
{
	return _arraySize;
}
variable array::getVariable(int index)
{
	return _arrOfVariables[index];
}
std::vector<variable> array::getArray()
{
	return _arrOfVariables;
}

class function
{
private:
	int _variableCount = 0;
	std::string _name, _type;
	std::vector<array> _arrVector;
	std::vector<variable> _varVector;
public:
	// Mutator Functions
	void resetFunction();
	void setName(std::string name);
	void setType(std::string type);
	void addArray(array arr);
	void addVariable(variable var);
	// Accessor Functions
	std::string getName();
	std::string getType();
	int getVariableCount();
	variable getVariable(int index);
	array getArray(int index);
	std::vector<array> getArrVector();
	std::vector<variable> getVarVector();
};

//Function Mutator Functions
void function::resetFunction()
{
	_variableCount = 0;
	_name = "";
	_type = "";
	_varVector.clear();
	_arrVector.clear();
}

void function::setName(std::string name)
{
	_name = name;
}
void function::setType(std::string type)
{
	_type = type;
}
void function::addArray(array arr)
{
	_arrVector.push_back(arr);
	_variableCount = _variableCount + arr.getArraySize();
}
void function::addVariable(variable var)
{
	_varVector.push_back(var);
	_variableCount = _variableCount + 1;
}

//Function Accessor Functions
std::string function::getName()
{
	return _name;
}
std::string function::getType()
{
	return _type;
}
int function::getVariableCount()
{
	return _variableCount;
}
variable function::getVariable(int index)
{
	return _varVector[index];
}
array function::getArray(int index)
{
	return _arrVector[index];
}
std::vector<array> function::getArrVector()
{
	return _arrVector;
}
std::vector<variable> function::getVarVector()
{
	return _varVector;
}