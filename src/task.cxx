/*
 * task.cxx
 *
 *  Created on: Sep 17, 2013
 *      Author: klaus
 */

#include "task.hxx"

using namespace std;

/*
 * Constructors/destructors
 */
task::task(const string& description, const time_t& due,
		const priority_t& priority) :
		id_(0), description_(description), created_(time(NULL)), due_(due), updated_(
				time(NULL)), priority_(priority), done_(false) {
}

/*
 * Member functions
 */
time_t task::getCreated() const {
	return created_;
}

const string& task::getDescription() const {
	return description_;
}

void task::setDescription(const string& description) {
	description_ = description;
	setUpdated();
}

bool task::isDone() const {
	return done_;
}

void task::setDone(bool done) {
	done_ = done;
	setUpdated();
}

time_t task::getDue() const {
	return due_;
}

void task::setDue(time_t due) {
	due_ = due;
	setUpdated();
}

unsigned long task::getId() const {
	return id_;
}

priority_t task::getPriority() const {
	return priority_;
}

void task::setPriority(priority_t priority) {
	priority_ = priority;
	setUpdated();
}

time_t task::getUpdated() const {
	return updated_;
}

void task::setUpdated() {
	updated_ = time(NULL);
}
