/*
 * todoList.hxx
 *
 *  Created on: Sep 17, 2013
 *      Author: klaus
 */

#ifndef TODOLIST_HXX_
#define TODOLIST_HXX_

#include <string>
#include <ctime>
#include <memory>
#include <stdexcept>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/vector.hxx>

class task;
//#include "task.hxx"

typedef std::vector<odb::lazy_shared_ptr<task> > tasks;

#pragma db object table("todo_list") pointer(std::shared_ptr)
class todoList {

private:
	friend class odb::access;

	todoList() {
	}

#pragma db id
	std::string title_;
#pragma db options("DEFAULT (strftime('%s'))")
	std::time_t created_;
#pragma db value_not_null \
		id_column("todo_list_title") \
		value_column("task_id") \
		table("contains")
	tasks tasks_;

public:
	todoList(const std::string& title);

	const std::string& getTitle() const;
	void setTitle(const std::string& title);

	std::time_t getCreated() const;

	const tasks& getTasks() const;

	void addTask(const tasks::value_type& task);
	void addTask(const tasks::value_type& task, size_t position);
	void removeTask(size_t position);
	void removeTask(const task& ta);
};

#ifdef ODB_COMPILER
#	include "task.hxx"
#endif

#endif /* TODOLIST_HXX_ */
