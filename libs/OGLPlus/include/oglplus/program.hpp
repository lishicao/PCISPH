/**
 *  @file oglplus/program.hpp
 *  @brief Program wrappers
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2010-2014 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once
#ifndef OGLPLUS_PROGRAM_1107121519_HPP
#define OGLPLUS_PROGRAM_1107121519_HPP

#include <oglplus/config/compiler.hpp>
#include <oglplus/object/wrapper.hpp>
#include <oglplus/object/sequence.hpp>
#include <oglplus/error/program.hpp>
#include <oglplus/error/prog_var.hpp>
#include <oglplus/data_type.hpp>
#include <oglplus/transform_feedback_mode.hpp>
#include <oglplus/program_resource.hpp>
#include <oglplus/primitive_type.hpp>
#include <oglplus/face_mode.hpp>
#include <oglplus/glsl_source.hpp>
#include <oglplus/vertex_attrib_slot.hpp>
#include <oglplus/detail/base_range.hpp>

#include <vector>
#include <cassert>
#include <tuple>

namespace oglplus {

class VertexAttribOps;

/// Class wrapping program construction/destruction functions
/** @note Do not use this class directly, use Program instead.
 *
 *  @glsymbols
 *  @glfunref{CreateProgram}
 *  @glfunref{DeleteProgram}
 *  @glfunref{IsProgram}
 */
template <>
class ObjGenDelOps<tag::Program>
{
protected:
	static void Gen(GLsizei count, GLuint* names)
	{
		assert(names != nullptr);
		for(GLsizei i=0; i<count; ++i)
		{
			names[i] = OGLPLUS_GLFUNC(CreateProgram)();
			OGLPLUS_CHECK_SIMPLE(CreateProgram);
		}
	}

	static void Delete(GLsizei count, GLuint* names)
	{
		assert(names != nullptr);
		for(GLsizei i=0; i<count; ++i)
		{
			OGLPLUS_GLFUNC(DeleteProgram)(names[i]);
			OGLPLUS_VERIFY_SIMPLE(DeleteProgram);
		}
	}

	static GLboolean IsA(GLuint name)
	{
		assert(name != 0);
		GLboolean result = OGLPLUS_GLFUNC(IsProgram)(name);
		OGLPLUS_VERIFY_SIMPLE(IsProgram);
		return result;
	}
};

/// Program binding operations
template <>
class ObjBindingOps<tag::Program>
{
protected:
	static GLuint _binding(void)
	{
		GLint name = 0;
		OGLPLUS_GLFUNC(GetIntegerv)(GL_CURRENT_PROGRAM, &name);
		OGLPLUS_VERIFY(
			GetIntegerv,
			Error,
			EnumParam(GLenum(GL_CURRENT_PROGRAM))
		);
		return name;
	}
public:
	/// Returns the currently bound (active) Program
	/**
	 *  @glsymbols
	 *  @glfunref{GetIntegerv}
	 */
	static ProgramName Binding(void)
	{
		return ProgramName(_binding());
	}

	/// Binds (uses) the specified @p program
	/**
	 *  @glsymbols
	 *  @glfunref{UseProgram}
	 */
	static void Bind(ProgramName program)
	{
		OGLPLUS_GLFUNC(UseProgram)(GetGLName(program));
		OGLPLUS_VERIFY(
			UseProgram,
			ObjectError,
			Object(program)
		);
	}
};

/// Common program operations
/** @note Do not use this class directly, use Program
 *  or NoProgram instead.
 */
template <>
class ObjCommonOps<tag::Program>
 : public ProgramName
 , public ObjBindingOps<tag::Program>
{
protected:
	ObjCommonOps(void){ }
public:
	using ObjBindingOps<tag::Program>::Bind;

	/// Binds (uses) this program object
	/**
	 *  @note The program must be linked before it is used.
	 *  @pre IsLinked()
	 *  @pre IsValid()
	 *
	 *  @glsymbols
	 *  @glfunref{UseProgram}
	 */
	void Bind(void) const
	{
		Bind(*this);
	}

	/// Uses this program object
	/**
	 *  @note The program must be linked before it is used.
	 *  @pre IsLinked()
	 *  @pre IsValid()
	 *
	 *  @glsymbols
	 *  @glfunref{UseProgram}
	 */
	void Use(void) const
	{
		Bind(*this);
	}
};


/// Class wrapping program functions (with direct state access)
/** @note Do not use this class directly, use Program instead.
 */
template <>
class ObjectOps<tag::DirectState, tag::Program>
 : public ObjZeroOps<tag::DirectState, tag::Program>
{
protected:
	ObjectOps(void){ }
public:
	GLint GetIntParam(GLenum query) const
	{
		GLint result;
		OGLPLUS_GLFUNC(GetProgramiv)(_name, query, &result);
		OGLPLUS_VERIFY(
			GetProgramiv,
			ObjectError,
			Object(*this).
			EnumParam(query)
		);
		return result;
	}

#if GL_VERSION_4_0 || GL_ARB_shader_subroutine
	GLint GetStageIntParam(GLenum stage, GLenum query) const
	{
		GLint result;
		OGLPLUS_GLFUNC(GetProgramStageiv)(_name, stage, query, &result);
		OGLPLUS_VERIFY(
			GetProgramStageiv,
			ObjectError,
			Object(*this).
			EnumParam(query)
		);
		return result;
	}
#endif

	/// Attaches the shader to this program
	/**
	 *  @glsymbols
	 *  @glfunref{AttachShader}
	 */
	ObjectOps& AttachShader(ShaderName shader);

	/// Attaches a group of shaders to this program
	ObjectOps& AttachShaders(const Sequence<ShaderName>& shaders);

	/// Detaches the shader from this program
	/**
	 *  @glsymbols
	 *  @glfunref{DetachShader}
	 */
	ObjectOps& DetachShader(ShaderName shader);

	/// Returns true if the program is already linked, false otherwise
	/**
	 *  @see Link
	 *  @see Validate
	 *
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{LINK_STATUS}
	 */
	bool IsLinked(void) const
	{
		return GetIntParam(GL_LINK_STATUS) == GL_TRUE;
	}

	/// Returns the linker output if the program is linked
	/**
	 *  @see IsLinked
	 *  @see Link
	 *
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @glfunref{GetProgramInfoLog}
	 */
	String GetInfoLog(void) const;

	/// Links this shading language program
	/**
	 *  @post IsLinked()
	 *  @throws Error LinkError
	 *  @see IsLinked
	 *
	 *  @glsymbols
	 *  @glfunref{LinkProgram}
	 *  @glfunref{GetProgram}
	 *  @glfunref{GetProgramInfoLog}
	 */
	ObjectOps& Link(void);

	/// Returns true if the program is validated, false otherwise
	/**
	 *  @see Validate
	 *
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{VALIDATE_STATUS}
	 */
	bool IsValid(void) const
	{
		return GetIntParam(GL_VALIDATE_STATUS) == GL_TRUE;
	}

	/// Validates this shading language program
	/**
	 *  @post IsValid()
	 *  @throws Error ValidationError
	 *  @see Link
	 *
	 *  @glsymbols
	 *  @glfunref{ValidateProgram}
	 *  @glfunref{GetProgram}
	 *  @glfunref{GetProgramInfoLog}
	 */
	ObjectOps& Validate(void);

	/// Sets the variables that will be captured during transform feedback
	/**
	 *  @throws Error
	 *
	 *  @glsymbols
	 *  @glfunref{TransformFeedbackVaryings}
	 */
	void TransformFeedbackVaryings(
		GLsizei count,
		const GLchar** varyings,
		TransformFeedbackMode mode
	);

	/// Sets the variable that will be captured during transform feedback
	/**
	 *  @throws Error
	 *
	 *  @glsymbols
	 *  @glfunref{TransformFeedbackVaryings}
	 */
	void TransformFeedbackVarying(const GLchar* varying)
	{
		TransformFeedbackVaryings(
			1, &varying,
			TransformFeedbackMode::SeparateAttribs
		);
	}

	template <typename std::size_t N>
	void TransformFeedbackVaryings(
		const GLchar* (&varyings)[N],
		TransformFeedbackMode mode
	)
	{
		TransformFeedbackVaryings(N, varyings, mode);
	}

	/// Sets the variables that will be captured during transform feedback
	/**
	 *  @throws Error
	 *
	 *  @glsymbols
	 *  @glfunref{TransformFeedbackVaryings}
	 */
	void TransformFeedbackVaryings(
		const std::vector<String>& varyings,
		TransformFeedbackMode mode
	) const;

#if OGLPLUS_DOCUMENTATION_ONLY
	/// Information about a single active vertex attribute or uniform
	/** Note that the Program's functions documented as returning instances
	 *  of ActiveVariableInfo actually return types convertible to
	 *  ActiveVariableInfo.
	 */
	class ActiveVariableInfo
	{
	public:
		/// Returns the index of the attribute or uniform
		GLuint Index(void) const;

		/// Returns the name (identifier) of the attribute or uniform
		const String& Name(void) const;

		/// Returns the size in units of Type
		const GLint Size(void) const;

		/// Returns the data type of the variable
		const SLDataType Type(void) const;
	};

	/// Helper class for efficient iteration of Program interface items
	/** Instances of this class are created by a program for its specific
	 *  interfaces (uniform, vertex attributes, subroutines, etc.) or
	 *  stages (vertex, geometry, fragment, etc.). Instances of an interface
	 *  context can be used (mostly internally) for efficient iteration
	 *  of individual items of a particular interface (uniforms, subroutines,
	 *  etc.). Contexts for various programs and various interfaces are not
	 *  interchangeable.
	 *  The InterfaceContext type should be treated as opaque and only used
	 *  with appropriate functions.
	 */
	typedef Unspecified InterfaceContext;

	/// The type of the range for traversing program resource information
	typedef Range<ProgramResource> ActiveResourceRange;

	/// The type of the range for traversing active vertex attributes
	typedef Range<ActiveVariableInfo> ActiveAttribRange;
	/// The type of the range for traversing active uniforms
	typedef Range<ActiveVariableInfo> ActiveUniformRange;
	/// The type of the range for traversing active subroutine uniforms
	typedef Range<ActiveVariableInfo> ActiveSubroutineUniformRange;
	/// The type of the range for traversing transform feedback varyings
	typedef Range<ActiveVariableInfo> TransformFeedbackVaryingRange;
	/// The type of the range for traversing program's shaders
	typedef Range<Managed<Shader> > ShaderRange;
#else
	typedef aux::ActiveVariableInfo ActiveVariableInfo;
	typedef aux::ProgramInterfaceContext InterfaceContext;

#if GL_VERSION_4_3
	typedef aux::ContextElementRange<
		aux::ProgramInterfaceContext,
		ProgramResource
	> ActiveResourceRange;
#endif

	typedef aux::ContextElementRange<
		aux::ProgramInterfaceContext,
		aux::ActiveAttribInfo
	> ActiveAttribRange;

	typedef aux::ContextElementRange<
		aux::ProgramInterfaceContext,
		aux::ActiveUniformInfo
	> ActiveUniformRange;

#if GL_VERSION_4_0 || GL_ARB_shader_subroutine
	typedef aux::ContextElementRange<
		aux::ProgramInterfaceContext,
		aux::ActiveSubroutineInfo
	> ActiveSubroutineRange;

	typedef aux::ContextElementRange<
		aux::ProgramInterfaceContext,
		aux::ActiveSubroutineUniformInfo
	> ActiveSubroutineUniformRange;
#endif
	typedef aux::ContextElementRange<
		aux::ProgramInterfaceContext,
		aux::TransformFeedbackVaryingInfo
	> TransformFeedbackVaryingRange;

	struct ShaderIterationContext
	{
		std::vector<GLuint> _shader_names;

		ShaderIterationContext(GLuint name, GLuint count);

		ShaderIterationContext(ShaderIterationContext&& temp)
		 : _shader_names(std::move(temp._shader_names))
		{ }
	};

	typedef aux::ContextElementRange<
			ShaderIterationContext,
			ShaderName
	> ShaderRange;
#endif // !OGLPLUS_DOCUMENTATION_ONLY

#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_4_3
	/// Returns the context for traversal of Program's active resources
	/**
	 *  @see ActiveResources
	 */
	InterfaceContext ActiveResourceContext(ProgramInterface intf) const;

	/// Returns a range allowing to do the traversal of interface's resources
	/** This instance of Program must be kept alive during the whole
	 *  lifetime of the returned range, i.e. the returned range must not
	 *  be used after the Program goes out of scope and is destroyed.
	 *
	 *  @see ProgramInterface
	 *  @see ProgramResource
	 *
	 *  @throws Error
	 */
	ActiveResourceRange ActiveResources(ProgramInterface intf) const;
#endif

	/// Returns the context for traversal of Program's active vertex attributes
	/**
	 *  @see ActiveAttribs
	 */
	InterfaceContext ActiveAttribContext(void) const;

	/// Returns a range allowing to do the traversal of active attributes
	/** This instance of Program must be kept alive during the whole
	 *  lifetime of the returned range, i.e. the returned range must not
	 *  be used after the Program goes out of scope and is destroyed.
	 *
	 *  @throws Error
	 */
	ActiveAttribRange ActiveAttribs(void) const;

	/// Returns the context for traversal of Program's active uniforms
	/**
	 *  @see ActiveUniforms
	 */
	InterfaceContext ActiveUniformContext(void) const;

	/// Returns a range allowing to do the traversal of active uniforms
	/** This instance of Program must be kept alive during the whole
	 *  lifetime of the returned range, i.e. the returned range must not
	 *  be used after the Program goes out of scope and is destroyed.
	 *
	 *  @throws Error
	 */
	ActiveUniformRange ActiveUniforms(void) const;

#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_4_0 || GL_ARB_shader_subroutine

	/// Returns the context for traversal of Program's active subroutines
	/**
	 *  @see ActiveSubroutines
	 */
	InterfaceContext ActiveSubroutineContext(ShaderType stage) const;

	/// Returns a range allowing to do the traversal of subroutines
	/** This instance of Program must be kept alive during the whole
	 *  lifetime of the returned range, i.e. the returned range must not
	 *  be used after the Program goes out of scope and is destroyed.
	 *
	 *  @throws Error
	 *
	 *  @glvoereq{4,0,ARB,shader_subroutine}
	 */
	ActiveSubroutineRange ActiveSubroutines(ShaderType stage) const;

	/// Returns the context for traversal of Program's active subr. uniforms
	/**
	 *  @see ActiveSubroutineUniforms
	 */
	InterfaceContext ActiveSubroutineUniformContext(ShaderType stage) const;

	/// Returns a range allowing to do the traversal of subroutine uniforms
	/** This instance of Program must be kept alive during the whole
	 *  lifetime of the returned range, i.e. the returned range must not
	 *  be used after the Program goes out of scope and is destroyed.
	 *
	 *  @throws Error
	 *
	 *  @glvoereq{4,0,ARB,shader_subroutine}
	 */
	ActiveSubroutineUniformRange ActiveSubroutineUniforms(ShaderType stage) const;
#endif

	/// Returns the context for traversal of Program's active TFB varyings
	/**
	 *  @see TransformFeedbackVaryings
	 */
	InterfaceContext TransformFeedbackVaryingContext(void) const;

	/// Returns a range allowing to do the traversal of feedback varyings
	/** This instance of Program must be kept alive during the whole
	 *  lifetime of the returned range, i.e. the returned range must not
	 *  be used after the Program goes out of scope and is destroyed.
	 *
	 *  @throws Error
	 */
	TransformFeedbackVaryingRange TransformFeedbackVaryings(void) const;

	/// Returns a range allowing to traverse shaders attached to this program
	ShaderRange AttachedShaders(void) const;

#if OGLPLUS_DOCUMENTATION_ONLY
	/// Information about a active uniform block
	/** Do not instantiate this class directly, instances are returned
	 *  by the ActiveUniformBlocks() function.
	 *
	 *  @see ActiveUniformBlocks
	 */
	class ActiveUniformBlockInfo
	{
		/// Returns the index of the attribute or uniform
		GLuint Index(void) const;

		/// Returns the name (identifier) of the named uniform block
		const String& Name(void) const;

		// TODO: active uniform indices, etc.
	};

	/// The type of the range for traversing active uniform blocks
	typedef Range<ActiveUniformBlockInfo> ActiveUniformRange;
#else
	typedef aux::ContextElementRange<
		aux::ProgramInterfaceContext,
		aux::ActiveUniformBlockInfo
	> ActiveUniformBlockRange;
#endif

	/// Returns a range allowing to do the traversal of active attributes
	/** This instance of Program must be kept alive during the whole
	 *  lifetime of the returned range, i.e. the returned range must not
	 *  be used after the Program goes out of scope and is destroyed.
	 *
	 *  @throws Error
	 */
	ActiveUniformBlockRange ActiveUniformBlocks(void) const;

#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_4_1 || GL_ARB_separate_shader_objects
	/// Makes this program separable
	/**
	 *  @glvoereq{4,1,ARB,separate_shader_objects}
	 *  @glsymbols
	 *  @glfunref{ProgramParameter}
	 */
	ObjectOps& MakeSeparable(bool para = true);
#endif // separate shader objects

#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_4_1 || GL_ARB_get_program_binary
	/// Makes this program retrievable in binary form
	/**
	 *  @see GetBinary
	 *
	 *  @glvoereq{4,1,ARB,get_program_binary}
	 *  @glsymbols
	 *  @glfunref{ProgramParameter}
	 */
	ObjectOps& MakeRetrievable(bool para = true);

	/// Returns this programs binary representation
	/**
	 *  @see MakeRetrievable
	 *  @see Binary
	 *
	 *  @glvoereq{4,1,ARB,get_program_binary}
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @glfunref{GetProgramBinary}
	 *  @gldefref{PROGRAM_BINARY_LENGTH}
	 */
	void GetBinary(std::vector<GLubyte>& binary, GLenum& format) const;

	/// Allows to specify to program code in binary form
	/**
	 *  @see MakeRetrievable
	 *  @see GetBinary
	 *
	 *  @glvoereq{4,1,ARB,get_program_binary}
	 *  @glsymbols
	 *  @glfunref{ProgramBinary}
	 */
	void Binary(const std::vector<GLubyte>& binary, GLenum format);
#endif // get program binary

	/// Returns the transform feedback buffer mode
	/**
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{TRANSFORM_FEEDBACK_BUFFER_MODE}
	 */
	TransformFeedbackMode TransformFeedbackBufferMode(void) const
	{
		return TransformFeedbackMode(
			GetIntParam(GL_TRANSFORM_FEEDBACK_BUFFER_MODE)
		);
	}

#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_3_2
	/// Returns the number of vertices that the geometry shader will output
	/**
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{GEOMETRY_VERTICES_OUT}
	 */
	GLint GeometryVerticesOut(void) const
	{
		return GetIntParam(GL_GEOMETRY_VERTICES_OUT);
	}
#endif

#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_4_1 || GL_ARB_gpu_shader5

	/// Returns the number of invocations of geometry shader per primitive
	/**
	 *  @glvoereq{4,1,ARB,gpu_shader5}
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{GEOMETRY_SHADER_INVOCATIONS}
	 */
	GLint GeometryShaderInvocations(void) const
	{
		return GetIntParam(GL_GEOMETRY_SHADER_INVOCATIONS);
	}
#endif // gpu shader 5

#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_3_2

	/// Returns the geometry shader input primitive type
	/**
	 *  @glverreq{3,2}
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{GEOMETRY_INPUT_TYPE}
	 */
	PrimitiveType GeometryInputType(void) const
	{
		return PrimitiveType(GetIntParam(GL_GEOMETRY_INPUT_TYPE));
	}

	/// Returns the geometry shader output primitive type
	/**
	 *  @glverreq{3,2}
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{GEOMETRY_OUTPUT_TYPE}
	 */
	PrimitiveType GeometryOutputType(void) const
	{
		return PrimitiveType(GetIntParam(GL_GEOMETRY_OUTPUT_TYPE));
	}
#endif

#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_4_0 || GL_ARB_tessellation_shader

	/// Returns the vertex order in tesselation evaluation shader
	/**
	 *  @glvoereq{4,0,ARB,tessellation_shader}
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{TESS_GEN_VERTEX_ORDER}
	 */
	FaceOrientation TessGenVertexOrder(void) const
	{
		return FaceOrientation(GetIntParam(GL_TESS_GEN_VERTEX_ORDER));
	}

	/// Returns the tesselation generator output primitive type
	/**
	 *  @glvoereq{4,0,ARB,tessellation_shader}
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{TESS_GEN_MODE}
	 */
	TessGenPrimitiveType TessGenMode(void) const
	{
		return TessGenPrimitiveType(GetIntParam(GL_TESS_GEN_MODE));
	}

	/// Returns the tesselation generator primitive spacing mode
	/**
	 *  @glvoereq{4,0,ARB,tessellation_shader}
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{TESS_GEN_SPACING}
	 */
	TessGenPrimitiveSpacing TessGenSpacing(void) const
	{
		return TessGenPrimitiveSpacing(GetIntParam(GL_TESS_GEN_SPACING));
	}

	/// Returns true if point mode is enabled in tesslation eval. shader
	/**
	 *  @glvoereq{4,0,ARB,tessellation_shader}
	 *  @glsymbols
	 *  @glfunref{GetProgram}
	 *  @gldefref{TESS_GEN_POINT_MODE}
	 */
	bool TessGenPointMode(void) const
	{
		return GetIntParam(GL_TESS_GEN_POINT_MODE) == GL_TRUE;
	}
#endif // tessellation shader

	/// Binds the location of a SL variable to the vertex_attrib
	/** This function binds the location of the vertex attribute
	 *  @c vertex_attrib to the shader variable identified by
	 *  @c identifier.
	 */
	void BindLocation(
		VertexAttribSlot vertex_attrib_slot,
		StrCRef identifier
	)
	{
		OGLPLUS_GLFUNC(BindAttribLocation)(
			_name,
			GLuint(vertex_attrib_slot),
			identifier.c_str()
		);
		OGLPLUS_CHECK(
			BindAttribLocation,
			ProgVarError,
			Program(*this).
			Identifier(identifier).
			Index(GLuint(vertex_attrib_slot))
		);
	}
};

/// Program operations with direct state access
typedef ObjectOps<tag::DirectState, tag::Program>
	ProgramOps;

/// Class that can be used to unbind the currently active program
/**
 *  @ingroup oglplus_objects
 */
typedef ObjectZero<ObjZeroOps<tag::DirectState, tag::Program>>
	NoProgram;

/// An @ref oglplus_object encapsulating the program object functionality
/**
 *  @ingroup oglplus_objects
 */
typedef Object<ProgramOps> Program;

// syntax-sugar operators

inline ProgramOps& operator << (
	ProgramOps& program,
	ShaderName shader
)
{
	program.AttachShader(shader);
	return program;
}

struct ProgAndXFBMode
{
	ProgramOps& prog;
	TransformFeedbackMode mode;

	ProgAndXFBMode(ProgramOps& p, TransformFeedbackMode m)
	 : prog(p)
	 , mode(m)
	{ }
};

inline ProgAndXFBMode operator << (
	ProgramOps& prog,
	TransformFeedbackMode mode
)
{
	return ProgAndXFBMode(prog, mode);
}

template <std::size_t N>
inline ProgramOps& operator << (
	ProgAndXFBMode pam,
	const GLchar* (&varyings)[N]
)
{
	pam.prog.TransformFeedbackVaryings(varyings, pam.mode);
	return pam.prog;
}

struct ProgXFBModeAndNames
{
	ProgramOps& prog;
	TransformFeedbackMode mode;
	std::vector<const GLchar*> names;

	ProgXFBModeAndNames(ProgAndXFBMode pam, const GLchar* name)
	 : prog(pam.prog)
	 , mode(pam.mode)
	{
		names.reserve(8);
		names.push_back(name);
	}

	ProgXFBModeAndNames(ProgXFBModeAndNames&& pman, const GLchar* name)
	 : prog(pman.prog)
	 , mode(pman.mode)
	 , names(std::move(pman.names))
	{
		names.push_back(name);
	}

	ProgXFBModeAndNames(ProgXFBModeAndNames&& tmp)
	 : prog(tmp.prog)
	 , mode(tmp.mode)
	 , names(std::move(tmp.names))
	{ }


#if !OGLPLUS_NO_DELETED_FUNCTIONS
	ProgXFBModeAndNames(const ProgXFBModeAndNames&) = delete;
#else
private:
	ProgXFBModeAndNames(const ProgXFBModeAndNames&);
public:
#endif

	~ProgXFBModeAndNames(void)
	{
		if(!names.empty())
		{
			prog.TransformFeedbackVaryings(
				names.size(),
				names.data(),
				mode
			);
		}
	}
};

inline ProgXFBModeAndNames operator << (
	ProgAndXFBMode pam,
	const GLchar* name
)
{
	return ProgXFBModeAndNames(pam, name);
}

inline ProgXFBModeAndNames operator << (
	ProgXFBModeAndNames&& pman,
	const GLchar* name
)
{
	return ProgXFBModeAndNames(std::move(pman), name);
}


#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_4_1 || GL_ARB_separate_shader_objects
/// A standalone program with a single shader of a specified type from GLSL source
/**
 *  @glsymbols
 *  @glfunref{CreateShaderProgram}
 *
 *  @see Program
 */
class ShaderProgram
 : public Program
{
private:
	static GLuint _make(
		ShaderType shader_type,
		GLsizei count,
		const GLchar** strings
	);

	void _check(void);
public:
	/// Creates a program with a single shader with specified type and source
	/**
	 *  @throws ValidationError
	 */
	ShaderProgram(
		ShaderType shader_type,
		const GLchar* source
	): Program(_make(shader_type, 1, &source))
	{ _check(); }

	/// Creates a program with a single shader with specified type and source
	/**
	 *  @throws ValidationError
	 */
	ShaderProgram(
		ShaderType shader_type,
		const GLchar* source,
		ObjectDesc&& object_desc
	): Program(_make(shader_type, 1, &source), std::move(object_desc))
	{ _check(); }

	/// Creates a program with a single shader with specified type and source
	/**
	 *  @throws ValidationError
	 */
	ShaderProgram(
		ShaderType shader_type,
		const GLSLSource& glsl_source
	): Program(_make(shader_type, glsl_source.Count(), glsl_source.Parts()))
	{ _check(); }

	/// Creates a single shader program with specified type, source and desc.
	/**
	 *  @throws ValidationError
	 */
	ShaderProgram(
		ShaderType shader_type,
		const GLSLSource& glsl_source,
		ObjectDesc&& object_desc
	): Program(
		_make(shader_type, glsl_source.Count(), glsl_source.Parts()),
		std::move(object_desc)
	)
	{ _check(); }
};
#endif

/// A class that allows to build programs in the constructor
/** This class allows to supply a list of shaders and other parameters
 *  to the constructor. The shaders are attached to the Program
 *  and it is linked and made active. Optionally the program can also
 *  be made separable.
 */
class QuickProgram
 : public Program
{
public:
	/// Attaches @p shaders, links and uses the program
	QuickProgram(const Sequence<ShaderName>& shaders)
	{
		AttachShaders(shaders);
		Link();
		Use();
	}

	/// Attaches @p shaders, links, uses and describes the program
	QuickProgram(
		ObjectDesc&& object_desc,
		const Sequence<ShaderName>& shaders
	): Program(std::move(object_desc))
	{
		AttachShaders(shaders);
		Link();
		Use();
	}

#if OGLPLUS_DOCUMENTATION_ONLY || GL_VERSION_4_1 || GL_ARB_separate_shader_objects
	/// Attaches @p shaders, makes separable, links and uses the program
	/**
	 *  @glvoereq{4,1,ARB,separate_shader_objects}
	 */
	QuickProgram(bool separable, const Sequence<ShaderName>& shaders)
	{
		AttachShaders(shaders);
		if(separable) MakeSeparable();
		Link();
		Use();
	}

	/// Attaches @p shaders, makes separable, links and uses the program
	/**
	 *  @glvoereq{4,1,ARB,separate_shader_objects}
	 */
	QuickProgram(
		ObjectDesc&& object_desc,
		bool separable,
		const Sequence<ShaderName>& shaders
	): Program(std::move(object_desc))
	{
		AttachShaders(shaders);
		if(separable) MakeSeparable();
		Link();
		Use();
	}
#endif
};

} // namespace oglplus

#if !OGLPLUS_LINK_LIBRARY || defined(OGLPLUS_IMPLEMENTING_LIBRARY)
#include <oglplus/program.ipp>
#endif // OGLPLUS_LINK_LIBRARY

#endif // include guard
