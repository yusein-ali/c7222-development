#include "freertos_task.hpp"

#include <utility>

namespace c7222 {

extern "C" {
bool c7222_grader_register_freertos_task(FreeRtosTask* task,
										 const char* name,
										 std::uint32_t stack_depth_words,
										 std::uint32_t priority);
void c7222_grader_unregister_freertos_task(FreeRtosTask* task);
bool c7222_grader_start_freertos_task(FreeRtosTask* task);
bool c7222_grader_delete_freertos_task(FreeRtosTask* task, std::uint32_t ticks_to_wait);
bool c7222_grader_suspend_freertos_task(FreeRtosTask* task);
bool c7222_grader_resume_freertos_task(FreeRtosTask* task);
bool c7222_grader_resume_freertos_task_from_isr(FreeRtosTask* task);
bool c7222_grader_set_freertos_task_priority(FreeRtosTask* task, std::uint32_t priority);
std::uint32_t c7222_grader_get_freertos_task_priority(const FreeRtosTask* task);
bool c7222_grader_is_freertos_task_running(const FreeRtosTask* task);
void c7222_grader_delay_ticks(std::uint32_t ticks);
void c7222_grader_yield(void);
}

extern "C" void c7222_grader_run_freertos_task_entry(FreeRtosTask* task) {
	if(task != nullptr) {
		task->RunTaskBody();
	}
}

FreeRtosTask::FreeRtosTask(const char* name,
						   std::uint32_t stack_depth_words,
						   std::uint32_t priority,
						   TaskFunction task_function,
						   void* task_arg) {
	(void)Initialize(name, stack_depth_words, priority, std::move(task_function), task_arg);
}

bool FreeRtosTask::Initialize(const char* name,
							  std::uint32_t stack_depth_words,
							  std::uint32_t priority,
							  TaskFunction task_function,
							  void* task_arg) {
	(void)Delete(0);
	task_function_ = std::move(task_function);
	task_arg_ = task_arg;
	priority_ = priority;

	const bool registered =
		c7222_grader_register_freertos_task(this, name, stack_depth_words, priority);
	if(!registered) {
		handle_ = nullptr;
		return false;
	}
	handle_ = this;

	const bool started = c7222_grader_start_freertos_task(this);
	if(!started) {
		c7222_grader_unregister_freertos_task(this);
		handle_ = nullptr;
		return false;
	}
	return true;
}

FreeRtosTask::~FreeRtosTask() {
	(void)Delete(0);
}

bool FreeRtosTask::Delete(std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	(void)c7222_grader_delete_freertos_task(this, ticks_to_wait);
	c7222_grader_unregister_freertos_task(this);
	handle_ = nullptr;
	return true;
}

bool FreeRtosTask::Suspend() {
	return (handle_ != nullptr) && c7222_grader_suspend_freertos_task(this);
}

bool FreeRtosTask::Resume() {
	return (handle_ != nullptr) && c7222_grader_resume_freertos_task(this);
}

bool FreeRtosTask::ResumeFromISR() {
	return (handle_ != nullptr) && c7222_grader_resume_freertos_task_from_isr(this);
}

bool FreeRtosTask::SetPriority(std::uint32_t priority) {
	if(handle_ == nullptr) {
		return false;
	}
	const bool ok = c7222_grader_set_freertos_task_priority(this, priority);
	if(ok) {
		priority_ = priority;
	}
	return ok;
}

std::uint32_t FreeRtosTask::GetPriority() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_get_freertos_task_priority(this);
}

bool FreeRtosTask::IsValid() const {
	return handle_ != nullptr;
}

bool FreeRtosTask::IsRunning() const {
	return (handle_ != nullptr) && c7222_grader_is_freertos_task_running(this);
}

void* FreeRtosTask::GetHandle() const {
	return handle_;
}

void FreeRtosTask::Delay(std::uint32_t ticks) {
	c7222_grader_delay_ticks(ticks);
}

void FreeRtosTask::Yield() {
	c7222_grader_yield();
}

void FreeRtosTask::RunTaskBody() {
	if(task_function_) {
		task_function_(task_arg_);
	}
}

} // namespace c7222
