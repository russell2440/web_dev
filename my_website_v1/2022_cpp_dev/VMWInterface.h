#pragma once

//==============================================================================
//==============================================================================

/** 
 * @file:   VMWInterface.h
 * @author: 
 * @since:  2/13/15
 * @brief   This file contains the declaration of the VMWInterface class. 
 *          This an interface to read/write to the VMW
 */

#include "DropPacketPolicy.h"
#include "MsgTypes.h"
#include "VMWMessageTypes.h"
#include "VMWUplinkMessage.h"

#include <boost/asio.hpp>
#include <memory>


namespace mia
{
  class ItmDelay;
  class DataRouter;

  /**
   * An instance of the VMWInterface class is owned by a DataRouter object
   * to manage the need to send and receive packets over a UDP socket.
   *
   * The VMWInterface object receives packets from a single listening endpoint,
   * supplied at construction-time, and sends packets to one of two configured
   * destination endpoints.  The socket I/O is performed asynchronously using
   * a boost::asio::io_service object (owned by the DataRouter) that is supplied
   * to the VMWInterface object at the time of its construction.
   *
   * Packets received from the UDP socket are processed and delivered to the
   * DataRouter to be routed to other interface objects owned and managed by
   * the DataRouter.
   *
   * The DataRouter can ask the VMWInterface object to send MPLS Packets
   * or to send a "Pass-Through" packet. 
   */
  class VMWInterface
  {
  public:

    /**
     * Creates a new VMWInterface
     *
     * @param router is a reference to the DataRouter object to which
     *        inbound message data will be delivered.
     *
     * @param io_service is a reference to the io_service object (owned
     *        by the DataRouter) that is used for asynchronous I/O (on 
     *        a UDP socket).
     *
     * @param endpoint is a reference to the endpoint object that the 
     *        UDP socket uses to listen to for incoming data.
     *
     * @param itm_delay is a const reference to the ItmDelay object (owned
     *        by the DataRouter) that is used to obtain the configured
     *        Itm Delay Time to be applied for routes.
     *
     * @param to_sv_drop_policies is a reference to a ToSvPolicies_t object
     *        that this class uses to determine if a packet needs to be 
     *        dropped during processing.
     * 
     */
    VMWInterface(
      DataRouter&                           router,
      boost::asio::io_service&              io_service,
      const boost::asio::ip::udp::endpoint& endpoint,
      const ItmDelay&                       itm_delay, 
      drop_pkt::ToSvPolicies_t&             to_sv_drop_policies);
    
    /**
     * Destructor.
     */
    ~VMWInterface();

    /**
     * The method that starts performing async reads on the socket owned
     * by this interface.
     */
    void Start();
  
    /**
     * The distribution point for the start of K-Frame signal.
     */
    void OnSOKF();

    /**
     * This method is a request to enqueue the itm packet such that it will
     * be sent to the SV (via the VMWInterface object's socket).
     *
     * @param itm_buf is the ITM (or vITM) packet to be sent to the SV.
     *
     * @param source_node is the source node of the packet.
     *
     * @param dest_node is the destination node of the packet.
     */
    void SendUplinkITM(
      const boost::asio::const_buffer& itm_buf,
      uint8_t source_node,
      uint8_t dest_node);

    /**
     * This method is a request to enqueue the mpls packet such that it will
     * be sent to the SV (via the VMWInterface object's socket).
     *
     * @param mpls_packet_buf is the MPLS Packet (VITM/MPLS) to be sent to
     *        the SV.
     *
     * @param source_node is the source node of the packet.
     *
     * @param dest_node is the destination node of the packet.
     *
     * @note This should only be called for VITM/MPLS data.
     */
    void SendUplinkMPLSPacket(
      const boost::asio::const_buffer& mpls_packet_buf,
      uint8_t source_node,
      uint8_t dest_node);

    /**
     * This method is a request to enqueue the supplied pass-through data
     * such that it will be sent to the SV (via the VMWInterface object's
     * socket).
     *
     * @param passthru_msg_buf  The pass-through data to send.
     */
    void SendUplinkPassThroughMessage(
      const boost::asio::const_buffer& passthru_msg_buf);
  

  private:

    //==================
    // Private Methods
    //==================

    /**
     * Make an asynchronous request to read a packet from the socket.
     */
    void DoRead();

    /**
     * Called when a message has been received from the socket, or when
     * a socket error has occured while attempting (asynchronously) to
     * receive a packet from the socket.
     *
     * @param ec is the socket error code (which could be "none").
     *
     * @param bytes_transfered is the number of bytes received.
     */
    void OnReadComplete(
      const boost::system::error_code& ec,
      size_t bytes_transferred);

    /**
     * This method is responsible for handling the data received over the
     * UDP socket from the VMW-A on either the CP or DP. 
     *
     * The received data is processed as a VMW Message containing a "batch"
     * of individual "MPLS packets".  Each MPLS Packet consists of an MPLS
     * Header followed by an ITM (which could be a vITM).
     * 
     * This method will attempt to process the VMW Message as a "batch" of
     * individual downlink MPLS packets and use the DataRouter to route the
     * downlink K-Band data appropriately.
     * 
     * @param read_buf refers to the bytes that have been read from the
     *        UDP socket and need to be handled as a downlink VMW Message.
     */
    void HandleReadData(const boost::asio::const_buffer& read_buf);

    /**
     * This method is responsible for the building of VMW messages from ITM
     * packets currently in the k-frame write delay map that become ready.  And
     * for the queueing to send those messages to either the Control or  Data
     * Plane.
     * 
     * It uses a helper class named VMWMessageBuilder to do the building and
     * queueing of the messages.
     */
    void HandleWriteDelayed();

    /**
     * Make an asynchronous request to write the packet that is at the
     * front of the "write queue" to the socket.
     *
     * @note This will perform a NO-OP if the write queue is currently empty.
     */
    void DoWrite();

    /**
     * Called when a packet has been written to the socket, or when
     * a socket error has occured while attempting (asynchronously) to
     * write a packet to the socket.
     *
     * @param ec is the socket error code (which could be "none").
     *
     * @param bytes_transfered is the number of bytes sent.
     */
    void OnWriteComplete(
      const boost::system::error_code& ec,
      size_t bytes_transferred);

    /**
     * Helper method to cancel and close the socket if it is currently open,
     * and to suppress any errors that may arise while attempting to cancel
     * and close the socket.
     */
    void CancelAndCloseSocket();


    //=======================
    // Private Data Members
    //=======================

    //-------------------------------------------------------------------------
    /// Reference to the dataRouter which was supplied to the constuctor.
    //-------------------------------------------------------------------------
    DataRouter& data_router_;

    //-------------------------------------------------------------------------
    /// Const reference to the ItmDelay object (owned by dataRouter) which
    /// was supplied to the constuctor.
    //-------------------------------------------------------------------------
    const ItmDelay& itm_delay_;

    //-------------------
    /// The UDP socket.
    //-------------------
    boost::asio::ip::udp::socket socket_;

    //------------------------------------------------------------------------
    /// The non-const reference to the TOSV drop packet policies object,
    /// supplied to the constructor.
    //------------------------------------------------------------------------
    drop_pkt::ToSvPolicies_t& to_sv_drop_policies_;

    //------------------------------------------------------------------------
    /// Relative K-Frame count.
    ///
    /// It has no relation to the actual K frame count on the SV and is used
    /// to assist in delaying packets (i.e. some number of K-Frames).
    ///
    /// The count starts at 0 and is incremented each time a SOKF is received.
    //------------------------------------------------------------------------
    uint32_t k_frame_count_;

    //------------------------------------------------------------------
    /// Blob of bytes to hold the inbound packet read from the socket.
    //------------------------------------------------------------------
    std::vector<uint8_t> read_data_;

    //--------------------------------------------------------------------------
    /// Each VMWUplinkMesssage in the write_queue_ represents one VMW Message,
    /// stored as a "blob" of bytes, that is destined for VMW-A running
    /// on either the CP or the DP.
    //--------------------------------------------------------------------------
    using WriteQueue_t = std::deque<VMWUplinkMessage>;

    WriteQueue_t write_queue_;

    //-------------------------------------------------------------------------
    /// The write_delayed_ contains ITM/MPLS (or VITM/MPLS) packets that have
    /// been requested to be sent, but are currently being "delayed" for some
    /// number of K-Frames.  When we receive notification of the start of a
    /// new K-Frame (a.k.a., SOKF), any delayed ITM messages that have become
    /// due to be written shall be pushed to the end of the write_queue_.
    //-------------------------------------------------------------------------
    using WriteDelayedMap_t = std::multimap<uint32_t, VMWUplinkMPLSPacket>;

    WriteDelayedMap_t write_delayed_;
  };
}
