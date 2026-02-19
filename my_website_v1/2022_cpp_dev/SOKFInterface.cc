//===========================================================================
//===========================================================================

#include "SOKFInterface.h"
#include "log.h"
#include "MsgTypes.h"
#include "stats.h"
#include "VMW_Messages.h"
#include <array>

//---------------------------
// Open anonymous namespace
//---------------------------

namespace
{
  static constexpr uint8_t MAX_K_FRAME_OFFSET   = 9;
  static constexpr uint8_t NUM_K_FRAME_OFFSETS  = (::MAX_K_FRAME_OFFSET + 1);
  static constexpr size_t SOKF_SIZE = 12;
}

//----------------------------
// Close anonymous namespace
//----------------------------

//==============================================================================
//==============================================================================
mia::SOKFInterface::SOKFInterface(
    boost::asio::io_service&              io_service,
    const boost::asio::ip::udp::endpoint& endpoint, 
    TimingNotificationCallback            callback)
: socket_(io_service, endpoint)
, callback_(callback)
, synchronizing_(true)
, prev_kframe_offset_(0)
, read_data_(MsgTypes::MAX_IP_PACKET_SIZE)
{
}

//==============================================================================
//==============================================================================
mia::SOKFInterface::~SOKFInterface()
{
  this->CancelAndCloseSocket();
}

//==============================================================================
//==============================================================================
void
mia::SOKFInterface::Start()
{
  //-------------------------------------------------------------------------
  // Make a request to read the next packet from the socket (asynchronous).
  //-------------------------------------------------------------------------
  this->DoRead();
}

//==============================================================================
//==============================================================================
void
mia::SOKFInterface::DoRead()
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
mia::SOKFInterface::OnReadComplete(
  const boost::system::error_code& ec,
  size_t bytes_transferred)
{
  static constexpr auto fn = "SOKFInterface::OnReadComplete()";

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
  // If the number of bytes received matches our expectations, then delegate
  // to a private helper method to "handle" the data read from the socket.
  //
  // Otherwise, log a warning and ignore the received packet with the
  // unexpected size.
  //--------------------------------------------------------------------------
  if (::SOKF_SIZE == bytes_transferred)
  {
    //------------------------------------------------------------
    // Delegate to our HandleRead() method to handle the inbound
    // message (i.e. stored in our read_data_ member).
    //------------------------------------------------------------
    this->HandleReadData();
  }
  else
  {
    if (log::warn::enabled())
    {
      log::warn(fn)
        << "Ignoring message not matching SOKF_SIZE bytes:"
        << " expected "
        << ::SOKF_SIZE
        << " bytes, but received "
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
mia::SOKFInterface::HandleReadData()
{
  static auto fn = "SOKFInterface::HandleReadData()";
 
  //------------------------------------------------------------------
  // To properly process the SOKF message we must extract
  // the message id, message length, and kframe offset from
  // the message.
  //------------------------------------------------------------------
  std::array<uint32_t, 3> words;

  boost::asio::buffer_copy(boost::asio::buffer(words),
                           boost::asio::buffer(this->read_data_));
  
  const uint32_t msg_id        = ntohl(words[0]);
  const uint32_t msg_len       = ntohl(words[1]);
  const uint32_t kframe_offset = ntohl(words[2]);
                              
  // Assure that the message ID is what is expected.
  if (msg_id != TACSokfMsgId)
  {
    mia::log::fatal(fn)
        << "Got unexpected message type on SOKF interface: '"
        << msg_id
        << "'";

    //------------------------------------------------------------------
    // We are done with the socket.  Attempt to cancel and close it
    // (suppressing any problems we encounter attempting to do that).
    //------------------------------------------------------------------
    this->CancelAndCloseSocket();
    
    return;
  } 
      
  if (msg_len != (::SOKF_SIZE))
  {
    mia::log::fatal(fn)
        << "Got unexpected message length on SOKF interface '"
        << msg_len
        << "'";

    //------------------------------------------------------------------
    // We are done with the socket.  Attempt to cancel and close it
    // (suppressing any problems we encounter attempting to do that).
    //------------------------------------------------------------------
    this->CancelAndCloseSocket();
      
    return;
  }
    
  if (kframe_offset > ::MAX_K_FRAME_OFFSET)
  {
    mia::log::fatal(fn)
        << "Got invalid kframe offset on SOKF interface '"
        << kframe_offset
        << "'";

    //------------------------------------------------------------------
    // We are done with the socket.  Attempt to cancel and close it
    // (suppressing any problems we encounter attempting to do that).
    //------------------------------------------------------------------
    this->CancelAndCloseSocket();
      
    return;
  }

  // We have received a valid SOKF message distribute the SOKF notification.
  this->callback_();
  
  // Prevent erroneously logging missing SOKFs when the program is starting.
  if (this->synchronizing_)
  {
    //----------------------------------------------------------------------
    // This is our first SOKF message to be processed.
    //
    // Store the current kframe_offset in our prev_kframe_offset_ data
    // member and flip our synchronizing_ flag to false so that the "else"
    // block of code will be executed the next time we are in this method.
    //----------------------------------------------------------------------
    this->prev_kframe_offset_ = kframe_offset;
    this->synchronizing_ = false;

    if (mia::log::info::enabled())
    {
      mia::log::info(fn) << "Synchronized.";
    }
  }
  else
  {
    //-------------------------------------------------------------------
    // Let's compute how many times we would have to increment our
    // prev_kframe_offset_ value to have it be equal-to the current
    // kframe_offset value (and we have to increment it by at least 1).
    // This is what we will call the "num_kframes_elapsed", which should
    // be a number between 1 and NUM_K_FRAME_OFFSETS (inclusively).
    //-------------------------------------------------------------------
    const uint8_t num_kframes_elapsed =
      (kframe_offset > this->prev_kframe_offset_)
      ? kframe_offset - this->prev_kframe_offset_
      : (::NUM_K_FRAME_OFFSETS - this->prev_kframe_offset_) + kframe_offset;

    //------------------------------------------------------------------
    // We expect, nominally, that just 1 K-Frame has elapsed since the
    // previous K-Frame offset was stored.  If more than 1 K-Frame has
    // elapsed, then we have missed some K-Frames (one less than the
    // number of K-Frames that have elapsed).
    //------------------------------------------------------------------
    if (num_kframes_elapsed > 1)
    {
      // Increment the statistic by the number of missed K-Frames.
      mia::stats::TotalSokfMissed::increment(num_kframes_elapsed - 1);
    }

    //----------------------------------------------------------
    // Now we can store the current kframe_offset value in our
    // prev_kframe_offset_ data member.
    //----------------------------------------------------------
    this->prev_kframe_offset_ = kframe_offset;
  }
}

//==============================================================================
//==============================================================================
void
mia::SOKFInterface::CancelAndCloseSocket()
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

