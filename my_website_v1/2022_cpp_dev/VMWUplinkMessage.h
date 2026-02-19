#pragma once

//===========================================================================
//===========================================================================

/**
 * @file    VMWUplinkMessage.h
 * @author  
 * @since   2022/06/16
 * @brief   This file contains the declaration of VMWUplinkMessage used by the
 *          VMWInterface class and its supporting code.
 */

#include <boost/asio.hpp>
#include <vector>


namespace mia
{

  /**
   * Each instance of the mia::VMWUplinkMessage class represents a
   * "VMW Message" destined to be sent to VMW-A on either the CP or the DP.
   */
  class VMWUplinkMessage
  {
  public:

    /**
     * Enumeration class for choosing the destination of the VMWUplinkMessage
     * as being VMW-A running on the CP or DP.
     */
    enum class Destination
    {
      CP,
      DP
    };


    /**
     * Static factory method to create an VMWUplinkMessage from the bytes
     * referenced by a supplied boost::asio::const_buffer object.
     *
     * @param destination is either SBCRole::CP or SBCRole::DP and
     *        identifies the destination SBC for the VMWUplinkMessage.
     *
     * @param bytes is a boost::asio::const_buffer object that refers
     *        to the bytes to be copied to the VMWUplinkMessage object
     *        being created.
     */
    static
    VMWUplinkMessage
    Create(Destination destination, const boost::asio::const_buffer& bytes)
    {
      // Need to supply the const_buffer to the buffer() function to
      // get a (copy) of the const_buffer, but returned as a const_buffers_1
      // (subclass of const_buffer) that fulfills all of the requirements
      // to be used as a "Const Buffer Sequence" (i.e., where, in this case,
      // the length of the sequence is always just the one const_buffer).
      return VMWUplinkMessage(destination, boost::asio::buffer(bytes));
    }


    /**
     * Static factory method to create an VMWUplinkMessage from the bytes
     * referenced by a supplied "Const Buffer Sequence" object.
     *
     * @param destination is either SBCRole::CP or SBCRole::DP and
     *        identifies the destination SBC for the VMWUplinkMessage.
     *
     * @param bytes is a const reference to an object that represents
     *        a "Const Buffer Sequence" referring to the bytes to be
     *        copied to the VMWUplinkMessage object being created.
     */
    template <typename CONST_BUF_SEQ_T>
    static
    VMWUplinkMessage
    Create(Destination destination, const CONST_BUF_SEQ_T& bytes)
    {
      return VMWUplinkMessage(destination, bytes);
    }


    /**
     * Move constructor.
     *
     * Use the default compiler-generated definition for the move constructor.
     *
     * @note Copy construction and copy assignment is not allowed.
     */
    VMWUplinkMessage(VMWUplinkMessage&&) = default;


    /**
     * Get a const reference to the vector of bytes held by the
     * VMWUplinkMessage object.
     *
     * @return a const reference to the vector of underlying message bytes.
     */
    const std::vector<uint8_t>&
    GetBytes() const
    {
      return this->bytes_;
    }


    /**
     * Determine if the VMWUplinkMessage is destined for VMW-A running
     * on the CP (true) or DP (false).
     *
     * @returns true if the VMWUplinkMessage is destined for VMW-A running
     *          on the CP, and false if destined for the DP.
     */
    bool
    IsDestinedForCP() const
    {
      return this->is_destined_for_cp_;
    }


  private:

    /**
     * Private (template) constructor.
     *
     * The CONST_BUF_SEQ_T type must meet the requirements for the
     * conceptual "Const Buffer Sequence" type.  See boost::asio docs
     * for more info.  This will not compile unless this is the case.
     *
     * @param destination is either SBCRole::CP or SBCRole::DP and
     *        identifies the destination SBC for the VMWUplinkMessage.
     *
     * @param bytes is a const reference to an object that represents
     *        a "Const Buffer Sequence" of the "message bytes" that
     *        will be copied to the VMWUplinkMessage object.
     */
    template <typename CONST_BUF_SEQ_T>
    VMWUplinkMessage(Destination destination, const CONST_BUF_SEQ_T& bytes)
    : is_destined_for_cp_(Destination::CP == destination)
    , bytes_(boost::asio::buffer_size(bytes))
    {
      boost::asio::buffer_copy(boost::asio::buffer(this->bytes_), bytes);
    }


    //=========================================================
    /// Disable Copy Constructor and Copy Assignment operator.
    ///
    /// @note Move construction is allowed.
    //=========================================================
    VMWUplinkMessage(const VMWUplinkMessage&) = delete;
    VMWUplinkMessage& operator=(const VMWUplinkMessage&) = delete;


    //------------------------------------------------------------------------
    /// Boolean flag that identifies the destination of the VMWUplinkMessage
    /// as VMW-A on the CP when true, and on the DP when false.
    //------------------------------------------------------------------------
    bool is_destined_for_cp_;

    //-------------------------------------------------------------
    // The vector of bytes that represent the VMW Uplink Message.
    //-------------------------------------------------------------
    std::vector<uint8_t> bytes_;
  };

}

