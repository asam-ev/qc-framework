/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _utilXerces_h__
#define _utilXerces_h__

#include <xercesc/dom/DOM.hpp>

// This is necessary for linux support
#define CONST_XMLCH(s) reinterpret_cast<const ::XMLCh *>(u##s)
static_assert(sizeof(::XMLCh) == sizeof(char16_t), "XMLCh is not sized correctly for UTF-16.");

#endif
