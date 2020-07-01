
#pragma once

#include "System.h"

// #define __DEBUG__


/// \brief Chain two functions together
/// \param[in] first function with return type void and no arguments
/// \param[in] second function with return type void and no arguments
/// \return None
static void Chain(std::function<void()> f1,
                  std::function<void()> f2) {
    f1();
    f2();
}

/// \brief An ExecutionContext exists part of a job. It has enough information to execute
///        I.e., it knows about its dependencies on other tasks and is provided a function
///        to execute on.
class ExecutionContext {
    private:
        uint32_t id_;
        std::vector<uint32_t> parent_ids_; // dependencies on these other executions/tasks
		
	public:
        ExecutionContext(uint32_t id) : id_(id) {}
        ExecutionContext(uint32_t id,
                         const std::vector<uint32_t>& parent_ids) :
                             id_(id),
                             parent_ids_(parent_ids) {}

        /// \brief Return the id of this execution context
        /// \return Id
        uint32_t ExecutionId() {
            return id_;
        }
		
        /// \brief Execute a given function within a certain execution context (meaning
        ///        start the execution when tasks we depend on are done.
        ///        In this function execution is handled by independent threads
        /// \param[in] Function to be executed. Parameterless, i.e., the job scheduler has
        ///            already provided/bound all the necessary params. Also no return
        ///            value for the given function since it operates on a state object.
        /// \param[in] A representation of the underlying system, which keeps track of
        ///            running workers (here: threads) and their state
        /// \return None
        void Execute(std::function<void()> work,
                     System& s) {

            #ifdef __DEBUG__
                std::cout << "Preparing ExecutionContext: #" << id_ << std::endl;
            #endif
    		
            std::vector<uint32_t> parent_ids = parent_ids_;
            std::function<void()> wait_for_parent_tasks = [parent_ids, &s]() -> void {
                s.WaitForTasks(parent_ids); };

            // fire up a new asynchronous execution for the given work function. A few
            //  important comments:
            // 1. before executing the given function, we ensure dependent tasks are done
            // 2. the return of std::async is a future (result), which cannot be copied
            //    to the "System" keeping track of active calculations. it has to be moved
            s.AddTask(id_,
                      std::move(std::async(Chain, wait_for_parent_tasks, work))
                      );
			
            #ifdef __DEBUG__
                std::cout << "ExecutionContext: #" << id_ << " scheduled " << std::endl;
            #endif
        }
};

