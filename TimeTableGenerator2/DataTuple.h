#pragma once

#include "SQLFunctions.h"

class DataTuple {
public:
	int lecture_id, credit_hours;
	std::string course_name;
	std::list<std::string> instructor_names, batch_codes;

	static std::list<DataTuple> loadData(SQLFunctions sql) {
		int numberOfLectures = std::stoi(sql.executeQueryCell("SELECT MAX(lecture_id) FROM lectures"));
		std::list<DataTuple> tempData;

		for (int i = 1; i <= numberOfLectures; i++) {
			DataTuple dtpTemp;
			dtpTemp.lecture_id = i;
			dtpTemp.course_name = sql.executeQueryCell("SELECT DISTINCT course_name FROM viewtemp WHERE lecture_id = " + std::to_string(i));
			dtpTemp.credit_hours = std::stoi(sql.executeQueryCell("SELECT DISTINCT credit_hours FROM viewtemp WHERE lecture_id = " + std::to_string(i) ));
			dtpTemp.instructor_names = sql.executeQueryColumn("SELECT DISTINCT instructor_name FROM viewtemp WHERE lecture_id = " + std::to_string(i));
			dtpTemp.batch_codes = sql.executeQueryColumn("SELECT DISTINCT batch FROM viewtemp WHERE lecture_id = " + std::to_string(i));
			tempData.push_back(dtpTemp);
		}
		return tempData;
	}

	void display() {

		int switches[5] = {0,0,0,0,0};

		auto itr1 = instructor_names.begin();
		auto itr2 = batch_codes.begin();

		while (true) {

			if (switches[0] == 0) {
				std::cout << lecture_id;
				switches[0] = 1;
			}
			std::cout << "\t";
			if (switches[1] == 0) {
				std::cout << credit_hours;
				switches[1] = 1;
			}
			std::cout << "\t";
			if (switches[2] == 0) {
				std::cout << *itr1;
				itr1++;
				if (itr1 == instructor_names.end())
					switches[2] = 1;
			}
			std::cout << " - ";
			if (switches[3] == 0) {
				std::cout << *itr2;
				itr2++;
				if(itr2 == batch_codes.end())
					switches[3] = 1;
			}
			std::cout << " - ";
			if (switches[4] == 0) {
				std::cout << course_name;
				switches[4] = 1;
			}
			std::cout << std::endl;
			if (switches[2] && switches[3])
				break;
		}
	}

	static std::list<std::string> getUniqueInstructers(SQLFunctions sql) {
		return sql.executeQueryColumn("SELECT DISTINCT instructor_name FROM viewtemp");
	}

	static std::list<std::string> getUniqueBatches(SQLFunctions sql) {
		return sql.executeQueryColumn("SELECT DISTINCT batch FROM viewtemp");
	}
};

