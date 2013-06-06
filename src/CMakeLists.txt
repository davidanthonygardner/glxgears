add_subdirectory (util)

add_subdirectory (demos)
add_subdirectory (tests)
add_subdirectory (trivial)
add_subdirectory (redbook)
add_subdirectory (samples)
add_subdirectory (perf)
add_subdirectory (objviewer)

add_subdirectory (glsl)
add_subdirectory (fp)
add_subdirectory (fpglsl)
add_subdirectory (vp)
add_subdirectory (vpglsl)
add_subdirectory (gs)
add_subdirectory (slang)
add_subdirectory (tools)

if (EGL_FOUND)
	add_subdirectory(egl)
endif ()

if (X11_FOUND)
	add_subdirectory (xdemos)
endif (X11_FOUND)

if (WIN32)
	add_subdirectory (wgl)
endif (WIN32)

add_subdirectory (data)
