/*
 * database.hxx
 *
 *  Created on: Sep 22, 2013
 *      Author: klaus
 */

//
// Create concrete database instance based on the DATABASE_* macros.
//
#ifndef DATABASE_HXX
#define DATABASE_HXX

#include <string>
#include <memory>
#include <cstdlib>  // std::exit
#include <iostream>

#include <odb/database.hxx>

#if defined(DATABASE_MYSQL)
#  include <odb/mysql/database.hxx>
#elif defined(DATABASE_SQLITE)
#  include <odb/connection.hxx>
#  include <odb/transaction.hxx>
#  include <odb/schema-catalog.hxx>
#  include <odb/sqlite/database.hxx>
#elif defined(DATABASE_PGSQL)
#  include <odb/pgsql/database.hxx>
#elif defined(DATABASE_ORACLE)
#  include <odb/oracle/database.hxx>
#elif defined(DATABASE_MSSQL)
#  include <odb/mssql/database.hxx>
#else
#  error unknown database; did you forget to define the DATABASE_* macros?
#endif

#if defined(DATABASE_SQLITE)
typedef std::unique_ptr<odb::sqlite::database> db_ptr;
#else
typedef std::unique_ptr<odb::database> db_ptr;
#endif

inline db_ptr create_database(int& argc, char* argv[]) {
	using namespace std;
	using namespace odb::core;

	if (argc > 1 && argv[1] == string("--help")) {
		cout << "Usage: " << argv[0] << " [options]" << endl << "Options:"
				<< endl;

#if defined(DATABASE_MYSQL)
		odb::mysql::database::print_usage (cout);
#elif defined(DATABASE_SQLITE)
		odb::sqlite::database::print_usage(cout);
#elif defined(DATABASE_PGSQL)
		odb::pgsql::database::print_usage (cout);
#elif defined(DATABASE_ORACLE)
		odb::oracle::database::print_usage (cout);
#elif defined(DATABASE_MSSQL)
		odb::mssql::database::print_usage (cout);
#endif

		exit(0);
	}

#if defined(DATABASE_MYSQL)
	db_ptr db (new odb::mysql::database (argc, argv));
#elif defined(DATABASE_SQLITE)
	db_ptr db(
			new odb::sqlite::database(argc, argv, false,
					SQLITE_OPEN_READWRITE));

#ifdef DEBUG
	db->tracer(stderr_tracer);
#endif

	// Create the database schema. Due to bugs in SQLite foreign key
	// support for DDL statements, we need to temporarily disable
	// foreign keys.
	//
	if (db->flags() & SQLITE_OPEN_CREATE) {
		connection_ptr c(db->connection());

		c->execute("PRAGMA foreign_keys=OFF");

		transaction t(c->begin());
		schema_catalog::create_schema(*db);
		t.commit();

		c->execute("PRAGMA foreign_keys=ON");
	}
#elif defined(DATABASE_PGSQL)
	db_ptr db (new odb::pgsql::database (argc, argv));
#elif defined(DATABASE_ORACLE)
	db_ptr db (new odb::oracle::database (argc, argv));
#elif defined(DATABASE_MSSQL)
	db_ptr db (new odb::mssql::database (argc, argv));
#endif

	return db;
}

#endif // DATABASE_HXX
