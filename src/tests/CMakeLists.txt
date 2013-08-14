include_directories (
	${CMAKE_SOURCE_DIR}/src/util
	${OPENGL_INCLUDE_PATH}
	${GLUT_INCLUDE_DIR}
	${GLEW_INCLUDE_DIR}
)

link_directories (
	${CMAKE_SOURCE_DIR}/src/util
)

link_libraries (
	util
)

set (subdir tests)

set (targets
	afsmultiarb
	antialias
	# XXX: Requires Linux kernel headers???
	#api_speed 
	arbfpspec
	arbfptest1
	arbfptexture
	arbfptrig
	arbgpuprog
	arbnpot
	arbnpot-mipmap
	arbvparray
	arbvptest1
	arbvptest3
	arbvptorus
	arbvpwarpmesh
	arraytexture
	backspecular
	blendxor
	blitfb
	bufferobj
	bug_3050
	bug_3101
	bug_3195
	bug_texstore_i8
	bumpmap
	calibrate_rast
	clip
	condrender
	copypixrate
	cva
	cva_huge
	cylwrap
	# XXX: Depends on Mesa GL header
	#debugger
	drawbuffers
	drawbuffers2
	drawstencil
	exactrast
	ext422square
	fbotest1
	fbotest2
	fbotest3
	fillrate
	floattex
	fogcoord
	fptest1
	fptexture
	# XXX: Requires Mesa source
	#getprocaddress
	getteximage
	glutfx
	gremedy
	interleave
	invert
	lineclip
	linehacks
	manytex
	mapbufrange
	minmag
	mipgen
	mipmap_comp
	mipmap_comp_tests
	mipmap_limits
	mipmap_tunnel
	mipmap_view
	multipal
	multitexarray
	multiwindow
	no_s3tc
	packedpixels
	pbo
	persp_hint
	prim
	prog_parameter
	quads
	random
	readrate
	rubberband
	scissor
	scissor-viewport
	seccolor
	shader_api
	shader-interp
	shadow-sample
	stencilreaddraw
	stencilwrap
	step
	streaming_rect
	subtex
	subtexrate
	tex1d
	texcmp
	texcompress2
	texcompsub
	texdown
	texfilt
	texgenmix
	texline
	texobj
	texrect
	unfilledclip
	viewmemory
	vparray
	vpeval
	vptest1
	vptest2
	vptest3
	vptorus
	vpwarpmesh
	yuvrect
	yuvsquare
	zbitmap
	zcomp
	zdrawpix
	zreaddraw
)

if (UNIX)
	set (targets ${targets} texleak)
endif (UNIX)


foreach (target ${targets})
	add_executable (${subdir}_${target} ${target}.c)
	set_target_properties (${subdir}_${target} PROPERTIES OUTPUT_NAME ${target})
	target_link_libraries (${subdir}_${target}
		${OPENGL_gl_LIBRARY}
		${OPENGL_glu_LIBRARY}
		${GLUT_glut_LIBRARY}
		${GLEW_glew_LIBRARY}
	)
	install (TARGETS ${subdir}_${target} DESTINATION ${subdir})
endforeach (target)

if (X11_FOUND)
	include_directories (
		${X11_INCLUDE_DIR}
	)

	set (targets
		auxbuffer
		jkrahntest
		sharedtex
		texobjshare
	)

	foreach (target ${targets})
		add_executable (${subdir}_${target} ${target}.c)
		set_target_properties (${subdir}_${target} PROPERTIES OUTPUT_NAME ${target})
		target_link_libraries (${subdir}_${target}
			${GLEW_glew_LIBRARY}
			${X11_GLU_LIB}
			${X11_GL_LIB}
			${X11_Xext_LIB}
			${X11_X11_LIB}
		)
		install (TARGETS ${subdir}_${target} DESTINATION ${subdir})
	endforeach (target)
endif (X11_FOUND)
