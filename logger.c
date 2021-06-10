#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

struct s_core;

struct s_core* core_instance();
void core_handle_error(int err_no, const char* msg);
enum log_level {
    log_level_trace,
    log_level_debug,
    log_level_info,
    log_level_warning,
    log_level_error
};
void core_log_message(enum log_level level, const char* format, ...);


typedef void (*log_fnc)(enum log_level level, const char* format, va_list ap);

struct s_log {
    log_fnc log_;
};

static FILE* logging_fd = 0;

struct s_log* log_new(log_fnc logger) {
    struct s_log* log_instance = malloc(sizeof(struct s_log));
    if(log_instance == 0) {
        // Окей, обработка выделения памяти есть, но при ней возникает рекурсия
        // core_error_handler -> core_instance -> log_new
        // core_handle_error(255, "Fatal error, out of memory");
        perror("Out of memory");
        abort();
    }
    log_instance->log_ = logger;
    // Присваивание в условии
    if (logging_fd == 0) {
        // Режим открытия файла.
        // Блин, я с ним дольше всего провозился.
        // Привык, режим открытия можно комбинировать: https://en.cppreference.com/w/cpp/io/basic_fstream/open
        logging_fd = fopen("test.log", "w");
        if (logging_fd == 0) {
            perror("Can`t open file");
            abort();
        }
        // А если файл не открылся? Например есть директория с таким именем
        // Раз не указано в условиях что делать, то как обычно просто завали процесс. Зато ошибка будет явной.
    }
    return log_instance;
}


void default_log(enum log_level level, const char* format, va_list ap) {
    vfprintf(logging_fd, format, ap);
}

struct s_error_handler {
    error_handler_fnc handle_;
};

struct s_error_handler* error_handler_new(error_handler_fnc handler) {
    // Выделяемый размер блока памяти равен размеру указателя
    struct s_error_handler* handler_ = malloc(sizeof(struct s_error_handler));
    // Нужна обработка ошибки выделения памяти
    if (handler_ == 0) {
        perror("Out of memory");
        abort();
    }
    handler_->handle_ = handler;
    return handler_;
}

void default_error_handler(int err_no, const char* msg) {
    printf("Message %d, %s", err_no, msg);
}

void logging_error_handler(int err_no, const char* msg) {
    core_log_message(log_level_error, "Message %d, %s", msg);
}

// Думаю стоит поменять местами имена handler и handle
// handler это обработчик, не место его размещения (handle)
struct s_core {
    struct s_error_handler* handler_;
    struct s_log* log_;
};

struct s_core* core_instance() {
    static struct s_core* core = 0;
    // Not thread-safe, maybe need?
    if (core == 0) {
        core = malloc(sizeof(struct s_core));
        // Нужна обработка нехватки памяти
        // Проблема не решается на уровне ее возникновения
        // Если не получилось выделить память на такую маленькую структуру, то дальше работать нет смысла
        // А вообще имеет сделать статическое размещение корневого логера (и его внутренностей), если уж запустилось, то для него точно память будет
        if (core == 0) {
            perror("Out of memory");
            abort();
        }
        core->log_ = log_new(default_log);
        core->handler_ = error_handler_new(default_error_handler);
    }
    return core;
}

void core_log_message(enum log_level level, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    core_instance()->log_->log_(level, format, ap);
    va_end(ap);
}

void core_handle_error(int err_no, const char* msg) {
    core_instance()->handler_->handle_(err_no, msg);
}

void core_set_error_handler(error_handler_fnc handler) {
    struct s_core* core = core_instance();
    core->handler_->handle_ = handler;
}
