//===========================================================================
//
//===========================================================================

#include "DataRouter.h"
#include "ConfigItems.h"
#include "log.h"
#include <SimulationCore/ChangeConfigCommand.hh>


//==============================================================================
//==============================================================================
mia::DataRouter::DataRouter()
: io_service_()
, task_()
, itm_delay_()
, tosim_drop_policies_()
, tosv_drop_policies_()

, tpn_interface_(*this,
                  this->io_service_, 
                  this->itm_delay_)

, vmw_interface_( *this,
                  this->io_service_,
                  config::Items().VMW_LISTEN_ADDRESS,
                  this->itm_delay_,
                  this->tosv_drop_policies_)

, kba_mpls_interface_(  
                  *this,
                  this->io_service_,
                  config::Items().KBA_LISTEN_ADDRESS)

, md_interface_(  *this,
                  this->io_service_, 
                  this->itm_delay_)

, sokf_interface_(this->io_service_,
                  config::Items().SOKF_LISTEN_ADDRESS,
                  [this] ()
                  {
                    // Perform K-Frame timing signal distribution.
                    this->tpn_interface_.OnSOKF();
                    this->vmw_interface_.OnSOKF();
                    this->md_interface_.OnSOKF();
                  }
                  )
{
  this->task_ = inf::util::Task::create(this->io_service_);
}


//==============================================================================
//==============================================================================
mia::DataRouter::~DataRouter()
{
  //-----------------------------------------------------------------------
  // Reset the unique_ptr to the Task object to be a nullptr, causing the
  // destruction of any Task object to which it was pointing (if any).
  //
  // This will stop the io_service and wait until the dedicated thread
  // running the io_service has finished.
  //-----------------------------------------------------------------------
  this->task_.reset();
}


//==============================================================================
//==============================================================================
void
mia::DataRouter::Start()
{
  //----------------------------------------------------------------------
  // Post the work to be done by the dedicated DataRouter thread that is
  // running the io_service object.
  //----------------------------------------------------------------------
  this->io_service_.post(
    [this] ()
    { this->Start_i(); });
}


//==============================================================================
//==============================================================================
void
mia::DataRouter::ChangeConfigToDefault()
{
  //----------------------------------------------------------------------
  // Post the work to be done by the dedicated DataRouter thread that is
  // running the io_service object.
  //----------------------------------------------------------------------
  this->io_service_.post(
    [this] ()
    { this->ChangeConfigToDefault_i(); });
}


//==============================================================================
//==============================================================================
void
mia::DataRouter::ChangeConfig(
  const Simulation::Core::ChangeConfig::CommandData& cmd_data)
{
  //----------------------------------------------------------------------
  // Post the work to be done by the dedicated DataRouter thread that is
  // running the io_service object.
  //----------------------------------------------------------------------
  this->io_service_.post(
    [this, cmd_data] ()
    { this->ChangeConfig_i(cmd_data); });
}


//==============================================================================
//==============================================================================
void
mia::DataRouter::ChangeConfigToDefault_i()
{
  //=========================================================================
  // The goal of this method is to change all dynamic config items to their
  // default values.
  //=========================================================================
  if (log::info::enabled())
  {
    log::info(Simulation::Core::ChangeConfig::COMMAND_NAME)
      << "Change all MIA specific dynamic config items to "
         "the configured default values.";
  }

  //----------------------------------------------------------------------
  // The TPNInterface object manages the dynamic "udpchksm" config item.
  //----------------------------------------------------------------------
  this->tpn_interface_.ChangeConfigToDefault();

  //------------------------------------------------------------------
  // The ITMDelay object manages the dynamic "itmdelay" config item.
  //------------------------------------------------------------------
  this->itm_delay_.changeConfig_defaultconfig();

  //------------------------------------------------------
  // Deal with the dynamic MIA drop packet config items.
  //------------------------------------------------------
  this->tosim_drop_policies_.changeConfig_defaultconfig();
  this->tosv_drop_policies_.changeConfig_defaultconfig();
}


//==============================================================================
//==============================================================================
void
mia::DataRouter::ChangeConfig_i(
  const Simulation::Core::ChangeConfig::CommandData& cmd_data)
{
  //===========================================================================
  // The goal of this method is to use the supplied ChangeConfig::CommandData
  // object to dictate how we should update any dynamic configuration items.
  //===========================================================================

  //====================================
  // Dynamic Config Item: UDP Checksum
  //====================================
  if (cmd_data.udpchksm)
  {
    const auto new_value = cmd_data.udpchksm.get();

    if (log::info::enabled())
    {
      log::info(Simulation::Core::ChangeConfig::COMMAND_NAME)
        << "Apply dynamic config item change: "
        << "[udpchksm="
        << (new_value ? "true" : "false")
        << "].";
    }

    // The TPNInterface object manages the dynamic "udpchksm" config item.
    this->tpn_interface_.ChangeConfigUDPChecksum(new_value);
  }

  //====================================
  // Dynamic Config Item: ITM Delay
  //====================================
  if (cmd_data.itmdelay)
  {
    const auto new_value = cmd_data.itmdelay.get();

    if (log::info::enabled())
    {
      log::info(Simulation::Core::ChangeConfig::COMMAND_NAME)
        << "Apply dynamic config item change: "
        << "[itmdelay=" << new_value << "].";
    }

    // The ITMDelay object manages the dynamic "itmdelay" config item.
    this->itm_delay_.changeConfig_itmdelay(new_value);
  }

  //====================================
  // Dynamic Config Item: MIA Route Delay
  //====================================
  if (cmd_data.miaroutedelay)
  {
    const auto& info = cmd_data.miaroutedelay.get();

    if (log::info::enabled())
    {
      log::info(Simulation::Core::ChangeConfig::COMMAND_NAME)
        << "Apply dynamic config item change for miaroutedelay: "
        << "[svnode="   << int(info.svnode)
        << ",simnode="  << int(info.simnode)
        << ",delay="    << int(info.delay)
        << "].";
    }

    // The ITMDelay object manages the dynamic "miaroutedelay" config item.
    this->itm_delay_.changeConfig_routedelay(
      info.svnode, 
      info.simnode, 
      info.delay);
  }

  //================================================
  // Dynamic Config Item: MIA Drop TO Sim Policies
  //================================================
  if (cmd_data.miadroptosim)
  {
    const auto& info = cmd_data.miadroptosim.get();

    if (log::info::enabled())
    {
      log::info(Simulation::Core::ChangeConfig::COMMAND_NAME)
        << "Apply dynamic config item change for miadroptosim: "
        << "[pt=" << drop_pkt::PayloadTypeId::ToLabel(info.pt)
        << ",algo=" << drop_pkt::Algorithm::ToLabel(info.algo)
        << ",interval=" << info.interval
        << "].";
    }

    this->tosim_drop_policies_.changeConfig(info.pt,
                                            info.algo,
                                            info.interval);
  }

  //================================================
  // Dynamic Config Item: MIA Drop TO SV Policies
  //================================================
  if (cmd_data.miadroptosv)
  {
    const auto& info = cmd_data.miadroptosv.get();

    if (log::info::enabled())
    {
      log::info(Simulation::Core::ChangeConfig::COMMAND_NAME)
        << "Apply dynamic config item change for miadroptosv: "
        << "[pt=" << drop_pkt::PayloadTypeId::ToLabel(info.pt)
        << ",algo=" << drop_pkt::Algorithm::ToLabel(info.algo)
        << ",interval=" << info.interval
        << "].";
    }

    this->tosv_drop_policies_.changeConfig(info.pt,
                                           info.algo,
                                           info.interval);
  }
}


//==============================================================================
//==============================================================================
void
mia::DataRouter::Start_i()
{
  //------------------------------------------------------------------------
  // We simply ask each Interface object to start reading from its socket.
  //------------------------------------------------------------------------
  this->tpn_interface_.Start();
  this->vmw_interface_.Start();
  this->kba_mpls_interface_.Start();
  this->md_interface_.Start();
  this->sokf_interface_.Start();
}


//==============================================================================
//==============================================================================
void
mia::DataRouter::RouteUplinkPassThroughMessage(
  const boost::asio::const_buffer& passthru_msg_buf)
{
  //=========================================================================
  // The MPLSInterface recieves packets from the KBA that need to be routed
  // as "pass-through" data to the VMW CP via the VMWInterface.
  //=========================================================================

  //---------------------------------------------------------------------------
  // The CP VMW destination endpoint may be configured with an invalid address
  // or port number. This is an indication that this DataRouter should NOT
  // send anything to the VMW CP (via the VMWInterface).
  //---------------------------------------------------------------------------
  const uint32_t cp_vmw_ip =
    config::Items().CP_VMW_DEST_ADDRESS.address().to_v4().to_ulong();

  const uint16_t cp_vmw_port =
    config::Items().CP_VMW_DEST_ADDRESS.port();

  if (cp_vmw_ip == config::INVALID_IP || cp_vmw_port == config::INVALID_PORT)
  {
    // There isn't a destination mapped for this packet so ignore it.
    return;
  }

  //----------------------------------------------------------------------
  // Now we deliver the passthru_msg_buf to the VMWInterface to send it.
  //----------------------------------------------------------------------
  this->vmw_interface_.SendUplinkPassThroughMessage(passthru_msg_buf);
}


//==============================================================================
//==============================================================================
void mia::DataRouter::RouteUplinkITM(
  const boost::asio::const_buffer& itm_buf, 
  uint8_t source_node, 
  uint8_t dest_node)
{
  this->vmw_interface_.SendUplinkITM(itm_buf, source_node, dest_node);
}


//==============================================================================
//==============================================================================
void mia::DataRouter::RouteUplinkMPLSPacket(
  const boost::asio::const_buffer& mpls_packet_buf,
  uint8_t source_node,
  uint8_t dest_node)
{
  this->vmw_interface_.SendUplinkMPLSPacket(
    mpls_packet_buf,
    source_node,
    dest_node);
}


//==============================================================================
//==============================================================================
void mia::DataRouter::RouteDownlinkPassThroughMessage(
      const boost::asio::const_buffer& passthru_msg_buf)
{
  //===========================================================================
  // Each message received by the VMWInterface contains one or more instances
  // of an "MPLS Packet".  The VMWInterface will invoke this method if at
  // least one MPLS Packet within the received message is destined for the
  // HPL Node.
  //
  // The supplied const_buffer object refers to the (entire) message received
  // by the VMWInterface (i.e., that contains at least one MPLS Packet that
  // is destined for the HPL Node).
  //===========================================================================

  //-------------------------------------------------------------------------
  // The KBA destination endpoint may be configured with an invalid address
  // or port number, which is an indication that this DataRouter should NOT
  // send anything to the KBA (via the MPLSInterface).
  //-------------------------------------------------------------------------
  const uint32_t kba_ip =
    config::Items().KBA_DEST_ADDRESS.address().to_v4().to_ulong();

  const uint16_t kba_port =
    config::Items().KBA_DEST_ADDRESS.port();

  if (kba_ip == config::INVALID_IP || kba_port == config::INVALID_PORT)
  {
    // There isn't a destination mapped for this packet so ignore it.
    return;
  }

  //----------------------------------------------------------------------
  // Now we deliver the passthru_msg_buf to the MPLSInterface to send it.
  //----------------------------------------------------------------------
  this->kba_mpls_interface_.SendDownlinkPassThroughMessage(passthru_msg_buf);
}


//==============================================================================
//==============================================================================
void mia::DataRouter::RouteDownlinkMPLSPacket(
      const boost::asio::const_buffer& mpls_packet_buf)
{
  //===========================================================================
  // Each message received by the VMWInterface contains one or more instances
  // of an "MPLS Packet".  The VMWInterface will invoke this method for each
  // MPLS Packet in a received message that is not destined for the HPL Node.
  //
  // Each MPLS Packet contains an MPLS Header and one ITM instance.
  //
  // The packet will be directed to either the TPN or MD interface.  The packet
  // direction is determined by the packet's destination node id and its
  // payload type.  And those values are within the ITM instance's header.
  //
  // The packet that is directed to the MD Interface will be stripped of it's
  // MPLS header.  Then just the ITM (or VITM) part will be sent on.
  // Otherwise the packet is directed to the TPN interface intact.
  //===========================================================================

  //--------------------------------------------------------------------------
  // If the destination node of the ITM is configured such that ITM data
  // should "Bypass TPN" *and* the Payload Type of the ITM indicates that
  // the ITM payload is mission data, then we want to send the ITM (or VITM)
  // to the Mission Data Interface (MDInterface).
  //--------------------------------------------------------------------------
  boost::asio::const_buffer itm_buf =
    mpls_packet_buf +
    sizeof (MsgTypes::MPLSHeader_t);

  const MsgTypes::itm_header_t* itm_header =
    MsgTypes::itm_header_t::CastFromBuffer(itm_buf);

  const uint8_t dstnid = itm_header->GetDestinationId();
  if (config::Items().MISSION_DATA_BYPASS_TPN[dstnid] &&
      itm_header->IsMissionDataPayloadType())
  {
    this->md_interface_.SendDownlinkITM(itm_buf);
  }
  else
  {
    //---------------------------------------------------------------------
    // Apply our (TOSIM) Drop Packet Policy for the packet's payload type,
    // which will return true if the packet should be dropped, and false
    // if the packet should be sent (to the TPN interface).  If the
    // decision is to drop the packet, then we simply return from this
    // method now, instead of continuing forward in the logic to actually
    // send the ITM (or VITM) to the TPN interface.
    //---------------------------------------------------------------------
    if (this->tosim_drop_policies_.apply(itm_header->GetPayloadType()))
    {
       return;
    }

    this->tpn_interface_.SendDownlinkMPLSPacket(mpls_packet_buf);
  }
}
