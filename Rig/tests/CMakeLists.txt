ADD_EXECUTABLE(test_rig_counter test_rig_counter.c)
TARGET_LINK_LIBRARIES(test_rig_counter rig check)
ADD_TEST(rig_counter test_rig_counter)

ADD_EXECUTABLE(test_rig_hash test_rig_hash.c)
TARGET_LINK_LIBRARIES(test_rig_hash rig check)
ADD_TEST(rig_hash test_rig_hash)

ADD_EXECUTABLE(test_rig_list test_rig_list.c)
TARGET_LINK_LIBRARIES(test_rig_list rig check)
ADD_TEST(rig_list test_rig_list)