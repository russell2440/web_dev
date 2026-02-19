#===============================================================================
#
#===============================================================================

set(MIA_SRC_FILES
    Config.cc
    ConfigReader.cc
    DataRouter.cc  
    MPLSInterface.cc
    TPNInterface.cc  
    TPNDownlinkPacket.cc
    VMWInterface.cc 
    VMWMessageParser.cc 
    VMWMessageBuilder.cc 
    MDInterface.cc
    SOKFInterface.cc
    Scheduler.cc
    SolfReceiver.cc
    ZmqReceiver.cc
    Commander.cc
    main.cc  )

link_directories(${VMWT_LIB}
                 ${TEAM_TOP}/sim-core)

add_executable(MIA ${MIA_SRC_FILES})

set_target_properties(MIA  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)

target_include_directories(MIA  PUBLIC ${VMWT_INCLUDE}
                                       ${HIREDIS_INCLUDE}
                                       ${TEAM_TOP}/sim-core)

target_link_libraries(MIA vmw_t boost_regex boost_date_time boost_thread infstring inflog infstats SimulationCore zmq czmq asio-zmq pugixml)

install(TARGETS MIA DESTINATION ${TSW_BIN_INSTALL_DIR})

