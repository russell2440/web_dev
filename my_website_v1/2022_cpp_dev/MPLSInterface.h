#pragma once
//===========================================================================
//===========================================================================

/** 
 * An interface to read/write to the KBA
 * 
 *  @author: 
 *  @since: 2/13/15
 */

#include "MsgTypes.h"

#include <boost/asio.hpp>

#include <deque>
#include <functional>
#include <vector>
#include <memory>


namespace mia
{
  class DataRouter;


  class MPLSInterface
  {
  public:

    /** 
     * Creates a new MPLSInterface
     *
     *  @param router   Reference to DataRouter, used to forward received data.
     *
     *  @param io_service DataRouter's io_service, used for initializing socket.
     *
     *  @param endpoint   Address and port, used for initializing socket.
     *
     */
    MPLSInterface(
      DataRouter&                           router,
      boost::asio::io_service&              io_service,
      const boost::asio::ip::udp::endpoint& endpoint); 

    /**
     * Destructor.
     */
    ~MPLSInterface();
   
    /**
     * Start performing async reads on the interface's socket.
     */
    void Start();

    /**
     * This method is called by the DataRouter to send a downlink
     * “Pass-Through” VMW Message to the KBA.
     *
     * @param passthru_msg_buf is a reference to the bytes that represent the
     *        pass-through VMW message to be sent.
     */
    void SendDownlinkPassThroughMessage(
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
     * Upon completion of the asynchronous read this method is called
     * to determine if a properly formatted message was received.
     *
     * @param ec is the socket error code (which could be "none").
     *
     * @param bytes_transfered is the number of bytes received.
     */
    void OnReadComplete(
      const boost::system::error_code& ec,
      size_t bytes_transferred);

    /**
     * This method sends the packet to the data router.
     */
    void HandleReadData(size_t bytes_transfered);

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

    //-------------------
    /// The UDP socket.
    //-------------------
    boost::asio::ip::udp::socket socket_;

    //------------------------------------------------------------------
    /// Blob of bytes to hold the inbound packet read from the socket.
    //------------------------------------------------------------------
    std::vector<uint8_t> read_data_;

    //------------------------------------------------------------------------
    /// The write_queue_ contains messages that are to be written to the
    /// socket (as soon as possible).
    //------------------------------------------------------------------------
    using WriteQueue_t = std::deque<std::unique_ptr<std::vector<uint8_t>>>;

    WriteQueue_t write_queue_;
  };
}
