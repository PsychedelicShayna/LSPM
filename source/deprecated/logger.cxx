#include "logger.hxx"

std::stringstream& Logger::operator<<(Priority priority) {
    this->activePriority_ = priority;
    return this->messageStream_;
}

void Logger::EndLog() {
    /* A constant, static map of Priority:string, containing the names of the various priorities as strings.
     * This is used further down in the message formatting stream to add the actual name of the message priority
     * as part of the message in the log. As this is static, it is only allocated and defined once, and will
     * be re-used accross multiple function calls to optimize efficiency. */
    const static std::map<const Priority, const std::string> priority_name_strings = {
        {Priority::CRITICAL, "CRITICAL"},
        {Priority::WARNING, "WARNING"},
        {Priority::ERROR, "ERROR"},
        {Priority::INFO, "INFO"}
    };

    /* This provides the same kind of Priority:string resolution as the map above, however
     * these strings are references to the file paths used for the logfiles of each priority
     * level respectively. As they are references, any updates made to these strings in the
     * class instance, will be reflected here, even though the map itself is constant. */
    const static std::map<const Priority, std::string&> priority_file_paths = {
        {Priority::CRITICAL, this->CriticalLogfilePath},
        {Priority::WARNING, this->WarningLogfilePath},
        {Priority::ERROR, this->ErrorLogfilePath},
        {Priority::INFO, this->InfoLogfilePath}
    };

    /* This byte contains the bits that specify where the message should be outputted to.
     * AND'ing this byte with an output type, e.g. Outputs::COUT will reveal if
     * outputting to that output type is enbaled or not. */
    uint8_t outputs_byte = this->PriorityOutputs.at(this->activePriority_);

    // Creates time objects, and stores the current local time into the local_time variable.
    std::time_t current_time = std::time(nullptr);
    std::tm* local_time = std::localtime(&current_time);

    // A temporary stream, used to construct the formatted log message.
    std::stringstream formatted_message_stream;

    // Adds the message priority string into the stream.
    formatted_message_stream << "(" << priority_name_strings.at(this->activePriority_) << ") ";

    // Adds the current time, formatted nicely into the stream.
    formatted_message_stream << std::put_time(local_time, "%d/%m/%Y %H:%M:%S");

    // Adds the hexadecimal representation of the class's memory address int othe stream.
    formatted_message_stream << " @ [" << std::hex << reinterpret_cast<uint64_t>(this);

    // Adds the optional logger name, if the size of the string is positive.
    if(this->LoggerName.size()) formatted_message_stream << "/" << this->LoggerName;

    // Adds the actual message at the end of the stream.
    formatted_message_stream << "]: " << this->messageStream_.str() << std::endl;

    // The string, containing the final, formatted message from the formatted_message_stream stream.
    std::string formatted_message = formatted_message_stream.str();

    // Clears the message stream, to avoid having the old message in a new message.
    this->messageStream_.str(std::string());

    /* AND's the outputs_byte with COUT, CERR, and CLOG to see if the message
     * should be outputted to cout/cerr/clog respectively, and then proceeds
     * to write them to the outputs where the condition is true. */
    if(outputs_byte & Outputs::COUT) std::cout << formatted_message;
    if(outputs_byte & Outputs::CERR) std::cerr << formatted_message;
    if(outputs_byte & Outputs::CLOG) std::clog << formatted_message;

    // If file outputting bit is present in the outputs_byte, then proceed to output to the applicable file paths.
    if(outputs_byte & Outputs::FILE) {

        // The path to the logfile of the current priority of the message being processed, as defined by activePriority_
        const std::string& current_priority_logfile_path = priority_file_paths.at(this->activePriority_);

        /* Checks if the size of the current_priority_logfile_path string is positive, and only
         * proceeds to log if it is. This is done as the size may be zero in the case that
         * logging to that logfile is disabled, in which case the message is not logged. */
        if(current_priority_logfile_path.size()) {
            /* Opens a new output stream to current_priority_logfile_path, using binary mode, and append mode.
             * Binary mode is used to avoid any potential unwanted formatting errors, by handling the data raw.
             * Append mode is used to append the message to a potentially pre-exisitng logfile, rather than overwriting. */
            std::ofstream output_stream(current_priority_logfile_path, std::ios::binary | std::ios::app);

            // Checks if the output_stream has been opened properly, writing the message and closing the stream if it was.
            if(output_stream.good()) {
                output_stream << formatted_message;
                output_stream.close();
            }
        }

        /* Repeats the same steps as the above code, except for the GeneralLogfilePath instead,
         * which is a logfile used for any and all logged messages, regardless of priority. */
        if(this->GeneralLogfilePath.size()) {
            std::ofstream output_stream(this->GeneralLogfilePath, std::ios::binary | std::ios::app);

            if(output_stream.good()) {
                output_stream << formatted_message;
                output_stream.close();
            }
        }
    }
}

Logger Logger::operator=(const Logger& old) {
    return Logger(old);
}

Logger::Logger(const Logger& old) : activePriority_(static_cast<Priority>(NULL)) {
    this->PriorityOutputs = old.PriorityOutputs;
    this->LoggerName = old.LoggerName;

    this->InfoLogfilePath = old.InfoLogfilePath;
    this->WarningLogfilePath = old.WarningLogfilePath;
    this->ErrorLogfilePath = old.ErrorLogfilePath;
    this->CriticalLogfilePath = old.CriticalLogfilePath;
    this->GeneralLogfilePath = old.GeneralLogfilePath;
}

Logger::Logger(Logger::ConfigurationMap priorityOutputs) : activePriority_(static_cast<Priority>(NULL)), PriorityOutputs(priorityOutputs) {}

Logger::Logger() : activePriority_(static_cast<Priority>(NULL)){
    /* Sets the default priority outputs to be none, meaning errors logged using them
     * by default will be outputted nowhere, unless otherwise specified. */
    this->PriorityOutputs = ConfigurationMap {
        {Priority::CRITICAL, Outputs::NONE},
        {Priority::WARNING, Outputs::NONE},
        {Priority::ERROR, Outputs::NONE},
        {Priority::INFO, Outputs::NONE},
    };
}
