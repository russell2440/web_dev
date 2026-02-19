#pragma once

//==============================================================================
//==============================================================================

/** 
 * @file:   VMWMessageParser.h
 * @author: Russell Shahenian
 * @since:  5/03/22
 * @brief   This file contains the declaration of the VMWMessageParser class. 
 *          This class contains an algorithm for parsing VMW Messages.
 */

#include "VMWMessageTypes.h"
#include "log.h"
#include <boost/asio.hpp>
#include <vector>


namespace mia
{

  class VMWMessageParser
  {
  public:
    
    /**
     * This function is used to perform the processing of a VMW Message
     * received by the VMWInterface.
     * 
     * The VMW Message represents a "group" of individual MPLS Packets.  Each
     * MPLS Packet is an MPLS Header followed by an ITM (or vITM).
     *
     * When successful, this method will return a non-empty vector of
     * const_buffer objects.  Each const_buffer object refers to an individual
     * MPLS packet represented by bytes within the supplied VMW message.
     *
     * Upon failure, appropriate warning/error messages will be logged, and an
     * empty vector will be returned to the caller.
     *
     * @param vmw_message_buf is the VMW message received by the VMWInterface.  
     *
     * @return A vector of const_buffer objects.  Each const_buffer object
     *         refers an individual MPLS packet (in the supplied VMW Message). 
     *
     * @note The returned vector will be empty if there were no MPLS packets
     *       within the VMW Message (which is unexpected) or if there was a
     *       problem encountered while trying to process the VMW Message.  If
     *       an empty vector is returned, an appropriate warn or error level
     *       message will be logged, and so the caller doesn't neeed to.
     */
    static
    std::vector<boost::asio::const_buffer>
    Perform(const boost::asio::const_buffer& vmw_message_buf);


  private:

    /**
     * Private constructor - only used by the public (and static) Perform()
     * method.
     *
     * @param vmw_message_buf refers to the message bytes recieved by the
     *        VMWInterface that need to be parsed/processed by the
     *        VMWMessageParser object.
     */
    VMWMessageParser(const boost::asio::const_buffer& vmw_message_buf);
  
    /**
     * This method is called by the public (static) Perform() method
     * shortly after construction of the VMWMessageParser object.
     *
     * It is responsible for processing the VMW Message bytes that
     * were supplied to this VMWMessageParser object upon its
     * construction.
     *
     * If any problem is encountered, then an appropriate error message will
     * be logged and false will be returned to the caller.
     *
     * Otherwise, each MPLS Packet within the VMW Message bytes will be
     * placed into a vector of const_buffer objects (a data member) and true
     * will be returned to the caller.
     *
     * The caller (the public Perform() method) will, upon success, invoke our
     * TakeMPLSBuffers() method in order to retrieve the vector of const_buffers
     * that were "mined" from the VMW Message bytes.
     *
     * @return true for success and false for failure.
     */
    bool Process();
    
    /**
     * This method will return the "group" of MPLS packets mined from the VMW
     * Message.
     *
     * @return The "group" of mined MPLS packets as a vector of buffers.
     *
     * @note The caller is "taking" the MPLS buffers from the VMWMessageParser
     *       object.  When this method returns, the VMWMessageParser object
     *       will no longer hold any MPLS buffers (as they have been "taken" by
     *       the caller).
     */
    std::vector<boost::asio::const_buffer> TakeMPLSBuffers();
  
    /**
     * This method will mine a VMW message header from the remaining
     * "unprocessed" bytes of the VMW message.
     *
     * @return A non-null pointer to the VMW Message Header upon success.
     *         A null pointer upon failure.
     */
    const vmw_message_header_t* MineVMWMessageHeader();
    
    /**
     * This method will mine an mpls packet from the remaining "unprocessed"
     * bytes of the VMW message.
     *
     * @return the mined MPLS packet.
     *
     * @note the returned const_buffer will refer to zero bytes if a problem is
     *       encountered while attempting to mine the MPLS Packet bytes from
     *       the remaining, unprocessed bytes of the VMW message.
     *       Prior to returning the zero-sized const_buffer on failure, a
     *       warn/error level message will be logged explaining the problem
     *       that was encountered.
     */
    boost::asio::const_buffer MineMPLSPacket();
    
    /**
     * This template function will mine an integer from the remaining
     * "unprocessed" bytes of the VMW message.
     * 
     * It will update the supplied reference to an INTEGER_T pointer to point
     * at the "mined" integer.
     *
     * @param field_name is the name of the field that is being "mined".
     *
     * @param is a reference to an INTEGER_T pointer supplied by the caller to
     *        point at at "mined" integer.
     *
     * @return true for success, false for failure.
     */
    template <typename INTEGER_T>
    bool
    MineInteger(
      const std::string& field_name, 
      const INTEGER_T*& mined_ptr)
    {
      static constexpr auto fn = "VMWMessgeParser::MineInteger()";

      static constexpr size_t INTEGER_SIZE = sizeof(INTEGER_T);

      const boost::asio::const_buffer int_buf =
        boost::asio::buffer(this->remainder_buf_, INTEGER_SIZE);

      if (boost::asio::buffer_size(int_buf) != INTEGER_SIZE)
      {
        if (log::error::enabled())
        {
          log::error(fn)
            << "Not enough bytes in VMW Message for "
            << field_name
            << " field.  Expected ["
            << INTEGER_SIZE
            << "] bytes, but only ["
            << boost::asio::buffer_size(int_buf)
            << "] bytes remain in the VMW Message.";
        }

        return false;
      }

      //----------------------------------------------------
      // Now we can update the remainder_buf and mined_ptr.
      // Then return true for success.
      //----------------------------------------------------
      this->remainder_buf_ = this->remainder_buf_ + INTEGER_SIZE;

      mined_ptr = boost::asio::buffer_cast<const INTEGER_T*>(int_buf);

      return true;
    }


    //-----------------------------------------------
    /// Original, unadulterated VMW Message Buffer.
    //-----------------------------------------------
    const boost::asio::const_buffer vmw_message_buf_;

    //------------------------------------------------------------------
    // The remainder of unprocessed bytes from the VMW Message Buffer.
    //------------------------------------------------------------------
    boost::asio::const_buffer remainder_buf_;

    //--------------------------------------------------------------------
    /// This is a vector of const_buffer objects that is parsed from the 
    /// VMW Downlink Message.
    /// Each object represents an ITM/MPLS or VITM/MPLS packet.
    //--------------------------------------------------------------------
    std::vector<boost::asio::const_buffer> mpls_buffers_;
  };
}
