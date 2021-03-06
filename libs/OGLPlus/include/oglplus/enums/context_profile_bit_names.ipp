/*
 *  .file oglplus/enums/context_profile_bit_names.ipp
 *
 *  Automatically generated header file. DO NOT modify manually,
 *  edit 'source/enums/oglplus/context_profile_bit.txt' instead.
 *
 *  Copyright 2010-2014 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

namespace enums {
OGLPLUS_LIB_FUNC StrLit ValueName_(
	ContextProfileBit*,
	GLenum value
)
#if (!OGLPLUS_LINK_LIBRARY || defined(OGLPLUS_IMPLEMENTING_LIBRARY)) && \
	!defined(OGLPLUS_IMPL_EVN_CONTEXTPROFILEBIT)
#define OGLPLUS_IMPL_EVN_CONTEXTPROFILEBIT
{
switch(value)
{
#if defined GL_CONTEXT_CORE_PROFILE_BIT
	case GL_CONTEXT_CORE_PROFILE_BIT: return StrLit("CONTEXT_CORE_PROFILE_BIT");
#endif
#if defined GL_CONTEXT_COMPATIBILITY_PROFILE_BIT
	case GL_CONTEXT_COMPATIBILITY_PROFILE_BIT: return StrLit("CONTEXT_COMPATIBILITY_PROFILE_BIT");
#endif
	default:;
}
OGLPLUS_FAKE_USE(value);
return StrLit();
}
#else
;
#endif
} // namespace enums

