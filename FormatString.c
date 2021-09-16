#include "FormatString.h"


void array_max_pos(int* array, int len, int ignore, int* dest) {
    int max = 0;
    *dest = 0;
    if (ignore == 0) {
        for (int i = 0; i < len; i++) {
            *dest = array[i] > max
                    ? i
                    : *dest;
            max = array[i] > max ? array[i] : max;
        }
    }
    else {
        int* ignore_index = calloc(ignore, sizeof(int));
        for (int i = 0; i < ignore; i++) {
            array_max_pos(array, len, ignore - (i + 1), ignore_index + i);
        }

        for (int i = 0; i < len; i++) {
            *dest = (array[i] > max && (!is_in_array(array, len, i)) ? i : *dest);
            max = (array[i] > max) && (!is_in_array(ignore_index, ignore, i)) ? array[i] : max;
        }
        free(ignore_index);
    }
}



void array_min_pos(int* array, int len, int ignore, int* dest) {
    int min = INT_MAX;
    *dest = 0;
    if (ignore == 0) {
        for (int i = 0; i < len; i++) {
            *dest = array[i] < min ? i : *dest;
            min = array[i] < min ? array[i] : min;
        }
    }
    else {
        int* ignore_index = calloc(ignore, sizeof(int));
        for (int i = 0; i < ignore; i++) {
            array_min_pos(array, len, ignore - (i + 1), ignore_index + i);
        }

        for (int i = 0; i < len; i++) {
            *dest = (array[i] < min && (!is_in_array(ignore_index, ignore, i)) ? i : *dest);
            min = (array[i] < min) && (!is_in_array(ignore_index, ignore, i)) ? array[i] : min;
        }
        free(ignore_index);
    }
}

const char top_four_bits = 240;
const char bottom_four_bits = 15;


/*
									Double or Long Detection
	------------------------------------------------------------------------------------------

										Fundamenals
	va_lists are char arrays with size 8 that store either the data or a pointer to an array or struct.
	64-bit Doubles are stored with 1 bit for sign and 10 bits for an exponent. The rest of the bits 53 bits are Mantissa.

										  Method
	Nearly all doubles contain at least one exponent bit thus if we can detect the first 1 and a half bytes (12 bits) and any
	of them are set then we can assume it is a double. This falls apart with very high longs but in my tests it is 100% accurate up to at least 10,000,000 for longs
	and 10,000 for doubles (incremeted by 0.0001)
*/

/*
	Gets the argument at index index of args va_list then detects whether its a double or long (See method above).
*/
double va_get_num(va_list args, int index, int num_of_non_floats, int num_of_floats, bool detection, bool* is_floating)
{
    double double_arg = _VA_POINTER_AT_IDX_D(args, index - num_of_non_floats);
    long long_arg = _VA_POINTER_AT_IDX_L(args, index - num_of_floats);

    if (detection) {
        *is_floating = true;
        int amount_zero_bytes = _FLOATING_POINT_MAX_ZERO_BYTES_;
        bool has_exponent = false;
        bool has_mantissa = false;
        if (((((int)double_arg) != double_arg)) && (double_arg != 0)) {
            if (!(((int)double_arg == 0) && double_arg < 0.0000001)) {
                for (int i = SIZE_OF_VA_ARG - 1; i >= 0; i--) {
                    if (*(((char*)&long_arg) + i) != '\0') {
                        {
                            if (i == SIZE_OF_VA_ARG - 1 || (i == (SIZE_OF_VA_ARG - 2) && (*(((char*)&long_arg) + i) & top_four_bits) != 0)) {
                                has_exponent = true;
                                amount_zero_bytes = !amount_zero_bytes ? amount_zero_bytes : amount_zero_bytes - 1;
                            }
                            if ((i != SIZE_OF_VA_ARG - 1) || (i == (SIZE_OF_VA_ARG - 2) && (*(((char*)&long_arg) + i) & bottom_four_bits) != 0)) {
                                has_mantissa = true;
                                amount_zero_bytes = !amount_zero_bytes ? amount_zero_bytes : amount_zero_bytes - 1;
                            }
                        }
                    }
                }
            }
        }
        else {
            *is_floating = true;
        }
        *is_floating &= (has_exponent & (amount_zero_bytes == 0));
    }

    if (!*is_floating)
        return (double)(long_arg);
    else
        return (double_arg);

    return -1;
}


/*
									Char or String Detection
	------------------------------------------------------------------------------------------

	Fundamentals: va_lists are char arrays with size 8 that store either the data or a pointer to an array or struct.

	Because chars are only 1 byte and pointers are not if the second byte in the char array is 0 then we know it must be a character otherwise it must be a string.

*/

/*
	Gets the argument at index idx of args va_list then detects whether it's a char or string (See method above).

	num_float_args is needed for linux compatibility
*/
char* va_get_char(va_list args, int index, int num_float_args) {
    bool is_char = *(_VA_POINTER_AT_IDX_C(args, (index - num_float_args)) + 1) == 0;

    int len = strlen(is_char ? _VA_POINTER_AT_IDX_C(args, index - num_float_args) : _VA_POINTER_AT_IDX_S(args, index - num_float_args));
    char* arg = calloc(len + 1, sizeof(char));


    strcpy_s(arg, len+1, is_char ? _VA_POINTER_AT_IDX_C(args, index - num_float_args) : _VA_POINTER_AT_IDX_S(args, index - num_float_args));
    return arg;
}




/*

	_BufferPtr: A pointer to a char array, either allocated or not, where the formatted string will be placed
	_Format: The format that will be placed into _BufferPtr
	_Args: The arguments which will be placed into the formatted string

	Syntax: Curley Brackets will be completely removed* and replaced by the argument matching their number or letter in
	the _Format string.

	Arguments will be read by lowest value first. e.g. if 55 is the lowest number in curley brackets it will be interpreted as the first number passed

	Numbers: The syntax for numbers is {n} where n a number pertaining to the desired argument.
	Numbers will be automatically detected as either long format or IEEE-754 (double) format.
	If automatic detection is incorrect and l or f can be added to specify format.

	Chars: The syntax for numbers is {*} where * is a letter pertaining to the desired argument.


	* Currently, the only way to retain curley brackets is to pass them as a char or char*

*/
void vformats(char** _BufferPtr, const char* _Format, va_list _Args) {
    if (!_Format || _Format[0] == '\0') {
        *_BufferPtr = calloc(1, 1);
        return;
    }

    // Counters
    int new_string_pos = 0;
    int new_string_len = 100;

    // Args
    char* _Buffer = *_BufferPtr;
    free (_Buffer);
    _Buffer = calloc(new_string_len, sizeof(char));





    // State identifiers
    bool reading_format_specifier = false;
    bool is_num = false;
    char current_char = '\0';


    // Replacement Variables
    int current_num_arg = 1;
    int current_char_arg = 1;
    bool reading_double_length_restrictor = 0;
    int* nums_by_appearance_in_format = malloc(current_num_arg * sizeof(int));
    nums_by_appearance_in_format[0] = INT_MAX;
    int* chars_by_appearance_in_format = malloc(current_char_arg * sizeof(int));
    chars_by_appearance_in_format[0] = INT_MAX;
    bool* arg_is_num_arr = calloc(current_num_arg + current_char_arg, sizeof(bool));
    bool* arg_is_double = calloc(current_num_arg, sizeof(bool));
    bool* is_explicitly_declared = calloc(current_num_arg, sizeof(bool));
    string number_str = new_string("");
    char replacement_char = '\0';

    unsigned long startLen = strlen(_Format);
    for (int i = 0; i < startLen; i++) {
        current_char = _Format[i];
        if (reading_format_specifier || current_char == '{') {
            reading_format_specifier = true;
            if (current_char == '}') {
                reading_format_specifier = false;
                if (is_num && !is_in_array(nums_by_appearance_in_format, current_num_arg, atoi(number_str.string))) {
                    arg_is_num_arr = realloc(arg_is_num_arr, current_num_arg + current_num_arg + 1);
                    arg_is_num_arr[current_num_arg + current_num_arg] = is_num;
                    nums_by_appearance_in_format = realloc(nums_by_appearance_in_format, ++current_num_arg * sizeof(int));
                    arg_is_double = realloc(arg_is_double, current_num_arg * sizeof(bool));
                    is_explicitly_declared = realloc(is_explicitly_declared, current_num_arg * sizeof(bool));
                    is_explicitly_declared[current_num_arg - 1] = replacement_char != '\0';
                    if (replacement_char != '\0') {
                        arg_is_double[current_num_arg-1] = (replacement_char == 'f');
                    }
                    nums_by_appearance_in_format[current_num_arg - 1] = atoi(number_str.string);
                    set_string(&number_str, "");
                }
                else if (!is_num && !is_in_array(chars_by_appearance_in_format, current_char_arg, (int) replacement_char) && !reading_double_length_restrictor) {
                    arg_is_num_arr = realloc(arg_is_num_arr, current_char_arg + current_num_arg + 1);
                    arg_is_num_arr[current_char_arg + current_num_arg] = is_num;
                    chars_by_appearance_in_format = realloc(chars_by_appearance_in_format, ++current_char_arg * sizeof(int));
                    chars_by_appearance_in_format[current_num_arg - 1] = replacement_char;
                }
                replacement_char = '\0';
                is_num = false;
                reading_double_length_restrictor = false;
            }
            else if (current_char == '.') {
                reading_double_length_restrictor = true;
            }
            else if (current_char >= '0' && current_char <= '9' && !reading_double_length_restrictor) {
                append_char(&number_str, current_char);
                is_num = true;
            }
            else if ((current_char >= 'A' && current_char <= 'Z') || (current_char >= 'a' && current_char <= 'z')) {
                replacement_char = current_char;
            }
        }
    }

    // Replacement Values
    int number_of_args = current_num_arg + current_char_arg;
    int ignoreAmount = 0;
    int number_of_number_args = current_num_arg;
    int num_of_general_args = 0;
    int num_of_float_args = 0;
    int current_arg = 0;
    double* replacement_numbers = calloc(number_of_number_args, sizeof(double));
    if (number_of_number_args != 0) {
        for (int i = 0; i < number_of_args; i++) {
            if (!arg_is_num_arr[i]) { num_of_general_args += IS_NOT_WINDOWS; continue; }

            array_min_pos(nums_by_appearance_in_format, number_of_number_args, ignoreAmount++, &current_arg);

            replacement_numbers[current_arg] = va_get_num(_Args, i, num_of_general_args, num_of_float_args, !is_explicitly_declared[current_arg], (arg_is_double + current_arg));

            num_of_general_args += !arg_is_double[current_arg] * IS_NOT_WINDOWS;
            num_of_float_args += arg_is_double[current_arg] * IS_NOT_WINDOWS;
        }
    }


    int number_of_char_args = current_char_arg;
    char** replacement_chars = calloc(number_of_char_args, 1 * sizeof(char*));
    if (number_of_char_args != 0) {
        current_arg = 0;
        ignoreAmount = 0;
        for (int i = 0; i < number_of_args; i++) {
            if (arg_is_num_arr[i]) continue;
            array_min_pos(chars_by_appearance_in_format, number_of_char_args, ignoreAmount++, &current_arg);
            replacement_chars[current_arg] = va_get_char(_Args, i, num_of_float_args);
        }
    }


    int string_pos = 0;
    int replacement_len = 0;

    is_num = false;
    set_empty_string(&number_str);
    int argument_index = 0;
    char* temp = calloc(100, sizeof(char));
    while (_Format[string_pos] != '\0') {
        current_char = _Format[string_pos];
        if (!reading_format_specifier && current_char != '{') {// Pass values
            if (new_string_len < new_string_pos) {
                _Buffer = realloc(_Buffer, new_string_pos + 1);
                new_string_len++;
            }
            _Buffer[new_string_pos++] = _Format[string_pos];
            _Buffer[new_string_pos] = '\0';
        }
        else { // Read replacement / replace
            if (current_char == '}') { // Replacement
                if (is_num) {
                    int current_number;
                    int double_restriction_length = -1;
                    if (!contains_char(number_str, '.'))
                        current_number = atoi(number_str.string);
                    else {
                        int periodIdx = char_position(number_str, '.');
                        char* temp_str = NULL;

                        before_index(number_str, periodIdx, &temp_str);
                        current_number = atoi(temp_str);

                        after_index(number_str, periodIdx, &temp_str);
                        double_restriction_length = atoi(temp_str);

                        free(temp_str - (periodIdx + 1));
                    }
                    for (int i = 0; i < number_of_number_args; i++) {
                        if (nums_by_appearance_in_format[i] == current_number) {
                            argument_index = i;
                            break;
                        }
                    }

                    if (arg_is_double[argument_index]) {
                        sprintf(temp, "%lf", replacement_numbers[argument_index]);
                        if (double_restriction_length != -1) {
                            temp[(double_restriction_length + char_array_length_before_char(temp, '.')) + 1] = '\0';
                            double_restriction_length = -1;
                        }
                    }
                    else {
                        sprintf(temp, "%ld", (long)replacement_numbers[argument_index]);
                    }
                    replacement_len = strlen(temp);

                    new_string_pos += replacement_len;
                    if (new_string_len < new_string_pos) {
                        _Buffer = realloc(_Buffer, new_string_pos + 2);
                        new_string_len = new_string_pos;
                    }
                    _Buffer[new_string_pos - replacement_len] = '\0';

                    strcat(_Buffer, temp);
                    set_empty_string(&number_str);
                    is_num = false;
                }
                else {
                    for (int i = 0; i < number_of_char_args; i++) {
                        if (chars_by_appearance_in_format[i] == replacement_char) {
                            argument_index = i;
                            break;
                        }
                    }
                    replacement_len = strlen(replacement_chars[argument_index]);

                    new_string_pos += replacement_len;
                    if (new_string_len < new_string_pos) {
                        _Buffer = realloc(_Buffer, new_string_pos + 2);
                        new_string_len = new_string_pos;
                    }
                    _Buffer[new_string_pos - replacement_len] = '\0';

                    strcat(_Buffer, (replacement_chars[argument_index]));

                }

                reading_format_specifier = false;
            }
            else { // Reading replacement value
                reading_format_specifier = true;


                if ((current_char >= '0' && current_char <= '9') || current_char == '.') {
                    append_char(&number_str, current_char);
                    is_num = true;
                }
                else if ((current_char >= 'A' && current_char <= 'Z') || (current_char >= 'a' && current_char <= 'z')) {
                    replacement_char = current_char;
                }


            }


        }
        string_pos++;
    }


    _Buffer[new_string_pos + 1] = '\0';
    *_BufferPtr = _Buffer;


    for (int i = 0; i < number_of_char_args; i++) {
        free(replacement_chars[i]);
    }
    free(replacement_chars);
    free(nums_by_appearance_in_format);
    free(chars_by_appearance_in_format);
    free(arg_is_num_arr);
    free(replacement_numbers);
    free(arg_is_double);
    free(is_explicitly_declared);
    free(temp);
    free(number_str.string);
}


/*
	_BufferPtr: A pointer to a char array, either allocated or not, where the formated string will be placed
	_Format: The format that will be placed into _BufferPtr

	Syntax: Curley Brackets will be completely removed* and replaced by the argument matching their number or letter in
	the _Format string.

	Arguments will be read by lowest value first. e.g. if 55 is the lowest number in curley brackets it will be interperated as the first number passed

	Numbers: The syntax for numbers is {n} where n a number pertaining to the desired argument.
	Numbers will be automatically detected as either long format or IEEE-754 (double) format.
	If automatic detection is incorrect and l or f can be added to specify format.

	Chars: The syntax for numbers is {*} where * is a letter pertaining to the desired argument.


	* Currently the only way to retain curley brackets is to pass them as a char or char*

*/
void formats(char** _Buffer, char* _Format, ...) {
    va_list args;

    va_start(args, _Format);

    vformats(_Buffer, _Format, args);

    va_end(args);
}


/*
	Prints a formated string using vformats

	_Format: The format that will be printed after replacement

	Syntax: Curley Brackets will be completely removed* and replaced by the argument matching their number or letter in
	the _Format string.

	Arguments will be read by lowest value first. e.g. if 55 is the lowest number in curley brackets it will be interperated as the first number passed

	Numbers: The syntax for numbers is {n} where n a number pertaining to the desired argument.
	Numbers will be automatically detected as either long format or IEEE-754 (double) format.
	If automatic detection is incorrect and l or f can be added to specify format.

	Chars: The syntax for numbers is {*} where * is a letter pertaining to the desired argument.


	* Currently the only way to retain curley brackets is to pass them as a char or string
*/
void vfprint_f(FILE* _Stream, char* _Format, va_list _Args) {
    char* Buffer = malloc(1);

    vformats(&Buffer, _Format, _Args);

    fputs(Buffer, _Stream);

    free(Buffer);
}


/*
	Prints a formated string using vfprint_f

	_Format: The format that will be printed after replacement

	Syntax: Curley Brackets will be completely removed* and replaced by the argument matching their number or letter in
	the _Format string.

	Arguments will be read by lowest value first. e.g. if 55 is the lowest number in curley brackets it will be interperated as the first number passed

	Numbers: The syntax for numbers is {n} where n a number pertaining to the desired argument.
	Numbers will be automatically detected as either long format or IEEE-754 (double) format.
	If automatic detection is incorrect and l or f can be added to specify format.

	Chars: The syntax for numbers is {*} where * is a letter pertaining to the desired argument.


	* Currently the only way to retain curley brackets is to pass them as a char or char*

*/
void print_f(char* _Format, ...) {
    va_list args;

    va_start(args, _Format);

    vfprint_f(stdout, _Format, args);

    va_end(args);
}


__attribute__((unused)) bool _TestFormatString(int iterations, double doubleInc) {
    char* buffer1 = calloc(16, 1);
    char* buffer2 = malloc(0);


    for (long i = 0; i < iterations; i++) {
        sprintf(buffer1, "%ld", i);
        printf("\b\b\b\b\b\b\b\b\b\b\b\b\bi: %ld", i);
        formats(&buffer2, "{0}", i);
        if (strcmp(buffer1, buffer2) != 0) {
            printf("INEQUAL: %ld != %s != %s\n", i, buffer1, buffer2);
            return false;
        }

    }


    // Exits if i is zero because formatString interperates 0 as long
    for (double i = doubleInc; i < iterations * doubleInc; i += doubleInc) {
        printf("\b\b\b\b\b\b\b\b\b\b\b\b\bi: %lf", i);
        sprintf(buffer1, "%lf", i);
        formats(&buffer2, "{0f}", i);
        if (strcmp(buffer1, buffer2) != 0) {
            printf("INEQUAL: %lf != %s != %s\n", i, buffer1, buffer2);
            return false;
        }
    }

    print_f("\nAll Tests Passed For {0} Iterations With a Double Increment Of {1}", iterations, doubleInc);

    free(buffer1);
    free(buffer2);

    return true;
}
