// ----------------------------------------------------------------------------
// dbglog.h - A set of macros that cleans up code that needs to produce debug
//            or log information.
//
// See copyright notice in LICENSE.TXT
// ----------------------------------------------------------------------------
//
// There are macros to handle the following decreasing levels of detail:
//
// 6 = TRACE
// 5 = DEBUG
// 4 = CRITICAL
// 3 = ERROR
// 2 = WARNING
// 1 = INFO
// 0 = FORCE - The printf is always compiled in and is called only when
//              the first parameter to the macro is non-0
//
// ----------------------------------------------------------------------------
// 
// The following #define should be set up before this file is included so
// that we can be sure that the correct macros are defined.
//
// #define DBG_LOG_LEVEL x
// ----------------------------------------------------------------------------

#ifndef DBG_LOG_LEVEL
# error "DBG_LOG_LEVEL is not defined!"
#endif

// ----------------------------------------------------------------------------
//
// FIXME: Currently the macros are defined at compile time, which means that
//        the debug level is fixed. It will be possible in later versions to
//        set up run time control of debug info at the expense of speed and
//        code size
// ----------------------------------------------------------------------------

#undef DBG_LOG_TRACE
#undef DBG_LOG_DEBUG
#undef DBG_LOG_CRITICAL
#undef DBG_LOG_ERROR
#undef DBG_LOG_WARNING
#undef DBG_LOG_INFO
#undef DBG_LOG_FORCE

// ----------------------------------------------------------------------------

#if DBG_LOG_LEVEL >= 6
#  define DBG_LOG_TRACE( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_TRACE( format, ... )
#endif

#if DBG_LOG_LEVEL >= 5
#  define DBG_LOG_DEBUG( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_DEBUG( format, ... )
#endif

#if DBG_LOG_LEVEL >= 4
#  define DBG_LOG_CRITICAL( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_CRITICAL( format, ... )
#endif

#if DBG_LOG_LEVEL >= 3
#  define DBG_LOG_ERROR( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_ERROR( format, ... )
#endif

#if DBG_LOG_LEVEL >= 2
#  define DBG_LOG_WARNING( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_WARNING( format, ... )
#endif

#if DBG_LOG_LEVEL >= 1
#  define DBG_LOG_INFO( format, ... ) printf( format, ## __VA_ARGS__ )
#else
#  define DBG_LOG_INFO( format, ... )
#endif

#define DBG_LOG_FORCE( force, format, ... ) {if(force) {printf( format, ## __VA_ARGS__  );}}

// ----------------------------------------------------------------------------
