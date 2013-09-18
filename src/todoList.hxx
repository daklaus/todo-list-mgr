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
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>

class task;

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
	std::vector<odb::lazy_shared_ptr<task> > tasks_;

public:
	todoList(const std::string& title);

	const std::string& getTitle() const;
	void setTitle(const std::string& title);

	std::time_t getCreated() const;

	const std::vector<odb::lazy_shared_ptr<task> >& getTasks() const;
};

#ifdef ODB_COMPILER
#	include "task.hxx"
#endif

#endif /* TODOLIST_HXX_ */
