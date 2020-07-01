
#pragma once

#include <vector>
#include <queue>
#include <set>
#include <unordered_map>
#include <string>
#include <iostream>
#include <chrono>


#include "State.h"
#include "ExecutionContext.h"
#include "System.h"


// just one example of what type of work the job scheduler can create. here a simple
//  function, which takes two integers. (plus the list of identifiers of preceding tasks)
typedef std::function<void(const uint32_t, const uint32_t)> void_work_function_t;


// A job consists of many tasks, which interdependencies are modeled via adjacency lists.
//  Traversal/Scheduling using BFS. (Todo: exploit parallelism where possible)

/// \brief The JobScheduler takes care of topological sorting of tasks (which depend on
///        other tasks. Topological sorting is performed via Kahn's algorithm.
///        Further, the scheduler ensure only a constant number of executions are active
///        at any time. (to prevent system overloads)
///        In this example, the overall goal of all executions is to modify some global
///        state. And, we are provided with a compute "System".
template <class T>
class JobScheduler {

    private:
        uint32_t job_id_;
        uint32_t max_concurrent_tasks_;
        std::shared_ptr<GlobalState<T>> global_state_;
        System system_;
		
        // E.g., key: 0 ->  value: [1,2]  means that tasks 1 & 2 depend on task 0
        std::unordered_map<uint32_t, std::vector<uint32_t>> task_adj_list_;
		
        // E.g., key: 6 -> value: [3,4,5]  means that task 6 depends on taks 3,4 and 5
        std::unordered_map<uint32_t, std::vector<uint32_t>> parent_tasks_;
		
        // E.g., key: 0 -> value: 1  means that task 0 has an indegree of 1
        std::unordered_map<uint32_t, uint32_t> indegrees_;
		
        // Data structure to compute the topological sorting of tasks/ExecutionContexts
        std::queue<ExecutionContext> task_queue_;
        
        // manage a unique list of all the tasks
        std::set<uint32_t> task_ids_;
		
        // keep track of which tasks have already been processed
        std::unordered_map<uint32_t, bool> processed_tasks_;

        /// \brief Simple representation of work being performed on some state.
        /// \return A function, which operates on some state
        void_work_function_t CreateWork();

        /// \brief Find initial tasks with indegree of 0. These are the starting points of
        ///        the overall scheduling algorithm.
        /// \return None.
        void QueueUpIndependentTasks();

        /// \brief Ensure no more than "max_concurrent_tasks_" many executions are running
        ///        at the same time. we consult the compute "System" for this information.
        ///        If we reach the limit, this function will halt the scheduler to start a
        ///        a new task.
        /// \return None.
        bool EnforceLoadLimit();

    public:
        JobScheduler(std::shared_ptr<GlobalState<T>> global_state) :
            global_state_(global_state) {
            job_id_ = 1234;
            max_concurrent_tasks_ = 4;
        }

        /// \brief Represent dependencies among tasks/executions via adjacency list and
        ///        fill up any relevant internal data structures.
        /// \param[in] A unique id representing an execution/task
        /// \param[in] The id of the task this task depends on. (if a certain task depends
        ///            on multiple other tasks, multiple calls to this function are done.
        /// \return None.
        void AddTask(uint32_t task_id,
                     uint32_t depends_on_task_id);

        /// \brief Helper function to print indegrees for all tasks
        /// \return None
        void PrintIndegrees();
		
		/// \brief Run the topological sorting algorithm and create Tasks/ExecutionContext
        /// \return True if tasks can be scheduled within a certain timeout limit. False
        ///         otherwise. (which is a sign that the system is constantly overloaded)
        bool ProcessTasks();
};
