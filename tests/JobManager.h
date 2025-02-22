#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

class BaseThread
{
public:
	explicit BaseThread(const std::string& name, bool autostart = true);
	virtual ~BaseThread();

	void Start();
	void Stop();

	void Run();

protected:
	virtual void Run_() = 0;

protected:
	std::thread m_thread;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::atomic_bool m_running;
	const std::string m_name;
};

// This class spawns a thread to process jobs (i.e. lambdas) added to a queue.
class JobManager : public BaseThread
{
public:
	explicit JobManager(bool autostart = true);
	~JobManager() override = default;

	void AddJob(std::function<void()> job);

private:
	void Run_() override;

private:
	std::deque<std::function<void()>> m_jobs;
};
