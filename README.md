# FormatString

A String Formater Written in C for fun. 

<h2> Syntax </h2>
There are 4 main functions in this project they are:

<h3> vformats </h3>
vformats is the main function. It is what actually dectects and replaces the variables in the Strings passed to it. <br>
All other api functions simply wrap this one. <br>
It's syntax is: <br>

```
void vformats(char** _BufferPtr, const char* _Format, va_list _Args) 

_BufferPtr is a pointer to a char* of any size.
_Format is the string that will be used as a the format.
_Args is the arguments list that were passed by a wrapper function.
```

 <br>
<h3> formats </h3>
formats is a wrapper function for vformats which takes variadic arguments instead of a va_list. <br>

It's syntax is:
```
void formats(char** _Buffer, char* _Format, ...)
_BufferPtr is a pointer to a char* of any size.
_Format is the string that will be used as a the format.
... is the variadic arguments that will be passed to vformats
```

<h3> vfprint_f </h3>
vfprint_f like the build in vfprintf function but using the vformats function to format the string.  <br>

It's syntax is:
```
void vfprint_f(FILE* _Stream, char* _Format, va_list args)
_Stream is file that the output string will be placed into. (Typically stdout)
_Format is the string that will be used as a the format.
args is the variadic arguments that will be passed to vformats
```