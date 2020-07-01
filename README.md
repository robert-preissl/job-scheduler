# job-scheduler
Topological sorting of dependent tasks via Kahn's algorithm + resource optimal execution via threads

<br />
<br />

# High-level goal

Design of a (C++ based) job scheduling algorithm to execute on interdepend tasks.

Tasks are represented in a graph format. Following [Kahn's algorithm](https://en.wikipedia.org/wiki/Topological_sorting#Kahn's_algorithm)
a topological ordering is possible if and only if the graph has no cycles, i.e., if it is a directed acyclic graph (DAG).
(Any DAG has at least one topological ordering)

Tasks scheduled in such a topological order are executed by independent threads (via *std::async*).
A main objective of this scheduler is to ensure that a certain constant number of actively
running executions is not exceeded. This is to ensure tasks are given optimal resources
to finish execution rather than executing hundreds or thousands of tasks at the same time.

<br />
<br />

# Example

          0     1
         * *   * *
        /   \ /   \
        2    3     4 *-- 6
        *         / 
         \       /
           ---5 *  
         
This graph highlights the following relationship among tasks: For instance, task #2 and task #3 depend
on task #0. Task #3 also depends on task #1. Task #5 depends on task #2. etc.

In addition, the time to finish a task varies. For instance task #2 takes +10 seconds to finish,
whereas task #0 or task#1 are relatively fast to execute.

Using this example, the job scheduler performs a topological sorting of tasks and starts
scheduling. Initially, we see tasks #0, #1 getting scheduled and executed on different
threads due to their indegrees of zero.
Right after, we start up task #2 (due to the BFS nature of Kahn's algorithm this is the next
task. which takes a while to run). However, following our approach our
job scheduler does not stall and schedules the next task, task #3 (since it indegree became zero after
task #0 and #1 were scheduled).
At this point no further task can be scheduled and executed until #2 gets done.
(#5 depends on #2. and #4 depends on #5 and #6 depends on #4).

Also, the system takes care of not over scheduling, i.e., putting too much load on the system
by keeping the number of actively running tasks below a certain threshold.

The resulting operating sequence is 0, 1, 3, 2, 5, 4, 6

<br />
<br />

# Building blocks

Two important building blocks of this implementation:

## Scheduler

Applies Kahn's algorithm to create and schedule tasks. It also ensure not to overload the system.

## Execution Context

A task created by the scheduler is in fact called an "execution context" in this project.
An execution context has the most minimal knowledge to be effective: it knows about what
work it has to perform (here, execution of a simple std::function on some shared global state) and also on what other tasks
it depends on. if there are other tasks which are still running we stall until they are done.
In the code, the waiting of parent tasks plus the work execution are chained together and
passed to *std::async*, which result is a *std::future* we keep track of.

<br />
<br />

# Correctness -- static and dynamic code analysis:

In this project several static analysis tools were applied to ensure stability and robustness.
Also, a dynamic analysis approach has been applied.
Implementation can be seen in the *Makefile*

-  [Address Sanitizer (Static analysis](https://clang.llvm.org/docs/AddressSanitizer.html)
-  [Thread Safety Analysis (Static analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html)
-  [Memory Sanitizer (Dynamic analysis)](https://clang.llvm.org/docs/MemorySanitizer.html)

In addition, [scan-build make](https://clang-analyzer.llvm.org/scan-build.html) can be run
as an alternative static analysis approach.

<br />
<br />

# Testing

Testing of some simple scenarios done via GTest suite.



