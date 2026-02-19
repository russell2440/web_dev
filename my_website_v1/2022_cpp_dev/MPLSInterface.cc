//===========================================================================
//===========================================================================

#include "MPLSInterface.h"
#include "log.h"
#include "DataRouter.h"
#include "ConfigItems.h"


//==============================================================================
//==============================================================================
mia::MPLSInterface::MPLSInterface(
  DataRouter&                           router,
  boost::asio::io_service&              io_service,
  const boost::asio::ip::udp::endpoint& endpoint)
: data_router_(router)
, socket_(io_service, endpoint)
, read_data_(MsgTypes::MAX_IP_PACKET_SIZE)
, write_queue_()
{
}


//==============================================================================
//==============================================================================
mia::MPLSInterface::~MPLSInterface()
{
  this->CancelAndCloseSocket();
}


//==============================================================================
//==============================================================================
void
mia::MPLSInterface::Start()
{
  //-------------------------------------------------------------------------
  // Make a request to read the next packet from the socket (asynchronous).
  //-------------------------------------------------------------------------
  this->DoRead();
}


//==============================================================================
//==============================================================================
void
mia::MPLSInterface::DoRead()
{
  //------------------------------------------------------------
  // Ask the socket to asynchronously receive the next packet.
  //------------------------------------------------------------
  this->socket_.async_receive(
    boost::asio::buffer(this->read_data_),
    [this] (boost::system::error_code ec, size_t bytes_transferred)
    {
      this->OnReadComplete(ec, bytes_transferred);
    });
}

//==============================================================================
//==============================================================================
void
mia::MPLSInterface::OnReadComplete(
  const boost::system::error_code& ec,
  size_t bytes_transferred)
{
  static constexpr auto fn = "MPLSInterface::OnReadComplete()";

  //---------------------------------------------------------------
  // First check for and handle the case where an error occurred.
  //---------------------------------------------------------------
  if (ec)
  {
    if (ec.value() == boost::system::errc::operation_canceled)
    {
      if (log::info::enabled())
      {
        log::info(fn)
          << "Operation cancelled due to socket shutdown: "
          << ec.message();
      }
    }
    else
    {
      //----------------------------------------------------------
      // Raise a fatal log message, which has the side-effect of
      // "faulting" the application.
      //----------------------------------------------------------
      log::fatal(fn) << "Socket error: " << ec.message();
    }

    //------------------------------------------------------------------
    // We are done with the socket.  Attempt to cancel and close it
    // (suppressing any problems we encounter attempting to do that).
    //------------------------------------------------------------------
    this->CancelAndCloseSocket();

    //-----------------------------
    // Don't proceed any further.
    //-----------------------------
    return;
  }

  //--------------------------------------------------------------------------
  // If the number of bytes is positive, then delegate to a private helper 
  // method to "handle" the data read from the socket.
  //
  // Otherwise, log a warning and ignore the received packet with the
  // unexpected size.
  //--------------------------------------------------------------------------
  if (bytes_transferred > 0)
  {
    //------------------------------------------------------------
    // Delegate to our HandleRead() method to handle the inbound
    // message (i.e. stored in our read_data_ member).
    //------------------------------------------------------------
    this->HandleReadData(bytes_transferred);
  }
  else
  {
    if (log::warn::enabled())
    {
      log::warn(fn)
        << "Ignoring message with a non-positive byte count:"
        << " received byte count of "
        << bytes_transferred
        << " bytes.";
    }
  }

  //--------------------------------------------------------------
  // Call our DoRead() method to make an asynchronous request to
  // read the next message from the socket.
  //--------------------------------------------------------------
  this->DoRead();
}


//==============================================================================
//==============================================================================
void
mia::MPLSInterface::HandleReadData(size_t bytes_transfered)
{
  static auto fn = "MPLSInterface::HandleReadData()";
  
  if (mia::log::notice::enabled())
  {
    mia::log::notice(fn) <<  "received bytes from KBA: "<<bytes_transfered;
  }

  //--------------------------------------------------------------------------
  // Route the entire received data through the Data Router as an uplink
  // passthrough message.
  //--------------------------------------------------------------------------
  this->data_router_.RouteUplinkPassThroughMessage(
    boost::asio::buffer(this->read_data_, bytes_transfered));
}


//==============================================================================
//==============================================================================
void
mia::MPLSInterface::SendDownlinkPassThroughMessage(
  const boost::asio::const_buffer& passthru_msg_buf)
{
  //----------------------------------------------------------------------
  // At this point we must copy the data from the boost::asio::buffer
  // into a vector that can be placed onto the write queue.
  //----------------------------------------------------------------------
  auto write_data = std::unique_ptr<std::vector<uint8_t>>(
    new std::vector<uint8_t>(boost::asio::buffer_size(passthru_msg_buf)));

  boost::asio::buffer_copy(boost::asio::buffer(*write_data), passthru_msg_buf);
    
  //--------------------------------------------------------------------------
  // We must first check to see if our write_queue_ is empty.  If it is empty,
  // then we will end up making it become non-empty once we push the packet
  // to the back of the write_queue_.  And, if we make the write_queue_
  // transition from being empty to being non-empty, then we will need to
  // initiate the "servicing" of the write_queue_ (i.e. to write the packets
  // in the write_queue_ to the socket, one-by-one, until the write_queue_
  // becomes empty once again).
  //
  // If our write_queue_ is currently NOT empty, then we won't be making it
  // transition from being empty to non-empty when we push the packet to the
  // back of the write_queue_ and, hence, we will not need to initiate the
  // "servicing" of the write_queue_ (since it will already be in the middle
  // of "servicing" the write_queue_).
  //--------------------------------------------------------------------------
  const bool initiate_write = this->write_queue_.empty();

  this->write_queue_.push_back(std::move(write_data));

  if (initiate_write)
  {
    //----------------------------------------------------------------------
    // Since we did transition the write_queue_ from being empty to being
    // non-empty, we must initiate the "servicing" of the write_queue_ (by
    // invoking our DoWrite() method).
    //----------------------------------------------------------------------
    this->DoWrite();
  }
}


//==============================================================================
//==============================================================================
void 
mia::MPLSInterface::DoWrite()
{
  //=========================================================================
  // NOTE: The caller of this private method *MUST* only invoke this method
  //       when the write_queue_ is NOT empty.
  //=========================================================================

  //------------------------------------------------------------------------
  // Make a request on the socket to asynchronously send the bytes for the
  // packet at the front of the write_queue_ to the configured destination
  // endpoint.  The supplied completion handler function will be invoked
  // when the asynchronous request has completed (successfully or not).
  //------------------------------------------------------------------------
  this->socket_.async_send_to(
    // The write buffer
    boost::asio::buffer(*(this->write_queue_.front())),
    // The destination endpoint
    config::Items().KBA_DEST_ADDRESS,
    // The completion handler, invoked when the asynchronous "send" operation
    // has completed, successfully or not.
    [this] (boost::system::error_code ec, size_t bytes_transferred)
    { this->OnWriteComplete(ec, bytes_transferred); });
}


//==============================================================================
//==============================================================================
void 
mia::MPLSInterface::OnWriteComplete(
  const boost::system::error_code& ec,
  size_t /* bytes_transferred*/)
{
  static constexpr auto fn = "MPLSInterface::OnWriteComplete()";

  //---------------------------------------------------------------
  // First check for and handle the case where an error occurred.
  //---------------------------------------------------------------
  if (ec)
  {
    if (ec.value() == boost::system::errc::operation_canceled)
    {
      if (log::info::enabled())
      {
        log::info(fn)
          << "Operation cancelled due to socket shutdown: "
          << ec.message();
      }
    }
    else
    {
      //----------------------------------------------------------
      // Raise a fatal log message, which has the side-effect of
      // "faulting" the application.
      //----------------------------------------------------------
      log::fatal(fn) << "Socket error: " << ec.message();
    }

    //------------------------------------------------------------------
    // We are done with the socket.  Attempt to cancel and close it
    // (suppressing any problems we encounter attempting to do that).
    //------------------------------------------------------------------
    this->CancelAndCloseSocket();

    //-----------------------------
    // Don't proceed any further.
    //-----------------------------
    return;
  }

  //------------------------------------------------------------------------
  // We can now remove the packet from the front of the write_queue_ since
  // it has been successfully sent to the socket.
  //------------------------------------------------------------------------
  this->write_queue_.pop_front();
  
  //--------------------------------------------------------------------------
  // If the write_queue_ is not empty, then we need to make the asynchronous
  // request to write the packet that is NOW at the front of the queue.
  //--------------------------------------------------------------------------
  if (!this->write_queue_.empty())
  {
    this->DoWrite();
  }
}


//==============================================================================
//==============================================================================
void
mia::MPLSInterface::CancelAndCloseSocket()
{
  //------------------------------------------------------------------------
  // Attempt to cancel and close the socket if it is currently open, and
  // supress any errors that may be encountered while attempting to do so.
  //------------------------------------------------------------------------
  if (this->socket_.is_open())
  {
    //------------------------------------------------------------------
    // Pass this to cancel() and close() methods to prevent exceptions
    //------------------------------------------------------------------
    boost::system::error_code ec;

    //---------------------------------------------
    // Cancel all outstanding I/O on this socket.
    //---------------------------------------------
    this->socket_.cancel(ec);

    //--------------------
    // Close the socket.
    //--------------------
    this->socket_.close(ec);
  }
}

