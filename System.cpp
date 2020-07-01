
#include "System.h"

/// \brief Track a new execution/task
/// \param[in] A unique id representing an execution/task
/// \return None
void System::AddTask(uint32_t id, std::future<void> f) {
    // std::lock_guard<std::mutex> guard(task_map_mutex);
    mutex.Lock();
    task_map[id] = std::move(f);
    mutex.Unlock();
}


/// \brief Check if a specific execution/task is done
/// \param[in] A unique id representing an execution/task
/// \return True if the operation is done. False, otherwise
bool System::CheckTaskDone(uint32_t task_id) {
    mutex.Lock();
    std::chrono::milliseconds span(100);
    bool ready = (task_map[task_id].wait_for(span) == std::future_status::ready);
    mutex.Unlock();
    return ready;
}


/// \brief Compute the number of actively running executions
/// \return Number of active executions/tasks
uint32_t System::NrRunningTasks() {
    mutex.Lock();
    uint32_t nr_running = 0;
	std::chrono::milliseconds span(10);
    for(const auto& t: task_map) {
        if(t.second.wait_for(span) == std::future_status::timeout) {
            nr_running++;
        }
    }
    mutex.Unlock();
    return nr_running;
}


/// \brief The executing instance of this function will stall until all tasks
///        provided to this function are done.
/// \param[in] A list of unique ids representing tasks which are to be waited for
/// \return None
void System::WaitForTasks(const std::vector<uint32_t>& task_ids) {
    std::set<uint32_t> finished_tasks;

    while(finished_tasks.size() != task_ids.size()) {
        for(const auto& task_id : task_ids) {
            if(finished_tasks.find(task_id) == finished_tasks.end() && CheckTaskDone(task_id)) {
                finished_tasks.insert(task_id);
            }
        }
                
        #ifdef __DEBUG__
            for(const auto& task : task_map) {
                bool completed = (task.second.wait_for(span) == std::future_status::ready);
                std::cout << "task id: " << task.first << " -> completed [yes/no]: " << completed << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        #endif
    }
}
