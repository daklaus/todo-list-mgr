/*
 * preferences.cxx
 *
 *  Created on: Sep 22, 2013
 *      Author: klaus
 */

#include "preferences.hxx"

/*
 * Constructors/destructors
 */
preferences::preferences() :
		showDoneTasks_(false) {
}

preferences::~preferences() {
}

/*
 * Member functions
 */
bool preferences::showDoneTasks() const {
	return showDoneTasks_;
}

void preferences::setShowDoneTasks(bool showDoneTasks) {
	showDoneTasks_ = showDoneTasks;
}
