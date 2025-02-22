#include "Logger.h"

#include <iostream>

namespace
{
// Allow enabling/disabling logging and/or tracing duing development.
bool g_enableLogging {true};
bool g_enableTrace {false};
}

// Human readable function/member name (https://stackoverflow.com/a/29856690)
std::string computeMethodName(
	const std::string& fn,
	const std::string& prettyFn)
{
	// If the input is a ctor, it gets the beginning of the class name, not of
	// the method. That's why later on we have to search for the first
	// parenthesis.
	size_t locFunName = prettyFn.find(fn);
	size_t begin = prettyFn.rfind(" ", locFunName) + 1;
	// Adding function.length() make this faster and also allows to handle
	// operator parenthesis.
	size_t end = prettyFn.find("(", locFunName + fn.length());
#if 1
	return (prettyFn.substr(begin, end - begin) + "()");
#else
	if (prettyFn[end + 1] == ')')
		return (prettyFn.substr(begin, end - begin) + "()");
	else
		return (prettyFn.substr(begin, end - begin) + "(...)");
#endif
}

void Logger::Enable()
{
	g_enableLogging = true;
}

void Logger::Disable()
{
	g_enableLogging = false;
}

Logger::Logger(const std::string& prefix)
{
	if (g_enableLogging)
	{
		m_oss << prefix << Trace::Indent();
	}
}

Logger::~Logger()
{
	if (g_enableLogging)
	{
		std::cout << m_oss.str() << std::endl;
	}
}


std::atomic_int Trace::m_level;

void Trace::Enable()
{
	g_enableTrace = true;
}

void Trace::Disable()
{
	g_enableTrace = false;
}


Trace::Trace(const std::string& name, bool enterOnly)
	: m_name(name)
	, m_enterOnly(enterOnly)
{
	if (g_enableTrace)
	{
		if (m_enterOnly)
		{
			Logger("TRACE:") << m_name;
		}
		else
		{
			Logger("TRACE:") << m_name << ": Enter";
		}
		m_level.fetch_add(1);
	}
}

Trace::~Trace()
{
	if (g_enableTrace)
	{
		m_level.fetch_sub(1);
		if (!m_enterOnly)
		{
			Logger("TRACE:") << m_name << ": Leave";
		}
	}
}

std::string Trace::Indent()
{
	return std::string(m_level.load() * 2, ' ');
}
