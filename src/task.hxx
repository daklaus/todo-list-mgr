/*
 * task.hxx
 *
 *  Created on: Sep 12, 2013
 *      Author: klaus
 */

#ifndef TASK_HXX
#define TASK_HXX

#include <string>
#include <ctime>
#include <memory>
#include <stdexcept>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/vector.hxx>

class todoList;

#define stringify( x ) # x

typedef odb::vector<odb::lazy_weak_ptr<todoList> > todoLists;

enum priority_t { //
	lowest = 0, //
	low, //
	normal, //
	high, //
	highest //
};
const std::vector<std::string> priority_strings = { //
		stringify(lowest), //
				stringify(low), //
				stringify(normal), //
				stringify(high), //
				stringify(highest) //
		};

#pragma db object pointer(std::shared_ptr)
class task {

private:
	friend class odb::access;

	task() {
	}

#pragma db id auto
	unsigned long id_;
	std::string description_;
	// TODO: Timestamp problems with sqlite
#pragma db options("DEFAULT (datetime('now','localtime'))")
	std::time_t created_;
#pragma db null
	std::time_t due_;
#pragma db options("DEFAULT (datetime('now','localtime'))")
	std::time_t updated_;
#pragma db default(normal)
	priority_t priority_;
#pragma db default(false)
	bool done_;
#pragma db value_not_null inverse(tasks_)
	todoLists todoLists_;

	void setUpdated();

public:
	task(const std::string& description, const std::time_t& due = 0,
			const priority_t& priority = normal);

	std::time_t getCreated() const;

	const std::string& getDescription() const;
	void setDescription(const std::string& description);

	bool isDone() const;
	void setDone(bool done);

	std::time_t getDue() const;
	void setDue(std::time_t due);

	unsigned long getId() const;

	priority_t getPriority() const;
	void setPriority(priority_t priority);

	std::time_t getUpdated() const;

	const todoLists& getTodoLists() const;
};

#ifdef ODB_COMPILER
#	include "todoList.hxx"
#endif

#endif // TASK_HXX
