/*
 * preferences.hxx
 *
 *  Created on: Sep 22, 2013
 *      Author: klaus
 */

#ifndef PREFERENCES_HXX_
#define PREFERENCES_HXX_

class preferences {
	/*
	 * Class members
	 */
private:
	bool showDoneTasks_;
public:
	bool showDoneTasks() const;
	void setShowDoneTasks(bool showDoneTasks);

	/*
	 * Implement singleton pattern
	 */
private:
	preferences();
	virtual ~preferences();
	preferences(preferences const&);
	void operator=(preferences const&);
public:
	static preferences& getInstance() {
		static preferences instance;
		return instance;
	}
};

#endif /* PREFERENCES_HXX_ */
