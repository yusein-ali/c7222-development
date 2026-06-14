#include "freertos_task.hpp"

#include "FreeRTOS.h"
#include "task.h"

#include <utility>

namespace c7222 {
namespace {

void TaskEntry(void* params) {
	auto* task = static_cast<FreeRtosTask*>(params);
	if(task != nullptr) {
		task->RunTaskBody();
	}
	vTaskDelete(nullptr);
}

} // namespace

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

	TaskHandle_t task_handle = nullptr;
	const BaseType_t created =
		xTaskCreate(TaskEntry,
					name,
					static_cast<configSTACK_DEPTH_TYPE>(stack_depth_words),
					this,
					static_cast<UBaseType_t>(priority),
					&task_handle);
	if(created != pdPASS) {
		handle_ = nullptr;
		return false;
	}
	handle_ = static_cast<void*>(task_handle);
	return true;
}

FreeRtosTask::~FreeRtosTask() {
	(void)Delete(0);
}

bool FreeRtosTask::Delete(std::uint32_t /*ticks_to_wait*/) {
	if(handle_ == nullptr) {
		return false;
	}
	vTaskDelete(static_cast<TaskHandle_t>(handle_));
	handle_ = nullptr;
	return true;
}

bool FreeRtosTask::Suspend() {
	if(handle_ == nullptr) {
		return false;
	}
	vTaskSuspend(static_cast<TaskHandle_t>(handle_));
	return true;
}

bool FreeRtosTask::Resume() {
	if(handle_ == nullptr) {
		return false;
	}
	vTaskResume(static_cast<TaskHandle_t>(handle_));
	return true;
}

bool FreeRtosTask::ResumeFromISR() {
	if(handle_ == nullptr) {
		return false;
	}
	return xTaskResumeFromISR(static_cast<TaskHandle_t>(handle_)) == pdTRUE;
}

bool FreeRtosTask::SetPriority(std::uint32_t priority) {
	if(handle_ == nullptr) {
		return false;
	}
	priority_ = priority;
	vTaskPrioritySet(static_cast<TaskHandle_t>(handle_), static_cast<UBaseType_t>(priority));
	return true;
}

std::uint32_t FreeRtosTask::GetPriority() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::uint32_t>(uxTaskPriorityGet(static_cast<TaskHandle_t>(handle_)));
}

bool FreeRtosTask::IsValid() const {
	return handle_ != nullptr;
}

bool FreeRtosTask::IsRunning() const {
	if(handle_ == nullptr) {
		return false;
	}
	const eTaskState state = eTaskGetState(static_cast<TaskHandle_t>(handle_));
	return state != eDeleted;
}

void* FreeRtosTask::GetHandle() const {
	return handle_;
}

void FreeRtosTask::Delay(std::uint32_t ticks) {
	vTaskDelay(static_cast<TickType_t>(ticks));
}

void FreeRtosTask::Yield() {
	taskYIELD();
}

std::uint32_t FreeRtosTask::GetTickCount() {
	return static_cast<std::uint32_t>(xTaskGetTickCount());
}

void FreeRtosTask::StartScheduler() {
	vTaskStartScheduler();
}

std::uint32_t FreeRtosTask::MsToTicks(std::uint32_t milliseconds) {
	return static_cast<std::uint32_t>(pdMS_TO_TICKS(milliseconds));
}

std::uint32_t FreeRtosTask::IdlePriority() {
	return static_cast<std::uint32_t>(tskIDLE_PRIORITY);
}

void FreeRtosTask::RunTaskBody() {
	if(task_function_) {
		task_function_(task_arg_);
	}
}

} // namespace c7222
