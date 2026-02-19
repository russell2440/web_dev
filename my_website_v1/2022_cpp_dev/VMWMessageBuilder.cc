//==============================================================================
//==============================================================================

#include "VMWMessageBuilder.h"
#include "ConfigItems.h"
#include "util.h"
#include "VMWMessageTypes.h"
#include "VMWUplinkMessage.h"
#include <VMW_Messages.h>
#include "MIADefs.h"
#include <vector>
#include <memory>


//===========================
// Open anonymous namespace
//===========================
namespace
{

  /**
   * Helper function to build and return an instance of the
   * VMWUplinkMessage class.
   *
   * @param destination identifies the CP or the DP as the
   *        message destination.
   *
   * @param mpls_packets represents the individual instances
   *        of the VMWUplinkMPLSPacket class that will be
   *        "batched" together into one VMW message.
   *
   * @return the instance of the VMWUplinkMessage class.
   */
  mia::VMWUplinkMessage
  BuildMessageBytes(
    mia::VMWUplinkMessage::Destination    destination,
    std::vector<mia::VMWUplinkMPLSPacket> mpls_packets)
  {
    // Build up a vector of const_buffers for bytes that
    // will contribute to make up the full message.
    std::vector<boost::asio::const_buffer> buffers;

    //---------------------
    // VMW Message Header
    //---------------------
    mia::vmw_message_header_t vmw_header;

    // For our message ID we're using KBandRecvMsgId.
    // The VMW_Message.h header, from the VMW-T (APIT) library defines it.
    //
    // Convert message_id from host to network long order.
    vmw_header.message_id = htonl(KBandRecvMsgId);

    buffers.push_back(boost::asio::buffer(&vmw_header, sizeof(vmw_header)));

    //-------------------
    // Num MPLS Packets
    //-------------------
    const uint16_t network_order_num_packets = htons(mpls_packets.size());

    buffers.push_back(
      boost::asio::buffer(&network_order_num_packets,
                          sizeof(network_order_num_packets)));

    //----------------------------------------
    // Each of the contributing MPLS Packets
    //----------------------------------------
    for (const auto& packet : mpls_packets)
    {
      buffers.push_back(boost::asio::buffer(packet.GetBytes()));
    }

    //----------------------------------------------------------
    // Convert message_length from host to network long order.
    //----------------------------------------------------------
    vmw_header.message_length = htonl(boost::asio::buffer_size(buffers));

    //------------------------------------------------------------
    // Now we can create and return the VMWUplinkMessage object.
    //------------------------------------------------------------
    return mia::VMWUplinkMessage::Create(destination, buffers);
  }

}
//===========================
// Close anonymous namespace
//===========================


//==============================================================================
//==============================================================================
mia::VMWMessageBuilder::VMWMessageBuilder(
    drop_pkt::ToSvPolicies_t& to_sv_drop_policies,
    WriteQueue_t&             write_queue)
: to_sv_drop_policies_(to_sv_drop_policies)
, write_queue_(write_queue)
, pending_control_packets_()
, pending_control_packets_size_(0)
, pending_data_packets_()
, pending_data_packets_size_(0)
{
}


//==============================================================================
//==============================================================================
void
mia::VMWMessageBuilder::add_packet(VMWUplinkMPLSPacket mpls_packet)
{
  //------------------------------------------------------------------------
  // Determine the destination of this packet from it's payload type.
  // Packets with Mission data payload types will be sent to the VMW data
  // plane and other packets will be sent to the VMW control plane.
  //------------------------------------------------------------------------
  const uint8_t payload_type = mpls_packet.GetPayloadType();

  if (util::IsMissionDataPayloadType(payload_type))
  {
    //------------------------------------------------------------------------
    // Build and enqueue for sending a VMW Uplink Message if pending packets
    // has no vacancy.
    // Add the incoming mpls packet to pending packets.
    //------------------------------------------------------------------------
    this->add_packet(
      this->pending_data_packets_,
      this->pending_data_packets_size_,
      std::move(mpls_packet),
      VMWUplinkMessage::Destination::DP);
  }
  else
  {
    //---------------------------------------------------------------------
    // Apply our (TOSV) Drop Packet Policy for the packet's payload type,
    // which will return true if the packet should be dropped, and false
    // if the packet should be sent (to the VMW interface).  If the
    // decision is to drop the packet, then simply return.  Otherwise,
    // when the decision is to not drop the packet, proceed in the logic
    // as normal.
    //---------------------------------------------------------------------
    if (this->to_sv_drop_policies_.apply(payload_type))
    {
      return;
    }

    //------------------------------------------------------------------------
    // Build and enqueue for sending a VMW Uplink Message if pending packets
    // has no vacancy.
    // Add the incoming mpls packet to pending packets.
    //------------------------------------------------------------------------
    this->add_packet(
      this->pending_control_packets_,
      this->pending_control_packets_size_,
      std::move(mpls_packet),
      VMWUplinkMessage::Destination::CP);
  }

  // The VMW_COMMON_MAX_PACKETS_PER_TIMESLOT is being used to identify a
  // threshold number of "pending (to be sent)" MPLS Packets that will cause
  // the WriteIfReady() method to package the pending MPLS packets into a
  // VMW Message (containing a "batch" of MPLS Packets) that will then be
  // enqueued to be sent.
  this->WriteIfReady(VMW_Packet::VMW_COMMON_MAX_PACKETS_PER_TIMESLOT);
} 


//==============================================================================
//==============================================================================
void
mia::VMWMessageBuilder::add_packet(
  std::vector<VMWUplinkMPLSPacket>& pending_packets,
  size_t& pending_packets_size,
  VMWUplinkMPLSPacket mpls_packet,
  VMWUplinkMessage::Destination destination
  )
{
  //--------------------------------------------------------------------------
  // First check that the pending packets has vacancy for the MPLS packet.
  // If not then build up a VMW Uplink Message from the pending packets
  // (thus emptying the pending packets).  And then enqueue to send it.
  // Then insert the packet onto the pending packets.
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  // A constant for the maximum allowed data size in VMWUplinkMessage.
  //
  // In the existing "ITM only" version of the MIA, we allow up to 720
  // ITM/MPLS packets. Each ITM/MPLS packet also has a 2-byte value preceding
  // it that designates its length.
  //
  // So, given that an ITM is 48 bytes and the MPLS Header is 4 bytes, we get
  // 54 bytes (48 + 4 + 2) for each ITM/MPLS packet, and can support up to
  // 720 of them. That comes out to be 720 * 54 = 38880. That's the number of
  // bytes that could follow the VMW Message header (8 bytes) and the "number
  // of MPLS packets" (2 bytes) in a max-sized VMW Message.
  //
  // So, we will increment the pending_packets_size by the ITM/MPLS or
  // VITM/MPLS bytes size plus 2 (for the "num bytes" preceding the MPLS
  // Header), then we compare that against 38880 as our "limit".
  //
  // Note: We really allow for 38890 in the max-sized VMW Message, because
  // of the 8 bytes for the VMW Message header and 2 bytes for the "number of
  // MPLS packets", as mentioned earlier.
  //--------------------------------------------------------------------------
  static const size_t MAX_MPLS_BATCH_SIZE = 38880;

  // Get the MPLS packet bytes size.  We will be needing it.
  const size_t mpls_packet_bytes_size = mpls_packet.GetBytes().size();

  //--------------------------------------------------------------------------
  // Within the VMW Message containing a "batch" of MPLS Packets, the bytes
  // for each MPLS Packet is preceded by two bytes that represent a uint16_t
  // value which identifies the number of bytes for the MPLS Packet that
  // follows (the uint16_t value).
  //
  // The MPLS_NUM_BYTES_SIZE is a constant used to account for the number of
  // bytes in the uint16_t value (which is 2 bytes) that precedes each MPLS
  // Packet within the "batch" of MPLS Packets.
  //--------------------------------------------------------------------------
  static constexpr size_t MPLS_NUM_BYTES_SIZE = sizeof(uint16_t);

  // A helper for checking that adding the MPLS Packet (and num_bytes) would
  // overflow the vector of pending packets.  If so build and queue up the
  // VMW message.
  const bool must_build_message = 
    ((pending_packets_size + mpls_packet_bytes_size + MPLS_NUM_BYTES_SIZE ) >
      MAX_MPLS_BATCH_SIZE);

  // Now check for need to build a message.
  if (must_build_message)
  {
    // No vacancy for the packet. So build and enque the message to send it.
    //
    // As a result of the move operation below, the pending_packets
    // will be modified to be an empty vector.
    this->write_queue_.push_back(
      ::BuildMessageBytes(destination, std::move(pending_packets)));

    // Zero out the pending packets accumulator.
    pending_packets_size = 0;
  }

  //--------------------------------------------------------------------------
  // Increment the pending packets accumulator with the MPLS packet size
  // plus MPLS_NUM_BYTES_SIZE.
  //--------------------------------------------------------------------------
  pending_packets_size += (mpls_packet_bytes_size + MPLS_NUM_BYTES_SIZE);

  // Push the MPLS packet onto the pending packets vector.
  pending_packets.push_back(std::move(mpls_packet));
}


//==============================================================================
//==============================================================================
void
mia::VMWMessageBuilder::finalize()
{
  // The THRESHOLD is being used to identify the minimum number of
  // "pending (to be sent)" MPLS Packets that will cause the WriteIfReady()
  // method to package the pending MPLS packets into a VMW Message
  // (containing a "batch" of MPLS Packets) that will then be enqueued
  // to be sent.
  static constexpr size_t THRESHOLD = 1;
  this->WriteIfReady(THRESHOLD);
}


//==============================================================================
//==============================================================================
void
mia::VMWMessageBuilder::WriteIfReady(size_t threshold)
{
  //--------------------------------------------------------------------------
  // Check for pending data or control packet groups having reached a maximum
  // threshold.  And then build and queue the messages for sending.
  //--------------------------------------------------------------------------

  //--------------------------
  // Pending Control Packets
  //--------------------------
  if (this->pending_control_packets_.size() >= threshold)
  {
    // As a result of the move operation, the pending_control_packets_ will
    // be modified to be an empty vector.
    this->write_queue_.push_back(
      ::BuildMessageBytes(
          VMWUplinkMessage::Destination::CP,
          std::move(this->pending_control_packets_)));
  }

  //-----------------------
  // Pending Data Packets
  //-----------------------
  if (this->pending_data_packets_.size() >= threshold)
  {
    // As a result of the move operation, the pending_data_packets_ will
    // be modified to be an empty vector.
    this->write_queue_.push_back(
      ::BuildMessageBytes(
          VMWUplinkMessage::Destination::DP,
          std::move(this->pending_data_packets_)));
  }
}
