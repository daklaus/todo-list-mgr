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
#include <list>
#include <memory>
#include <stdexcept>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

class task;

typedef std::vector<odb::lazy_shared_ptr<task> > tasks;

#pragma db object table("todo_list")
class todoList {

private:
	friend class odb::access;

	todoList() {
	}

#pragma db id
	std::string title_;
#pragma db options("DEFAULT CURRENT_TIMESTAMP")
	std::time_t created_;
#pragma db value_not_null inverse(todoLists_)
	tasks tasks_;

public:
	todoList(const std::string& title);

	const std::string& getTitle() const;
	void setTitle(const std::string& title);

	std::time_t getCreated() const;

	const tasks& getTasks() const;

	void addTask(odb::lazy_shared_ptr<task>& task);
	void addTask(odb::lazy_shared_ptr<task>& task, int position);
	void removeTask(int position);

};

#ifdef ODB_COMPILER
#	include "task.hxx"
#endif

#endif /* TODOLIST_HXX_ */
