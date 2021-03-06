#ifndef _INCLUDE_EXTENSION_VERSION_INFORMATION_H_
#define _INCLUDE_EXTENSION_VERSION_INFORMATION_H_

/**
 * @file Contains module version information.
 * @brief This file will redirect to an autogenerated version if being compiled via
 * the build scripts.
 */

#if defined MODULE_GENERATED_BUILD
	#if defined RC_COMPILE
		#undef MODULE_USE_VERSIONLIB
	#endif
	#if defined MODULE_USE_VERSIONLIB
		#include <versionlib.h>
	#else 
		#include <module_version_auto.h>
	#endif
#else
	#define EXTENSION_BUILD_TAG        "manual"
	#define EXTENSION_BUILD_LOCAL_REV  "0"
	#define EXTENSION_BUILD_CSET       "0"
	#define EXTENSION_BUILD_MAJOR      "1"
	#define EXTENSION_BUILD_MINOR      "5"
	#define EXTENSION_BUILD_RELEASE    "5"

	#define EXTENSION_BUILD_UNIQUEID   EXTENSION_BUILD_LOCAL_REV ":" EXTENSION_BUILD_CSET

	#define EXTENSION_VERSION_STRING   EXTENSION_BUILD_MAJOR "." EXTENSION_BUILD_MINOR "." EXTENSION_BUILD_RELEASE "-" EXTENSION_BUILD_TAG
	#define EXTENSION_VERSION_FILE     1,5,5,0
#endif

#define EXTENSION_BUILD_TIMESTAMP  __DATE__ " " __TIME__

#if !defined(MODULE_GENERATED_BUILD) || !defined(MODULE_USE_VERSIONLIB)
	#define EXTENSION_VERSION      EXTENSION_VERSION_STRING
	#define EXTENSION_BUILD_ID     EXTENSION_BUILD_UNIQUEID
	#define EXTENSION_BUILD_TIME   EXTENSION_BUILD_TIMESTAMP
#endif

#endif /* _INCLUDE_EXTENSION_VERSION_INFORMATION_H_ */

