SET(rigSources
	rig_counter.c
	rig_hash.c
	rig_list.c
	rig_mem.c
	rig_misc.c
	rig_queue.c
	rig_smr_epoch.c
	rig_smr_hp.c
	rig_stack.c
	rig_threads.c)

ADD_LIBRARY(rig SHARED ${rigSources})

SET_TARGET_PROPERTIES(rig
	PROPERTIES
	SOVERSION ${PROJECT_VERSION_MAJOR}
	VERSION ${PROJECT_VERSION_NOREV}
)

TARGET_LINK_LIBRARIES(rig ${CMAKE_THREAD_LIBS_INIT})

INSTALL(TARGETS rig DESTINATION lib${LIB_SUFFIX})
