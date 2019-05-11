#ifndef LOGGER_HXX
#define LOGGER_HXX

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#include <array>
#include <ctime>
#include <map>

class Logger {
public:
    enum Priority { INFO = 0, WARNING = 1, ERROR = 2, CRITICAL = 3 };

    enum Outputs {
        NONE = 0b0000,
        COUT = 0b0001,
        CERR = 0b0010,
        CLOG = 0b0100,
        FILE = 0b1000,
    };

private:
    // The string stream used to build a new log message, that will eventually get written into the appropriate outputs.
    std::stringstream messageStream_;

    // The priority of the current message being written to the messageStream_.
    Priority activePriority_;

public:
    // Typedef to make the syntax of a Priority/Destination pair array easier to type.
    typedef std::map<Priority, uint8_t> ConfigurationMap;

    // The ConfigArray that stores the current Priority/Destination settings.
    ConfigurationMap PriorityOutputs;

    // A custom name used to identify the logger instance in the log outputs, can be left uninitialized if unwanted.
    std::string LoggerName;

    /* File paths where log messages with individual priorities
     * marked with the FILE flag get stored. If a path is invalid,
     * then the path isn't used, and the message isn't logged. */
    std::string InfoLogfilePath;
    std::string WarningLogfilePath;
    std::string ErrorLogfilePath;
    std::string CriticalLogfilePath;

    /* General file path where all priorities marked with the FILE
     * flag go to. If the path is invalid, then it's not used. */
    std::string GeneralLogfilePath;

    // Overload that grants access to messaegStream_, and configures the activePriority_.
    std::stringstream& operator<<(Priority);

    // Returns an EndLogType, and finalizes the message writing the messaegStream_ to the outputs.
    void EndLog();

    Logger operator=(const Logger&);

    Logger(const Logger&);
    Logger(ConfigurationMap);
    Logger();
};

#endif // LOGGER_HXX
