#pragma once


#ifdef __cplusplus
extern "C"
{
#endif


void core_handle_error(int err_no, const char* msg);

typedef void (*error_handler_fnc)(int err_no, const char* msg);
void core_set_error_handler(error_handler_fnc handler);

void logging_error_handler(int err_no, const char* msg);


#ifdef __cplusplus
} // extern "C"
#endif
