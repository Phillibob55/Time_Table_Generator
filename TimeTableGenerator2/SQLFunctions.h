#pragma once

#include <mysql.h>
#include <cstring>
#include <iostream>
#include <string>
#include <list>
#include <array>


class SQLFunctions {
private:
	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES* res;
	std::string schema;
public:
	SQLFunctions() { conn = NULL; res = NULL;}
	SQLFunctions(const std::string &password, const std::string &schemaName) {
		this->conn = NULL;
		this->res = NULL;
		this->schema = schemaName;
		this->connectToDatabase(password, schemaName);
	}
	void connectToDatabase(const std::string &pass, const std::string &schemaName) {
		const char* password = pass.c_str();
		const char* schema = schemaName.c_str();

		this->conn = mysql_init(0);
		this->conn = mysql_real_connect(this->conn, "localhost", "root", password, schema, 3306, NULL, 0);

		if (this->conn) {
			std::cout << "Database Successfully connected...." << std::endl;
		}
		else {
			std::cout << "Database connection failed :(" << std::endl;
		}
	}
	
	std::list<std::string> executeQueryColumn(const std::string& query) {
		//This works only for "SELECT column FROM Table_name WHERE condition" type queries. 'SELECT column' is mandatory.
		std::list<std::string> dataFromDatabase;
		const char* q = query.c_str();
		int qstate = mysql_query(this->conn, q);

		if (!qstate) {
			this->res = mysql_store_result(this->conn);
			while (this->row = mysql_fetch_row(this->res)) {
				dataFromDatabase.push_back(row[0]);
			}
		}
		else {
			std::cout << "Query failed: " << mysql_error(conn) << std::endl;
		}
		return dataFromDatabase;
	}

	std::string executeQueryCell(const std::string& query) {
		//This works only for "SELECT column FROM Table_name WHERE condition" type queries. 'SELECT column' is mandatory.
		std::string dataFromDatabase;
		const char* q = query.c_str();
		int qstate = mysql_query(this->conn, q);

		if (!qstate) {
			this->res = mysql_store_result(this->conn);
			while (this->row = mysql_fetch_row(this->res)) {
				dataFromDatabase = row[0];
			}
		}
		else {
			std::cout << "Query failed: " << mysql_error(conn) << std::endl;
		}
		return dataFromDatabase;
	}

};

