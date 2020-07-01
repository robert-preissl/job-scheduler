
#pragma once

#include <unordered_map>
#include <stdio.h>
#include <string>
#include <iostream>
#include <set>
#include <future>         // std::async, std::future
#include <vector>

#include "mutex.h"

/// \brief A very simplified view of a compute a system.
///        This system keeps track of running executions and their state (active or done)
class System {
    private:
        // key: task-id   value: the future (result0 of the asynchronous task
        std::unordered_map<uint32_t, std::future<void>> task_map GUARDED_BY(mutex);
        Mutex mutex;
        
        /// \brief Check if a specific execution/task is done
        /// \param[in] A unique id representing an execution/task
        /// \return True if the operation is done. False, otherwise
        bool CheckTaskDone(uint32_t task_id);

    public:	
        /// \brief Track a new execution/task
        /// \param[in] A unique id representing an execution/task
        /// \return None
        void AddTask(uint32_t id, std::future<void> f);

        /// \brief Compute the number of actively running executions
        /// \return Number of active executions/tasks
        uint32_t NrRunningTasks();

        /// \brief The executing instance of this function will stall until all tasks
        ///        provided to this function are done.
        /// \param[in] A list of unique ids representing tasks which are to be waited for
        /// \return None
        void WaitForTasks(const std::vector<uint32_t>& task_ids);
};
