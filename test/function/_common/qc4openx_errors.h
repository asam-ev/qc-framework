/*
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _QC4OPENX_UTILITY_ERRORS_H
#define _QC4OPENX_UTILITY_ERRORS_H

namespace qc4openx
{
/// \copydoc a_util::result::Result
using a_util::result::Result;
/// \copydoc a_util::result::is_ok
using a_util::result::isOk;
/// \copydoc a_util::result::is_failed
using a_util::result::isFailed;

/// \cond nodoc
_MAKE_RESULT(0, ERR_NOERROR);
_MAKE_RESULT(-2, ERR_UNKNOWN);
_MAKE_RESULT(-3, ERR_UNEXPECTED);
_MAKE_RESULT(-4, ERR_UNKNOWN_FORMAT);
_MAKE_RESULT(-5, ERR_FILE_NOT_FOUND);
_MAKE_RESULT(-6, ERR_FAILED);

/// \endcond
} // namespace qc4openx
#ifndef COMPARATORS_OVERLOADED
#define COMPARATORS_OVERLOADED
inline qc4openx::Result operator|(const qc4openx::Result &lhs, const qc4openx::Result &rhs)
{
    if (a_util::result::isFailed(lhs))
    {
        return lhs;
    }
    return rhs;
}

inline qc4openx::Result &operator|=(qc4openx::Result &lhs, const qc4openx::Result &rhs)
{
    lhs = lhs | rhs;
    return lhs;
}
#endif // COMPARATORS_OVERLOADED
/// Provide legacy RETURN_IF_FAILED implementation
#ifndef RETURN_IF_FAILED
#define RETURN_IF_FAILED(s)                                                                                            \
    {                                                                                                                  \
        qc4openx::Result _errcode(s);                                                                                  \
        if (qc4openx::isFailed(_errcode))                                                                              \
        {                                                                                                              \
            return (_errcode);                                                                                         \
        }                                                                                                              \
    }
#endif

#endif // _QC4OPENX_UTILITY_ERRORS_H
