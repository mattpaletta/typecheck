macro(generateProtos flist outtarget)
	if (${TYPECHECK_ENABLE_BLOATY})
		find_program(protocPath protoc)
	else()
		set(protocPath $<TARGET_FILE:protobuf::protoc>)
	endif()
	set(${outtarget}_DEPENDS "") # create empty list
	set(${outtarget}_out_files "")
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${outtarget})

	foreach(f IN LISTS ${flist})
		get_filename_component(fdir ${f} DIRECTORY)
		get_filename_component(fname ${f} NAME)
		get_filename_component(fname_no_ext ${f} NAME_WLE)

		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${outtarget}/${fname_no_ext}.pb.h ${CMAKE_CURRENT_BINARY_DIR}/${outtarget}/${fname_no_ext}.pb.cc
			COMMAND ${protocPath} --proto_path=${fdir} --cpp_out=${CMAKE_CURRENT_BINARY_DIR}/${outtarget} ${fname}
			DEPENDS protobuf::protoc ${fdir}/${fname}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${outtarget}
			COMMENT "${fname} - Generating Protobuf")
		list(APPEND ${outtarget}_out_files ${CMAKE_CURRENT_BINARY_DIR}/${outtarget}/${fname_no_ext}.pb.h)
		list(APPEND ${outtarget}_out_files ${CMAKE_CURRENT_BINARY_DIR}/${outtarget}/${fname_no_ext}.pb.cc)

		list(APPEND ${outtarget}_DEPENDS "${fname}_proto_gen")
	endforeach()
	add_library(${outtarget} STATIC ${${outtarget}_out_files})
	if (TARGET protobuf::libprotobuf)
		target_link_libraries(${outtarget} PUBLIC protobuf::libprotobuf)
	else()
		target_link_libraries(${outtarget} PUBLIC libprotobuf)
	endif()
	target_include_directories(${outtarget} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
	# target_include_directories(${outtarget} INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
	foreach(d IN LISTS ${${outtarget}_DEPENDS})
		add_dependencies(${outtarget} ${d})
	endforeach()
endmacro()
