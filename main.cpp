
#include "JobScheduler.h"

int main() {
	std::shared_ptr<GlobalState<std::string>> global_state_ptr =
	    std::make_shared<GlobalState<std::string>>();
	    
	std::unique_ptr<JobScheduler<std::string>> job_ptr =
	    std::make_unique<JobScheduler<std::string>>(global_state_ptr);
	
    // AddTask(int task_id, int depends_on_task_id) 
	job_ptr->AddTask(2, 0); // means that task #2 depends on task #0
	job_ptr->AddTask(3, 0);
	job_ptr->AddTask(3, 1);
	job_ptr->AddTask(4, 1);
	job_ptr->AddTask(5, 2);
	job_ptr->AddTask(4, 5);
	job_ptr->AddTask(6, 4);
	
	/*
	
	     0  1
	    / \/ \
	    2  3  4 -- 6
	    \     /
	     5----  
	*/
	
	job_ptr->PrintIndegrees();	
	bool success = job_ptr->ProcessTasks();
	std::cout << "Success = " << success << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(20));

	global_state_ptr->Print();
	
	return 0;
}
