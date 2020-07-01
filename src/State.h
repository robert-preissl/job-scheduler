
#pragma once

#include "mutex.h"

#include <iostream>

/// \brief Simple representation of some global state, which is changed by tasks scheduled
///        by the job scheduler. Here, all tasks execute the same function "Add" to change
///        state. This can be enhanced to be more complex, but kept simple for illustration
///        purposes.
template <class T>
class GlobalState {
    private:
        // use clang's thread safety analyzer https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
        Mutex mutex_;
        T state_ GUARDED_BY(mutex_); // ensure that shared global state alteration is
                                     //  performed in thread safe manner

    public:
        GlobalState() : state_() {}
        ~GlobalState() {
            std::cout << "GlobalState destructed!" << std::endl;
        }
		
        /// \brief Change state
        /// \param[in] Some data element, which is incorporated into the state
        /// \return None.
        void Add(T x) {
            // protect access of this shared data among multiple threads
            mutex_.Lock();
            state_ += x;
            mutex_.Unlock();
        }
		
        /// \brief Print state
        /// \return None.
        void Print() {
            mutex_.Lock();
            std::cout << "state = " << state_ << std::endl;
            mutex_.Unlock();
        }

        /// \brief Return state in a thread safe manner.
        /// \return state.
        T GetState() {
            T state_copy;
            mutex_.Lock();
            state_copy = state_;
            mutex_.Unlock();
            return state_copy;
        }
};
