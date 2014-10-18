/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 7-4 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_H
#define PHPDBG_H

#ifdef PHP_WIN32
# define PHPDBG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHPDBG_API __attribute__ ((visibility("default")))
#else
# define PHPDBG_API
#endif

#include "php.h"
#include "php_globals.h"
#include "php_variables.h"
#include "php_getopt.h"
#include "zend_builtin_functions.h"
#include "zend_extensions.h"
#include "zend_modules.h"
#include "zend_globals.h"
#include "zend_ini_scanner.h"
#include "zend_stream.h"
#ifndef _WIN32
# include "zend_signal.h"
#endif
#include "SAPI.h"
#include <fcntl.h>
#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__)
# include <windows.h>
# include "config.w32.h"
# undef  strcasecmp
# undef  strncasecmp
# define strcasecmp _stricmp 
# define strncasecmp _strnicmp 
#else
# include "php_config.h"
#endif
#ifndef O_BINARY
#	define O_BINARY 0
#endif
#include "php_main.h"

#ifdef ZTS
# include "TSRM.h"
#endif

#ifdef HAVE_LIBREADLINE
#   include <readline/readline.h>
#   include <readline/history.h>
#endif
#ifdef HAVE_LIBEDIT
#   include <editline/readline.h>
#endif

/* {{{ remote console headers */
#ifndef _WIN32
#	include <sys/socket.h>
#	include <sys/un.h>
#	include <sys/select.h>
#	include <sys/types.h>
#endif /* }}} */

/* {{{ strings */
#define PHPDBG_NAME "phpdbg"
#define PHPDBG_AUTHORS "Felipe Pena, Joe Watkins and Bob Weinand" /* Ordered by last name */
#define PHPDBG_URL "http://phpdbg.com"
#define PHPDBG_ISSUES "http://github.com/krakjoe/phpdbg/issues"
#define PHPDBG_VERSION "0.4.0"
#define PHPDBG_INIT_FILENAME ".phpdbginit"
#define PHPDBG_DEFAULT_PROMPT "prompt>"
/* }}} */

/* Hey, apple. One shouldn't define *functions* from the standard C library as marcos. */
#ifdef memcpy
#define memcpy_tmp(...) memcpy(__VA_ARGS__)
#undef memcpy
#define memcpy(...) memcpy_tmp(__VA_ARGS__)
#endif

#if !defined(PHPDBG_WEBDATA_TRANSFER_H) && !defined(PHPDBG_WEBHELPER_H)

#ifdef ZTS
# define PHPDBG_G(v) TSRMG(phpdbg_globals_id, zend_phpdbg_globals *, v)
#else
# define PHPDBG_G(v) (phpdbg_globals.v)
#endif

#include "phpdbg_sigsafe.h"
#include "phpdbg_lexer.h"
#include "phpdbg_cmd.h"
#include "phpdbg_utils.h"
#include "phpdbg_btree.h"
#include "phpdbg_watch.h"

int phpdbg_do_parse(phpdbg_param_t *stack, char *input TSRMLS_DC);

#define PHPDBG_NEXT   2
#define PHPDBG_UNTIL  3
#define PHPDBG_FINISH 4
#define PHPDBG_LEAVE  5

/*
 BEGIN: DO NOT CHANGE DO NOT CHANGE DO NOT CHANGE
*/

/* {{{ tables */
#define PHPDBG_BREAK_FILE            0
#define PHPDBG_BREAK_SYM             1
#define PHPDBG_BREAK_OPLINE          2
#define PHPDBG_BREAK_METHOD          3
#define PHPDBG_BREAK_COND            4
#define PHPDBG_BREAK_OPCODE          5
#define PHPDBG_BREAK_FUNCTION_OPLINE 6
#define PHPDBG_BREAK_METHOD_OPLINE   7
#define PHPDBG_BREAK_FILE_OPLINE     8
#define PHPDBG_BREAK_MAP             9
#define PHPDBG_BREAK_TABLES          10 /* }}} */

/* {{{ flags */
#define PHPDBG_HAS_FILE_BP            (1<<1)
#define PHPDBG_HAS_SYM_BP             (1<<2)
#define PHPDBG_HAS_OPLINE_BP          (1<<3)
#define PHPDBG_HAS_METHOD_BP          (1<<4)
#define PHPDBG_HAS_COND_BP            (1<<5)
#define PHPDBG_HAS_OPCODE_BP          (1<<6)
#define PHPDBG_HAS_FUNCTION_OPLINE_BP (1<<7)
#define PHPDBG_HAS_METHOD_OPLINE_BP   (1<<8)
#define PHPDBG_HAS_FILE_OPLINE_BP     (1<<9) /* }}} */

/*
 END: DO NOT CHANGE DO NOT CHANGE DO NOT CHANGE
*/

#define PHPDBG_IN_COND_BP             (1<<10)
#define PHPDBG_IN_EVAL                (1<<11)

#define PHPDBG_IS_STEPPING            (1<<12)
#define PHPDBG_STEP_OPCODE            (1<<13)
#define PHPDBG_IS_QUIET               (1<<14)
#define PHPDBG_IS_QUITTING            (1<<15)
#define PHPDBG_IS_COLOURED            (1<<16)
#define PHPDBG_IS_CLEANING            (1<<17)

#define PHPDBG_IN_UNTIL               (1<<18)
#define PHPDBG_IN_FINISH              (1<<19)
#define PHPDBG_IN_LEAVE               (1<<20)

#define PHPDBG_IS_REGISTERED          (1<<21)
#define PHPDBG_IS_STEPONEVAL          (1<<22)
#define PHPDBG_IS_INITIALIZING        (1<<23)
#define PHPDBG_IS_SIGNALED            (1<<24)
#define PHPDBG_IS_INTERACTIVE         (1<<25)
#define PHPDBG_IS_BP_ENABLED          (1<<26)
#define PHPDBG_IS_REMOTE              (1<<27)
#define PHPDBG_IS_DISCONNECTED        (1<<28)
#define PHPDBG_WRITE_XML              (1<<29)

#define PHPDBG_SHOW_REFCOUNTS         (1<<30)

#define PHPDBG_IN_SIGNAL_HANDLER      (1<<30)

#define PHPDBG_SEEK_MASK              (PHPDBG_IN_UNTIL|PHPDBG_IN_FINISH|PHPDBG_IN_LEAVE)
#define PHPDBG_BP_RESOLVE_MASK	      (PHPDBG_HAS_FUNCTION_OPLINE_BP|PHPDBG_HAS_METHOD_OPLINE_BP|PHPDBG_HAS_FILE_OPLINE_BP)
#define PHPDBG_BP_MASK                (PHPDBG_HAS_FILE_BP|PHPDBG_HAS_SYM_BP|PHPDBG_HAS_METHOD_BP|PHPDBG_HAS_OPLINE_BP|PHPDBG_HAS_COND_BP|PHPDBG_HAS_OPCODE_BP|PHPDBG_HAS_FUNCTION_OPLINE_BP|PHPDBG_HAS_METHOD_OPLINE_BP|PHPDBG_HAS_FILE_OPLINE_BP)

#ifndef _WIN32
#	define PHPDBG_DEFAULT_FLAGS (PHPDBG_IS_QUIET|PHPDBG_IS_COLOURED|PHPDBG_IS_BP_ENABLED)
#else
#	define PHPDBG_DEFAULT_FLAGS (PHPDBG_IS_QUIET|PHPDBG_IS_BP_ENABLED)
#endif /* }}} */

/* {{{ output descriptors */
#define PHPDBG_STDIN 			0
#define PHPDBG_STDOUT			1
#define PHPDBG_STDERR			2
#define PHPDBG_IO_FDS 			3 /* }}} */

#define phpdbg_try_access \
	{                                                            \
		JMP_BUF *__orig_bailout = PHPDBG_G(sigsegv_bailout); \
		JMP_BUF __bailout;                                   \
                                                                     \
		PHPDBG_G(sigsegv_bailout) = &__bailout;              \
		if (SETJMP(__bailout) == 0) {
#define phpdbg_catch_access \
		} else {                                             \
			PHPDBG_G(sigsegv_bailout) = __orig_bailout;
#define phpdbg_end_try_access() \
		}                                                    \
			PHPDBG_G(sigsegv_bailout) = __orig_bailout;  \
	}


/* {{{ structs */
ZEND_BEGIN_MODULE_GLOBALS(phpdbg)
	HashTable bp[PHPDBG_BREAK_TABLES];           /* break points */
	HashTable registered;                        /* registered */
	HashTable seek;                              /* seek oplines */
	phpdbg_frame_t frame;                        /* frame */
	zend_uint last_line;                         /* last executed line */

	phpdbg_lexer_data lexer;                     /* lexer data */
	phpdbg_param_t *parser_stack;                /* param stack during lexer / parser phase */

#ifndef _WIN32
	struct sigaction old_sigsegv_signal;         /* segv signal handler */
#endif
	phpdbg_btree watchpoint_tree;                /* tree with watchpoints */
	phpdbg_btree watch_HashTables;               /* tree with original dtors of watchpoints */
	HashTable watchpoints;                       /* watchpoints */
	zend_llist watchlist_mem;                    /* triggered watchpoints */
	zend_bool watchpoint_hit;                    /* a watchpoint was hit */
	void (*original_free_function)(void *);      /* the original AG(mm_heap)->_free function */

	char *exec;                                  /* file to execute */
	size_t exec_len;                             /* size of exec */
	zend_op_array *ops;                 	     /* op_array */
	zval *retval;                                /* return value */
	int bp_count;                                /* breakpoint count */
	int vmret;                                   /* return from last opcode handler execution */

	zend_op_array *(*compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);
	HashTable file_sources;

	FILE *oplog;                                 /* opline log */
	struct {
		FILE *ptr;
		int fd;
	} io[PHPDBG_IO_FDS];                         /* io */
#ifndef _WIN32
	size_t (*php_stdiop_write)(php_stream *, const char *, size_t TSRMLS_DC);
#endif
	int in_script_xml;                           /* in <stream> output mode */
	struct {
		zend_bool active;
		int type;
		int fd;
		char *tag;
		char *msg;
		int msglen;
		char *xml;
		int xmllen;
	} err_buf;                                   /* error buffer */
	zend_ulong req_id;                           /* "request id" to keep track of commands */

	char *prompt[2];                             /* prompt */
	const phpdbg_color_t *colors[PHPDBG_COLORS]; /* colors */
	char *buffer;                                /* buffer */
	zend_bool last_was_newline;                  /* check if we don't need to output a newline upon next phpdbg_error or phpdbg_notice */

	char input_buffer[PHPDBG_MAX_CMD];           /* stdin input buffer */
	int input_buflen;                            /* length of stdin input buffer */
	phpdbg_signal_safe_mem sigsafe_mem;          /* memory to use in async safe environment (only once!) */

	JMP_BUF *sigsegv_bailout;                    /* bailout address for accesibility probing */

	zend_ulong flags;                            /* phpdbg flags */

	char *socket_path;                           /* phpdbg.path ini setting */
	char *sapi_name_ptr;                         /* store sapi name to free it if necessary to not leak memory */
	int socket_fd;                               /* file descriptor to socket (wait command) (-1 if unused) */
	int socket_server_fd;                        /* file descriptor to master socket (wait command) (-1 if unused) */
ZEND_END_MODULE_GLOBALS(phpdbg) /* }}} */

#endif

#endif /* PHPDBG_H */
