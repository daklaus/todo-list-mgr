/*
 * todo-list-mgr.cxx
 *
 *  Created on: Sep 22, 2013
 *      Author: klaus
 */

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <ios>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <fstream>
#include <locale>

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "database.hxx"
#include "task.hxx"
#include "task-odb.hxx"
#include "todoList.hxx"
#include "todoList-odb.hxx"
#include "stringFunctions.hxx"
#include "preferences.hxx"

using namespace std;
using namespace odb::core;

/*
 * Enums
 */
enum menu_t {
	mainmenu, taskmenu, listmenu, preferencesmenu
};

/*
 * Typedefs (1 of 2)
 */
typedef void menufunc_ret_t;
typedef task& taskmenufunc_param_t;
typedef todoList& listmenufunc_param_t;

/*
 * Static functions (1 of 2)
 */
static void mainMenu(void);
static menufunc_ret_t showAllTasks(void);
static menufunc_ret_t showList(void);
static menufunc_ret_t createTask(void);
static menufunc_ret_t editTask(void);
static menufunc_ret_t createList(void);
static menufunc_ret_t editList(void);
static menufunc_ret_t editPreferences(void);
static menufunc_ret_t showMainMenu(void);
static menufunc_ret_t doNothing(void) {
}
static menufunc_ret_t editDescription(taskmenufunc_param_t);
static menufunc_ret_t editDue(taskmenufunc_param_t);
static menufunc_ret_t editPriority(taskmenufunc_param_t);
static menufunc_ret_t toggleDone(taskmenufunc_param_t);
static menufunc_ret_t addTaskToList(taskmenufunc_param_t);
static menufunc_ret_t removeTaskFromList(taskmenufunc_param_t);
static menufunc_ret_t deleteTask(taskmenufunc_param_t);
static menufunc_ret_t showTask(taskmenufunc_param_t);
static menufunc_ret_t showTaskMenu(taskmenufunc_param_t);
static menufunc_ret_t doNothingWithTask(taskmenufunc_param_t) {
}
static menufunc_ret_t renameList(listmenufunc_param_t);
static menufunc_ret_t deleteList(listmenufunc_param_t);
static menufunc_ret_t moveTask(listmenufunc_param_t);
static menufunc_ret_t showListMenu(listmenufunc_param_t);
static menufunc_ret_t doNothingWithList(listmenufunc_param_t) {
}
static menufunc_ret_t toggleShowDoneTasks(void);
static menufunc_ret_t showPrefMenu(void);

/*
 * Structs
 */
struct menuEntryBase {
	const string entry;
//	const char key;

//	menuEntry(const string& entry, const char& key, const menufunc_t& function) :
//			entry(entry), key(key), function(function) {
//	}

protected:
	menuEntryBase(const string& entry) :
			entry(entry) {
	}
};

typedef menufunc_ret_t (*menufunc_t)(void);
struct menuEntry: public menuEntryBase {
	const menufunc_t function;

	menuEntry(const string& entry, const menufunc_t& function) :
			menuEntryBase(entry), function(function) {
	}
};

typedef menufunc_ret_t (*taskmenufunc_t)(taskmenufunc_param_t);
struct taskMenuEntry: public menuEntryBase {
	const taskmenufunc_t function;

	taskMenuEntry(const string& entry, const taskmenufunc_t& function) :
			menuEntryBase(entry), function(function) {
	}
};

typedef menufunc_ret_t (*listmenufunc_t)(listmenufunc_param_t);
struct listMenuEntry: public menuEntryBase {
	const listmenufunc_t function;

	listMenuEntry(const string& entry, const listmenufunc_t& function) :
			menuEntryBase(entry), function(function) {
	}
};

/*
 * Typedefs (2 of 2)
 */
//typedef unordered_map<char, const menuEntryBase> menu_entries_base_t;
typedef unordered_map<char, const menuEntry> menu_entries_t;
typedef unordered_map<char, const taskMenuEntry> taskmenu_entries_t;
typedef unordered_map<char, const listMenuEntry> listmenu_entries_t;

/*
 * Static functions (2 of 2)
 */
static char getCharFromLine(void);
static void printMenu(const menu_t& menu, const string& menuName);
static void readDue(time_t& due, bool askForOverwriteIfInputBlank);
static void readPriority(priority_t& priority,
		bool askForOverwriteIfInputBlank);
static shared_ptr<todoList> selectList(const string& caption,
		query<todoList> q = query<todoList>());
static string openEditor(const string& text = "");
static void printListTasks(const todoList& list);
static shared_ptr<task> selectTask();

/*
 * Static variables
 */
// Database instance
static db_ptr db;

// Menu structure
static const menu_entries_t mainMenuEntries = { //
		{ 'q', menuEntry("(Q)uit", doNothing) }, //
				{ 'm', menuEntry("Show this (m)enu", showMainMenu) }, //
				{ 'p', menuEntry("(P)references", editPreferences) }, //
				{ 'd', menuEntry("E(d)it list", editList) }, //
				{ 'r', menuEntry("C(r)eate list", createList) }, //
				{ 'e', menuEntry("(E)dit task", editTask) }, //
				{ 'c', menuEntry("(C)reate task", createTask) }, //
				{ 'l', menuEntry("Show (l)ist", showList) }, //
				{ 'a', menuEntry("Show (a)ll tasks", showAllTasks) } //
		};
static const taskmenu_entries_t taskMenuEntries = { //
		{ 'c', taskMenuEntry("(C)ancel", doNothingWithTask) }, //
				{ 'm', taskMenuEntry("Show this (m)enu", showTaskMenu) }, //
				{ 's', taskMenuEntry("(S)how task", showTask) }, //
				{ 'd', taskMenuEntry("(D)elete task", deleteTask) }, //
				{ 'r', taskMenuEntry("(R)emove task from list",
						removeTaskFromList) }, //
				{ 'a', taskMenuEntry("(A)dd task to list", addTaskToList) }, //
				{ 'o', taskMenuEntry("Toggle d(o)ne", toggleDone) }, //
				{ 'p', taskMenuEntry("Edit (p)riority", editPriority) }, //
				{ 'u', taskMenuEntry("Edit d(u)e", editDue) }, //
				{ 'e', taskMenuEntry("(E)dit description", editDescription) } //
		};
static const listmenu_entries_t listMenuEntries = { //
		{ 'c', listMenuEntry("(C)ancel", doNothingWithList) }, //
				{ 'm', listMenuEntry("Show this (m)enu", showListMenu) }, //
				{ 'v', listMenuEntry("Mo(v)e task", moveTask) }, //
				{ 'd', listMenuEntry("(D)elete list", deleteList) }, //
				{ 'r', listMenuEntry("(R)ename list", renameList) } //
		};
static const menu_entries_t prefMenuEntries = { //
		{ 'c', menuEntry("(C)ancel", doNothing) }, //
				{ 'm', menuEntry("Show this (m)enu", showPrefMenu) }, //
				{ 'd', menuEntry("Toggle show (d)one tasks",
						toggleShowDoneTasks) } //
		};

static const string menuPointPrefix = "* ";
static const string menuPromptPrefix = "> ";
static const string inputPromptPostfix = ": ";
static const string dateFormat = "%d.%m.%Y";
// TODO: Read editor from the command line with the Code Synthesis CLI project
static const string editor = "/usr/bin/vim";

/*
 * Main
 */
int main(int argc, char* argv[]) {
	/*
	 * Initialize database
	 */
	try {
		db = create_database(argc, argv);
	} catch (const odb::exception& e) {
		cerr << e.what() << endl;
		return 1;
	}

	mainMenu();

	return EXIT_SUCCESS;
}

/*
 * Static function definitions
 */
static void mainMenu(void) {
	showMainMenu();

	char choice;
	do {
		cout << menuPromptPrefix;
		choice = getCharFromLine();
		choice = tolower(choice);

		menu_entries_t::const_iterator entryIt(mainMenuEntries.find(choice));
		if (entryIt != mainMenuEntries.end()) {
			// If map contains the entry
			entryIt->second.function();
		}
	} while (choice != 'q' && !cin.eof());

	if (cin.eof()) {
		cout << endl;
	}
}

/*
 * List menu functions
 */

static menufunc_ret_t showMainMenu(void) {
	printMenu(mainmenu, "Main menu");
}

static menufunc_ret_t showAllTasks(void) {
	transaction t(db->begin());

	query<task> q;
	if (!preferences::getInstance().showDoneTasks()) {
		q = query<task>::done == false;
	}
	result<task> tasks(db->query<task>(q));

	if (tasks.empty()) {
		cout << "No tasks available!" << endl;
		return;
	}

	cout << "Task ID | Priority | ";
	if (preferences::getInstance().showDoneTasks()) {
		cout << "Done | ";
	}
	cout << "Description" << endl << "--------------------------------";
	if (preferences::getInstance().showDoneTasks()) {
		cout << "-------";
	}
	cout << endl;

	string done;
	string desc;
	for (task& ta : tasks) {
		cout << setw(7) << ta.getId() << " | " << setw(8)
				<< priority_strings.at(ta.getPriority()) << " | ";

		if (preferences::getInstance().showDoneTasks()) {
			done = "no";
			if (ta.isDone())
				done = "yes";
			cout << setw(4) << done << " | ";
		}
		desc = ta.getDescription();
		replace(desc.begin(), desc.end(), '\n', ' ');
		cout << desc.substr(0, 40) << endl;
	}
}

static menufunc_ret_t showList(void) {
	// Select list
	shared_ptr<todoList> list(selectList("Select a list to show"));
	if (!list) {
		return;
	}

	// Print tasks
	printListTasks(*list);
}

static menufunc_ret_t createTask(void) {

	// Read due time
	time_t due;
	readDue(due, false);

	if (cin.eof())
		return;

	// Read priority
	priority_t priority;
	readPriority(priority, false);

	if (cin.eof())
		return;

	// Select list
	shared_ptr<todoList> list(
			selectList("Select a list where to add the new task"));
	if (!list) {
		return;
	}

	// Enter description
	string desc(openEditor());

#ifdef DEBUG
	cout << "List before:" << endl;
	printListTasks(*list);
#endif

	lazy_shared_ptr<task> ta(*db, new task(desc, due, priority));
	list->addTask(ta);

	transaction t(db->begin());
	db->persist(*ta);
	db->update(list);
	t.commit();

#ifdef DEBUG
	cout << "List after:" << endl;
	printListTasks(*list);
#endif
}

static menufunc_ret_t editTask(void) {
	shared_ptr<task> ta(selectTask());
	if (!ta) {
		return;
	}

	cout << "Task values:" << endl;
	showTask(*ta);
	cout << endl;

	showTaskMenu(*ta);

	char choice;
	do {
		cout << "Edit task" << menuPromptPrefix;
		choice = getCharFromLine();
		choice = tolower(choice);

		taskmenu_entries_t::const_iterator entryIt(
				taskMenuEntries.find(choice));
		if (entryIt != taskMenuEntries.end()) {
			// If map contains the entry
			entryIt->second.function(*ta);
		}

		// Exit task menu after task is deleted (choice was 'd')
	} while (choice != 'c' && choice != 'd' && !cin.eof());
}

static menufunc_ret_t createList(void) {
	cout << "List title" << inputPromptPostfix;

	string input;
	getline(cin, input);
	if (cin.eof())
		return;
	input = trim(input);

	shared_ptr<todoList> list(new todoList(input));

	transaction t(db->begin());
	db->persist(list);
	t.commit();
}

static menufunc_ret_t editList(void) {
	shared_ptr<todoList> li(selectList("Select list to edit"));
	if (!li) {
		return;
	}

	showListMenu(*li);

	char choice;
	do {
		cout << "Edit list" << menuPromptPrefix;
		choice = getCharFromLine();
		choice = tolower(choice);

		listmenu_entries_t::const_iterator entryIt(
				listMenuEntries.find(choice));
		if (entryIt != listMenuEntries.end()) {
			// If map contains the entry
			entryIt->second.function(*li);
		}
	} while (choice != 'c' && !cin.eof());
}

static menufunc_ret_t editPreferences(void) {
	showPrefMenu();

	char choice;
	do {
		cout << "Edit preferences" << menuPromptPrefix;
		choice = getCharFromLine();
		choice = tolower(choice);

		menu_entries_t::const_iterator entryIt(prefMenuEntries.find(choice));
		if (entryIt != prefMenuEntries.end()) {
			// If map contains the entry
			entryIt->second.function();
		}
	} while (choice != 'c' && !cin.eof());
}

/*
 * Task menu functions
 */

static menufunc_ret_t showTaskMenu(taskmenufunc_param_t task) {
	printMenu(taskmenu, "Edit task menu");
}

static menufunc_ret_t editDescription(taskmenufunc_param_t task) {

	task.setDescription(openEditor(task.getDescription()));

	transaction t(db->begin());
	db->update(task);
	t.commit();
}

static menufunc_ret_t editDue(taskmenufunc_param_t task) {

	time_t due(task.getDue());
	readDue(due, true);

	task.setDue(due);

	transaction t(db->begin());
	db->update(task);
	t.commit();
}

static menufunc_ret_t editPriority(taskmenufunc_param_t task) {

	priority_t priority(task.getPriority());
	readPriority(priority, true);

	task.setPriority(priority);

	transaction t(db->begin());
	db->update(task);
	t.commit();
}

static menufunc_ret_t toggleDone(taskmenufunc_param_t task) {

	task.setDone(!task.isDone());

	transaction t(db->begin());
	db->update(task);
	t.commit();

	cout << "Task set to ";
	if (task.isDone())
		cout << "done";
	else
		cout << "undone";
	cout << "!" << endl;
}

static menufunc_ret_t addTaskToList(taskmenufunc_param_t task) {
	// TODO: implement
	cout << "Sorry, this function is not yet implemented" << endl;

}

static menufunc_ret_t removeTaskFromList(taskmenufunc_param_t task) {
	// TODO: implement
	cout << "Sorry, this function is not yet implemented" << endl;

}

static menufunc_ret_t deleteTask(taskmenufunc_param_t task) {
	// TODO: implement
	cout << "Sorry, this function is not yet implemented" << endl;

}

static menufunc_ret_t showTask(taskmenufunc_param_t task) {
	cout << "Task ID: " << task.getId() << endl;
	cout << "Priority: " << priority_strings.at(task.getPriority()) << endl;
	cout << "Done: " << boolalpha << task.isDone() << noboolalpha << endl;

	{
		char timeStrBuffer[50];
		time_t rawTime(task.getCreated());
		tm *brokenTime(localtime(&rawTime));
		strftime(timeStrBuffer, 50, dateFormat.c_str(), brokenTime);
		cout << "Created: " << timeStrBuffer << endl;
	}

	{
		char timeStrBuffer[50];
		time_t rawTime(task.getUpdated());
		tm *brokenTime(localtime(&rawTime));
		strftime(timeStrBuffer, 50, dateFormat.c_str(), brokenTime);
		cout << "Last updated: " << timeStrBuffer << endl;
	}

	cout << "Due: ";
	if (task.getDue() <= 0) {
		cout << "none";
	} else {
		char timeStrBuffer[50];
		time_t rawTime(task.getDue());
		tm *brokenTime(localtime(&rawTime));
		strftime(timeStrBuffer, 50, dateFormat.c_str(), brokenTime);
		cout << timeStrBuffer;
	}
	cout << endl;

	transaction t(db->begin());
	cout << "Lists:" << endl;
	for (todoLists::const_iterator i(task.getTodoLists().begin());
			i != task.getTodoLists().end(); ++i) {
		cout << i->load()->getTitle() << endl;
	}

	cout << "Description:" << endl << task.getDescription() << endl;
}

/*
 * List menu functions
 */

static menufunc_ret_t renameList(listmenufunc_param_t list) {
	// TODO: implement
	cout << "Sorry, this function is not yet implemented" << endl;
}

static menufunc_ret_t deleteList(listmenufunc_param_t list) {
	// TODO: implement
	cout << "Sorry, this function is not yet implemented" << endl;
}

static menufunc_ret_t moveTask(listmenufunc_param_t list) {
	// TODO: implement
	cout << "Sorry, this function is not yet implemented" << endl;
}

static menufunc_ret_t showListMenu(listmenufunc_param_t list) {
	printMenu(listmenu, "Edit list menu");
}

/*
 * Preferences menu functions
 */

static menufunc_ret_t toggleShowDoneTasks(void) {
	preferences& pref = preferences::getInstance();
	pref.setShowDoneTasks(!pref.showDoneTasks());

	cout << "Done tasks will ";
	if (!pref.showDoneTasks())
		cout << "not ";
	cout << "be shown on task lists!" << endl;
}

static menufunc_ret_t showPrefMenu(void) {
	printMenu(preferencesmenu, "Edit task menu");
}

/*
 * Help functions
 */

static char getCharFromLine(void) {
	string input;

	getline(cin, input);
	input = trim(input);

	if (input.empty()) {
		return ' ';
	}

	return input.front();
}

static void readDue(time_t& due, bool askForOverwriteIfInputBlank) {
	string input;
	char answer;
	bool fail;
	time_t tmpDue;

	do {
		fail = false;
		tmpDue = 0;

		cout << "Due (default: none; Format: " << dateFormat << ")"
				<< inputPromptPostfix;

		getline(cin, input);
		input = trim(input);

		if (input.empty()) {
			if (askForOverwriteIfInputBlank) {
				cout
						<< "Do you really want to overwrite due with 'none' (y/n)? [n]"
						<< inputPromptPostfix;
				answer = getCharFromLine();
				answer = tolower(answer);
				if (answer != 'y') {
					return;
				}
			}
		} else {

			time_t rawTime(0);
			tm *brokenTimePtr(localtime(&rawTime));
			if (strptime(input.c_str(), dateFormat.c_str(),
					brokenTimePtr) == NULL) {
				fail = true;
			} else {
				tmpDue = mktime(brokenTimePtr);
				if (tmpDue == -1) {
					fail = true;
				}
			}
		}

		if (fail) {
			cout << "Parsing the date failed! Format is \"" << dateFormat
					<< "\"" << endl;
		}
	} while (fail);

	due = tmpDue;

#ifdef DEBUG
	cout << ctime(&due);
#endif
}

static void readPriority(priority_t& priority,
		bool askForOverwriteIfInputBlank) {
	string input;
	bool fail;
	priority_t tmpPriority;
	char answer;

	do {
		fail = false;
		tmpPriority = normal;

		cout << "Priority (default: " << tmpPriority << "; Values: 0-"
				<< priority_strings.size() - 1 << ")" << inputPromptPostfix;

		getline(cin, input);
		input = trim(input);

		if (input.empty()) {
			if (askForOverwriteIfInputBlank) {
				cout << "Do you really want to overwrite priority with "
						<< tmpPriority << " (y/n)? [n]" << inputPromptPostfix;
				answer = getCharFromLine();
				answer = tolower(answer);
				if (answer != 'y') {
					return;
				}
			}
		} else {

			size_t iPrio;
			istringstream iss(input);
			iss >> iPrio;

			if (iss.fail() || iPrio < 0
					|| iPrio > priority_strings.size() - 1) {
				fail = true;
			} else {
				tmpPriority = static_cast<priority_t>(iPrio);
			}
		}

		if (fail) {
			cout << "Invalid input!" << endl;
		}
	} while (fail);

#ifdef DEBUG
	cout << tmpPriority << endl;
#endif

	priority = tmpPriority;
}

static shared_ptr<task> selectTask() {

	string input;
	unsigned long taskId;
	bool fail;
	shared_ptr<task> ret;

	transaction t(db->begin());

	do {
		fail = false;
		taskId = 0;

		cout << "Enter task id" << inputPromptPostfix;

		getline(cin, input);
		input = trim(input);

		if (input.empty()) {
			return shared_ptr<task>();
		}

		istringstream iss(input);
		iss >> taskId;

		if (iss.fail()) {
			fail = true;
			cout << "Invalid input!" << endl;
		} else {
			try {
				ret = db->load<task>(taskId);
			} catch (odb::object_not_persistent& e) {
				cout << "No task with this id in the database!" << endl;
				fail = true;
			}
		}
	} while (fail);
//
#ifdef DEBUG
	cout << ret->getDescription() << endl;
#endif
	return ret;
}

static shared_ptr<todoList> selectList(const string& caption,
		query<todoList> q) {

	cout << caption << ":" << endl;

	transaction t(db->begin());

	result<todoList> lists(db->query<todoList>(q));

	if (lists.empty()) {
		cout << "No lists available!" << endl;
		return shared_ptr<todoList>();
	}

	std::vector<todoList> vLists;
	for (todoList& list : lists) {
		vLists.push_back(list);
	}

	bool fail;
	size_t j;
	size_t listIdx;
	do {
		fail = false;

		j = 0;
		for (std::vector<todoList>::iterator i(vLists.begin());
				i != vLists.end(); ++i, ++j) {
			ostringstream oss;
			oss << j + 1 << ")";
			cout << setw(3) << left << oss.str()
					<< resetiosflags(ios_base::adjustfield) << i->getTitle()
					<< endl;
		}
		cout << "Select" << inputPromptPostfix;

		string input;
		getline(cin, input);
		input = trim(input);

		if (input.empty()) {
			return shared_ptr<todoList>();
		}

		istringstream iss(input);
		iss >> listIdx;
		--listIdx;

		if (iss.fail() || listIdx < 0 || listIdx > vLists.size() - 1) {
			fail = true;
		}

		if (fail) {
			cout << "Invalid input!" << endl;
		}
	} while (fail);

#ifdef DEBUG
	cout << listIdx << endl;
#endif
	shared_ptr<todoList> ret(new todoList(vLists.at(listIdx)));

#ifdef DEBUG
	cout << ret->getTitle() << endl;
#endif

	return ret;
}

static string openEditor(const string& text) {
	// TODO: Improve the whole function later:
	// 1. tmp file name generation
	// 2. fork - exec - wait instead of system

	// Generate tmp file name
	char filename[L_tmpnam];
	tmpnam(filename);

	// Write into it
	fstream tmpfile(filename);
	tmpfile << text;
	tmpfile.close();

	// Execute editor
	std::string cmd = editor + " " + filename;
	system(cmd.c_str());

	// Read from it
	tmpfile.open(filename);
	stringstream buffer;
	buffer << tmpfile.rdbuf();
	string ret(buffer.str());

	// Remove the file
	remove(filename);

#ifdef DEBUG
	cout << ret << endl;
#endif

	return ret;
}

static void printListTasks(const todoList& list) {
	tasks taskList = list.getTasks();

	if (taskList.empty()) {
		cout << "No tasks available!" << endl;
		return;
	}

	cout << "List Index | Task ID | Priority | ";
	if (preferences::getInstance().showDoneTasks()) {
		cout << "Done | ";
	}
	cout << "Description" << endl
			<< "---------------------------------------------";
	if (preferences::getInstance().showDoneTasks()) {
		cout << "-------";
	}
	cout << endl;

	transaction t(db->begin());

	string done;
	string desc;
	int i(-1);
	for (lazy_shared_ptr<task> taPtr : taskList) {
		++i;
		shared_ptr<task> ta(taPtr.load());

		if (!preferences::getInstance().showDoneTasks() && ta->isDone())
			continue;

		cout << setw(10) << i << " | " << setw(7) << ta->getId() << " | "
				<< setw(8) << priority_strings.at(ta->getPriority()) << " | ";

		if (preferences::getInstance().showDoneTasks()) {
			done = "no";
			if (ta->isDone())
				done = "yes";
			cout << setw(4) << done << " | ";
		}
		desc = ta->getDescription();
		replace(desc.begin(), desc.end(), '\n', ' ');
		cout << desc.substr(0, 40) << endl;
	}
}

static void printMenu(const menu_t& menu, const string& menuName) {
	// TODO: There must be a better way to do this

	cout << menuName << ':' << endl;

	switch (menu) {
	case mainmenu:
		for (menu_entries_t::value_type pair : mainMenuEntries) {
			cout << menuPointPrefix << pair.second.entry << endl;
		}
		break;
	case taskmenu:
		for (taskmenu_entries_t::value_type pair : taskMenuEntries) {
			cout << menuPointPrefix << pair.second.entry << endl;
		}
		break;
	case listmenu:
		for (listmenu_entries_t::value_type pair : listMenuEntries) {
			cout << menuPointPrefix << pair.second.entry << endl;
		}
		break;
	case preferencesmenu:
		for (menu_entries_t::value_type pair : prefMenuEntries) {
			cout << menuPointPrefix << pair.second.entry << endl;
		}
		break;
	}

}
