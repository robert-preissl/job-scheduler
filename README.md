# job-scheduler
Topological sorting of dependent tasks via Kahn's algorithm + resource constraint execution via threads

# High-level goal:

Design of a (C++ based) job scheduling algorithm to execute on interdepend tasks.

Tasks are represented in a graph format. Following [Kahn's algorithm](https://en.wikipedia.org/wiki/Topological_sorting#Kahn's_algorithm)
a topological ordering is possible if and only if the graph has no cycles, i.e., if it is a directed acyclic graph (DAG).
(Any DAG has at least one topological ordering)

Tasks scheduled in such a topological order are executed by independent threads (via *std::async*).
A main objective of this scheduler is to ensure that a certain constant number of actively
running executions is not exceeded. This is to ensure tasks are given optimal resources
to finish execution rather than executing hundreds or thousands of tasks at the same time.

# Example

          0     1
         * *   * *
        /   \ /   \
        2    3     4 *-- 6
        *         / 
         \       /
           ---5 *  
         
This graph highlights the following relationship among tasks: For instance, task #2 and task #3 depend
on task #0, whereas task #3 also depends on task #1. Task #5 depends on task #2. etc.

In addition, the time to finish a task varies. For instance task #2 takes +10 seconds to finish,
whereas task #0 or task#1 are relatively fast to execute.

Using this example, the job scheduler performs a topological sorting of tasks and starts
scheduling. Initially, we see tasks #0, #1 and executed on different
threads due to the indegrees of zero.
Right after we start up task #2 (which takes a while to run). With this approach our
job scheduler does not stall and schedules the next task, task #3 (since it indegree became zero after
task #0 and #1 were scheduled).
At this point no further task can be scheduled and executed until #2 gets done.
(#5 depends on #2. and #4 depends on #5 and #6 depends on #4).

# Correctness -- static and dynamic code analysis:



# Testing

Testing of some simple scenarios done via GTest suite.



