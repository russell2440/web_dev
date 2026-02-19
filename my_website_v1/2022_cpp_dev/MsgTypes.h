#pragma once

//============================================================================
//============================================================================

/** 
 * Single location for types and values
 * used by the MIA.
 * 
 *  @author: 
 *  @since: 8/11/15
 */

#include <boost/asio.hpp>
#include <inf/util/UnionPair.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <SimulationCore/ITM.hh>
#include <cstdint>
#include <limits>
#include <unordered_set>
#include <vector>
#include <sim-core/SimulationCore/CommonStructs.hh>


namespace mia {
namespace MsgTypes {

  //-------------------------------------------------------------
  // These 2 namespaces and enumerated types have been added to
  // improve readability regarding the meaning of the
  // data/control bit of the ITE header and the repurposed
  // congestion indicator bit of the ITM header.
//-------------------------------------------------------------
  namespace ITE_DC_Bit
  {
    enum
    {
      CONTROL_MSG = 0,
      DATA_MSG    = 1
    };
  }

  namespace ITM_CI_Bit
  {
    enum
    {
      ITM_MSG     = 0,
      VITM_MSG    = 1
    };
  }

  enum
  {
    //--------------------------------------------------------------------------
    // The MAX_IP_PACKET_SIZE is used to pre-size arrays (or vectors) of bytes
    // that will be used by the MIA Interface objects to store received
    // network packet bytes.
    //--------------------------------------------------------------------------
    MAX_IP_PACKET_SIZE = std::numeric_limits<uint16_t>::max(),
    
    // Value indicating the data encapsulated in the IP packet is UDP.
    UDP_PROTOCOL = 17,
    
    // Value indicating the data encapsulated in the IP packet is MPLS/IP.
    MPLS_IN_IP_PROTOCOL = 137,
    
    // Value used for XOR operation used to calculate the ITM header checksum.
    PARITY_P3P4_FLIP = 0x18,
    
    // Port used for the source and destination addresses in the UDP header.
    ITME_PORT = 50000
 };


  /**
   * The itm_header_common_base_t struct serves as a "mix-in" base type for each
   * of the concrete ITM Header structs (other than the "raw" variant).
   *
   * The itm_header_common_base_t class represents the first 2-bytes of
   * a 5-byte ITM Header.
   *
   * The term "mix-in" indicates that this is not to be used polymorphically
   * (i.e., no virtual methods including no virtual destructor).  Instead,
   * it is for code reuse purposes such that a concrete ITM Header struct
   * can derive from this itm_header_common_base_t struct to "mix-in" (inherit)
   * its data and methods.
   *
   * The key point here is...  
   *
   * If there is anything "virtual", then the data structure will be "bloated"
   * with a virtual table and the sizeof() the ITM header types will be larger
   * than 5.
   *
   * Nobody outside of this code should care that there is a common base class
   * for each of these ITM header types - just that they all magically have
   * the data members and methods defined by the base class. 
   */
  struct itm_header_common_base_t
  {
    //==============================================
    // Data Members (first 2 bytes of ITM Header).
    //==============================================

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      uint8_t ci  :  1;    // congestion indicator
      uint8_t plt :  3;    // payload type 
      uint8_t hpc :  4;    // hop count
#else
      uint8_t hpc :  4;    // hop count
      uint8_t plt :  3;    // payload type 
      uint8_t ci  :  1;    // congestion indicator
#endif

      uint8_t dstnid;      // destination node id


    //==========
    // Methods
    //==========

    /**
     * This method determines if this is a VITM packet.
     *
     * @return true for this being a VITM, and false otherwise.
     */
    inline bool IsVITM() const
    {
      return (this->ci == ITM_CI_Bit::VITM_MSG);
    }


    /**
     * This method determines if this is a Fixed size ITM packet.
     *
     * @return true for this being a Fixed site ITM, and false otherwise.
     */
    inline bool IsFixedSizeITM() const
    {
      return !this->IsVITM();
    }


    /**
     * This method gets the destination node ID value from the packet.
     *
     * @return the destination node id value.
     */
    inline uint8_t GetDestinationId() const
    {
      return this->dstnid;
    }


    /**
     * This method gets the payload type value from the packet.
     *
     * @return the payload type value.
     */
    inline uint8_t GetPayloadType() const
    {
      return this->plt;
    }


    /**
     * This method is used to determine if the payload type of the ITM
     * (or VITM) is considered to be a "mission-data" payload type (or not).
     *
     * @return true if it is a mission-data payload type, and false otherwise.
     */
    inline bool IsMissionDataPayloadType() const
    {
      using PayloadType = Simulation::ITM::PayloadType;

      static const std::unordered_set<uint8_t> MISSION_PAYLOAD_TYPES =
      {
        PayloadType::MDV,      // (0) MISSION_DATA_VOICE
        PayloadType::MDD,      // (1) MISSION_DATA_DATA
        PayloadType::MCD_ISU,  // (3) MISSION_CONTROL_DATA
      };

      return (MISSION_PAYLOAD_TYPES.count(this->plt) > 0);
    }


  protected:

    /**
     * Protected destructor (no polymorphic destruction allowed).
     * 
     * In other words, by making it protected, the compiler won't allow any
     * code to destruct the base type instance unless the base type destructor
     * is being invoked by a subclass' destructor.
     *
     * The "= default" is stating that we want the destructor to be generated
     * by the compiler the same way it would have if we hadn't declared a
     * destructor.
     */
    ~itm_header_common_base_t() = default;
  }__attribute__((packed,aligned(1)));


  /**
   * ITM Header for payload types 0, 1 and 3 (Mission Data Payload Types).
   *
   * The first 2-bytes of this 5-byte ITM Header type are defined by the
   * itm_header_common_base_t struct used as a mix-in base type.
   *
   * As such, this struct defines the remaining 3-bytes for this
   * specific 5-byte ITM Header type.
   */
  struct itm_header1_t : public itm_header_common_base_t
  {
    //=====================================================
    // Data Members covering final 3 bytes of ITM Header.
    //=====================================================

    uint8_t lcn1;        // LCN byte 1
    uint8_t lcn2;        // LCN byte 2
    uint8_t hdrcsum;     // ITM header checksum
  }__attribute__((packed,aligned(1)));


  /**
   * ITM Header for payload types 0, 1 and 3 (Mission Data Payload Types)
   * received from the GW-Sim (i.e. via the MIA's MDInterface object).
   *
   * The first 2-bytes of this 5-byte ITM Header type are defined by the
   * itm_header_common_base_t struct used as a mix-in base type.
   *
   * As such, this struct defines the remaining 3-bytes for this
   * specific 5-byte ITM Header type.
   */
  struct itm_header1_alt_t : public itm_header_common_base_t
  {
    //=====================================================
    // Data Members covering final 3 bytes of ITM Header.
    //=====================================================

    uint8_t lcn1;        // LCN byte 1
    uint8_t lcn2;        // LCN byte 2
    uint8_t sourceNode;  // Source Node ID
  }__attribute__((packed,aligned(1)));


  /**
   * ITM Header for payload types 2, 4, 6 and 7.
   *
   * The first 2-bytes of this 5-byte ITM Header type are defined by the
   * itm_header_common_base_t struct used as a mix-in base type.
   *
   * As such, this struct defines the remaining 3-bytes for this
   * specific 5-byte ITM Header type.
   */
  struct itm_header2_t : public itm_header_common_base_t
  {
    //=====================================================
    // Data Members covering final 3 bytes of ITM Header.
    //=====================================================

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint8_t spare : 5;   // spare
    uint8_t sapi  : 3;   // SAPI
#else
    uint8_t sapi  : 3;   // SAPI
    uint8_t spare : 5;   // spare
#endif

    uint8_t snid;        // Source Node ID
    uint8_t hdrcsum;     // ITM header checksum
  }__attribute__((packed,aligned(1)));


  /**
   * ITM Header for payload type 5.
   *
   * The first 2-bytes of this 5-byte ITM Header type are defined by the
   * itm_header_common_base_t struct used as a mix-in base type.
   *
   * As such, this struct defines the remaining 3-bytes for this
   * specific 5-byte ITM Header type.
   */
  struct itm_header3_t : public itm_header_common_base_t
  {
    //=====================================================
    // Data Members covering final 3 bytes of ITM Header.
    //=====================================================

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint8_t parity : 1;  // parity bit
    uint8_t ols    : 3;  // Online SVARC
    uint8_t swv    : 4;  // Software Version
#else
    uint8_t swv    : 4;  // Software Version
    uint8_t ols    : 3;  // Online SVARC
    uint8_t parity : 1;  // parity bit
#endif

    uint8_t snid;        // Source Node ID
    uint8_t hdrcsum;     // ITM header checksum
  }__attribute__((packed,aligned(1)));


  /**
   * ITM Header structure to access raw bytes.
   *
   * Unlike the other variants of the ITM Header struct definition,
   * this "raw" variant does not derive from the mix-in base struct,
   * itm_header_common_base_t, for the first 2-bytes of the 5-bytes ITM
   * Header.  Instead, this variant defines all 5-bytes as a simple
   * fixed-size array.
   */
  struct itm_header_raw_t
  {
    //----------------------------------------------------
    /// Array containing all 5 bytes for the ITM header.
    //----------------------------------------------------
    uint8_t data[sizeof(itm_header1_t)];
  }__attribute__((packed,aligned(1)));


  /**
   * Union to access itm header by different types.
   *
   * For most use-cases, one should simply invoke the appropriate
   * public method (if available).  Otherwise, one can access the
   * 5-byte ITM Header as any one of the many variants defined
   * within this itm_header_t union type.
   */
  union itm_header_t
  {
    //============================================================
    // Data Members are each variation of the 5-byte ITM header.
    //============================================================

    itm_header1_t     type1;
    itm_header1_alt_t type1_alt;
    itm_header2_t     type2;
    itm_header3_t     type3;
    itm_header_raw_t  raw;


    //==========
    // Methods
    //==========

    /**
     * This static method will return an ITM header pointer that is cast from
     * the buffer argument.
     *
     * @param buf is the buffer the header pointer is cast from.
     * 
     * @return the header pointer, or a nullptr if there aren't enough bytes
     *         referred to by the supplied const_buffer object to account for
     *         the size of the ITM Header.
     *
     * @note It is the caller's responsibility to verify that the passed in 
     *       buffer is large enough to contain the header to avoid the return
     *       of nullptr.
     */
    static
    const itm_header_t*
    CastFromBuffer(const boost::asio::const_buffer& buf)
    {
      // First check buffer_size and return nullptr if it is too small.
      // Otherwise return the result of a buffer_cast() operation.
      if (boost::asio::buffer_size(buf) < sizeof(itm_header_t))
      {
        return nullptr;
      }

      return boost::asio::buffer_cast<const itm_header_t*>(buf);
    }


    /**
     * Calculate and set the ITM header checksum
     */
    inline void SetITMChecksum()
    {
      // Assign a pointer to the 'raw' form of the header and work with the
      // pointer.
      const uint8_t* hdr_ptr = this->raw.data;
      uint8_t parity;

      parity  = CommonStructs::itm_checksum_parity_table0[hdr_ptr[0]];
      parity ^= CommonStructs::itm_checksum_parity_table1[hdr_ptr[1]];
      parity ^= CommonStructs::itm_checksum_parity_table2[hdr_ptr[2]];
      parity ^= CommonStructs::itm_checksum_parity_table3[hdr_ptr[3]];

      // The header checksum is stored in the last byte of the header.
      this->raw.data[4] = parity ^ PARITY_P3P4_FLIP;
    }


    /**
     * This method determines if this is a VITM packet.
     *
     * @return true for this being a VITM, and false otherwise.
     */
    inline bool IsVITM() const
    {
      // It doesn't matter which type we use (other than the "raw" one).
      return this->type1.IsVITM();
    }


    /**
     * This method determines if this is a Fixed size ITM packet.
     *
     * @return true for this being a Fixed site ITM, and false otherwise.
     */
    inline bool IsFixedSizeITM() const
    {
      // It doesn't matter which type we use (other than the "raw" one).
      return this->type1.IsFixedSizeITM();
    }


    /**
     * This method gets the destination node ID value from the packet.
     *
     * @return the destination node id value.
     */
    inline uint8_t GetDestinationId() const
    {
      // It doesn't matter which type we use (other than the "raw" one).
      return this->type1.GetDestinationId();
    }

    
    /**
     * This method gets the payload type value from the packet.
     *
     * @return the payload type value.
     */
    inline uint8_t GetPayloadType() const
    {
      // It doesn't matter which type we use (other than the "raw" one).
      return this->type1.GetPayloadType();
    }


    /**
     * This method is used to determine if the payload type of the ITM
     * (or VITM) is considered to be a "mission-data" payload type (or not).
     *
     * @return true if it is a mission-data payload type, and false otherwise.
     */
    inline bool IsMissionDataPayloadType() const
    {
      // It doesn't matter which type we use (other than the "raw" one).
      return this->type1.IsMissionDataPayloadType();
    }
  }__attribute__((packed,aligned(1)));


  //------------------------------
  /// MPLS header structure.
  //------------------------------
  struct MPLSHeader_t
  {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint32_t type       :  2;
    uint32_t identifier :  8;
    uint32_t path       :  3;
    uint32_t port       :  4;
    uint32_t reserved   :  3;
    uint32_t QOS        :  3;
    uint32_t spare      :  1;  
    uint32_t TTL        :  8;    
#else
    uint32_t TTL        :  8;    
    uint32_t spare      :  1;  
    uint32_t QOS        :  3;
    uint32_t reserved   :  3;
    uint32_t port       :  4;
    uint32_t path       :  3;
    uint32_t identifier :  8;
    uint32_t type       :  2;
#endif


    //==========
    // Methods
    //==========
    
    /**
     * This method creates and returns an MPLS header with all fields set
     * to 1's except for the QOS field.  That field is filled with the
     * value supplied by the caller.
     *
     * @param qos is the qos value supplied by the caller.
     *
     * @return the created MPLS header.
     */
    static inline
    MPLSHeader_t
    CreateFakeIMPLS(uint8_t qos)
    {
      MPLSHeader_t mpls_header;

      mpls_header.TTL = 0xff;
      mpls_header.spare = 0x1;
      mpls_header.reserved = 0x7;
      mpls_header.port = 0xf;
      mpls_header.path = 0x7;
      mpls_header.identifier = 0xff;
      mpls_header.type = 0x3;

      mpls_header.QOS = qos;

      return mpls_header;
    }

    
    /**
     * This will return an MPLSHeader_t by value which is the same as "this"
     * MPLSHeader_t object except that the underlying 32-bit integer will be
     * converted from host to network order.
     *
     * @return a by-value instance of the MPLSHeader_t after having converted
     *         the underlying 32-bits from host to network order.
     */
    MPLSHeader_t HostToNetworkOrder() const
    {
      using union_pair_t = inf::util::UnionPair<MPLSHeader_t, uint32_t>;


      // Create the "union pair" object, setting the MPLSHeader_t field (first)
      // to be a copy of "this" MPLSHeader object.
      union_pair_t pair = {*this};

      // Convert the uint32_t field (second) from host to network order.
      pair.second = htonl(pair.second);

      // Return a copy of the MPLSHeader_t object from the union pair to the
      // caller (it has now been converted from host to network order).
      return pair.first;
    }


    MPLSHeader_t NetworkToHostOrder() const
    {
      // The HostToNetworkOrder() does the same byte-flipping that is
      // required here, since the ntohl() and htonl() functions perform
      // the identical operation.
      return this->HostToNetworkOrder();
    }
  }__attribute__((packed,aligned(1)));    
  
  
  /**
   * This is the definition of the ITE common Header.
   * The common header covers the common fields from the
   * first 2 bytes of the ITE Data or Control message.
   * Specifically, the data/control bit and the source node id.
   */
  struct ite_common_header_t
  {
    //====================================================
    // Data Members (first 2 common bytes of ITE Header).
    //====================================================

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint8_t dc        :  1;    // data/control indicator
    uint8_t reserved  :  7;    // reserved
#else
    uint8_t reserved  :  7;    // reserved
    uint8_t dc        :  1;    // data/control indicator
#endif

    uint8_t srcnid;            // source node id
    
    
    /**
     * This static method will return an ITE common header pointer that is 
     * cast from the buffer argument.
     *
     * @param buf is a const buffer referring to the bytes of the ITE
     *        common header.
     * 
     * @return the header pointer, or a nullptr if there aren't enough bytes
     *         referred to by the supplied const_buffer object to account for
     *         the size of the ITE common Header.
     *
     * @note It is the caller's responsibility to verify that the passed in 
     *       buffer is large enough to contain the header to avoid the return
     *       of nullptr.
     */
    static
    const ite_common_header_t*
    CastFromBuffer(const boost::asio::const_buffer& buf)
    {
      // First check buffer_size and return nullptr if it is too small.
      // Otherwise return the result of a buffer_cast() operation.
      if (boost::asio::buffer_size(buf) < sizeof(ite_common_header_t))
      {
        return nullptr;
      }

      return boost::asio::buffer_cast<const ite_common_header_t*>(buf);
    }

    
    /**
    * This method determines if this is an ITE Data Message.
    *
    * @return true if this is an ITE Data Message, and false if this
    *         is an ITE Control Message.
    */
    inline bool IsDataMessage() const
    {
      return (this->dc == ITE_DC_Bit::DATA_MSG);
    }
  }__attribute__((packed,aligned(1)));
}} // Close MsgType and mia namespaces

