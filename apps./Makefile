# Dependency tracking
# Target library
lib := libuthread.a

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Werror -MMD -D_XOPEN_SOURCE
CFLAGS += -g

# Additional flags for specific files
context.o: CFLAGS += -Wno-deprecated-declarations

# Source files
SRCS := queue.c uthread.c context.c sem.c preempt.c
OBJS := $(SRCS:.c=.o)

# Test Source files
TEST_SRCS := queue_tester.c uthread_hello.c uthread_yield.c sem_simple.c sem_count.c sem_prime.c sem_buffer.c
TEST_OBJS := $(TEST_SRCS:.c=.o)
TEST_EXEC := queue_tester uthread_hello uthread_yield sem_simple sem_count sem_prime sem_buffer

# Default target
all: $(lib) $(TEST_EXEC)
deps := $(OBJS:.o=.d)
-include $(deps)

# Library target
$(lib): $(OBJS)
	@echo "Creating archive $@"
	$(Q)ar rcs $@ $^

# Object file targets
%.o: %.c
	@echo "CC    $@"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

# Test targets
queue_tester: queue_tester.o $(lib)
	@echo "LD    $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^
sem_count: sem_count.o $(lib)
	@echo "LD    $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^
sem_buffer: sem_buffer.o $(lib)
	@echo "LD    $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^
sem_prime: sem_prime.o $(lib)
	@echo "LD    $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^
sem_simple: sem_simple.o $(lib)
	@echo "LD    $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

uthread_hello: uthread_hello.o $(lib)
	@echo "LD    $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

uthread_yield: uthread_yield.o $(lib)
	@echo "LD    $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

# Clean target
clean:
	@echo "CLEAN"
	$(Q)rm -f $(OBJS) $(lib) $(OBJS:.o=.d) $(TEST_OBJS) $(TEST_EXEC) $(TEST_OBJS:.o=.d)
