
CXX = g++
CXX_VERSION =-std=c++17
CXX_OPT = -g -O3

# treat warnings as error
CXX_WARNINGS=-Werror



# ----------------------  for Static & Dynamic analysis ----------------------

# activate clang's AddressSanitizer (a fast memory error detector)
# https://clang.llvm.org/docs/AddressSanitizer.html:
ADDRESS_SANITIZER = -fsanitize=address -fno-omit-frame-pointer

# activate clang's Memory sanitizer (dynamic)
# https://clang.llvm.org/docs/MemorySanitizer.html
MEMORY_SANITIZER = -fsanitize-memory-track-origins

# activate clang's ThreadSafetyAnalysis:
# https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
THREAD_SAFETY_ANALYZER = -Wthread-safety
 
CXXFLAGS=$(CXX_VERSION) $(CXX_OPT) $(ADDRESS_SANITIZER)
# ----------------------  (end) Static & Dynamic analysis ----------------------




# ----------------------  for GTest: ----------------------
# the root directory for googletest
GTEST_DIR = /Users/robert/Downloads/googletest-release-1.7.0/

# the path where googletest has been installed (where "make was executed)
GTEST_BUILD_DIR = $(GTEST_DIR)/build

GTEST_INCL = -I $(GTEST_DIR)/include/ -I $(GTEST_DIR)/include/gtest/internal/
GTEST_LINK = -L$(GTEST_BUILD_DIR) -lgtest -lgtest_main
# ----------------------  end GTest ----------------------




DEPS = src/ExecutionContext.h src/System.h src/State.h
OBJ_MAIN = src/JobScheduler.o src/System.o src/main.o
OBJ_TEST = src/JobScheduler.o src/System.o test/testAllMain.o test/TestJobScheduler.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(THREAD_SAFETY_ANALYZER) $(CXX_WARNINGS)

job_scheduler: $(OBJ_MAIN)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(THREAD_SAFETY_ANALYZER) $(MEMORY_SANITIZER)

test_job_scheduler: $(OBJ_TEST)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(GTEST_INCL) $(GTEST_LINK)

clean:
	rm job_scheduler test_job_scheduler src/*.o test/*.o
