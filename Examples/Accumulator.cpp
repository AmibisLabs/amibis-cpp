/**
 * @defgroup Examples Examples of how to use OMiSCID.
 *
 */

 /**
 * @file Examples/Accumulator.cpp
 * @ingroup Examples
 * @brief Demonstartion of an accumulator server.
 *
 * @author Dominique Vaufreydaz
 */

// Standard includes
#include <ServiceControl/UserFriendlyAPI.h>

#include <iostream>
using namespace std;

using namespace Omiscid;

namespace Omiscid {

/**
 * @class AccumulatorConnectorListener
 * @ingroup Examples
 * @brief this class is used to monitor incomming activity from intput connector of a service
 */
class AccumulatorConnectorListener : public ConnectorListener
{
public:
    /* @brief constructor */
    AccumulatorConnectorListener();
    /* @brief destructor */
    virtual ~AccumulatorConnectorListener();

    /* @brief callback function to receive data */
    virtual void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg);

private:
    Mutex Locker;        /*!< Lock access to my variable */

    float Accu;            /*!< my accumulator */
};

} // namespace Omiscid


    /* @brief constructor */
AccumulatorConnectorListener::AccumulatorConnectorListener()
{
    // set initial value to the Accu
    Accu = 0.0f;
}

    /* @brief destructor */
AccumulatorConnectorListener::~AccumulatorConnectorListener()
{
}

    /* @brief callback function to receive data */
void AccumulatorConnectorListener::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
    // Error management
    bool IsWrong = true;    // by default, the command is wrong
    SimpleString ErrorMessage = "Bad command.";

    // Used to read operand ?
    float fRightOperand = 0.0f;

    // A pointer to the command line and to the begining of the usufull data
    char * Command;
    int  StartUsefullInfo;

    // parse string to understand command
    // Start lock myself using my mutex (needed if multiple clients)
    Locker.EnterMutex();

    // Get the pointer to the data
    Command = Msg.GetBuffer();

    // skip data while there are spaces...
    for( StartUsefullInfo = 0; Command[StartUsefullInfo] != '\0'; StartUsefullInfo++ )
    {
        if ( Command[StartUsefullInfo] != ' ' &&  Command[StartUsefullInfo] != '\t' &&
             Command[StartUsefullInfo] != '\r' && Command[StartUsefullInfo] != '\n' )
        {
            break;
        }
    }

    // Is it a legal command ?
    if ( Command[StartUsefullInfo] == '-' || Command[StartUsefullInfo] == '+' ||
            Command[StartUsefullInfo] == '/' || Command[StartUsefullInfo] == '*' ||
            Command[StartUsefullInfo] == '=' )
    {
        // look for a float like 3.14 and then for
        if ( sscanf( &Command[StartUsefullInfo+1], "%f", &fRightOperand ) == 1 )
        {
            IsWrong = false;
        }
        // else IsWrong == true;

        // If it is ok,
        if ( IsWrong == false )
        {
            // run the command
            switch( Command[StartUsefullInfo] )
            {
                case '-':
                    // Compute new value
                    Accu -= fRightOperand;
                    break;

                case '+':
                    // Compute new value
                    Accu += fRightOperand;
                    break;

                case '*':
                    // Compute new value
                    Accu *= fRightOperand;
                    break;

                case '/':
                    if ( fRightOperand == 0.0f )
                    {
                        // we can not divide by 0
                        IsWrong = true;
                    }
                    else
                    {
                        // Compute new value
                        Accu /= fRightOperand;
                    }
                    break;

                default:     // here, Command[0] == '='
                    Accu = fRightOperand;
                    break;
            }
        }
    }
    if ( IsWrong == false )
    {
        // export the new Accu value, so the clients will receive notification
        TheService.SetVariableValue("Accu", Accu);
    }
    else
    {
        // Discard value and send back to the rigth "person" an error message in safe mode (TCP)
        TheService.SendReplyToMessage( (char*)ErrorMessage.GetStr(), (int)ErrorMessage.GetLength(), Msg, false );
    }

    // Unlock myself
    Locker.LeaveMutex();
}



/* @brief main program entry for the Accumulator. No need to give parameter */
int main(int argc, char*argv[] )
{
    // TrackMemoryLeaks TML;

    // Instanciate a Connector listener (must *not* be destroyed before it has
    // been removed for the connector or until the service is destroyed).
    AccumulatorConnectorListener MyCommandsListener;

    // Ask to the service factory to create a Service. The service is not
    // register yet. We do not provide the service class, the default value 'Service'
    // will be used
    Service * pAccuServer = ServiceFactory.Create( "Accumulator" );

    // If something tricky occurred, exit
    if ( pAccuServer == NULL )
    {
        fprintf( stderr, "Could not register the Accumulator service.\n" );

        // Exit the thread
        return -1;
    }

    for (int i = 0 ; i < 1400 ; i++)
    {
       SimpleString var = "v";
       var += i;
       pAccuServer->AddVariable(var, "bla", "bla", ReadWriteAccess);
       pAccuServer->SetVariableValue(var, "a");
     }

    // Just add a Variable of ReadAccess (read only) that can not be modifies by other services.
    // Set its value to the starting value
    pAccuServer->AddVariable( "Accu", "float", "Accumulator value", ReadWriteAccess );
    pAccuServer->SetVariableValue( "Accu", 0.0f );

    // Add a Connector to receive and send messages (AnInOutput) and set my callback
    // object to receive notification of messages
    pAccuServer->AddConnector( "Commands", "Input for commands. output of errors.", AnInOutput );
    pAccuServer->AddConnectorListener( "Commands", &MyCommandsListener );

    // register the service and launch everything
    pAccuServer->Start();

    // Trace
    fprintf( stderr, "Accumulator service started.\n" );

    // Create an Event and wait for it. As no one will signal it, we will
    // stay forever stuck here.
    Event Forever;
    Forever.Wait();

    // exit
    return 0;
}
