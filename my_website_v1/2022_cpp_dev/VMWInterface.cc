//==============================================================================
//==============================================================================

#include "VMWInterface.h"
#include "ConfigItems.h"
#include "DataRouter.h"
#include "ItmDelay.h"
#include "log.h"
#include "MIADefs.h"
#include "stats.h"
#include "VMWMessageBuilder.h"
#include "VMWMessageParser.h"


//==============================================================================
//==============================================================================
mia::VMWInterface::VMWInterface(
    DataRouter&                           router,
    boost::asio::io_service&              io_service,
    const boost::asio::ip::udp::endpoint& endpoint,
    const ItmDelay&                       itm_delay, 
    drop_pkt::ToSvPolicies_t&             to_sv_drop_policies)
: data_router_(router)
, itm_delay_(itm_delay)
, socket_(io_service, endpoint)
, to_sv_drop_policies_(to_sv_drop_policies)
, k_frame_count_(0)
, read_data_(MsgTypes::MAX_IP_PACKET_SIZE)
, write_queue_()
, write_delayed_()
{
}


//==============================================================================
//==============================================================================
mia::VMWInterface::~VMWInterface()
{
  this->CancelAndCloseSocket();
}


//==============================================================================
//==============================================================================
void
mia::VMWInterface::Start()
{
  //-------------------------------------------------------------------------
  // Make a request to read the next packet from the socket (asynchronous).
  //-------------------------------------------------------------------------
  this->DoRead();
}


//==============================================================================
//==============================================================================
void
mia::VMWInterface::DoRead()
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
mia::VMWInterface::OnReadComplete(
  const boost::system::error_code& ec,
  size_t bytes_transferred)
{
  static constexpr auto fn = "VMWInterface::OnReadComplete()";

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
    this->HandleReadData(boost::asio::buffer(this->read_data_, 
                                             bytes_transferred));
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
mia::VMWInterface::HandleReadData(const boost::asio::const_buffer& read_buf)
{
  //--------------------------------------------------------------------------
  // NOTE: This code ASSUMES that the supplied buffer will always refer to at
  // least 1 byte. The caller should ensure that this is true.
  //--------------------------------------------------------------------------
  static auto fn = "VMWInterface::HandleReadData()";
  
  if (log::debug::enabled())
  {
    log::debug(fn) 
      <<  "received bytes from VMW: " 
      << boost::asio::buffer_size(read_buf);
  }

  //---------------------------------------------------------------------------
  // The received data is considered a VMW Message containing a "group" of
  // individual "MPLS packets".  Each MPLS Packet consists of an MPLS Header
  // followed by an ITM (or VITM).
  //
  // VMWMessageParser::Perform() will process the recieved data and return a
  // vector of MPLS buffers (where each MPLS buffer refers to an ITM/MPLS or
  // a VITM/MPLS).
  // 
  // If there was any problem processing the received VMW message the returned
  // vector will be empty (and appropriate error and/or warning messsages will
  // have been logged already if an empty vector is returned).
  //
  // Otherwise, each const_buffer in the vector will reference bytes within the
  // received data (i.e., our read_data_ vector) that represent an individual
  // MPLS/ITM (or MPLS/VITM) packet.
  //---------------------------------------------------------------------------
  std::vector<boost::asio::const_buffer> mpls_buffers =
    VMWMessageParser::Perform(read_buf);

  //----------------------------------------------------------------------------
  // If the supplied vector of MPLS buffers is empty, it means that there was a
  // problem processing the recieved VMW Message. Hence there are no
  // buffers to be "routed" here.  If this is the case, then we will increment
  // a statistic indicating that we've discarded the receved VMW Messge and
  // then return to our caller (since there is nothing to be "routed").
  //----------------------------------------------------------------------------
  if (mpls_buffers.empty())
  {
    stats::TotalInvalidMplsPacketsDiscarded::increment();

    return;
  }

  //--------------------------------------------------------------------------
  // Attempt to route the MPLS packets to their proper destination.
  // Route each packet as an MPLS packet unless it's destined for the HPL.
  // HPL destined packets are skipped and then the entire message will be
  // routed as pass-through to the HPL.
  //--------------------------------------------------------------------------
  bool route_to_hpl_node = false;

  for (const auto& mpls_buf : mpls_buffers)
  {
    const auto itm_buf = mpls_buf + sizeof(MsgTypes::MPLSHeader_t);

    const MsgTypes::itm_header_t* itm_header =
      MsgTypes::itm_header_t::CastFromBuffer(itm_buf);

    //-----------------------------------------------------------------------
    // Deal with the possibility that the ITM is destined for the HPL Node.
    //-----------------------------------------------------------------------
    if (itm_header->GetDestinationId() == config::Items().HPL_NODE_ID)
    {
      //--------------------------------------------------------------------
      // Record that at least a single MPLS packet is destined for the HPL.
      //--------------------------------------------------------------------
      route_to_hpl_node = true;
    }
    else
    {
      //----------------------------------------------------------------------
      // Ask the DataRouter to route the MPLS packet since it isn't destined
      // for the configured HPL node id.
      //----------------------------------------------------------------------
      this->data_router_.RouteDownlinkMPLSPacket(mpls_buf);
    }
  }

  //----------------------------------------------------------------------
  // If any of the individual MPLS packets were found to be destined for
  // the HPL node, then we need to route the entire message received from
  // VMW to the DataRouter to be sent to the KBA(via the MPLSInterface)
  // as "Pass-Through" data.
  //----------------------------------------------------------------------
  if (route_to_hpl_node)
  {
    this->data_router_.RouteDownlinkPassThroughMessage(read_buf);
  }

  //------------------------------------------------------------
  // We know that there was at least one individual MPLS Packet
  // "converted" (to an ITM/vITM).  So we will unconditionally
  // increment the TotaMplsPacketsConverted statistic by one.
  //------------------------------------------------------------
  stats::TotalMplsPacketsConverted::increment();
}

    
//==============================================================================
//==============================================================================
void
mia::VMWInterface::SendUplinkITM(
  const boost::asio::const_buffer& itm_buf,
  uint8_t source_node,
  uint8_t dest_node)
{
  //----------------------------------------------------------------------
  // Get the ITM Header from the itm_buf and determine the payload type.
  //----------------------------------------------------------------------
  const MsgTypes::itm_header_t* itm_header =
    MsgTypes::itm_header_t::CastFromBuffer(itm_buf);

  const uint8_t payload_type = itm_header->GetPayloadType();

  //--------------------------------------------------------------------------
  // We will provide a QOS value for the MPLS header that we create.
  // That value comes from one of three places.
  //  For Fixed Size ITM packets:
  //  1) the QOS value will be supplied from the ITM packets payload type.
  //  For VITM packets the QOS value will be supplied from either:
  //  2) the mia::Defs::mpls_qos::OAM enum value if it is an ITE Control
  //     Message or 
  //  3) the Config Item MPLS_QOS_FOR_ITE_DATA_MSG if it's an ITE Data Message.
  //
  // We clip a byte off of the payload for Fixed Sized Mission Data Payload
  // type ITMs.
  // This is done to replicate the behavior of the SV hardware when it passes
  // real message data to the real VMW.  It clips one byte of the payload.
  // So we will do it here.
  // We will use all bytes for the remaining type ITMs.
  // 
  // NOTE: DataRouter is 'under contract' to give us enough bytes for this
  //                  to work (which has a similar contract with its caller).
  //--------------------------------------------------------------------------
  boost::asio::const_buffer use_this_itm_buf = itm_buf;

  uint8_t qos_value = payload_type;

  if (itm_header->IsVITM())
  {
    // This is a VITM packet.
    qos_value = mia::Defs::mpls_qos::OAM;

    const boost::asio::const_buffer vitm_payload_buf =
      itm_buf + sizeof(MsgTypes::itm_header_t);

    const MsgTypes::ite_common_header_t* ite_header =
      MsgTypes::ite_common_header_t::CastFromBuffer(vitm_payload_buf);

    if (ite_header->IsDataMessage())
    {
      // With an ITE data message payload.
      qos_value = config::Items().MPLS_QOS_FOR_ITE_DATA_MSG;
    }
  }
  else
  {
    // This is a Fixed Sized ITM packet.

    if (itm_header->IsMissionDataPayloadType())
    {
    // With a Mission Data Payload.
      use_this_itm_buf =
        boost::asio::buffer(itm_buf, boost::asio::buffer_size(itm_buf) - 1);
    }
  }

  //-----------------------------------------------------------
  // Build and populate the Fake IMPLS header.  Use the 
  // provided QOS value.  Put it in Network order.
  //-----------------------------------------------------------
  const MsgTypes::MPLSHeader_t mpls_header =
    MsgTypes::MPLSHeader_t::
      CreateFakeIMPLS(qos_value).HostToNetworkOrder();

  //------------------------------------------------------------------
  // Compute the packet size (MPLS Header + ITM bytes) and store the
  // result in network order.
  //------------------------------------------------------------------
  const uint16_t packet_size =
    htons(sizeof(MsgTypes::MPLSHeader_t) +
          boost::asio::buffer_size(use_this_itm_buf));

  //----------------------------------------------------------
  // Build a vector of const_buffer objects representing the
  // (network order) packet_size, the mpls_header, and the
  // ITM bytes to be used.
  //----------------------------------------------------------
  const std::vector<boost::asio::const_buffer> buffers =
  {
    boost::asio::buffer(&packet_size, sizeof(packet_size)),
    boost::asio::buffer(&mpls_header, sizeof(mpls_header)),
    use_this_itm_buf
  };

  //------------------------------------------------------------------------
  // Determine how many K-Frames we should delay writing the packet to the
  // socket (which could be 0 K-Frames).
  //------------------------------------------------------------------------
  const uint32_t k_frame_to_delay =
    this->itm_delay_.get(source_node, dest_node);

  //-------------------------------------------------------------------------
  // Calculate the specific "K-Frame Count" value at which time the packet
  // should be written to the socket and then insert the packet (and the
  // computed K-Frame Count time) into our writeDelayed_ multimap.
  //-------------------------------------------------------------------------
  const uint32_t k_frame_count_to_write =
    this->k_frame_count_ + k_frame_to_delay;

  //------------------------------------------------------
  // Schedule the VMWUplinkMPLSPacket to be handled later.
  //------------------------------------------------------
  this->write_delayed_.emplace(
    k_frame_count_to_write,
    VMWUplinkMPLSPacket::Create(payload_type, buffers));
}


//==============================================================================
//==============================================================================
void
mia::VMWInterface::SendUplinkMPLSPacket(
  const boost::asio::const_buffer& mpls_packet_buf,
  uint8_t source_node,
  uint8_t dest_node)
{
  //------------------------------------------------------------------
  // Compute the packet size and store the result in network order.
  //------------------------------------------------------------------
  const uint16_t packet_size =
    htons(boost::asio::buffer_size(mpls_packet_buf));

  //----------------------------------------------------------
  // Build a vector of const_buffer objects representing the
  // (network order) packet_size, and the mpls_packet bytes to be used.
  //----------------------------------------------------------
  const std::vector<boost::asio::const_buffer> buffers =
  {
    boost::asio::buffer(&packet_size, sizeof(packet_size)),
    mpls_packet_buf
  };

  //------------------------------------------------------------------------
  // Determine how many K-Frames we should delay writing the packet to the
  // socket (which could be 0 K-Frames).
  //------------------------------------------------------------------------
  const uint32_t k_frame_to_delay =
    this->itm_delay_.get(source_node, dest_node);

  //-------------------------------------------------------------------------
  // Calculate the specific "K-Frame Count" value at which time the packet
  // should be written to the socket and then insert the packet (and the
  // computed K-Frame Count time) into our writeDelayed_ multimap.
  //-------------------------------------------------------------------------
  const uint32_t k_frame_count_to_write =
    this->k_frame_count_ + k_frame_to_delay;

  //----------------------------------------------------------------------
  // Determine the payload type.
  // This is done by getting the ITM header from the ITM (or VITM) part of
  // this MPLS packet.  And then get the payload type from the header.
  //----------------------------------------------------------------------
  const MsgTypes::itm_header_t* itm_header =
    MsgTypes::itm_header_t::CastFromBuffer(
      mpls_packet_buf +
      sizeof(MsgTypes::MPLSHeader_t));

  const uint8_t payload_type = itm_header->GetPayloadType();

  //------------------------------------------------------
  // Schedule the VMWUplinkMPLSPacket to be handled later.
  //------------------------------------------------------
  this->write_delayed_.emplace(
    k_frame_count_to_write,
    VMWUplinkMPLSPacket::Create(payload_type, buffers));
}


//==============================================================================
//==============================================================================
void
mia::VMWInterface::SendUplinkPassThroughMessage(
  const boost::asio::const_buffer& passthru_msg_buf)
{
  //----------------------------------------------------------------------------
  // We first check to see if our write_queue_ is empty.  
  //
  // If it is empty, then we will end up making it become non-empty once we 
  // push the packet to the back of the write_queue_. And, if we make the 
  // write_queue_ transition from being empty to being non-empty, then we will 
  // need to initiate the "servicing" of the write_queue_ (i.e. to write the 
  // packets in the write_queue_ to the socket, one-by-one, until the 
  // write_queue_ becomes empty once again). 
  //
  // If our _write_queue_ is currently NOT empty then it's in the middle being
  // serviced. Hence we will not need to initiate the "servicing" of the 
  // write_queue_.
  //----------------------------------------------------------------------------
  const bool initiate_write = this->write_queue_.empty();

  //---------------------------------------------------------------------------
  // Queue up the element into the write queue for an eventual async send to 
  // the uplink destination.
  //---------------------------------------------------------------------------
  this->write_queue_.push_back(
    VMWUplinkMessage::Create(VMWUplinkMessage::Destination::CP,
                             passthru_msg_buf));

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
mia::VMWInterface::OnSOKF()
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
  bool write_queue_empty_at_start = this->write_queue_.empty();

  //---------------------------------------------------------------------------
  // Now we want to delegate to a private method which will handle any of the
  // scheduled/delayed MPLS Packets that are now due to be sent (i.e., since
  // we incremented the k_frame_count_ by 1).
  //---------------------------------------------------------------------------
  this->HandleWriteDelayed();

  //----------------------------------------------------------------------
  // Start the servicing of the write_queue if it is not already being 
  // serviced.
  //----------------------------------------------------------------------
  if (write_queue_empty_at_start && !this->write_queue_.empty())
  {
    this->DoWrite();
  }
}


//==============================================================================
//==============================================================================
void 
mia::VMWInterface::HandleWriteDelayed()
{
  //----------------------------------------------------------------------------
  // In this method we look for ITM packets in k-frame delay map that are ready
  // to be sent.  And then we use the ready packets to build VMW messages.  And
  // then we queue the messages up for sending over a socket to their
  // destination (which is either the CP or DP).
  // 
  // We use a helper class called VMWMessageBuilder to build the ready packets
  // into the VMW Message and to queue up the messages.
  //
  // We instantiate the message builder with a reference to our write_queue_
  // and to_sv_drop_poicies_ objects.
  //----------------------------------------------------------------------------

  //------------------------------------------------
  // Create the VMW message builder helper object.
  //------------------------------------------------
  VMWMessageBuilder message_builder(this->to_sv_drop_policies_,
                                    this->write_queue_);

  //--------------------------------------------------------------------------
  // This loop is for finding the ITMs/vITMs in the write_delayed_ multimap
  // that are ready to be sent and supplying the ITM/vITM (bytes) to the
  // VMWMessageBuilder.
  //--------------------------------------------------------------------------
  while (!this->write_delayed_.empty() &&
         (this->write_delayed_.begin()->first <= this->k_frame_count_))
  {
    const auto itr = this->write_delayed_.begin();

    message_builder.add_packet(std::move(itr->second));

    this->write_delayed_.erase(itr);
  }

  //-------------------------------------------------------------------
  // This gives the VMWMessageBuilder the opportunity to deal with any
  // "partial" groups of MPLS Packets.
  //-------------------------------------------------------------------
  message_builder.finalize();
}


//==============================================================================
//==============================================================================
void
mia::VMWInterface::DoWrite()
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
    // The message bytes.
    boost::asio::buffer(this->write_queue_.front().GetBytes()),
    // The destination endpoint.
    this->write_queue_.front().IsDestinedForCP()
       ? config::Items().CP_VMW_DEST_ADDRESS
       : config::Items().DP_VMW_DEST_ADDRESS,
    // The completion handler, invoked when the asynchronous "send" operation
    // has completed, successfully or not.
    [this] (boost::system::error_code ec, size_t bytes_transferred)
    { this->OnWriteComplete(ec, bytes_transferred); });
}


//==============================================================================
//==============================================================================
void
mia::VMWInterface::OnWriteComplete(
  const boost::system::error_code& ec,
  size_t /*bytes_transferred*/)
{
  static constexpr auto fn = "VMWInterface::OnWriteComplete()";

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
    // We are done with the socket. Attempt to cancel and close it
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
mia::VMWInterface::CancelAndCloseSocket()
{
  //------------------------------------------------------------------------
  // Attempt to cancel and close the socket if it is currently open, and
  // supress any errors that may be encountered while attempting to do so.
  //------------------------------------------------------------------------
  if (this->socket_.is_open())
  {
    //------------------------------------------------------------------
    // Pass this to cancel() and close() methods to prevent exceptions.
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
