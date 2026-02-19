#pragma once

//===========================================================================
//===========================================================================

/** 
 * An interface to read the Start Of K-Frame from the vehicle simulation.
 * 
 *  @author: 
 *  @since: 9/1/16
 */

#include <boost/asio.hpp>
#include <functional>
#include <vector>

namespace mia
{

  class SOKFInterface
  {
  public:
  
    /**
     * The function to pass to this object which is to be called on receipt of
     * each timing signal.
     */
    using TimingNotificationCallback = std::function< void() >;
  
    /** 
     * Creates a new SOKFInterface
     *
     *  @param io_service Router's io_service for async operations
     *
     *  @param endpoint   Endpoint used by the socket
     *
     *  @param callback   The function to be called when the K-Frame
     *                    signal arrives. It is assumed that the callback
     *                    function is non-blocking and returns quickly.
     */
    SOKFInterface(
      boost::asio::io_service&              io_service,
      const boost::asio::ip::udp::endpoint& endpoint, 
      TimingNotificationCallback            callback);
  
    /**
     * Destructor.
     */
    ~SOKFInterface();
    
    /**
     * Start performing async reads on the interface's socket.
     */
    void Start();
    
    
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
     * If the received packet is valid this method calls the SOKF
     * callback and checks for any missed k frames.
     */
    void HandleReadData();

    /**
     * Helper method to cancel and close the socket if it is currently open,
     * and to suppress any errors that may arise while attempting to cancel
     * and close the socket.
     */
    void CancelAndCloseSocket();

    
    //=======================
    // Private Data Members
    //=======================

    //-------------------
    /// The UDP socket.
    //-------------------
    boost::asio::ip::udp::socket socket_;

    //-----------------------------------------------------------
    /// Callback routine to handle the SOKF message.
    //-----------------------------------------------------------
    TimingNotificationCallback callback_;
    
    //----------------------------------------------------------
    /// Flag to Prevent erroneously logging missing SOKFs when 
    /// the program is starting.
    //----------------------------------------------------------
    bool synchronizing_;

    //-----------------------------------------------------------------
    /// This is used to determine if any k frame offsets have been 
    /// missed and calculate the number of missed k frame offsets. 
    //-----------------------------------------------------------------
    uint8_t prev_kframe_offset_;
    
    //------------------------------------------------------------------
    /// Blob of bytes to hold the inbound packet read from the socket.
    //------------------------------------------------------------------
    std::vector<uint8_t> read_data_;
 };
}
