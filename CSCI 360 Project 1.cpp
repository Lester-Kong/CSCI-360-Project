// Lester Kong
// Project 1
// CSCI 360

#include <iostream>
#include <vector>
#include <fstream>
#include <cctype>
#include "Project Helper.hpp"

std::string localArguments(int varCounter)				//Helper function to return the specific register that will be allocated to the appropriate argument
{
	if(varCounter == 0)
	{
		return "%edi";
	}
	else if(varCounter == 1)
	{
		return "%esi";
	}
	else if(varCounter == 2)
	{
		return "%edx";
	}
	else if(varCounter == 3)
	{
		return "%ecx";
	}
	else if(varCounter == 4)
	{
		return "%r8d";
	}
	else if(varCounter == 5)
	{
		return "%r9d";
	}
	return "how did you get here?";
}

void sortInt(std::string stringLine, variable &var, function &func, array &arr, int &varCounter, int &addressCounter, int &localVariables)	//Helper function to sort through variable declarations
{
	var.setType(stringLine.substr(0, stringLine.find(" ")));
	if(stringLine.find(')') != std::string::npos)																					//Case if local variables
	{
		while(stringLine != "")																										//Case if there are variables
		{
			addressCounter = addressCounter + 4;
			if(stringLine.find(',') == std::string::npos)

			if(stringLine.substr(stringLine.find(" "), stringLine.find(',') - stringLine.find(" ")).find('[') != std::string::npos)		//Case if array
			{
				arr.setArraySize(std::stoi(stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1)));
				arr.setName(stringLine.substr(stringLine.find(" ") + 1, stringLine.find('[') - stringLine.find(" ") - 1));
				if(varCounter < 6)								
				{
					for(int i = 0; i < arr.getArraySize(); i++)
					{
						if(varCounter < 6)
						{
							var.setAddress(localVariables * -4);
							std::cout << "	movl " << localArguments(varCounter) << ", " << varCounter * -4 - 4<< "(%rbp)\n";		//Allocate 6 arguments to the registers
						}
						else
						{
							var.setAddress(addressCounter);
							addressCounter = addressCounter + 4;
						}
						arr.addVariable(var);
						varCounter = varCounter + 1;
						localVariables = localVariables + 1;
						var.resetVariable();
					}
				}
				else if(varCounter >= 6)
				{
					for(int i = 0; i < arr.getArraySize(); i++)
					{
						var.setAddress(addressCounter);
						addressCounter = addressCounter + 4;
						arr.addVariable(var);
						varCounter = varCounter + 1;
						var.resetVariable();
					}
				}
				func.addArray(arr);
				arr.resetArray();
			}
			else		// Case if variable
			{
				if(stringLine.find(',') != std::string::npos)
					var.setName(stringLine.substr(stringLine.find(" ") + 1, stringLine.find(',') - stringLine.find(" ") - 1));
				else
					var.setName(stringLine.substr(stringLine.find(" ") + 1, stringLine.find(')') - stringLine.find(" ") - 1));
				if(func.getName() != "main" && varCounter < 6)
				{
					std::cout << "	movl " << localArguments(varCounter) << ", " << varCounter * -4 - 4<< "(%rbp)\n";
					var.setAddress(varCounter * -4 - 4);
					localVariables = localVariables + 1;
					varCounter = varCounter + 1;
				}
				else
					var.setAddress(addressCounter);
			}
			func.addVariable(var);
			var.resetVariable();
			if(stringLine.find(',') != std::string::npos)																			//If there are still variables, shorten the string
				stringLine = stringLine.substr(stringLine.find(',') + 1, stringLine.find(')') - stringLine.find(','));
			else
				stringLine = "";
		}
	}
	else if(stringLine.find('[') != std::string::npos)																				//Case if array
	{
		arr.setArraySize(std::stoi(stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1)));
		arr.setName(stringLine.substr(stringLine.find(" ") + 1, stringLine.find('[') - stringLine.find(" ") - 1));
		stringLine = stringLine.substr(stringLine.find('{') + 1, stringLine.find('}') - stringLine.find('{'));
		for(int i = 0; i < arr.getArraySize(); i++)
		{
			addressCounter = addressCounter + 4;
			std::cout << "	movl $";
			if(stringLine.find(',') != std::string::npos)				
				var.setValue(std::stoi(stringLine.substr(0, stringLine.find(','))));
			else
				var.setValue(std::stoi(stringLine.substr(0, stringLine.find('}'))));
			if(func.getName() != "main")
				var.setAddress(addressCounter);
			else
				var.setAddress(addressCounter * -1);
			arr.addVariable(var);
			std::cout << var.getValue() << ", " << var.getAddress() << "(%rbp)\n";
			
			varCounter = varCounter + 1;
			localVariables = localVariables + 1;
			var.resetVariable();
			stringLine = stringLine.substr(stringLine.find(',') + 1, stringLine.find('}') - stringLine.find(','));
		}
		func.addArray(arr);
		arr.resetArray();
		var.resetVariable();
	}
	else																															//Case if variables
	{
		while(stringLine != "")	
		{
			addressCounter = addressCounter + 4;
			std::cout << "	movl ";
			var.setName(stringLine.substr(stringLine.find(" ") + 1, stringLine.find('=') - stringLine.find(" ") - 1));
			if(func.getName() != "main")
			{
				var.setAddress(localVariables * -4);
				localVariables = localVariables + 1;
			}
			else
				var.setAddress(addressCounter * -1);

			if(isdigit(stringLine[stringLine.find('=') + 1]) || isdigit(stringLine[stringLine.find('=') + 2]))						//Case if the variable is assigned a constant
			{
				if(stringLine.find(',') != std::string::npos)
					var.setValue(std::stoi(stringLine.substr(stringLine.find('=') + 1, stringLine.find(',') - stringLine.find('=') - 1)));
				else
					var.setValue(std::stoi(stringLine.substr(stringLine.find('=') + 1, stringLine.find(';') - stringLine.find('=') - 1)));
				std::cout << "$" << var.getValue() << ", " << var.getAddress() << "(%rbp)\n";
			}
			else if(isalpha(stringLine[stringLine.find('=') + 1]))				//Case if the variable is assigned another variable
			{
				for(int i = 0; i < func.getVarVector().size(); i++)
				{
					if(stringLine.find(',') != std::string::npos)
					{
						if(func.getVariable(i).getName() == stringLine.substr(stringLine.find('=') + 1, stringLine.find(',') - stringLine.find('=') - 1))
						{
							std::cout << func.getVariable(i).getAddress() << "(%rbp), %eax\n";
							std::cout << "	movl %eax, " << var.getAddress() << "(%rbp)\n";
						}
					}
					else
					{
						if(func.getVariable(i).getName() == stringLine.substr(stringLine.find('=') + 1, stringLine.find(';') - stringLine.find('=') - 1))
						{
							std::cout << func.getVariable(i).getAddress() << "(%rbp), %eax\n";
							std::cout << "	movl %eax, " << var.getAddress() << "(%rbp)\n";
						}
					}	
				}
			}
			
			func.addVariable(var);
			var.resetVariable();
			if(stringLine.find(',') != std::string::npos)																			//If there are still variables, shorten the string
				stringLine = stringLine.substr(stringLine.find(',') + 1, stringLine.find(')') - stringLine.find(','));
			else
				stringLine = "";
		}
	}
}

void sortArith(std::string stringLine, function func)				//Sorting arithmetic statements such as "a = b;" or "a = b + 1;"
{
	char operation;
	int arrOperandIndex1, arrOperandIndex2, arrOperandIndex3;
	array arrOperand1, arrOperand2, arrOperand3;
	variable varOperand1, varOperand2, varOperand3;

	//First operand
	
	if(stringLine.substr(0, stringLine.find('=')).find('[') != std::string::npos)		//Array
	{
		for(int i = 0; i < func.getArrVector().size(); i++)	
		{
			if(stringLine.substr(0, stringLine.find('=')) == func.getArray(i).getName())
			{
				arrOperand1 = func.getArray(i);
				arrOperandIndex1 = std::stoi(stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1));
			}
		}							
	}	
	else																				//Variables
	{																			
		for(int i = 0; i < func.getVarVector().size(); i++)		
		{
			if(stringLine.substr(0, stringLine.find('=')) == func.getVariable(i).getName())
				varOperand1 = func.getVariable(i);
		}						
	}

	
	stringLine = stringLine.substr(stringLine.find('=') + 1, stringLine.find(';') - stringLine.find('='));
	
	if(stringLine.find("+") != std::string::npos)									//Second Operand
	{
		operation = '+';
		if(stringLine.substr(0, stringLine.find('+')).find('[') != std::string::npos)		//Array
		{
			for(int i = 0; i < func.getArrVector().size(); i++)								
			{
				if(stringLine.substr(0, stringLine.find('+')) == func.getArray(i).getName())
				{
					arrOperand2 = func.getArray(i);
					arrOperandIndex2 = std::stoi(stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1));
				}
			}
		}
		else																				//Variable
		{
			for(int i = 0; i < func.getVarVector().size(); i++)								
			{
				if(stringLine.substr(0, stringLine.find('+')) == func.getVariable(i).getName())
					varOperand2 = func.getVariable(i);
			}
		}
		stringLine = stringLine.substr(stringLine.find('+') + 1, stringLine.find(';') - stringLine.find('+'));
	}									
	else if(stringLine.find("-") != std::string::npos)
	{
		operation = '-';
		if(stringLine.substr(0, stringLine.find('-')).find('[') != std::string::npos)		//Array
		{
			for(int i = 0; i < func.getArrVector().size(); i++)								
			{
				if(stringLine.substr(0, stringLine.find('-')) == func.getArray(i).getName())
				{
					arrOperand2 = func.getArray(i);
					arrOperandIndex2 = std::stoi(stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1));
				}
			}
		}
		else																				//Variable
		{
			for(int i = 0; i < func.getVarVector().size(); i++)								
			{
				if(stringLine.substr(0, stringLine.find('-')) == func.getVariable(i).getName())
					varOperand2 = func.getVariable(i);
			}
		}
		stringLine = stringLine.substr(stringLine.find('-') + 1, stringLine.find(';') - stringLine.find('-'));
	}
	else if(stringLine.find("*") != std::string::npos)
	{
		operation = '*';
		if(stringLine.substr(0, stringLine.find('*')).find('[') != std::string::npos)		//Array
		{
			for(int i = 0; i < func.getArrVector().size(); i++)								
			{
				if(stringLine.substr(0, stringLine.find('*')) == func.getArray(i).getName())
				{
					arrOperand2 = func.getArray(i);
					arrOperandIndex2 = std::stoi(stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1));
				}
			}
		}
		else																				//Variable
		{
			for(int i = 0; i < func.getVarVector().size(); i++)								
			{
				if(stringLine.substr(0, stringLine.find('*')) == func.getVariable(i).getName())
					varOperand2 = func.getVariable(i);
			}
		}
		stringLine = stringLine.substr(stringLine.find('*') + 1, stringLine.find(';') - stringLine.find('*'));
	}
	else if(stringLine.find('+') == std::string::npos && stringLine.find('-') == std::string::npos && stringLine.find('*') == std::string::npos)	//Case if a variable is being assigned
	{																																				//a constant or another variable
		operation = '=';
		if(stringLine.substr(0, stringLine.find('*')).find('[') != std::string::npos)		//Array
		{
			for(int i = 0; i < func.getArrVector().size(); i++)								
			{
				if(stringLine.substr(0, stringLine.find('*')) == func.getArray(i).getName())
				{
					arrOperand2 = func.getArray(i);
					arrOperandIndex2 = std::stoi(stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1));
				}
			}
		}
		else																				//Variable
		{
			for(int i = 0; i < func.getVarVector().size(); i++)								
			{
				if(stringLine.substr(0, stringLine.find('*')) == func.getVariable(i).getName())
					varOperand2 = func.getVariable(i);
			}
		}
	}

	
	if(arrOperand2.getName() != "")
	{
		std::cout << "	movl ";
		std::cout << arrOperand2.getVariable(arrOperandIndex2).getAddress() << "(%rbp), %edx\n";
	}
	else if(varOperand2.getName() != "")
	{
		std::cout << "	movl ";
		std::cout << varOperand2.getAddress() << "(%rbp), %edx\n"; 
	}
	//Third Operand

	if(stringLine.substr(0, stringLine.find(';')).find('[') != std::string::npos)		//Array
	{
		for(int i = 0; i < func.getArrVector().size(); i++)								
		{

			if(stringLine.substr(0, stringLine.find('[')) == func.getArray(i).getName())
			{
				arrOperand3 = func.getArray(i);
				if(isdigit(stringLine[stringLine.find('[') + 1]))
					arrOperandIndex3 = std::stoi(stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1));
				else
				{
					std::cout << "	movl ";
					for(int j = 0; j < func.getVarVector().size(); j++)
					{
						if(func.getVariable(j).getName() == stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1))
						{
							std::cout << func.getVariable(j).getAddress() << "(%rbp), %eax\n";
							std::cout << "	cltq\n";
						}
					}
				}
			}
		}
	}
	else																				//Variables
	{
		for(int i = 0; i < func.getVarVector().size(); i++)								
		{
			if(stringLine.substr(0, stringLine.find(';')) == func.getVariable(i).getName())
				varOperand3 = func.getVariable(i);
		}
	}

	if(arrOperand3.getName() != "")
	{
		std::cout << "	movl ";
		std::cout << arrOperand3.getVariable(arrOperandIndex3).getAddress();
		if(isdigit(stringLine[stringLine.find('[') + 1]))
			std::cout << "(%rbp), %eax\n";
		else
			std::cout << "(%rbp,%rax,4), %eax\n";
	}
	else if(varOperand3.getName() != "")
	{
		std::cout << "	movl ";
		std::cout << varOperand3.getAddress() << "(%rbp), %eax\n"; 
	}

	if(operation == '+')
		std::cout << "	addl %edx, %eax\n";	
	else if(operation == '*')
		std::cout << "	mull %edx, %eax\n";
	else if(operation == '-')
		std::cout << "	subl %edx, %eax\n";

	
	std::cout << "	movl %eax, ";
	if(arrOperand1.getName() != "")
		std::cout << arrOperand3.getVariable(arrOperandIndex3).getAddress();
	else if(varOperand1.getName() != "")
		std::cout << varOperand1.getAddress() << "(%rbp)\n"; 
}

void sortIf(std::string stringLine, function func)					//Sorting if-statements 
{
	std::string comparator = "";
	int arrOperandIndex1, arrOperandIndex2;
	array arrOperand1, arrOperand2;
	variable varOperand1, varOperand2;

	if(stringLine.find(">=") != std::string::npos)				//Isolate the comparator
		comparator = ">=";
	else if(stringLine.find("<=") != std::string::npos)
		comparator = "<=";
	else if(stringLine.find(">") != std::string::npos)
		comparator = ">";
	else if(stringLine.find("<") != std::string::npos)
		comparator = "<";
	
	stringLine = stringLine.substr(3, stringLine.find(')') - 3);
	
	//First operand
	if(stringLine.substr(0, stringLine.find(comparator)).find('[') != std::string::npos)		//Case if arrays
	{
		arrOperandIndex1 = std::stoi(stringLine.substr(stringLine.find('[') + 1, stringLine.find(']') - stringLine.find('[') - 1));
		for(int i = 0; i < func.getArrVector().size(); i++)
			if(func.getArray(i).getName() == stringLine.substr(0, stringLine.find('[')))
				arrOperand1 = func.getArray(i);
	}
	
	else		//Case if variables
		for(int i = 0; i < func.getVarVector().size(); i++)
			if(func.getVariable(i).getName() == stringLine.substr(0, stringLine.find('[')))
				varOperand1 = func.getVariable(i);

	stringLine = stringLine.substr(stringLine.find(comparator) + 1, stringLine.find(')') - stringLine.find(comparator) - 1);

	if(arrOperand1.getName() != "")
	{
		std::cout << "	movl " << arrOperand1.getVariable(arrOperandIndex1).getAddress() << "(%rbp), %eax\n";
	}
	else if(varOperand1.getName() != "")
		std::cout << "	movl " << varOperand1.getAddress() << "(%rbp), %eax\n";

	//Second operand
	std::cout << "	movl $" << stringLine << ", %rdx\n";	 
	std::cout << "	cmpl %eax, %rdx\n";
	
	if(comparator == ">=")
		std::cout << "	jl ";
	else if(comparator == "<=")
		std::cout << "	jg ";
	else if(comparator == ">")
		std::cout << "	jle ";
	else if(comparator == ">")
		std::cout << "	jge ";
	std::cout << ".L2\n";
}

void functionCall(std::string stringLine, function func)			//Sorting through function calls.
{
	int sixarguments = 0;
	std::vector<variable> remainingArguments;
	while(stringLine.find(',') != std::string::npos)					//Case for multiple variables/arrays
	{
		for(int i = 0; i < func.getVarVector().size(); i++)				//Variables
		{
			if(func.getVariable(i).getName() == stringLine.substr(0, stringLine.find(',')))
			{
				if(sixarguments < 6)
				{
					std::cout << "	movl " << func.getVariable(i).getAddress() << "(%rbp), " << localArguments(sixarguments) << '\n';
					sixarguments = sixarguments + 1;
				}
				else
					remainingArguments.push_back(func.getVariable(i));
				stringLine = stringLine.substr(stringLine.find(',') + 1, stringLine.find(';') - stringLine.find(','));
			}
		}
		for(int i = 0; i < func.getArrVector().size(); i++)			//Arrays
		{
			if(func.getArray(i).getName() == stringLine.substr(0, stringLine.find(',')))
			{
				for(int j = 0; j < func.getArray(i).getArraySize(); j++)
				{
					if(sixarguments < 6)
					{
						std::cout << "	movl " << func.getArray(i).getVariable(j).getAddress() << "(%rbp), " << localArguments(sixarguments) << '\n';
						sixarguments = sixarguments + 1;
					}
					else
						remainingArguments.push_back(func.getArray(i).getVariable(j));
				}
				stringLine = stringLine.substr(stringLine.find(',') + 1, stringLine.find(';') - stringLine.find(','));
			}
		}
	}
	//Case for the last remaining variable/array
	for(int i = 0; i < func.getVarVector().size(); i++)			//Variables
	{
		if(func.getVariable(i).getName() == stringLine.substr(0, stringLine.find(')')))
		{
			if(sixarguments < 6)
			{
				std::cout << "	movl " << func.getVariable(i).getAddress() << "(%rbp), " << localArguments(sixarguments) << '\n';
				sixarguments = sixarguments + 1;
			}
			else
				remainingArguments.push_back(func.getVariable(i));
		}
	}
	for(int i = 0; i < func.getArrVector().size(); i++)			//Arrays
	{
		if(func.getArray(i).getName() == stringLine.substr(0, stringLine.find(')')))
		{
			for(int j = 0; j < func.getArray(i).getArraySize(); j++)
			{
				if(sixarguments < 6)
				{
					std::cout << "	movl " << func.getArray(i).getVariable(j).getAddress() << "(%rbp), " << localArguments(sixarguments) << '\n';
					sixarguments = sixarguments + 1;
				}
				else
					remainingArguments.push_back(func.getArray(i).getVariable(j));
			}
		}
	}
	//If, after 6 arguments, there still remains more arguments, push the remaining arguments into a vector and push them on the stack in reverse order
	if(!remainingArguments.empty())
	{
		for(int i = remainingArguments.size() - 1; i >= 0; i--)
		{
			std::cout << "	movl " << remainingArguments[i].getAddress() << "(%rbp), %eax\n";
			std::cout << "	pushl %eax\n";
		}
	}
}

void sortFor(std::string stringLine, function &func, int &varCounter, int &addressCounter, bool &forState, variable &forLoopi, int &forLoopCondition, std::string forComparator)	//Sorts for loop statements with specific
{																																													//variables for the loop variable and condition
	forLoopCondition = 0;
	forLoopi.resetVariable();
	forState = true;
	if(stringLine.find(">=") != std::string::npos)			//Find the comparator
		forComparator = ">=";
	else if(stringLine.find("<=") != std::string::npos)
		forComparator = "<=";
	else if(stringLine.find(">") != std::string::npos)
		forComparator = ">";
	else if(stringLine.find("<") != std::string::npos)
		forComparator = "<";

	addressCounter = addressCounter + 4;
	std::cout << "	movl ";
	forLoopi.setName(stringLine.substr(stringLine.find(" ") + 1, stringLine.find('=') - stringLine.find(" ") - 1));		//push the for loop variable
	forLoopi.setAddress(addressCounter * -1);
	if(stringLine.find(',') != std::string::npos)
		forLoopi.setValue(std::stoi(stringLine.substr(stringLine.find('=') + 1, stringLine.find(',') - stringLine.find('=') - 1)));
	else
		forLoopi.setValue(std::stoi(stringLine.substr(stringLine.find('=') + 1, stringLine.find(';') - stringLine.find('=') - 1)));
	func.addVariable(forLoopi);

	forLoopCondition = std::stoi(stringLine.substr(stringLine.find(forComparator) + 1 + forComparator.size() - 1, stringLine.find(';') - stringLine.find(forComparator) - 1)); //isolate the loop condition

	std::cout << "$" << forLoopi.getValue() << ", " << forLoopi.getAddress() << "(%rbp)\n";
	std::cout << "	jmp .F2\n";
	std::cout << ".F3:\n";
}

int main()
{
	std::string stringLine, forComparator;
	bool elseState = false, forState = false;
	array arr;
	variable var, forLoopi;
	function func;	
	int localVariables = 0, forLoopCondition = 0;
	std::vector<function> funcVector;
	int varCounter=0, addressCounter=0;
	std::ifstream readLine{"test3.c"};
	while(readLine)
	{
		std::getline(readLine,stringLine);
		if(stringLine.find('}') != std::string::npos && elseState == true)
		{
			std::cout << ".L3:\n";
			elseState = false;
		}
		else if(stringLine.find('}') != std::string::npos && forState == true)
		{
			forState = false;
			std::cout << "	addl $1, " << forLoopi.getAddress() << "(%rbp)\n";
			std::cout << ".F2:\n";
			std::cout << "	cmpl $" << forLoopCondition << ", " << forLoopi.getAddress() << "(%rbp)\n";
			if(forComparator == ">=")
				std::cout << "	jge ";
			else if(forComparator == "<=")
				std::cout << "	jle ";
			else if(forComparator == ">")
				std::cout << "	jg ";
			else if(forComparator == ">")
				std::cout << "	jl ";
			std::cout << " .F3\n";
		}
		else if(stringLine.find("else") != std::string::npos)
		{
			elseState = true;
			std::cout << "	jmp .L3\n";
			std::cout << ".L2:\n";
		}
		else if(stringLine.find("int") != std::string::npos)												//A function, variable, or an array
		{
			if(stringLine.find("for") != std::string::npos)												//for loop
			{
				stringLine = stringLine.substr(stringLine.find(" ") + 1, stringLine.find('{') - stringLine.find("int ") - 1);
				sortFor(stringLine, func, varCounter, addressCounter, forState, forLoopi, forLoopCondition, forComparator);
			}
			else if(stringLine.find("(") != std::string::npos)											//function declaration
			{
				func.resetFunction();
				stringLine = stringLine.substr(4, stringLine.length() - 4);								//Remove "int "
				addressCounter = 0;
				localVariables = 0;
				func.setType("int");
				func.setName(stringLine.substr(0, stringLine.find('(')));
				std::cout << func.getName() << ":\n"; 
				std::cout << "	pushq %rbp\n";
				std::cout << "	movq %rsp, %rbp\n";
				if(stringLine.find("int") != std::string::npos)										//Case if local variables
				{
					stringLine = stringLine.substr(stringLine.find('(') + 1, stringLine.length() - stringLine.find('('));	//Isolate the local variables
					sortInt(stringLine, var, func, arr, varCounter, addressCounter, localVariables);
				}
				funcVector.push_back(func);
			}
			else
				sortInt(stringLine, var, func, arr, varCounter, addressCounter, localVariables);
		}
		else if(stringLine.find("if") != std::string::npos)									//if statement
		{
			sortIf(stringLine, func);
		}
		else if(stringLine.find(");") != std::string::npos)									//Function call
		{
			std::string operand = "", functionName;
			int operandAddress = 0;

			if(stringLine.find('=') != std::string::npos)									//Case if arithmetic statement
			{
				operand = stringLine.substr(0, stringLine.find('='));
				for(int i = 0; i < func.getVarVector().size(); i++)
					if(func.getVarVector()[i].getName() == operand)
						operandAddress = func.getVarVector()[i].getAddress();
				stringLine = stringLine.substr(stringLine.find('=') + 1, stringLine.find(';') - stringLine.find('='));
			}

			for(int i = 0; i < funcVector.size(); i++)
			{
				if(funcVector[i].getName() == stringLine.substr(0, stringLine.find('(')))	//Obtain callee function's name
				{
					functionName = funcVector[i].getName();
					if(funcVector[i].getVariableCount() >= 6)
						std::cout << "	subq $16, %rsp\n";
				}
			}
			stringLine = stringLine.substr(stringLine.find('(') + 1, stringLine.find(';') - stringLine.find('('));
			if(stringLine.find("();") == std::string::npos)
				functionCall(stringLine, func);
			std::cout << "	call " << functionName << '\n';
			if(operand != "")
				std::cout << "	movl %eax, " << operandAddress << "(%rbp)" << '\n';
		}
		else if(stringLine.find("=") != std::string::npos)									//Arithmetic statement
			sortArith(stringLine, func);
		else if(stringLine.find("return") != std::string::npos && func.getName() != "main")								//Return statement
		{
			std::cout << "	movl ";
			stringLine = stringLine.substr(stringLine.find(" ") + 1, stringLine.find(";") - stringLine.find(" ") - 1);
			for(int i = 0; i < func.getVarVector().size(); i++)
			{
				if(func.getVariable(i).getName() == stringLine)
				{
					std::cout << func.getVariable(i).getAddress() << "(%rbp), %eax\n";
				}
			}
			std::cout << "	popq %rbp\n";
			std::cout << "	ret\n";
		}
		else if(stringLine.find("return") != std::string::npos && func.getName() == "main")
		{
			std::cout << "	movl $0, %eax\n";
		}
	}
	std::cout << "	popq %rbp\n";
	std::cout << "	ret\n";
}