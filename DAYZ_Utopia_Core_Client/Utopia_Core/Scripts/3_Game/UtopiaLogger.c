class UtopiaLogger
{
    static const string LOG_DIR = "$profiles:Utopia_Logs";  // Main log directory //skip

    // Map to keep track of individual log files based on log type
    static ref map<string, string> logFileMap = new map<string, string>(); //skip

    // Initialize and ensure log file for a specific log type
    static void InitLogFile(string logType) //skip
    {
        // Determine the log directory and file path based on the logType
        string logDir; //skip
        switch (logType) //skip
        {
            case "CTF":
                logDir = LOG_DIR + "/CTFLog"; //skip
                break;
            case "KOTH":
                logDir = LOG_DIR + "/KothLog"; //skip
                break;
            case "Airdrop":
                logDir = LOG_DIR + "/AirdropLog"; //skip
                break;
            case "AuthAPI":
                logDir = LOG_DIR + "/AuthLog"; //skip
                break;
            default:
                logDir = LOG_DIR + "/GeneralLog"; //skip
                break;
        }

        // Add timestamp to the file name for unique log files per server start
        string logFile = logDir + "/" + logType + "Log_" + GetDate() + "_" + GetTime() + ".log"; //skip

        // Ensure the log directory exists
        EnsureDirectoryExists(logDir); //skip

        // Create a new log file for this server session
        FileHandle fileHandle = OpenFile(logFile, FileMode.WRITE); //skip
        if (fileHandle != 0) //skip
        {
            FPrintln(fileHandle, "[UtopiaLogger][" + logType + "] Log started at: " + GetTime()); //skip
            CloseFile(fileHandle); //skip
            Print("[UtopiaLogger] Created log file: " + logFile); //skip
        }

        // Store the file path in the map for future use
        logFileMap.Set(logType, logFile); //skip
    }

    // General logging function with a specific log type
    static void Log(string message, string logType = "General") //skip
    {
        // Initialize the log file if it does not exist or is not initialized
        if (!logFileMap.Contains(logType)) //skip
        {
            InitLogFile(logType); //skip
        }

        string logFile = logFileMap.Get(logType); //skip

        Print("[UtopiaLogger][" + logType + "] " + message); //skip

        // Write to the appropriate log file
        FileHandle logFileHandle = OpenFile(logFile, FileMode.APPEND); //skip
        if (logFileHandle != 0) //skip
        {
            FPrintln(logFileHandle, "[" + GetTime() + "][" + logType + "] " + message); //skip
            CloseFile(logFileHandle); //skip
        }
    }

    // Log different types of messages with specific log levels
    static void LogDebug(string message, string logType = "General") //skip
    {
        Log("DEBUG: " + message, logType); //skip
    }

    static void LogInfo(string message, string logType = "General") //skip
    {
        Log("INFO: " + message, logType); //skip
    }

    static void LogWarning(string message, string logType = "General") //skip
    {
        Log("WARNING: " + message, logType); //skip
    }

    static void LogError(string message, string logType = "General") //skip
    {
        Log("ERROR: " + message, logType); //skip
    }

    // Helper function to ensure the log directory exists
    static void EnsureDirectoryExists(string dir) //skip
    {
        if (!FileExist(dir)) //skip
        {
            MakeDirectory(dir); //skip
            Print("[UtopiaLogger] Created directory: " + dir); //skip
        }
    }

    // Helper function to get the current date
    static string GetDate() //skip
    {
        int year, month, day; //skip
        GetYearMonthDay(year, month, day); //skip
        return year.ToStringLen(4) + "-" + month.ToStringLen(2) + "-" + day.ToStringLen(2); //skip
    }

    // Helper function to get the current time
    static string GetTime() //skip
    {
        int hour, minute, second; //skip
        GetHourMinuteSecond(hour, minute, second); //skip
        return hour.ToStringLen(2) + "-" + minute.ToStringLen(2) + "-" + second.ToStringLen(2); //skip
    }
}
