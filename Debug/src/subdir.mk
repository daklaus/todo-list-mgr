################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CXX_SRCS += \
../src/preferences.cxx \
../src/stringFunctions.cxx \
../src/task-odb.cxx \
../src/task.cxx \
../src/todo-list-mgr.cxx \
../src/todoList-odb.cxx \
../src/todoList.cxx 

OBJS += \
./src/preferences.o \
./src/stringFunctions.o \
./src/task-odb.o \
./src/task.o \
./src/todo-list-mgr.o \
./src/todoList-odb.o \
./src/todoList.o 

CXX_DEPS += \
./src/preferences.d \
./src/stringFunctions.d \
./src/task-odb.d \
./src/task.d \
./src/task-odb.d \
./src/todo-list-mgr.d \
./src/todoList-odb.d \
./src/todoList.d \
./src/todoList-odb.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cxx
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDATABASE_SQLITE -DDEBUG -O0 -g3 -pedantic -Wall -c -fmessage-length=0 -std=c++11 -Wno-unknown-pragmas -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

../src/task-odb.hxx: ../src/task.hxx
	@echo 'Building file: $<'
	@echo 'ODB Compiler: task.hxx'
	odb --std c++11 --database sqlite --generate-query --generate-schema --generate-session --output-dir ../src ../src/task.hxx
	@echo 'Finished building: $<'
	@echo ' '

../src/task-odb.cxx: ../src/task-odb.hxx
../src/task-odb.ixx: ../src/task-odb.hxx
../src/task.sql: ../src/task-odb.hxx

src/%.o: ./src/%.cxx
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDATABASE_SQLITE -DDEBUG -O0 -g3 -pedantic -Wall -c -fmessage-length=0 -std=c++11 -Wno-unknown-pragmas -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

../src/todoList-odb.hxx: ../src/todoList.hxx
	@echo 'Building file: $<'
	@echo 'ODB Compiler: todoList.hxx'
	odb --std c++11 --database sqlite --generate-query --generate-schema --generate-session --output-dir ../src ../src/todoList.hxx
	@echo 'Finished building: $<'
	@echo ' '

../src/todoList-odb.cxx: ../src/todoList-odb.hxx
../src/todoList-odb.ixx: ../src/todoList-odb.hxx
../src/todoList.sql: ../src/todoList-odb.hxx


