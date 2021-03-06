/**
 *  @file eglplus/string_query.hpp
 *  @brief EGL string query enumeration
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2010-2013 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once
#ifndef EGLPLUS_STRING_QUERY_1303292057_HPP
#define EGLPLUS_STRING_QUERY_1303292057_HPP

#include <eglplus/enumerations.hpp>

namespace eglplus {

/// EGL string query enumeration
/**
 *  @ingroup eglplus_enumerations
 */
EGLPLUS_ENUM_CLASS_BEGIN(StringQuery, EGLenum)
#include <eglplus/enums/string_query.ipp>
EGLPLUS_ENUM_CLASS_END(StringQuery)

#if !EGLPLUS_NO_ENUM_VALUE_NAMES
#include <eglplus/enums/string_query_names.ipp>
#endif

#if !EGLPLUS_ENUM_VALUE_RANGES
#include <eglplus/enums/string_query_range.ipp>
#endif

} // namespace eglplus

#endif // include guard
