# Fuse Interpreter

## Overview

The **Fuse** interpreter is a customizable application designed to execute programs written in a user-defined syntax. It supports debugging features and enables the interpretation of instructions defined in an external configuration file. The interpreter handles 32-bit unsigned integer variables with customizable names and allows operations such as arithmetic, logical, and input/output processing.

## Features

- **Customizable Syntax**: The interpreter uses a configuration file to map operation names to aliases, allowing users to define their own instruction syntax.
- **32-bit Unsigned Integer Variables**: Supports variables with user-defined names, allowing operations to be performed on them.
- **Arithmetic and Logical Operations**: Includes a variety of unary and binary operations such as addition, subtraction, multiplication, division, and bitwise operations.
- **Input/Output**: Allows input from standard input and output to standard output in configurable numeral bases.
- **Debugging**: Supports debugging with breakpoints that allow users to interact with the program state in an interactive mode.

## Features of Supported Instructions

### Unary Operations
- **not**: Bitwise negation.
- **input**: Input a value from standard input in a specified numeral base.
- **output**: Output the value of a variable to standard output in a specified numeral base.

### Binary Operations
- **add**: Addition.
- **mult**: Multiplication.
- **sub**: Subtraction.
- **pow**: Power operation (modular exponentiation).
- **div**: Integer division.
- **rem**: Remainder of division.
- **xor**: Bitwise XOR.
- **and**: Bitwise AND.
- **or**: Bitwise OR.
- **=**: Assignment of value to a variable or initialization of a variable with an expression or constant.

### Example Command Syntax
```ruby
# main.fuse

var_1 = 1F4;
var_2 = mult(var_2, 4);
Var3 = add(div(var_2, 5), rem(var_1, 2));
print(Var3);
```

### Customizable Syntax

The syntax of the instructions can be modified through the configuration file. For example:
```ruby
right=: # Variables where results are stored should be placed on the right of the operation.
    
op(): # For unary operations, the argument is placed after the operator and surrounded by parentheses.
    
()op: # For unary operations, the argument is placed before the operator and surrounded by parentheses.
```

### Example Configuration File

```ruby
# .fuseconfig

right= # Comment explaining behavior
(op)
add sum
#mult prod and this is also a comment
[sub minus
pow ^ this is...]
div /
rem %
xor <>
xor ><
#xor <>
input in
output print
= ->
```

### Number Systems
```plaintext
base_input: The base for input values (default is 10).

base_output: The base for output values (default is 10).

base_assign: The base for assignment (default is 10).
```

### Command-Line Arguments
```bash
-f <input_file> # Path to the input file containing instructions to be executed.
-c <config_file> # Path to the file with configs.

--debug, -d # Enable debugging mode.

--base_input <uint_num> # Specify the base for input values (default: 10).
--base_output <uint_num> # Specify the base for output values (default: 10).
--base_assign <uint_num> # Specify the base for assignment values (default: 10).
```

### Debugging Mode

When the --debug (or equivalent flags) argument is provided, the interpreter enters a debug mode:

    The interpreter halts execution at the BREAKPOINT comment in the input file.
    The user can interact with the interpreter in an interactive mode to:
        View variable values and memory dumps.
        Modify variable values.
        Declare or cancel the declaration of variables.
        Resume or terminate the execution.

#### Example Debugger Actions:

    d: Display the value of a specified variable in hexadecimal, along with its memory dump.
    v: Display all known variables and their values.
    t: Change the value of an existing variable.
    n: Declare and initialize a new variable.
    r: Remove a variable from memory.
    c: Resume execution of the program.
    q: End the debug session.

### Example Usage
Running the Interpreter
```bash
fuse --base_input 16 --base_output 16 --base_assign 10 -f input_file.txt -c config_file.txt
```
Running with Debug Mode
```bash
fuse --debug --base_input 16 --base_output 16 --base_assign 10 -f input_file.txt -c config_file.txt
```

### Conclusion

The Fuse interpreter provides a customizable environment for interpreting and debugging programs with a flexible syntax. Through its configuration file and debugging features, it allows for detailed control over the execution flow and variable management, making it an ideal tool for testing and exploring custom instruction sets.

