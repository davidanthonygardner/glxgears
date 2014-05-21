include_directories (
	${CMAKE_SOURCE_DIR}/src/util
	${X11_INCLUDE_DIR}
	${GLEW_INCLUDE_DIR}
)

link_directories (
	${CMAKE_SOURCE_DIR}/src/util
)

link_libraries (
	util
	${X11_GL_LIB}
	${X11_GLU_LIB}
	${GLEW_glew_LIBRARY}
	${X11_X11_LIB}
	${X11_Xext_LIB}
)

add_library (pbutil pbutil.c)

if (BUILD_SHARED_LIBS)
	install (TARGETS pbutil DESTINATION ${LIBDIR})
endif (BUILD_SHARED_LIBS)

set (subdir xdemos)

set (targets
	glsync
	glthreads
	glxcontexts
	glxdemo
	glxgears
	glxgears_fbconfig
	glxgears_pixmap
	glxheads
	glxpbdemo
	glxpixmap
	glxsnoop
	glxswapcontrol
	manywin
	msctest
	multictx
	offset
	omlsync
	opencloseopen
	overlay
	pbdemo
	pbinfo
	shape
	sharedtex
	sharedtex_mt
	texture_from_pixmap
	wincopy
	# XXX: Requires xmesa.h
	#xdemo
	xfont
	yuvrect_client
)

foreach (target ${targets})
	add_executable (${subdir}_${target} ${target}.c)
	set_target_properties (${subdir}_${target} PROPERTIES OUTPUT_NAME ${target})
	install (TARGETS ${subdir}_${target} DESTINATION ${subdir})
endforeach (target)

target_link_libraries (${subdir}_glthreads pthread)
target_link_libraries (${subdir}_glxgears_fbconfig pbutil)
target_link_libraries (${subdir}_pbdemo pbutil)
target_link_libraries (${subdir}_pbinfo pbutil)
target_link_libraries (${subdir}_sharedtex_mt pthread)

add_executable (corender corender.c ipc.c) 
add_executable (xrotfontdemo xrotfontdemo.c xuserotfont.c)
add_executable (glxinfo glxinfo.c glinfo_common.c)

install (TARGETS glxinfo corender xrotfontdemo DESTINATION demos)
