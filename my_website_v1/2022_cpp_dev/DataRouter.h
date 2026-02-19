#pragma once

//===========================================================================
//
//===========================================================================

#include "VMWInterface.h"
#include "TPNInterface.h"
#include "MPLSInterface.h"
#include "MDInterface.h"
#include "SOKFInterface.h"

#include "ItmDelay.h"
#include "DropPacketPolicy.h"

#include <SimulationCore/ChangeConfigCommand.hh>
#include <inf/util/Task.h>

#include <boost/asio.hpp>


/** 
 *  This file defines the class that is responsible for routing data between 
 *  the different interfaces
 *
 *  @author: 
 *  @since: 2/13/15
 */

namespace mia 
{
  /**
   * An instance of the DataRouter class plays a central role in the MIA 
   * application. It manages instances of the various Interface classes and 
   * decides how to handle the inbound messages that are received from the 
   * sockets managed by the various Interface objects.  When the DataRouter 
   * receives inbound messages it will decide what to do with the message, 
   * which may involve asking one or more Interface objects to "send" a 
   * message over its socket.  
   * 
   * This object follows an active object design pattern:
   * e.g.:  https://www.state-machine.com/active-object
   */
  class DataRouter
  {
  public:

    /**
     * Constructor
     *
     * @throw std::exception for any problem encountered.
     *
     * @note This will spawn the dedicated thread for the DataRouter and
     *       instruct it to run() the io_service of the DataRouter.
     */
    DataRouter();

    /**
     * Destructor
     *
     * @note This will ensure that the dedicated thread has finished.
     */
    ~DataRouter();

    /**
     * This is called to start the DataRouter object which will, in turn, ask 
     * each of the interface objects to start reading (asynchronously) from 
     * their socket.
     *
     * @note This method is called by a thread OTHER than the thread that is
     *       running the DataRouter io_service.
     */
    void Start();

    /**
     * This is called to request that the DataRouter change the dynamic 
     * configuration items to their default values.
     *
     * @note This method is called by a thread OTHER than the thread that is
     *       running the DataRouter io_service.
     */
    void ChangeConfigToDefault();

    /**
     * This is called to request that the DataRouter change the dynamic 
     * configuration items as described by the supplied 
     * ChangeConfig::CommandData object.
     *
     * @param cmd_data is a const reference to the CommandData object for the
     *        ChangeConfig command, which contains the new dynamic CI values.
     *
     * @note This method is called by a thread OTHER than the thread that is
     *       running the DataRouter io_service.
     */
    void ChangeConfig(
      const Simulation::Core::ChangeConfig::CommandData& cmd_data);

    /**
     * This method is called by the MPLSInterface when it receives an uplink
     * "pass-through" VMW Message from the KBA.
     *
     * @param passthru_msg_buf is a reference to the bytes that represent the
     *        uplink pass-through message.
     */
    void RouteUplinkPassThroughMessage(
      const boost::asio::const_buffer& passthru_msg_buf);

    /**
     * This method is called by the TPNInterface to route an uplink ITM
     * received over ITME, or by the MDInterface to route an uplink ITM or
     * VITM received from the GW-Sim.
     *
     * @param itm_buf is a reference to the bytes representing the uplink
     *        ITM or VITM.
     *
     * @param source_node is the id for the node from which the ITM (or VITM)
     *        originated.
     *
     * @param dest_node is the id for the node to which the ITM (or VITM)
     *        should be sent.
     */
    void RouteUplinkITM(
      const boost::asio::const_buffer& itm_buf, 
      uint8_t source_node, 
      uint8_t dest_node);

    /**
     * This method is called by the TPNInterface to route an uplink
     * "MPLS Packet" (VITM/MPLS) received over VITM/MPLS/IP.
     *
     * @param mpls_packet_buf is a reference to the bytes representing the
     *        uplink MPLS Packet data (VITM/MPLS).
     *
     * @param source_node is the id for the node from which the MPLS Packet
     *        data originated.
     *
     * @param dest_node is the id for the node to which the MPLS Packet data
     *        should be sent.
     *
     * @note Uplink MPLS packets routed via this method (by the TPNInterface)
     *       should always be VITM/MPLS (i.e., not a fixed-size ITM).
     */
    void RouteUplinkMPLSPacket(
      const boost::asio::const_buffer& mpls_packet_buf,
      uint8_t source_node,
      uint8_t dest_node);

    /**
     * This method is called by the VMWInterface to route a downlink
     * "Pass Through" VMW Message to the MPLSInterface.
     * 
     * @param passthru_msg_buf is a reference to the bytes that represent the
     *        downlink pass-through message.
     */
    void RouteDownlinkPassThroughMessage(
      const boost::asio::const_buffer& passthru_msg_buf);

    /**
     * This method is called by the VMWInterface to route a downlink MPLS
     * Packet (ITM/MPLS or VITM/MPLS) to either the TPNInterface or the
     * MDInterface.
     *
     * @param mpls_packet_buf is a reference to the bytes that represent the
     *        downlink MPLS Packet data (ITM/MPLS or VITM/MPLS) that is to be
     *        routed.
     */
    void RouteDownlinkMPLSPacket(
      const boost::asio::const_buffer& mpls_packet_buf);


  private:

    //==================
    // Private Methods
    //==================

    /**
     * Handle a request to set all dynamic config items to their default config
     * values.
     *
     * @note  The public method is the one making an asynchronous request, 
     *        and this is the private operation that is actually "carrying out"
     *        the request (i.e., using the dedicated thread that is running 
     *        the io_service object that is "owned" by the DataRouter).
     */
    void ChangeConfigToDefault_i();

    /**
     * Handle a request to set all dynamic config items to the 
     * values that have been supplied.
     *
     * @param cmd_data is a const reference to the CommandData object for the
     *        ChangeConfig command, which contains the new dynamic CI values.
     *
     * @note  The public method is the one making an asynchronous request, 
     *        and this is the private operation that is actually "carrying out"
     *        the request (i.e., using the dedicated thread that is running 
     *        the io_service object that is "owned" by the DataRouter).
     */
    void ChangeConfig_i(
      const Simulation::Core::ChangeConfig::CommandData& cmd_data);

    /**
     * This private method is called to actually perform the request to start 
     * the DataRouter.
     *
     * @note  The public method is the one making an asynchronous request, 
     *        and this is the private operation that is actually "carrying out"
     *        the request (i.e., using the dedicated thread that is running 
     *        the io_service object that is "owned" by the DataRouter).
     */
    void Start_i();

    //=======================
    // Private Data Members
    //=======================

    //-------------------------------------------------------------------
    // The io_service object which provides the event loop machinery to
    // support asynchronous request handling.
    //-------------------------------------------------------------------
    boost::asio::io_service io_service_;
  
    //--------------------------------------------------------------------
    // The Task object represents the dedicated thread that will started
    // upon construction to "run" the io_service event loop.  The thread
    // will be stopped upon destruction of the Task object.
    //--------------------------------------------------------------------
    std::unique_ptr<inf::util::Task> task_;
  
    //----------------------------------------------------------------------
    // The object that provides the means to obtain the Itm Delay value to
    // be used for a specific route.  A reference to this object will be
    // supplied to each of the subordinate Interface objects that have a
    // need to get Itm Delay values.
    //----------------------------------------------------------------------
    ItmDelay itm_delay_;
  
    //--------------------------------------------------------------------------
    // The TOSIM (Ground-Bound) drop packet policies, used by this DataRouter
    // object to decide if/how/when to drop packets destined for the (GW-)Sim.
    //--------------------------------------------------------------------------
    drop_pkt::ToSimPolicies_t tosim_drop_policies_;
  
    //---------------------------------------------------------------------
    // The TOSV (SV-Bound) drop packet policies, used by the VMWInterface
    // object to decide if/how/when to drop packets destined for the SV.
    //---------------------------------------------------------------------
    drop_pkt::ToSvPolicies_t tosv_drop_policies_;
  
    //------------------------
    // The interface objects
    //------------------------
    TPNInterface  tpn_interface_;
    VMWInterface  vmw_interface_;
    MPLSInterface kba_mpls_interface_;
    MDInterface   md_interface_;
    SOKFInterface sokf_interface_;
  };
}
