//============================================================================
//============================================================================

#include "DataRouter.h"
#include "log.h"
#include "StatsService.h"
#include "Scheduler.h"
#include "SolfReceiver.h"
#include "ConfigInit.h"
#include "ZmqSubscription.h"

#include <inf/log/service.h>
#include <signal.h>
#include <time.h>
#include <inf/util/app_lifecycle.h>
#include <memory>




/**
 * The main() function for the MIA application.
 *
 * @param argc is the number of command-line args in the argv array.
 *
 * @param argv is the array of command-line args.
 *
 * @return 0 for success, and non-zero for failure/fault.
 */
int
main (int argc, char *argv[])
{
  static constexpr auto fn = "main()";

  srand(time(NULL));

  //----------------------------------------------------------------------
  // Attempt to read, process, and validate the startup config items.
  //
  // NOTE: Upon success, the mia specific configuration items will
  //       be made accessible, via the mia::config::Items() function,
  //       anywhere within the mia code (see ConfigItems.h).
  //----------------------------------------------------------------------
  const auto config_reader = mia::config::Init(argc, argv);

  //-------------------------------------------------------------------------
  // Exit from main() in failure, by returning a non-zero value, if the
  // configuration processing failed.  The mia::config::Init() will log
  // the specific error(s) before returning a nullptr for the failure case.
  //-------------------------------------------------------------------------
  if (!config_reader)
  { return 1; }

  //--------------------------------------------------------------------------
  // Now attempt to setup the MIA application and run it until shutdown.
  //--------------------------------------------------------------------------
  try
  {
    //--------------------------------------------------
    // Attempt to configure and start the log service.
    //--------------------------------------------------
    inf::log::service::start(config_reader->GetLogConfig());

    //-------------------------------------------------------------
    // Ask the ConfigReader object to "dump" the config items and
    // version number to the log.
    //-------------------------------------------------------------
    config_reader->dumpToLog<mia::log::info>();

    inf::log::printToConsole(
      std::string("MIA Version: ") + TSW_VERSION_STRING);

    //------------------------------------------------------------------------
    // Create the mia::StatsService object using a local (stack) variable,
    // supplying it with a reference to the inf::stats::Config object.
    //
    // The stats service will be stopped when this object falls out-of-scope,
    // if the stats service is running when this object destructs.
    //------------------------------------------------------------------------
    mia::stats::Service theStatsService(config_reader->GetStatsConfig());

    //---------------------------------------------------------------------
    // Create the "DataRouter" object.  The "DataRouter" object serves as
    // the central component for the MIA application.
    //
    // This object follows an active object design pattern:
    //  e.g.:  https://www.state-machine.com/active-object
    //
    // Note:  The DataRouter object will be destructed when this local 
    //        variable de-scopes within the main thread thread of 
    //        execution. 
    //---------------------------------------------------------------------
    mia::DataRouter router;

    //-------------------------------------------------------------------
    // Create the Scheduler object.  The Scheduler object manages the
    // "queue" of commands.  The Scheduler constructor will start a new
    // thread dedicated to "running" the Scheduler.
    //
    // This object follow' an active object design pattern:
    //  e.g.:  https://www.state-machine.com/active-object
    //
    // Note:  The scheduler object will be destructed when this local 
    //        variable de-scopes within the main thread thread of 
    //        execution. 
    //-------------------------------------------------------------------
    mia::Scheduler scheduler;

    //---------------------------------------------------------------------
    // Create an ZMQSubscription object using a local stack variable, in
    // order to configure the ZMQ subscriptions to the EventService for
    // the MIA and to start the thread to handle the received events.
    //
    // When the local ZmqSubscription variable falls out-of-scope, its
    // destructor will ensure that we unsubscribe from the EventService
    // and stop the thread that is handling the received events.
    //---------------------------------------------------------------------
    mia::ZmqSubscription zmq_subscription(scheduler, router);

    //------------------------------------------------------------------------
    // Similarly, we create a SolfReceiver object using a local stack
    // variable.  The SolfReceiver is supplied with a non-const reference 
    // to the scheduler and ZMQReceiver instances. 
    //
    // The SolfReciever uses the scheduler object to be told of new LFrame
    // events.
    //
    // The SolfReceiver, upon construction, will start a thread to handle
    // incoming SOLF messages.  Each SOLF that is received will be
    // delivered to the ZmqReceiver object such that the ZmqReceiver thread
    // will send the SOLF to its Commander object to assist in the processing
    // of incoming ZMQ messages (ie, XML born from a test script).
    //------------------------------------------------------------------------
    mia::SolfReceiver solf_receiver(scheduler,
                                    zmq_subscription.GetZmqReceiver());

    //----------------------------------------------------------------------
    // Attempt to setup the DataRouter object in order to be able to start
    // the processing of packets for the MIA.
    //----------------------------------------------------------------------
    router.Start();

    //--------------------------------------------------------------------
    // Setup the UNIX signals that can cause the application to shutdown.
    //--------------------------------------------------------------------
    boost::asio::signal_set& signals = inf::util::app_lifecycle::getSignalSet();

    signals.add(SIGQUIT);
    signals.add(SIGUSR1);
    signals.add(SIGUSR2);

    //---------------------------------------------------------------------
    // Now have the mainline thread run the "app_lifecycle" event loop so
    // that it can react to shutdown and fault "events", returning 0 for
    // shutdown (successful) and 1 for fault (failure).
    //---------------------------------------------------------------------
    return inf::util::app_lifecycle::run() ? 0 : 1;
  }
  catch (const std::exception& e)
  {
    if (mia::log::error::enabled())
    {
      mia::log::error(fn)
        << "Caught std::exception in main(): " << e.what();
    }
  }
  catch (...)
  {
    if (mia::log::error::enabled())
    {
      mia::log::error(fn) << "Caught unknown (...) exception in main().";
    }
  }

  //-----------------------
  // Return successfully.
  //-----------------------
  return 0;
}
