
#include "gtest/gtest.h"

#include "./../JobScheduler.h"

class TestJobSchedulerFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            global_state_ptr = std::make_shared<GlobalState<std::string>>();
            job_ptr = std::make_shared<JobScheduler<std::string>>(global_state_ptr);
        }
  		
        void TearDown() override {}

        std::shared_ptr<JobScheduler<std::string>> job_ptr;
        std::shared_ptr<GlobalState<std::string>> global_state_ptr;
};

// test overall functionality
TEST_F(TestJobSchedulerFixture, TestA) {

	/*
         0  1
        / \/ \
        2  3  4 -- 6
        \     /
         5----  
	*/

    // AddTask(int task_id, int depends_on_task_id) 
    job_ptr->AddTask(2, 0); // means that task #2 depends on task #0
    job_ptr->AddTask(3, 0);
    job_ptr->AddTask(3, 1);
    job_ptr->AddTask(4, 1);
    job_ptr->AddTask(5, 2);
    job_ptr->AddTask(4, 5);
    job_ptr->AddTask(6, 4);
	
    bool success = job_ptr->ProcessTasks();
    // job scheduler has processed and scheduled all tasks
    // Note, this does not mean all the tasks have been finished
    EXPECT_TRUE(success);
	
    // sleeping long enough to ensure all tasks have executed
    std::this_thread::sleep_for(std::chrono::seconds(30));
    std::string state = global_state_ptr->GetState();
    
    EXPECT_EQ(state, "0132546");
}


// test overall functionality
TEST_F(TestJobSchedulerFixture, TestB) {

    /*
	
          0
         / \
        1   2
       / \   \
       3  4   5
	    \__\___\
                6
	
	*/

    // AddTask(int task_id, int depends_on_task_id) 
    job_ptr->AddTask(1, 0); // means that task #1 depends on task #0
    job_ptr->AddTask(2, 0);
    job_ptr->AddTask(3, 1);
    job_ptr->AddTask(4, 1);
    job_ptr->AddTask(5, 2);
    job_ptr->AddTask(6, 3);
    job_ptr->AddTask(6, 4);
    job_ptr->AddTask(6, 5);
	
    bool success = job_ptr->ProcessTasks();
    // job scheduler has processed and scheduled all tasks
    // Note, this does not mean all the tasks have been finished
    EXPECT_TRUE(success);
	
    // sleeping long enough to ensure all tasks have executed
    std::this_thread::sleep_for(std::chrono::seconds(30));
    std::string state = global_state_ptr->GetState();
    
    EXPECT_EQ(state, "0134256");    
}


// test corner cases
TEST_F(TestJobSchedulerFixture, TestEmpty) {

	/*
	    < NO TASKS >
	*/
	
    bool success = job_ptr->ProcessTasks();
    // job scheduler has processed and scheduled all tasks
    // Note, this does not mean all the tasks have been finished
    EXPECT_TRUE(success);
	
    // sleeping long enough to ensure all tasks have executed
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::string state = global_state_ptr->GetState();
    
    EXPECT_EQ(state, "");    
}
