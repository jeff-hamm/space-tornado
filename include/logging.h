#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>
#include <Print.h>
#include <WString.h>

#define LOG_BUFFER_SIZE 100
#define MAX_LOG_MESSAGE_LENGTH 256

class LoggerClass : public Print {
private:
    Print* serialPrint;
    String logBuffer[LOG_BUFFER_SIZE];
    int logIndex;
    int logCount;
    char messageBuffer[MAX_LOG_MESSAGE_LENGTH];
    int bufferPos;

public:
    LoggerClass();
    
    void addLogger(Print& print);
    
    size_t write(uint8_t byte) override;
    size_t write(const uint8_t* buffer, size_t size) override;
    
    String getLogsAsHtml();
    String getLogsAsJson();
    
    void clearLogs();
    int getLogCount() const { return logCount; }
    
private:
    void addMessageToBuffer(const String& message);
};

extern LoggerClass Logger;

#endif // LOGGING_H

