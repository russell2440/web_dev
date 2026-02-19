//===========================================================================
//===========================================================================

#include "MDInterface.h"
#include "ConfigItems.h"
#include "DataRouter.h"
#include "ItmDelay.h"
#include "log.h"

//==============================================================================
//==============================================================================
mia::MDInterface::MDInterface(
    DataRouter&              data_router,
    boost::asio::io_service& io_service,
    const ItmDelay&          itm_delay)
: router_(data_router)
, itm_delay_(itm_delay)
, socket_(io_service, config::Items().MISSION_DATA_LISTEN_ADDRESS)
, k_frame_count_(0)
, read_data_(MsgTypes::MAX_IP_PACKET_SIZE)
, write_queue_()
, write_delayed_()
{}


//==============================================================================
//==============================================================================
mia::MDInterface::~MDInterface()
{
  this->CancelAndCloseSocket();
}


//==============================================================================
//==============================================================================
void 
mia::MDInterface::Start()
{
  //-------------------------------------------------------------------------
  // Make a request to read the next packet from the socket (asynchronous).
  //-------------------------------------------------------------------------
  this->DoRead();
}


//==============================================================================
//==============================================================================
void
mia::MDInterface::SendDownlinkITM(const boost::asio::const_buffer& itm_buf)
{
  //-------------------------------------------------------------------------
  // At this point we must copy the data from the boost::asio::const_buffer
  // into a vector that can be placed onto the write queue.
  //-------------------------------------------------------------------------
  auto itm_data = std::unique_ptr<std::vector<uint8_t>>(
      new std::vector<uint8_t>(boost::asio::buffer_size(itm_buf)));

  boost::asio::buffer_copy(boost::asio::buffer(*itm_data), itm_buf);

  //----------------------------------------------------------------------
  // Extract the header data in order to get the destination node.
  // Note: We assume that the itm_buf received is at least large
  //       enough to contain the itm header because it is the callers
  //       responsibility to verify packet length.
  //----------------------------------------------------------------------
  const MsgTypes::itm_header_t* itm_header =
    MsgTypes::itm_header_t::CastFromBuffer(boost::asio::buffer(*itm_data));
  
  //----------------------------------------------------
  // Determine the souce and destination nodes
  //
  // The destination node is always in the ITM header.
  //----------------------------------------------------
  const uint8_t dest_node = itm_header->GetDestinationId();

  //----------------------------------------------------------------------
  // Mission data types don't contain the source node in the ITM header,
  // so we read it from the configuration.
  //----------------------------------------------------------------------
  const uint8_t source_node = config::Items().LOCAL_NODE_ID;

  //------------------------------------------------------------------------
  // Determine how many K-Frames we should delay writing the packet to the
  // socket (which could be 0 K-Frames).
  //
  // The mission data interface always sends ITMs from the SV so the
  // source node is the SV node parameter.
  //------------------------------------------------------------------------
  const uint32_t k_frame_to_delay =
    this->itm_delay_.get(source_node, dest_node);

  //-------------------------------------------------------------------------
  // If the packet is to be delayed, then we will calculate the specific
  // "K-Frame Count" value, which is the time that the packet should be written
  // to the socket.  And then we insert the packet (and the computed K-Frame
  // Count time) into our write_delayed_ multimap.
  //-------------------------------------------------------------------------
  if (k_frame_to_delay > 0)
  {
    const uint32_t k_frame_count_to_write =
      this->k_frame_count_ + k_frame_to_delay;

    this->write_delayed_.emplace(k_frame_count_to_write, std::move(itm_data));

    // We are done in this case, so don't continue further.
    return;
  }

  //--------------------------------------------------------------------------
  // We only get here if the packet should NOT be delayed.
  //
  // We first check to see if our write_queue_ is empty.  If it is empty,
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

  this->write_queue_.push_back(std::move(itm_data));

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
mia::MDInterface::OnSOKF()
{
  //-----------------------------------------------------------------------
  // Increment our k_frame_count_ member since we've received a new SOKF.
  //-----------------------------------------------------------------------
  ++this->k_frame_count_;

  //-----------------------------------------------------------------------
  // Before we potentially update our write_queue_, we want to determine
  // (and remember) if the write_queue_ is currently empty.  This will be
  // needed later in this method to determine if we should initiate the
  // "servicing" of the write_queue_.
  //-----------------------------------------------------------------------
  const bool write_queue_empty_at_start = this->write_queue_.empty();

  //------------------------------------------------------------------------
  // For each "write delayed" packet that is now "due" to be written to
  // the socket, we want to push the packet to the end of the write_queue_
  // and remove the packet from the write_delayed_ collection.
  //------------------------------------------------------------------------
  while (!this->write_delayed_.empty() &&
         (this->write_delayed_.begin()->first <= this->k_frame_count_))
  {
    const auto delayed_itr = this->write_delayed_.begin();

    this->write_queue_.push_back(std::move(delayed_itr->second));
    this->write_delayed_.erase(delayed_itr);
  }

  //---------------------------------------------------------------------------
  // If we made the write_queue_ transition from being empty to being
  // non-empty, then we need to initiate the "servicing" of the write_queue_.
  //---------------------------------------------------------------------------
  if (write_queue_empty_at_start && !this->write_queue_.empty())
  {
    this->DoWrite();
  }
}

//==============================================================================
//==============================================================================
void
mia::MDInterface::DoRead()
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
mia::MDInterface::OnReadComplete(
  const boost::system::error_code& ec,
  size_t bytes_transferred)
{
  static constexpr auto fn = "MDInterface::OnReadComplete()";

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
  // Delegate to a private helper method to "handle" the data read 
  // from the socket.
  //--------------------------------------------------------------------------
  this->HandleReadData(boost::asio::buffer(read_data_,bytes_transferred));

  //--------------------------------------------------------------
  // Call our DoRead() method to make an asynchronous request to
  // read the next message from the socket.
  //--------------------------------------------------------------
  this->DoRead();
}

//==============================================================================
//==============================================================================
void
mia::MDInterface::HandleReadData(const boost::asio::const_buffer& itm_packet)
{
  static auto fn = "MDInterface::HandleReadData()";
  
  // Capture the itm_packet buffer size.  We will be using it.
  size_t itm_buffer_size = boost::asio::buffer_size(itm_packet);

  //--------------------------------------------------------------------
  // Now check that the ITM (or VITM) packet length will cover the size
  // of the ITM header.  Othewise drop the packet.
  //--------------------------------------------------------------------
  static const size_t ITM_HEADER_SIZE = sizeof(MsgTypes::itm_header_t);

  if (itm_buffer_size < ITM_HEADER_SIZE)
  {
    // The packet size won't cover the size of a header.  Drop the packet.
    if (log::error::enabled())
    {
      log::error(fn)
        << "The received ITM (or VITM) is smaller than an ITM header. "
        << "The ITM header size is ["
        << ITM_HEADER_SIZE
        << "] bytes. "
        << "The actual size is [" << itm_buffer_size << "] bytes. "
        << "Dropping Packet.";
    }
    return;
  }

  //--------------------------------------------------------------------------
  // Here we perform some sanity checks on the packet's payload size.
  //
  // And if these checks fail, we raise an error-level log event and return
  // control to the caller rather than proceeding to route the problematic
  // uplink packet through the DataRouter.
  //--------------------------------------------------------------------------

  // Capture this packet's payload size.
  const size_t itm_payload_size =
    itm_buffer_size - sizeof(MsgTypes::itm_header_t);

  // Check for this being an ITM (or VITM) packet.
  const auto itm_header =
    boost::asio::buffer_cast<const MsgTypes::itm_header1_alt_t*>(itm_packet);


  if (itm_header->IsVITM())
  {
    // Perform a VITM payload size check.
    if ((itm_payload_size < Defs::MIN_VITM_PAYLOAD_SIZE) ||
        (itm_payload_size > Defs::MAX_VITM_PAYLOAD_SIZE))
    {
      // Drop the packet.
      if (log::error::enabled())
      {
        log::error(fn)
          << "VITM payload size is incorrect. "
          << "The expected size is between ["
          << Defs::MIN_VITM_PAYLOAD_SIZE
          << "] bytes and ["
          << Defs::MAX_VITM_PAYLOAD_SIZE
          << "] bytes. "
          << "The actual size is [" << itm_payload_size << "] bytes. "
          << "Dropping Packet.";
      }

      return;
    }
  }
  else
  {
    // Perform a Fixed ITM payload size check.
    if (itm_payload_size != Defs::FIXED_ITM_PAYLOAD_SIZE)
    {
      // Drop the packet.
      if (log::error::enabled())
      {
        log::error(fn)
          << "Fixed ITM payload size is incorrect. "
          << "The expected size is ["
          << Defs::FIXED_ITM_PAYLOAD_SIZE
          << "] bytes. "
          << "The actual size is [" << itm_payload_size << "] bytes. "
          << "Dropping Packet.";
      }
      
      return;
    }
  }

  // Drop Non MD payload ITMs (or VITMs).
  if (!itm_header->IsMissionDataPayloadType())
  {
    // Non mission data ITM ignore it.
    if (log::warn::enabled())
    {
      log::warn(fn)
        << "Received ITM with payload type " 
        << int(itm_header->GetPayloadType())
        << " on mission data socket";
    }

    return;
  }

  // Route the ITM packet through the Data Router to the VMW interface.
  this->router_.RouteUplinkITM(
    itm_packet,
    itm_header->sourceNode,
    itm_header->GetDestinationId());
}


//==============================================================================
//==============================================================================
void
mia::MDInterface::DoWrite()
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
    boost::asio::buffer(*this->write_queue_.front()),
    // The destination endpoint
    config::Items().MISSION_DATA_DEST_ADDRESS,
    // The completion handler, invoked when the asynchronous "send" operation
    // has completed, successfully or not.
    [this] (boost::system::error_code ec, size_t bytes_transferred)
    { this->OnWriteComplete(ec, bytes_transferred); });
}


//==============================================================================
//==============================================================================
void
mia::MDInterface::OnWriteComplete(
  const boost::system::error_code& ec,
  size_t /*bytes_transferred*/)
{
  static constexpr auto fn = "MDInterface::OnWriteComplete()";

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
mia::MDInterface::CancelAndCloseSocket()
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
