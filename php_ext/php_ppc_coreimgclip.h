/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PPC_COREIMGCLIP_H
#define PHP_PPC_COREIMGCLIP_H

extern zend_module_entry ppc_coreimgclip_module_entry;
#define phpext_ppc_coreimgclip_ptr &ppc_coreimgclip_module_entry

#define PHP_PPC_COREIMGCLIP_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_PPC_COREIMGCLIP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PPC_COREIMGCLIP_API __attribute__ ((visibility("default")))
#else
#	define PHP_PPC_COREIMGCLIP_API
#endif

extern "C" {
#ifdef ZTS
#include "TSRM.h"
#endif
}

PHP_MINIT_FUNCTION(ppc_coreimgclip);
PHP_MSHUTDOWN_FUNCTION(ppc_coreimgclip);
PHP_RINIT_FUNCTION(ppc_coreimgclip);
PHP_RSHUTDOWN_FUNCTION(ppc_coreimgclip);
PHP_MINFO_FUNCTION(ppc_coreimgclip);

PHP_FUNCTION(ppc_coreimgclip);	/* For testing, remove later. */
/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/

ZEND_BEGIN_MODULE_GLOBALS(ppc_coreimgclip)
	//long  global_value;
	//char *global_string;
	int runtime;
ZEND_END_MODULE_GLOBALS(ppc_coreimgclip)


/* In every utility function you add that needs to use variables 
   in php_ppc_coreimgclip_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as PPC_COREIMGCLIP_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define PPC_COREIMGCLIP_G(v) TSRMG(ppc_coreimgclip_globals_id, zend_ppc_coreimgclip_globals *, v)
#else
#define PPC_COREIMGCLIP_G(v) (ppc_coreimgclip_globals.v)
#endif

#endif	/* PHP_PPC_COREIMGCLIP_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
