# FormatString

## Building
This project uses CMake.<br>
<br>
The option `BUILD_EXAMPLE_CODE` will build and executable with Example.c. If no option is specified CMake will build the project as a library.<br>
<br>
For anyone who doesn't know how to use cmake there are build instructions are on the [wiki](../../wiki/Building).<br>
<br>

## Syntax 

### General
Any regular character will be preserved in the format. Any curly set will be replaced with the argument specified. 
Curly sets are used to specify the argument which replace the set. If two of the same
argument identifiers are used in the same function call they will both be replaced by the same argument.
e.g.
```
This is the first argument {0} and this is the same {0}
```
Curly sets are sorted by minimum numbers so the lowest number will always be replaced by the first argument.
This is also true of characters. e.g. 
```
In this string {55} is the first argument.
```
```
In this string {55} is the second argument {1}.
```

### Numbers
Numbers are identified with number curly sets e.g. `{0}` could be the first argument. 
Number formats are automatically detected but if they are wrong types can be explicitly
defined with a `l` or `f` e.g. `{0f}` is a float or double and `{0l}` is an int or long. Type specifiers can 
be placed anywhere in the curly set. Additionally, any floating number, either explicitly declared or not, 
can be limited in length <br>
e.g. A call to formats with the arguments:
```
"This double is not limited {0}. This double is {0.2}" and 1.234
```
Will produce the output `This double is not limited 1.234000. This double is 1.23`

### String and Characters
Numbers are identified with number curly sets e.g. `{0}` could be the first argument.

### Functions
For Function Syntax See [Functions](https://github.com/mossx-dev/FormatString/wiki/Functions)



