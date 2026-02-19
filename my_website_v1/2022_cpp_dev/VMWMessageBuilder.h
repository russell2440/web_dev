#pragma once

//==============================================================================
//==============================================================================

/** 
 * @file:   VMWMessageBuilder.h
 * @author: Russell Shahenian
 * @since:  5/06/22
 * @brief   This file contains the declaration of the VMWMessageBuilder class. 
 *          This a class for building VMW Messages and queue them for sending
 *          to the Control or Data planes.
 */
#include "VMWMessageTypes.h"
#include "DropPacketPolicy.h"
#include "MsgTypes.h"
#include "VMWUplinkMessage.h"
#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <deque>


namespace mia
{
  using WriteQueue_t = std::deque<VMWUplinkMessage>;

  /**
   * This object helps the VMWInterface object.  It helps to build and enqueue
   * messages to be sent to VMW on either the Control Plane (CP) or
   * Data Plane (DP). 
   *
   * It is responsible for taking in ITM messages.  And then building out any
   * "full" or "partial" VMW messages.
   *
   * The builder is meant to be instantiated on each k-frame tick to process
   * any k-frame delayed ITM packets that become ready on the current k-frame
   * tick.
   */
  class VMWMessageBuilder
  {
  public:

    /**
     * The Constructor.
     *
     * @param to_sv_drop_policies is used as a simulation tool for dropping
     *        packets.
     *
     * @param write_queue is used to queue up built messeges for sending out
     *        the "write" socket.
     */
    VMWMessageBuilder(
      drop_pkt::ToSvPolicies_t& to_sv_drop_policies,
      WriteQueue_t&             write_queue);

    /**
     * This method is responsible for adding an individual VMWUplinkMPLSPacket
     * object to the builder.  This can trigger the a VMW Message containing a
     * "batch" of MPLS Packets to be built and enqueued to be sent to either
     * the CP or DP.
     *
     * @param mpls_packet is the packet to be added.
     */
    void add_packet(VMWUplinkMPLSPacket mpls_packet);

    /**
     * This method is responsible for building any "partial" VMW messages and
     * queuing them for being sent to the CP or DP.
     */
    void finalize();


  private:

    /**
     * This method is used to add a supplied VMWUplinkMPLSPacket object
     * to a supplied vector of "pending" VMWUplinkMPLSPacket objects.
     *
     * However, before performing the operation to push the supplied packet
     * object on to the vector, this method will potentially build a VMW uplink
     * message from the vector of pending packets, and enqueue it to be sent
     * to the supplied destination (CP or DP).  If this occurs, the vector of
     * pending packets will be made to be empty as a result (and the
     * supplied pending_packets_size value will be reset to be 0).
     *
     * The catalyst for building and enqueueing the VMW uplink message
     * is when the supplied pending_packets_size plus the size of the
     * supplied uplink MPLS packet exceeds the limit for the number
     * of bytes allowed for a single VMW uplink message.
     *
     * Once the supplied uplink packet is pushed to the back of the
     * supplied vector of pending packets, the supplied
     * pending_packets_size will be incremented by the size of
     * the supplied uplink packet.
     *
     * @param pending_packets is a reference to the vector of pending packets
     *        to which the supplied VMWUplinkMPLSPacket object will be added.
     *
     * @param pending_packets_size is a reference to the current number of
     *        bytes represented by all of the uplink MPLS packets in the
     *        supplied vector of pending packets.
     *
     * @param mpls_packet is the VMWUplinkMPLSPacket object that is to
     *        be added to the supplied vector of pending_packets.
     *
     * @param destination is for the vector of pending packets, identifying
     *        either the CP or DP as the destination of the VMW uplink message
     *        if it happens to get built and enqueued to be sent.
     **/
    void add_packet(
      std::vector<VMWUplinkMPLSPacket>& pending_packets,
      size_t& pending_packets_size,
      VMWUplinkMPLSPacket mpls_packet,
      VMWUplinkMessage::Destination destination);

    /**
     * This method is responsible packaging up pending MPLS packets up into a
     * VMW Uplink Message and enquing it to be sent.
     *
     * @param threshold  is the number of pending MPLS packets must be reached
     *        in order to be packaged up into a VMW Uplink Message.
     */
    void WriteIfReady(size_t threshold);


    //=======================
    // Private Data Members
    //=======================

    //------------------------------------------------------------------------
    /// The non-const reference to the TOSV drop packet policies object,
    /// supplied to the constructor.
    //------------------------------------------------------------------------
    drop_pkt::ToSvPolicies_t& to_sv_drop_policies_;

    //---------------------------------------------------------------------
    /// The write_queue_ contains VMWUplinkMessage objects that are to be
    /// sent to the socket (as soon possible).
    //---------------------------------------------------------------------
    WriteQueue_t& write_queue_;

    //------------------------------------------------------------------------
    // The vector used to hold pending VMW MPLS packets destined for the CP.
    //------------------------------------------------------------------------
    std::vector<VMWUplinkMPLSPacket> pending_control_packets_;

    //------------------------------------------------------------------------
    // An accumlator to track the pending control packets size.
    //------------------------------------------------------------------------
    size_t pending_control_packets_size_;

    //------------------------------------------------------------------------
    // The vector used to hold pending VMW MPLS packets destined for the DP.
    //------------------------------------------------------------------------
    std::vector<VMWUplinkMPLSPacket> pending_data_packets_;

    //------------------------------------------------------------------------
    // An accumlator to track the pending data packets size.
    //------------------------------------------------------------------------
    size_t pending_data_packets_size_;
  };
}
