#pragma once

//===========================================================================
//===========================================================================

/** 
 * An interface to read/write mission data directly to the gwsim
 * 
 * @author: 
 * @since: 8/11/15
 */

#include "MsgTypes.h"
#include <boost/asio.hpp>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <vector>


namespace mia
{

  //-------------------------
  // Forward Declaration(s)
  //-------------------------
  class ItmDelay;
  class DataRouter;


  /**
   * The MDInterface class is created and owned by the Data Router.
   * it is responsible for the sending and receiving of mission data
   * packets as well as managing any required K frame delays.
   */
  class MDInterface
  {
  public:

    /**
     * Creates a new MDInterface
     *
     * @param data_router is the DataRouter to forward received data.
     *
     * @param io_service Router's io_service for async operations
     *
     * @param itm_delay Const reference to the ItmDelay object (owned by the
     *                  DataRouter) that is used to obtain the Itm Delay Time
     *                  to be used for routes.
     *
     * @throw std::exception for any problem encountered (e.g. socket open
     *        failure).
     */
    MDInterface(
      DataRouter&              data_router,
      boost::asio::io_service& io_service,
      const ItmDelay&          itm_delay);
    
    /**
     * Destructor.
     */
    ~MDInterface();
   
    /**
     * Start performing async reads on the interface's socket.
     */
    void Start();

    /**
     * Make a request to enqueue the supplied ITM (or VITM) packet to be sent.
     *
     * @param itm_buf is a reference to an ITM (or VITM) packet.
     *
     * @note To avoid undetermined behavior, it is the caller's responsibility
     *       to ensure that the supplied const_buffer size is at least large 
     *       enough to account for an ITM header.
     */
    void SendDownlinkITM(const boost::asio::const_buffer& itm_buf);

    /**
     * The distribution point for the start of K-Frame signal.
     */
    void OnSOKF();

    
  private:

    //==================
    // Private Methods
    //==================

    /**
     * Make an asynchronous request to read a packet from the socket.
     */
    void DoRead();

    /**
     * Called when a packet has been received from the socket, or when
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
     * If the received packet is valid this method sends the packet to 
     * the data router.
     *
     * @param itm_packet is the ITM packet to route.
     */
    void HandleReadData(const boost::asio::const_buffer& itm_packet);

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
    DataRouter& router_;

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

    //------------------------------------------------------------------------
    /// The write_queue_ contains ITM messages that are to be written to the
    /// socket (as soon as possible).
    //------------------------------------------------------------------------
    using WriteQueue_t = std::deque<std::unique_ptr<std::vector<uint8_t>>>;

    WriteQueue_t write_queue_;

    //-------------------------------------------------------------------------
    /// The write_delayed_ contains ITM messages that have been requested
    /// to be sent, but are currently being "delayed" for some number of
    /// K-Frames.  When we receive notification of the start of a new K-Frame
    /// (a.k.a., SOKF), any delayed ITM messages that have become due to be
    /// written shall be pushed to the end of the write_queue_.
    //-------------------------------------------------------------------------
    using WriteDelayedMap_t =
      std::multimap<uint32_t, std::unique_ptr<std::vector<uint8_t>>>;

    WriteDelayedMap_t write_delayed_;
 };
}
