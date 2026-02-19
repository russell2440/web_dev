#pragma once

//==============================================================================
//==============================================================================

/** 
 * @file:   VMWMessageTypes.h
 * @author: Russell Shahenian
 * @since:  6/17/22
 * @brief   This file contains the definitions of types used by the
 *          VMWInterface class and/or its supporting classes.
 */

#include <boost/asio.hpp>
#include <vector>


namespace mia
{

  /**
   * The VMWUplinkMPLSPacket class is used by the VMWInterface class and/or
   * its supporting classes and represents the bytes for a single MPLS/ITM
   * or MPLS/VITM packet.
   *
   * This will be used to build an uplink VMW Message representing
   * a "batch" of MPLS Packets.  The bytes stored in a VMWUplinkMPLSPacket
   * object should start with a two-byte (network-ordered uint16_t) value
   * for the number of bytes in the MPLS/ITM or MPLS/VITM.  The four bytes
   * representing the MPLS Header will be next, followed by the bytes for
   * the ITM (or VITM).
   */
  class VMWUplinkMPLSPacket
  {
  public:

    /**
     * Static factory method to create an VMWUplinkMPLSPacket from the bytes
     * referenced by a supplied boost::asio::const_buffer object.
     *
     * @param payload_type is the payload type from the ITM (or VITM) header.
     *
     * @param bytes is a boost::asio::const_buffer object that refers
     *        to the bytes to be copied to the VMWUplinkMPLSPacket object
     *        being created.
     */
    static
    VMWUplinkMPLSPacket
    Create(uint8_t payload_type, const boost::asio::const_buffer& bytes)
    {
      // Need to supply the const_buffer to the buffer() function to
      // get a (copy) of the const_buffer, but returned as a const_buffers_1
      // (subclass of const_buffer) that fulfills all of the requirements
      // to be used as a "Const Buffer Sequence" (i.e., where, in this case,
      // the length of the sequence is always just the one const_buffer).
      return VMWUplinkMPLSPacket(payload_type, boost::asio::buffer(bytes));
    }


    /**
     * Static factory method to create an VMWUplinkMPLSPacket from the bytes
     * referenced by a supplied "Const Buffer Sequence" object.
     *
     * @param payload_type is the payload type from the ITM (or VITM) header.
     *
     * @param bytes is a const reference to an object that represents
     *        a "Const Buffer Sequence" referring to the bytes to be
     *        copied to the VMWUplinkMPLSPacket object being created.
     */
    template <typename CONST_BUF_SEQ_T>
    static
    VMWUplinkMPLSPacket
    Create(uint8_t payload_type, const CONST_BUF_SEQ_T& bytes)
    {
      // All other types will cause this templated version to be used
      // and, in this case, we assume that the CONST_BUF_SEQ_T meets
      // all of the requirements to be used as a "Const Buffer Sequence".
      //
      // If it isn't such a type, then the constructor will fail to compile.
      return VMWUplinkMPLSPacket(payload_type, bytes);
    }


    /**
     * Move constructor.
     *
     * Use the default compiler-generated definition for the move constructor.
     *
     * @note Copy construction and copy assignment is not allowed.
     */
    VMWUplinkMPLSPacket(VMWUplinkMPLSPacket&&) = default;


    /**
     * Get a const reference to the vector of bytes held by the
     * VMWUplinkMPLSPacket object.
     *
     * @return a const reference to the vector of underlying bytes.
     */
    const std::vector<uint8_t>&
    GetBytes() const
    {
      return this->bytes_;
    }


    /**
     * Get the payload type of the ITM (or VITM).
     *
     * @return the payload type.
     */
    uint8_t
    GetPayloadType() const
    {
      return this->payload_type_;
    }


  private:

    /**
     * Private (template) constructor.
     *
     * The CONST_BUF_SEQ_T type must meet the requirements for the
     * conceptual "Const Buffer Sequence" type.  See boost::asio docs
     * for more info.  This will not compile unless this is the case.
     *
     * @param payload_type is the payload type from the ITM (or VITM) header.
     *
     * @param bytes is a const reference to an object that represents
     *        a "Const Buffer Sequence" referring to the bytes to be
     *        copied to the VMWUplinkMPLSPacket object being constructed.
     */
    template <typename CONST_BUF_SEQ_T>
    VMWUplinkMPLSPacket(uint8_t payload_type, const CONST_BUF_SEQ_T& bytes)
    : payload_type_(payload_type)
    , bytes_(boost::asio::buffer_size(bytes))
    {
      boost::asio::buffer_copy(boost::asio::buffer(this->bytes_), bytes);
    }


    //=========================================================
    /// Disable Copy Constructor and Copy Assignment operator.
    ///
    /// @note Move construction is allowed.
    //=========================================================
    VMWUplinkMPLSPacket(const VMWUplinkMPLSPacket&) = delete;
    VMWUplinkMPLSPacket& operator=(const VMWUplinkMPLSPacket&) = delete;


    //-----------------------------------------
    // The payload type of the ITM (or VITM).
    //-----------------------------------------
    uint8_t payload_type_;

    //-------------------------------------------------------------------------
    // The bytes that represent the 2-byte "size" of the MPLS/ITM or MPLS/VITM
    // data, followed by the bytes that represent the MPLS/ITM or MPLS/VITM.
    //-------------------------------------------------------------------------
    std::vector<uint8_t> bytes_;
  };


  /**
   * A struct that specifies the VMW message Header.
   */
  struct vmw_message_header_t
  {
    uint32_t message_id;
    uint32_t message_length;
  }__attribute__((packed,aligned(1)));

}
