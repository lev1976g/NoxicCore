macro(GetCompilerVersion out_version)
	#Test for gnu compilers
	IF(CMAKE_COMPILER_IS_GNUCXX )
		EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE tmp_version )
		SET( ${out_version} ${tmp_version} )
	ELSEIF( CMAKE_COMPILER_IS_GNUC)
		EXECUTE_PROCESS( COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE tmp_version)
		SET( ${out_version} ${tmp_version} )
	ELSEIF( MSVC )
		SET( ${out_version} ${MSVC_VERSION} )
	ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE tmp_version)
		SET(${out_version} ${tmp_version})
	ELSE()
		message(FATAL_ERROR "This function does not support the current compiler!")
	ENDIF()
endmacro(GetCompilerVersion)