//==============================================================================
//==============================================================================

#include "VMWMessageParser.h"
#include "log.h"
#include "MIADefs.h"
#include "MsgTypes.h"
#include "stats.h"


//==============================================================================
//==============================================================================
mia::VMWMessageParser::VMWMessageParser(
  const boost::asio::const_buffer& vmw_message_buf)
: vmw_message_buf_(vmw_message_buf)
, remainder_buf_(this->vmw_message_buf_)
, mpls_buffers_()
{
}


//==============================================================================
//==============================================================================
std::vector<boost::asio::const_buffer>
mia::VMWMessageParser::Perform(const boost::asio::const_buffer& vmw_message_buf)
{
  //--------------------------------------------------------------------------
  // We will create an instance of the VMWMessageParser class to be used to
  // process the bytes that represent a received VMW Message containing a
  // group of MPLS Packets.
  //--------------------------------------------------------------------------
  VMWMessageParser parser(vmw_message_buf);

  //--------------------------------------------------------------------------
  // Ask the VMWMessageParser object to process the message and return an
  // empty vector if the parser is unsuccessful.
  //--------------------------------------------------------------------------
  if (!parser.Process())
  {
    // Return an empty vector.
    return {};
  }

  //--------------------------------------------------------------------------
  // Since the parser was successful, we can ask it to give us the vector of
  // MPLS const_buffer objects that were “mined” from the VMW Message bytes.
  //--------------------------------------------------------------------------
  return parser.TakeMPLSBuffers();
}


//==============================================================================
//==============================================================================
std::vector<boost::asio::const_buffer>
mia::VMWMessageParser::TakeMPLSBuffers()
{
  //---------------------------------------------------
  // Return parser's group of MPLS buffers.
  // Note: this empties the 'mpls_buffers_' vector.
  //---------------------------------------------------
  return std::move(this->mpls_buffers_);
}


//==============================================================================
//==============================================================================
bool
mia::VMWMessageParser::Process()
{
  static constexpr auto fn = "VMWMessageParser::Process()";

  //-----------------------------------------------------------
  // Get the VMW message size because we will need it.
  //
  // Get it now since we will be modifying the remainder_buf_
  // shortly and it currently holds the whole message.
  //-----------------------------------------------------------
  const size_t vmw_message_size =
    boost::asio::buffer_size(this->remainder_buf_);

  //---------------------------------------------------
  // Mine a VMW message header from the remainder buf.
  //---------------------------------------------------
  const vmw_message_header_t* vmw_message_hdr = this->MineVMWMessageHeader();

  if (vmw_message_hdr == nullptr)
  {
    return false;
  }
  
  //---------------------------------------------------------------------------
  // We're checking that the message length specified in the VMW message 
  // header actually matches the length of the entire received message.
  //
  // NOTE: The "message_length" field is stored in network order thus must be
  //       converted to host order in order to access the value.
  //---------------------------------------------------------------------------
  const uint32_t message_length = ntohl(vmw_message_hdr->message_length);
  
  if (vmw_message_size != message_length)
  {
    if (log::error::enabled())
    {
      log::error(fn)
        << "The size of the received VMW message ["
        << vmw_message_size
        << "] "
        << "does not match length specified in the VMW message header ["
        << message_length 
        << "].";
    }
  
    // The check failed. The sizes didn't match up. 
    return false;
  }
  
  //------------------------------------------------------------------------
  // Mine the number of packets from the VMW message remainder.
  //
  // NOTE: The "num_packets" 16-bit unsigned integer is stored in network order
  //       thus must be converted to host order in order to access the value.
  //------------------------------------------------------------------------
  const uint16_t* num_packets_ptr = nullptr; 

  if (!this->MineInteger( "Number of MPLS packets", num_packets_ptr))
  {
    // The mining failed. Return false.
    return false;
  }

  const uint16_t num_packets = ntohs(*num_packets_ptr);

  //-------------------------------------
  // Attempt to process all the packets.
  //-------------------------------------
  for (uint16_t i = 0; i < num_packets; ++i)
  {
    //------------------------------------------------------
    // Mine an MPLS packet from the remaining VMW message.
    //------------------------------------------------------
    boost::asio::const_buffer mpls_buf = this->MineMPLSPacket();

    // Check for the failure case of a zero length buf.
    if (boost::asio::buffer_size(mpls_buf) == 0)
    {
      return false;
    }

    //---------------------------------------------------------------
    // And push an MPLS packet (buffer) onto the MPLS buffers vector.
    //---------------------------------------------------------------
    this->mpls_buffers_.push_back(mpls_buf);
  }

  //--------------------------------------
  // Check for any extraneous data.
  //--------------------------------------
  if (boost::asio::buffer_size(this->remainder_buf_) != 0)
  {
    if (log::warn::enabled())
    {
      log::warn(fn) 
        << "Ignoring extraneous bytes in received group of MPLS packets.";
    }
  }

  //---------------------------------------------------------------------------
  // Return true to indicate success, which informs the caller that it may now
  // obtain the vector of "mined" MPLS buffers by calling our public
  // TakeMPLSBuffers() method.
  //---------------------------------------------------------------------------
  return true;
}
    

//============================================================================
//============================================================================
const mia::vmw_message_header_t* 
mia::VMWMessageParser::MineVMWMessageHeader()
{
  static constexpr auto fn = "VMWMessageParser::MineVMWMessageHeader()";

  //------------------------------------------------------------------
  // Attempt to mine a pointer to a vmw_message_header.
  //------------------------------------------------------------------
  static constexpr size_t VMW_MESSAGE_HEADER_SIZE =
    sizeof(vmw_message_header_t);

  const boost::asio::const_buffer header_buf = 
    boost::asio::buffer(this->remainder_buf_, VMW_MESSAGE_HEADER_SIZE);

  //----------------------------------------------------------------------------
  // Make sure that there are at least enough bytes for the vmw message header.
  //----------------------------------------------------------------------------
  if (boost::asio::buffer_size(header_buf) != VMW_MESSAGE_HEADER_SIZE)
  {
    if (log::error::enabled())
    {
      log::error(fn)
        << "There is not enough bytes in the VMW Message "
        << "for the VMW Message Header."
        << " Expected ["
        << VMW_MESSAGE_HEADER_SIZE
        << "] bytes; but only ["
        << boost::asio::buffer_size(header_buf)
        << "] bytes remain in the VMW Message.";
    }

    // Return a nullptr since we failed to "mine" anything.
    return nullptr;
  }

  //--------------------------------------------------------------
  // We're successful.
  // Update the remainder buf.
  // Cast the VMW message header pointer from the header buf
  // and return it.
  //--------------------------------------------------------------
  this->remainder_buf_ = this->remainder_buf_ + VMW_MESSAGE_HEADER_SIZE;

  return boost::asio::buffer_cast<const vmw_message_header_t*>(header_buf);
}


//============================================================================
//============================================================================
boost::asio::const_buffer
mia::VMWMessageParser::MineMPLSPacket()
{
  static constexpr auto fn = "VMWMessageParser::MineMPLSPacket()";

  //----------------------------------------------------------------------
  // Mine the mpls packet length from the VMW message buffer.
  //
  // NOTE: The "mpls_packet_length" 16-bit unsigned integer is stored in
  //       network order thus must be converted to host order in order to
  //       access the value.
  //----------------------------------------------------------------------
  const uint16_t* mpls_packet_length_ptr = nullptr; 

  if (!this->MineInteger("MPLS packet length", mpls_packet_length_ptr))
  {
    // Return a zero length buffer on failure.
    return {};
  }

  const uint16_t mpls_packet_length = ntohs(*mpls_packet_length_ptr);

  //--------------------------------------------------------------------
  // Now check that the mpls packet length will cover the combined size
  // of the MPLS header and the ITM header.
  //--------------------------------------------------------------------
  static const size_t MIN_MPLS_PACKET_SIZE =
    sizeof(MsgTypes::MPLSHeader_t) + sizeof(MsgTypes::itm_header_t);

  if (mpls_packet_length < MIN_MPLS_PACKET_SIZE)
  {
    // Guess not.
    if (log::error::enabled())
    {
      log::error(fn)
        << "The mined mpls packet length is too small. "
        << "The minimum MPLS packet size: ["
        << MIN_MPLS_PACKET_SIZE
        << "] bytes; "
        << "The mined mpls packet length is: ["
        << mpls_packet_length 
        << "] bytes. "
        << "Dropping entire VMW Message.";
    }

    // Return zero length buffer on fail.
    return {};
  }

  //--------------------------------------------
  // Make an mpls buf from the remainder buf.
  //--------------------------------------------
  boost::asio::const_buffer mpls_buf = 
    boost::asio::buffer(this->remainder_buf_, mpls_packet_length);    
  
  //--------------------------------------------------------------------------
  // Check that the mpls_buf size is the expected mpls packet length.
  //--------------------------------------------------------------------------
  if (boost::asio::buffer_size(mpls_buf) != mpls_packet_length)
  {
    if (log::error::enabled())
    {
      log::error(fn)
        << "Not enough bytes remain in received message ["
        << boost::asio::buffer_size(mpls_buf)
        << "] bytes; but the mpls_packet_length specifies ["
        << mpls_packet_length
        << "] bytes. "
        << "Dropping entire VMW Message.";
    }

    // Return zero length buffer on fail.
    return {};
  }

  //--------------------------------------------------------------------------
  // Here lets do some sanity checks on the MPLS packet size.
  // A fixed size ITM size must be 48 bytes.
  // A VITM size must land between a defined min and max. 
  // And if these checks fail, we drop this packet and then that will cause
  // us to drop the message.
  //--------------------------------------------------------------------------

  // Mine the ITM (or VITM) header from the mpls packet.
  const auto itm_buf = (mpls_buf + sizeof(MsgTypes::MPLSHeader_t));
  const MsgTypes::itm_header_t* itm_header =
    MsgTypes::itm_header_t::CastFromBuffer(itm_buf);


  // Check for ITM (or VITM).
  if (itm_header->IsVITM())
  {
    // Create some local constants for the VITM/MPLS sanity check.
    static const size_t MIN_MPLS_VITM_SIZE =
      sizeof(MsgTypes::MPLSHeader_t) +
      sizeof(MsgTypes::itm_header_t) +
      Defs::MIN_VITM_PAYLOAD_SIZE;

    static const size_t MAX_MPLS_VITM_SIZE =
      sizeof(MsgTypes::MPLSHeader_t) +
      sizeof(MsgTypes::itm_header_t) +
      Defs::MAX_VITM_PAYLOAD_SIZE;

    // Perform VITM/MPLS sanity check.
    if ((mpls_packet_length < MIN_MPLS_VITM_SIZE) ||
        (mpls_packet_length > MAX_MPLS_VITM_SIZE))
    {
      // Drop the packet.
      if (log::error::enabled())
      {
        log::error(fn)
          << "MPLS packet length for VITM is the incorrect size. "
          << "Expected length between ["
          << MIN_MPLS_VITM_SIZE
          << "] bytes and ["
          << MAX_MPLS_VITM_SIZE
          << "] bytes. "
          << "Actual length of ["
          << mpls_packet_length
          << "] bytes. "
          << "Dropping entire VMW Message.";
      }

      return {};
    }
  }
  else
  {
    // Create a local constant for the ITM/MPLS sanity check.
    static const size_t MPLS_FIXED_ITM_SIZE =
      sizeof(MsgTypes::MPLSHeader_t) +
      sizeof(MsgTypes::itm_header_t) +
      Defs::FIXED_ITM_PAYLOAD_SIZE;

    // Perform ITM/MPLS sanity check.
    if (mpls_packet_length != MPLS_FIXED_ITM_SIZE)
    {
      // Drop the packet.
      if (log::error::enabled())
      {
        log::error(fn)
          << "MPLS packet length for fixed ITM is the incorrect size. "
          << "Expected length of [" << MPLS_FIXED_ITM_SIZE << "] bytes. "
          << "Actual length of [" << mpls_packet_length << "] bytes. "
          << "Dropping entire VMW Message.";
      }

      return {};
    }
  }

  //-------------------------------------------------
  // We're successful.
  // Update the remainder buf.
  // Return a buffer that refers to the mpls packet
  //-------------------------------------------------
  this->remainder_buf_ = this->remainder_buf_ + mpls_packet_length;

  return mpls_buf;
}
