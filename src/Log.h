#pragma once
#include <iostream>
#include <sstream>

enum class TextColor
{
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
    BrightBlack = 90,
    BrightRed = 91,
    BrightGreen = 92,
    BrightYellow = 93,
    BrightBlue = 94,
    BrightMagenta = 95,
    BrightCyan = 96,
    BrightWhite = 97
};

enum class BackgroundColor
{
    Black = 40,
    Red = 41,
    Green = 42,
    Yellow = 43,
    Blue = 44,
    Magenta = 45,
    Cyan = 46,
    White = 47,
    BrightBlack = 100,
    BrightRed = 101,
    BrightGreen = 102,
    BrightYellow = 103,
    BrightBlue = 104,
    BrightMagenta = 105,
    BrightCyan = 106,
    BrightWhite = 107
};

enum class LogType
{
    Normal,
	Warning,
    Error
};

class Log
{

public:

    Log(LogType logType = LogType::Normal) :
		m_LogType{logType}
    {
	    switch (logType)
    	{
	    	case LogType::Normal: 
                m_TextColor = TextColor::White ;
                m_BackgroundColor = BackgroundColor::BrightBlack;
                break;
			case LogType::Warning: 
                m_TextColor = TextColor::Black;
                m_BackgroundColor = BackgroundColor::Yellow;
                break;
			case LogType::Error: 
                m_TextColor = TextColor::Black;
                m_BackgroundColor = BackgroundColor::BrightRed;
                break;
	    }

    }

    Log(BackgroundColor backgroundColor, TextColor textColor) :
	    m_TextColor{ textColor },
        m_BackgroundColor{ backgroundColor }
    {}

    Log(TextColor textColor, BackgroundColor backgroundColor) :
        m_TextColor{ textColor },
        m_BackgroundColor{ backgroundColor }
    {}


    ~Log()
    {

            if constexpr (not m_EnableLog)
                return;

            std::string prefix;
            switch (m_LogType)
            {
            case LogType::Normal:
                prefix = m_NormalLogPrefix;
                break;
            case LogType::Warning:
                prefix = m_WaringLogPrefix;
                break;
            case LogType::Error:
                prefix = m_ErrorLogPrefix;
                break;
            }

            std::cout

                << std::boolalpha

                // Set prefix color
                << "\033[" << static_cast<int>(TextColor::BrightWhite) << "m"

                << prefix

                // Set colors
                << "\033[" << static_cast<int>(m_TextColor) << ";" << static_cast<int>(m_BackgroundColor) << "m"
                << " " << m_Stream.str() << " "

                << "\033[0m" // Rest
                << "\n";	 // Return
        
    }

    Log(Log&& other) = delete;
    Log(const Log& other) = delete;
	Log& operator=(Log&& other) = delete;
    Log& operator=(const Log& other) = delete;


    template <typename StreamInputType>
    Log& operator<<(const StreamInputType& value)
    {
        m_Stream << value;
        return *this;
    }


private:

    inline static const std::string m_NormalLogPrefix{ "> " };
    inline static const std::string m_ErrorLogPrefix{ "!ERROR! " };
    inline static const std::string m_WaringLogPrefix{ "!WARNING! " };
    inline static const bool m_EnableLog{ true};

    TextColor m_TextColor{};
    BackgroundColor m_BackgroundColor{};
    LogType m_LogType{};

    std::stringstream m_Stream;
};