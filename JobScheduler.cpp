
#include "JobScheduler.h"


/// \brief Simple representation of work being performed on some state.
/// \return A function, which operates on some state. In reality, a job scheduler would
///         create more diverse, more complex work items.
///         In this example we also experiment with the fact that certain tasks can take
///         longer as other tasks. Duration is simulated via sleeps.
template <class T>
void_work_function_t JobScheduler<T>::CreateWork() {
    void_work_function_t work = [this](
        const uint32_t sleep_time_sec,
        const uint32_t data) {

        // if the data element being processed is a "2", we sleep extra
        if(data == 2) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    	
        std::this_thread::sleep_for(std::chrono::seconds(sleep_time_sec));
        global_state_->Add(std::to_string(data));
        
        #ifdef __DEBUG__
            std::cout << "Done adding data = " << data << std::endl;
        #endif
    };
    return work;
}


/// \brief Find initial tasks with indegree of 0. These are the starting points of
///        the overall scheduling algorithm.
/// \return None.
template <class T>
void JobScheduler<T>::QueueUpIndependentTasks() {
    std::set<uint32_t> dependent_tasks;
    
    for(const auto& t : task_adj_list_) {
        for(const auto d: t.second) {
            dependent_tasks.insert(d);
        }
    }
		
    for(const auto& t : task_ids_) {
        if(dependent_tasks.find(t) == dependent_tasks.end()) {
            task_queue_.push(ExecutionContext(t));
        }
    }
}


/// \brief Ensure no more than "max_concurrent_tasks_" many executions are running
///        at the same time. we consult the compute "System" for this information.
///        If we reach the limit, this function will halt the scheduler to start a
///        a new task.
/// \return None.
template <class T>
bool JobScheduler<T>::EnforceLoadLimit() {
    const uint32_t max_wait_cycles = 1000;
    uint32_t cycles = 0;

    #ifdef __DEBUG__
	    std::cout << " -> nr_running_tasks = " << system_.NrRunningTasks() << std::endl;
	#endif
	
    while(system_.NrRunningTasks() > max_concurrent_tasks_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if(++cycles >= max_wait_cycles) {
            break;
        }
    }
    
    if(cycles >= max_wait_cycles) {
        return false;
    }
    return true;
}


/// \brief Represent dependencies among tasks/executions via adjacency list and
///        fill up any relevant internal data structures.
/// \param[in] A unique id representing an execution/task
/// \param[in] The id of the task this task depends on. (if a certain task depends
///            on multiple other tasks, multiple calls to this function are done.
/// \return None.
template <class T>
void JobScheduler<T>::AddTask(uint32_t task_id, uint32_t depends_on_task_id) {
    task_adj_list_[depends_on_task_id].emplace_back(task_id);
    task_ids_.insert(task_id);
    task_ids_.insert(depends_on_task_id);
    parent_tasks_[task_id].emplace_back(depends_on_task_id);
    indegrees_[task_id]++;
}


/// \brief Helper function to print indegrees for all tasks
/// \return None	
template <class T>
void JobScheduler<T>::PrintIndegrees() {
    for(const auto& t_i : indegrees_) {
        std::cout << "ExecutionContext id: " << t_i.first << " has indegree: " << t_i.second << std::endl;
    }
}


/// \brief Run the topological sorting algorithm and create Tasks/ExecutionContext
/// \return True if tasks can be scheduled within a certain timeout limit. False
///         otherwise. (which is a sign that the system is constantly overloaded)
template <class T>
bool JobScheduler<T>::ProcessTasks() {
    for(const auto& t : task_ids_) {
        processed_tasks_[t] = false;
    }

    // get the tasks which are not dependent on any other tasks
    // if this is empty we likely have some cyclic dependencies and cannot perform work
    QueueUpIndependentTasks();

    while(!task_queue_.empty()) {
        ExecutionContext processing = task_queue_.front();
        task_queue_.pop();

        if(!EnforceLoadLimit()) {
            std::cerr << "Tasks taking too long to finish. System overloaded. EXIT" << std::endl;
            return false;
        }

        uint32_t task_id = processing.ExecutionId();
    	uint32_t sleep_time_sec = task_id;
    	uint32_t data = task_id;
    	// schedule this task (note, this does not mean it will be executed right away.)
    	processing.Execute(
    	    std::bind(CreateWork(), sleep_time_sec, data),
    	    system_);

        for(const auto& next : task_adj_list_[processing.ExecutionId()]) {
            indegrees_[next]--;
            if(!processed_tasks_.at(next) && indegrees_[next] == 0) {
                task_queue_.push(ExecutionContext(next, parent_tasks_[next]));
                processed_tasks_[next] = true;
            }
        }
    }

    return true;
}

// explicit instantiation(s) of JobScheduler
template class JobScheduler<std::string>;
