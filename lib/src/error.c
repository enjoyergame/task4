/*
Коды ошибок и функции работы с ошибками
Кучуков Ридаль Радикович
МК-101
*/

#include "passgen/error.h"

const char *passgen_error_message(PassgenError code)
{
    switch (code) {
        case PASSGEN_OK:
            return "";
        case PASSGEN_ERR_MISSING_VALUE:
            return "Error: option is missing a value";
        case PASSGEN_ERR_UNKNOWN_SEPARATOR:
            return "Error: invalid separator syntax";
        case PASSGEN_ERR_NOT_A_NUMBER:
            return "Error: option value must be a number";
        case PASSGEN_ERR_DUPLICATE_OPTION:
            return "Error: option specified more than once";
        case PASSGEN_ERR_DUPLICATE_CHAR:
            return "Error: duplicate character in set";
        case PASSGEN_ERR_CONFLICT_N_MINMAX:
            return "Error: options -n and -minl/-maxl are incompatible";
        case PASSGEN_ERR_CONFLICT_A_C:
            return "Error: options -a and -C are incompatible";
        case PASSGEN_ERR_CONFLICT_PAIR_N:
            return "Error: paired option is incompatible with -n";
        case PASSGEN_ERR_INCOMPLETE_PAIR:
            return "Error: only one of the paired options was specified";
        case PASSGEN_ERR_INCOMPLETE_RANGE:
            return "Error: only one length bound (-minl/-maxl) was specified";
        case PASSGEN_ERR_RANGE_ORDER:
            return "Error: minimum length (-minl) cannot be greater than maximum length (-maxl)";
        case PASSGEN_ERR_PROB_OVERFLOW:
            return "Error: sum of probabilities exceeds 1.0";
        case PASSGEN_ERR_PROB_INVALID:
            return "Error: probability out of valid range";
        case PASSGEN_ERR_UNKNOWN_SYMBOL:
            return "Error: symbol in probability rules not found in the alphabet";
        case PASSGEN_ERR_INVALID_CHARSET_GROUP:
            return "Error: invalid character set group (expected a, A, D, or S)";
        case PASSGEN_ERR_EMPTY_ALPHABET:
            return "Error: resulting character set is empty";
        case PASSGEN_ERR_BUFFER_TOO_SMALL:
            return "Error: output buffer is too small for the password";
        case PASSGEN_ERR_NO_MEMORY:
            return "Error: memory allocation failed";
        case PASSGEN_ERR_UNKNOWN:
        default:
            return "Error: unknown error";
    }
}