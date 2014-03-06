
/*
 * Adding a builtin_define to make sure _MIOSIX is always
 * defined, in particular
 * - when newlib is compiled, as there are some #ifdef _MIOSIX
 * - when libgcc is compiled, as gthr-miosix.h includes pthread.h from newlib
 *   which again requires _MIOSIX to be defined
 * - when Miosix processes are compiled, to allow #ifdef _MIOSIX
 */

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()         \
    do {                                 \
        TARGET_BPABI_CPP_BUILTINS();     \
        builtin_define("_MIOSIX");       \
        builtin_assert("system=miosix"); \
    } while(false)
