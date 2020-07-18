macro(vcpkg_find_cpp_redis)
	if(NOT cpp_redis_included)
		find_path(TACOPIE_INCLUDE_DIR tacopie/tacopie)
		find_library(TACOPIE_LIBRARY tacopie)

		add_library(tacopie::tacopie INTERFACE IMPORTED)
		set_target_properties(tacopie::tacopie PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${TACOPIE_INCLUDE_DIR}"
			INTERFACE_LINK_LIBRARIES "${TACOPIE_LIBRARY}"
		)

		find_path(REDIS_INCLUDE_DIR cpp_redis/cpp_redis)
		find_library(REDIS_LIBRARY cpp_redis)

		add_library(cpp_redis::cpp_redis INTERFACE IMPORTED)
		set_target_properties(cpp_redis::cpp_redis PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${REDIS_INCLUDE_DIR}"
			INTERFACE_LINK_LIBRARIES "${REDIS_LIBRARY}"
		)

		target_link_libraries(cpp_redis::cpp_redis INTERFACE tacopie::tacopie)

		set(cpp_redis_included TRUE)
	endif()
endmacro()