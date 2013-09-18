// file      : hello/driver.cxx
// copyright : not copyrighted - public domain

#include <memory>   // std::auto_ptr
#include <iostream>

#include <odb/database.hxx>
#include <odb/transaction.hxx>

#include "database.hxx" // create_database
#include "task.hxx"
#include "task-odb.hxx"
#include "todoList.hxx"
#include "todoList-odb.hxx"

using namespace std;
using namespace odb::core;

int main(int argc, char* argv[]) {
	try {
		shared_ptr<database> db(create_database(argc, argv));

		unsigned long first_id, second_id;

		lazy_shared_ptr<task> first(*db, new task("Hole Wäsche"));
		lazy_shared_ptr<task> second(*db, new task("Lerne Black Sabbath - Iron Man"));
		shared_ptr<todoList> standard(new todoList("Standard List"));

//		standard->getTasks().push_back(first);

		transaction t(db->begin());

		first_id = db->persist(*first);
		second_id = db->persist(*second);

		t.commit();

//		unsigned long john_id, joe_id;
//
//		// Create a few persistent person objects.
//		//
//		{
//			task john("John", "Doe", 33);
//			task jane("Jane", "Doe", 32);
//			task joe("Joe", "Dirt", 30);
//
//			transaction t(db->begin());
//
//			// Make objects persistent and save their ids for later use.
//			//
//			john_id = db->persist(john);
//			db->persist(jane);
//			joe_id = db->persist(joe);
//
//			t.commit();
//		}
//
//		typedef odb::query<task> query;
//		typedef odb::result<task> result;
//
//		// Say hello to those over 30.
//		//
//		{
//			transaction t(db->begin());
//
//			result r(db->query<task>(query::age > 30));
//
//			for (result::iterator i(r.begin()); i != r.end(); ++i) {
//				cout << "Hello, " << i->first() << " " << i->last() << "!"
//						<< endl;
//			}
//
//			t.commit();
//		}
//
//		// Joe Dirt just had a birthday, so update his age.
//		//
//		{
//			transaction t(db->begin());
//
//			auto_ptr<task> joe(db->load<task>(joe_id));
//			joe->age(joe->age() + 1);
//			db->update(*joe);
//
//			t.commit();
//		}
//
//		// Alternative implementation without using the id.
//		//
//		/*
//		 {
//		 transaction t (db->begin ());
//
//		 result r (db->query<person> (query::first == "Joe" &&
//		 query::last == "Dirt"));
//
//		 result::iterator i (r.begin ());
//
//		 if (i != r.end ())
//		 {
//		 auto_ptr<person> joe (i.load ());
//		 joe->age (joe->age () + 1);
//		 db->update (*joe);
//		 }
//
//		 t.commit ();
//		 }
//		 */
//
//
//		// John Doe is no longer in our database.
//		//
//		{
//			transaction t(db->begin());
//			db->erase<task>(john_id);
//			t.commit();
//		}
	} catch (const odb::exception& e) {
		cerr << e.what() << endl;
		return 1;
	}
}
