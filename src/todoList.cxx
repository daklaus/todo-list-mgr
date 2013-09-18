/*
 * todoList.cxx
 *
 *  Created on: Sep 17, 2013
 *      Author: klaus
 */

#include "todoList.hxx"

using namespace std;

/*
 * Constructors/destructors
 */
todoList::todoList(const std::string& title) :
		title_(title), created_(time(NULL)) {
}

/*
 * Member functions
 */
const std::string& todoList::getTitle() const {
	return title_;
}

void todoList::setTitle(const std::string& title) {
	title_ = title;
}

const std::vector<odb::lazy_shared_ptr<task> >& todoList::getTasks() const {
	return tasks_;
}

std::time_t todoList::getCreated() const {
	return created_;
}
