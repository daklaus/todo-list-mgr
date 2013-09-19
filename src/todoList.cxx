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

const tasks& todoList::getTasks() const {
	return tasks_;
}

std::time_t todoList::getCreated() const {
	return created_;
}

void todoList::addTask(odb::lazy_shared_ptr<task>& task) {
	tasks_.push_back(task);
}

void todoList::addTask(odb::lazy_shared_ptr<task>& task, int position) {
	tasks::iterator pos(tasks_.begin() + position);

	if (pos > tasks_.end() || pos < tasks_.begin())
		throw out_of_range(
				"The position is out of the range of the tasks vector");

	tasks_.insert(pos, task);
}

void todoList::removeTask(int position) {
	tasks::iterator pos(tasks_.begin() + position);

	if (pos > tasks_.end() - 1 || pos < tasks_.begin())
		throw out_of_range(
				"The position is out of the range of the tasks vector");

	tasks_.erase(pos);
}
