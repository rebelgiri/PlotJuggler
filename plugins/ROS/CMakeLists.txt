# Qt related stuff
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)

QT5_WRAP_UI ( COMMON_UI_SRC
    qnodedialog.ui
    rule_editing.ui
    dialog_select_ros_topics.ui
    dialog_with_itemlist.ui
    )

SET( COMMON_SRC
    dialog_select_ros_topics.h
    dialog_select_ros_topics.cpp
    dialog_with_itemlist.h
   )

set(PLUGIN_BASE_DIR ${PROJECT_SOURCE_DIR}/include/PlotJuggler)

#############

if(COMPILING_WITH_CATKIN)

    include_directories(
        ./ ../
        ${PROJECT_SOURCE_DIR}/include/
        ros1_introspection/include
        ${catkin_INCLUDE_DIRS}  )

    add_subdirectory(ros1_introspection)

    list( APPEND COMMON_SRC
        qnodedialog.cpp
        rule_editing.cpp
        ros1_parsers/ros_parser.cpp
        )

    add_library( commonROS STATIC ${COMMON_SRC} ${COMMON_UI_SRC} )
    target_link_libraries( commonROS PRIVATE
        ${Qt5Widgets_LIBRARIES}
        ${Qt5Xml_LIBRARIES}
        ${catkin_LIBRARIES}
        ros_type_introspection_static
        fmt::fmt-header-only
        marl )

    #########
    add_library( DataLoadROS SHARED
        DataLoadROS/dataload_ros.cpp
        ${PLUGIN_BASE_DIR}/dataloader_base.h )
    target_link_libraries( DataLoadROS  commonROS)
    #############
    add_library( DataStreamROS SHARED
        DataStreamROS/datastream_ROS.cpp
        ${PLUGIN_BASE_DIR}/datastreamer_base.h )
    target_link_libraries( DataStreamROS  commonROS )
    #############
    add_library( RosTopicPublisher SHARED
        TopicPublisherROS/statepublisher_rostopic.cpp
        ${PLUGIN_BASE_DIR}/statepublisher_base.h)
    target_link_libraries( RosTopicPublisher commonROS )
    #############
    QT5_WRAP_UI ( ROSOUT_UI RosoutPublisher/logwidget.ui )
    add_library( RosoutPublisher SHARED ${ROSOUT_UI}
        RosoutPublisher/rosout_publisher.cpp
        RosoutPublisher/logs_table_model.cpp
        RosoutPublisher/logwidget.cpp
        RosoutPublisher/modelfilter.cpp
        ${PLUGIN_BASE_DIR}/statepublisher_base.h
        )
    target_link_libraries( RosoutPublisher commonROS )
#######################################################################
elseif(COMPILING_WITH_AMENT)

    find_package(ament_cmake REQUIRED)
    find_package(rclcpp REQUIRED)
    find_package(rcpputils REQUIRED)
    find_package(rosbag2 REQUIRED)
    find_package(rosbag2_transport REQUIRED)
    find_package(fastcdr REQUIRED)
    find_package(Boost REQUIRED)

    include_directories(
        ./
        ${Boost_INCLUDE_DIRS}
        ros2_introspection/include
        ros2_parsers)

    list( APPEND COMMON_SRC
        ros2_introspection/src/ros2_introspection.cpp
        ros2_introspection/src/stringtree.cpp
        ros2_parsers/ros2_parser.cpp )

    set(AMENT_DEPENDECIES
        rclcpp
        rosbag2
        rcpputils
        fastcdr
        rosbag2_transport )

    add_library( commonROS STATIC ${COMMON_SRC} ${COMMON_UI_SRC})
    target_compile_definitions(commonROS PUBLIC DISABLE_RULE_EDITING)
    target_link_libraries( commonROS
        ${Qt5Widgets_LIBRARIES}
        ${Qt5Xml_LIBRARIES}
        fastcdr)
    ament_target_dependencies( commonROS ${AMENT_DEPENDECIES})

    #############
    add_library( DataLoadROS2 SHARED
        DataLoadROS2/dataload_ros2.cpp
        ${PLUGIN_BASE_DIR}/dataloader_base.h )
    target_link_libraries( DataLoadROS2 commonROS)
    ament_target_dependencies( DataLoadROS2 ${AMENT_DEPENDECIES})
    #############
    add_library( DataStreamROS2 SHARED
        DataStreamROS2/datastream_ros2.cpp
        ${PLUGIN_BASE_DIR}/datastreamer_base.h )
    target_link_libraries( DataStreamROS2  commonROS fastcdr)
    ament_target_dependencies( DataStreamROS2 ${AMENT_DEPENDECIES})
endif()

#######################################################################


if(COMPILING_WITH_CATKIN)
    install(TARGETS
        DataLoadROS
        DataStreamROS
        RosTopicPublisher
        RosoutPublisher
        DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
        )
elseif(COMPILING_WITH_AMENT)
    install(TARGETS
        DataLoadROS2
        DataStreamROS2
        DESTINATION bin  )
endif()
