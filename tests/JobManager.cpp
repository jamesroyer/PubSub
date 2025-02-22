#include "JobManager.h"

#include <iostream>

// Set to 1 to enable debug output and 0 to disable debug output.
#define SHOW_DEBUG_OUTPUT 0

#define LOGGER \
	if (!SHOW_DEBUG_OUTPUT) \
		; \
	else \
		std::cout

BaseThread::BaseThread(const std::string& name, bool autostart)
	: m_name(name)
{
	if (autostart)
	{
		Start();
	}
}

BaseThread::~BaseThread()
{
	Stop();
}

void BaseThread::Start()
{
	if (!m_thread.joinable())
	{
		std::unique_lock lock(m_mutex);
		LOGGER << m_name << ": Start(): Starting thread..." << std::endl;
		m_thread = std::thread(&JobManager::Run, this);
	}
}

void BaseThread::Stop()
{
	if (m_thread.joinable())
	{
		std::unique_lock lock(m_mutex);
		LOGGER << m_name << ": Stop(): Stopping thread..." << std::endl;
		m_running.store(false);
		lock.unlock();
		m_cv.notify_one();
		LOGGER << m_name << ": Stop(): Waiting for thread to terminate..." << std::endl;
		m_thread.join();
		LOGGER << m_name << ": Stop(): Thread terminated" << std::endl;
	}
}

void BaseThread::Run()
{
	Run_();
}


// This class spawns a thread to process jobs (i.e. lambdas) added to a queue.
JobManager::JobManager(bool autostart)
	: BaseThread("JobManager", autostart)
{
}

void JobManager::AddJob(std::function<void()> job)
{
	std::unique_lock lock(m_mutex);
	m_jobs.push_back(job);
	lock.unlock();
	m_cv.notify_one();
}

void JobManager::Run_()
{
	m_running = true;
	for (;;)
	{
		std::unique_lock lock(m_mutex);

		if (!m_running)
		{
			LOGGER << "JobManager::Run_(): Terminating thread..." << std::endl;
			break;
		}

		if (m_jobs.size() > 0)
		{
			auto job = m_jobs.front();
			m_jobs.pop_front();
			job();
		}
		else
		{
			m_cv.wait(lock);
		}
	}
}
