# CSCI-360-Project
Submitted on April 2022, this project is capable of translating standard C code into MIPS machine language.

This program consists of a helper file and a source file. The helper file is comprised of 3 class data structure: variable, array, and function. 

The variable data structure contains data value such as name, the type, address, etc. This is meant to substitute a variable's name and any information about 
it I might need to retrieve.

The array data structure contains a vector of variables. This is meant to substitute for an array that the program might translate. 

Finally, the function data structure contains a vector of arrays and a vector of variables. This is meant to substitute for a function and all the arrays 
and variables it may contain and their information. 

This program will read line-by-line and offer a real-time translation of a source code file it may read. There will also be various helper functions that
are used to repeat processes that may occur often, such as sorting through declared variables or function calls.
