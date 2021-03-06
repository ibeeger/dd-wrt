/* Generated by ./xlat/gen.sh from ./xlat/nf_osf_msg_types.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(OSF_MSG_ADD) || (defined(HAVE_DECL_OSF_MSG_ADD) && HAVE_DECL_OSF_MSG_ADD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((OSF_MSG_ADD) == (0), "OSF_MSG_ADD != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define OSF_MSG_ADD 0
#endif
#if defined(OSF_MSG_REMOVE) || (defined(HAVE_DECL_OSF_MSG_REMOVE) && HAVE_DECL_OSF_MSG_REMOVE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((OSF_MSG_REMOVE) == (1), "OSF_MSG_REMOVE != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define OSF_MSG_REMOVE 1
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat nf_osf_msg_types in mpers mode

# else

static
const struct xlat nf_osf_msg_types[] = {
 XLAT(OSF_MSG_ADD),
 XLAT(OSF_MSG_REMOVE),
 XLAT_END
};

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */
