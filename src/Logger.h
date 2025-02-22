#pragma once

#include <atomic>
#include <sstream>

// Human readable function/member name (https://stackoverflow.com/a/29856690)
std::string computeMethodName(
	const std::string& fn,
	const std::string& prettyFn);

#define COMPACT_PRETTY_FUNCTION \
	computeMethodName(__FUNCTION__, __PRETTY_FUNCTION__)

// Use RAII to trace entry/exit from functions with indentation.
//
// usage:
//   void Fn1()
//   {
//       Trace trace(COMPACT_PRETTY_FUNCTION);
//   }
//
//   void Fn2()
//   {
//       TRACE(); // same as above but less typing.
//   }
//
// Notes:
// - Not thread safe since 'm_level' is global.
//   - Could use TLS (thread local storage) or something similar to fix.
class Trace
{
public:
	Trace(const std::string& name, bool enterOnly = false);
	~Trace();

	static std::string Indent();

public:
	static void Enable();
	static void Disable();

private:
	const std::string m_name;
	bool m_enterOnly;

private:
	static std::atomic_int m_level;
};

#define TRACE() Trace trace_(COMPACT_PRETTY_FUNCTION)
#define TRACE_SIMPLE() Trace trace_(COMPACT_PRETTY_FUNCTION, true)

class Logger
{
public:
	explicit Logger(const std::string& prefix);
	~Logger();

	template <typename T>
	Logger& operator<<(const T& value)
	{
		m_oss << value;
		return *this;
	}

public:
	static void Enable();
	static void Disable();

private:
	std::ostringstream m_oss;
};

#define LOG(prefix) Logger(prefix) << COMPACT_PRETTY_FUNCTION << ": "

#define FATAL() LOG("FATAL: ")
#define ERROR() LOG("ERROR: ")
#define INFO()  LOG("INFO : ")
#define DEBUG() LOG("DEBUG: ")
