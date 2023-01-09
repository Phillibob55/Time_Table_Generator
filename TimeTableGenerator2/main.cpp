#pragma once

#include "LIBXLFunctions.h"
#include <algorithm>
#include <vector>
using namespace std;

int algorithm(list<DataTuple> &dtp, LIBXLFunctions lbxl) {
	array<string, 4> rooms = { "G01", "G03", "G04", "G05"};
	array<string, 2> labs = { "CS Lab", "Computing Lab"};
	// NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//ANY CHANGES MADE HERE IN rooms[] and labs[] MUST ALSO BE UPDATED IN LIBXLFunctions::initializeSheetsInEachBook()
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	int assignedSlotCount = 0;
	int labCount = 0, twoCredCount = 0, threeCredCount = 0;

	{ // Using these code blocks to isolate the algorithm loops. TRUST ME, this helps REDUCE BUGS
		for (auto itrData = dtp.begin(); itrData != dtp.end(); itrData++) {
			
			bool shouldISkip = false;
			//(*itrData).display();
			if ((*itrData).credit_hours == 1) {
				for (int i = 0; i < labs.size(); i++) {
					//cout << "Currently looking in " << labs[i] << endl;

					// Get vacant time slot pairs for lab i
					list<int> roomVacant = lbxl.findVacantForLab(1, labs[i]);
					if (roomVacant.empty() == true) {
						//cout << labs[i] << " is not Vacant at all!" << endl;
						continue;
					}

					for (auto itrRoom = roomVacant.begin(); itrRoom != roomVacant.end(); itrRoom++) {

						int rowRoom = *itrRoom;
						int colRoom = *(++itrRoom);

						// Get vacant time slot pairs for instructor
						//list<int> teacherVacant = lbxl.findVacantForLab(0, (*itrData).getInstructorName());
						
						list<int> allTeacherVacant = lbxl.createDummyVacantList();;

						for (auto itrTeach = (*itrData).instructor_names.begin(); itrTeach != (*itrData).instructor_names.end(); itrTeach++) {
							list<int> teacherVacant = lbxl.findVacantForLab(0, (*itrTeach));
							allTeacherVacant = lbxl.findMututalVacant(allTeacherVacant, teacherVacant);
						}


						//cout << "Currently looking in " << (*itrData).getInstructorName() << "\'s Time Table" << endl;
						for (auto itrTeacher = allTeacherVacant.begin(); itrTeacher != allTeacherVacant.end(); itrTeacher++) {
							int rowTeacher = *itrTeacher;
							int colTeacher = *(++itrTeacher);

							// If a free time slot in lab list has a match in teacher's time slot list:
							if (rowRoom == rowTeacher && colRoom == colTeacher) {

								// Get vacant time slot pairs for the class
								
								list<int> allBatchesVacant = lbxl.createDummyVacantList();;

								for (auto itrBatchCode = (*itrData).batch_codes.begin(); itrBatchCode != (*itrData).batch_codes.end(); itrBatchCode++) {
									list<int> batchVacant = lbxl.findVacantForLab(2, (*itrBatchCode));
									allBatchesVacant = lbxl.findMututalVacant(allBatchesVacant, batchVacant);
								}
								
								//cout << "Currently looking in " << (*itrData).getDepartment() + (*itrData).getBatch() << "\'s Time Table" << endl;
								for (auto itrBatch = allBatchesVacant.begin(); itrBatch != allBatchesVacant.end(); itrBatch++) {
									int rowBatch = *itrBatch;
									int colBatch = *(++itrBatch);

									// If the free time slot has a match in batch's time slot list too:
									if (rowRoom == rowBatch && colRoom == colBatch) {

										// Write data on the match time slots in teacher, lab and batch excel sheet
										Sheet* temp = lbxl.getSheetByName(lbxl.books[1], labs[i]);
										lbxl.mergeWrite(rowRoom, colRoom, 1, temp, *itrData, labs[i]);

										for (auto itrTeach = (*itrData).instructor_names.begin(); itrTeach != (*itrData).instructor_names.end(); itrTeach++) {
											temp = lbxl.getSheetByName(lbxl.books[0], (*itrTeach));
											lbxl.mergeWrite(rowRoom, colRoom, 0, temp, *itrData, labs[i]);
										}

										for (auto itrBatchCode = (*itrData).batch_codes.begin(); itrBatchCode != (*itrData).batch_codes.end(); itrBatchCode++) {
											temp = lbxl.getSheetByName(lbxl.books[2], (*itrBatchCode));
											lbxl.mergeWrite(rowRoom, colRoom, 2, temp, *itrData, labs[i]);
										}

										itrData = --dtp.erase(itrData);
										shouldISkip = true;
										//cout << "Slot Match Found!!! YATTA!!!" << endl;
										cout << "Slot Assignment # " << ++assignedSlotCount << endl;
										labCount++;
									}

									if (shouldISkip == true) break;

								}
							}
							if (shouldISkip == true) break;
						}
						if (shouldISkip == true) break;
					}
					if (shouldISkip == true) break;
				}
			}
			//if (itrData == dtp.end()) break;
			//cout << "Loop Ended" << endl;
		}
	}
	cout << "Labs Assigned  = " << labCount << endl;
	//-----------------------
	{
		for (auto itrData = dtp.begin(); itrData != dtp.end(); itrData++) {

			bool shouldISkip = false;
			//(*itrData).display();
			if ((*itrData).credit_hours == 2) {
				for (int i = 0; i < rooms.size(); i++) {
					//cout << "Currently looking in " << rooms[i] << endl;
					
					// Get vacant time slot pairs for room i
					list<int> roomVacant = lbxl.findVacantForTwoCreditHour(1, rooms[i]);
					for (auto itrRoom = roomVacant.begin(); itrRoom != roomVacant.end(); itrRoom++) {

						int rowRoom = *itrRoom;
						int colRoom = *(++itrRoom);

						// Get vacant time slot pairs for instructor
						list<int> allTeacherVacant = lbxl.createDummyVacantList();;

						for (auto itrTeach = (*itrData).instructor_names.begin(); itrTeach != (*itrData).instructor_names.end(); itrTeach++) {
							list<int> teacherVacant = lbxl.findVacantForTwoCreditHour(0, (*itrTeach));
							allTeacherVacant = lbxl.findMututalVacant(allTeacherVacant, teacherVacant);
						}
						
						
						//cout << "Currently looking in " << (*itrData).getInstructorName() << "\'s Time Table" << endl;
						for (auto itrTeacher = allTeacherVacant.begin(); itrTeacher != allTeacherVacant.end(); itrTeacher++) {
							int rowTeacher = *itrTeacher;
							int colTeacher = *(++itrTeacher);

							// If a free time slot in room list has a match in teacher's time slot list:
							if (rowRoom == rowTeacher && colRoom == colTeacher) {

								// Get vacant time slot pairs for the class
								list<int> allBatchesVacant = lbxl.createDummyVacantList();;

								for (auto itrBatchCode = (*itrData).batch_codes.begin(); itrBatchCode != (*itrData).batch_codes.end(); itrBatchCode++) {
									list<int> batchVacant = lbxl.findVacantForTwoCreditHour(2, (*itrBatchCode));
									allBatchesVacant = lbxl.findMututalVacant(allBatchesVacant, batchVacant);
								}
								
								//cout << "Currently looking in " << (*itrData).getDepartment() + (*itrData).getBatch() << "\'s Time Table" << endl;
								for (auto itrBatch = allBatchesVacant.begin(); itrBatch != allBatchesVacant.end(); itrBatch++) {
									int rowBatch = *itrBatch;
									int colBatch = *(++itrBatch);

									// If the free time slot has a match in batch's time slot list too:
									if (rowRoom == rowBatch && colRoom == colBatch) {

										// Write data on the match time slots in teacher, lab and batch excel sheet
										Sheet* temp = lbxl.getSheetByName(lbxl.books[1], rooms[i]);
										lbxl.mergeWrite(rowRoom, colRoom, 1, temp, *itrData, rooms[i]);

										for (auto itrTeach = (*itrData).instructor_names.begin(); itrTeach != (*itrData).instructor_names.end(); itrTeach++) {
											temp = lbxl.getSheetByName(lbxl.books[0], (*itrTeach));
											lbxl.mergeWrite(rowRoom, colRoom, 0, temp, *itrData, rooms[i]);
										}

										for (auto itrBatchCode = (*itrData).batch_codes.begin(); itrBatchCode != (*itrData).batch_codes.end(); itrBatchCode++) {
											temp = lbxl.getSheetByName(lbxl.books[2], (*itrBatchCode));
											lbxl.mergeWrite(rowRoom, colRoom, 2, temp, *itrData, rooms[i]);
										}

										itrData = --dtp.erase(itrData);
										shouldISkip = true;
										//cout << "Slot Match Found!!! YATTA!!!" << endl;
										cout << "Slot Assignment # " << ++assignedSlotCount << endl;
										twoCredCount++;
									}

									if (shouldISkip == true) break;

								}
							}
							if (shouldISkip == true) break;
						}
						if (shouldISkip == true) break;
					}
					if (shouldISkip == true) break;
				}
			}
		}
	}
	cout << "2 cred lecs Assigned  = " << twoCredCount << endl;
	//-----------------------------
	list<DataTuple> dtpTemp(dtp);
	{
		for (int j = 0; j < 2; j++) {

			dtp.clear();

			// Using a vector wrapper to shuffle our list
			vector< reference_wrapper< DataTuple > > vec(dtpTemp.begin(), dtpTemp.end());
			random_shuffle(vec.begin(), vec.end());
			for (auto vectorIterator = dtpTemp.begin(); vectorIterator != dtpTemp.end(); vectorIterator++) {
				dtp.push_back(*vectorIterator);
			}

			//dtp = list<DataTuple>(dtpTemp);
			for (auto itrData = dtp.begin(); itrData != dtp.end(); itrData++) {
				
				bool shouldISkip = false;
				//(*itrData).display();
				if ((*itrData).credit_hours == 3) {
					for (int i = 0; i < rooms.size(); i++) {
						//cout << "Currently looking in " << rooms[i] << endl;

						// Get vacant time slot pairs for room i
						list<int> roomVacant = lbxl.findVacantForClass(1, rooms[i]);
						for (auto itrRoom = roomVacant.begin(); itrRoom != roomVacant.end(); itrRoom++) {

							int rowRoom = *itrRoom;
							int colRoom = *(++itrRoom);

							// Get vacant time slot pairs for instructor
							list<int> allTeacherVacant = lbxl.createDummyVacantList();;

							for (auto itrTeach = (*itrData).instructor_names.begin(); itrTeach != (*itrData).instructor_names.end(); itrTeach++) {
								list<int> teacherVacant = lbxl.findVacantForClass(0, (*itrTeach));
								allTeacherVacant = lbxl.findMututalVacant(allTeacherVacant, teacherVacant);
							}
							
							
							//cout << "Currently looking in " << (*itrData).getInstructorName() << "\'s Time Table" << endl;
							for (auto itrTeacher = allTeacherVacant.begin(); itrTeacher != allTeacherVacant.end(); itrTeacher++) {
								int rowTeacher = *itrTeacher;
								int colTeacher = *(++itrTeacher);

								// If a free time slot in room list has a match in teacher's time slot list:
								if (rowRoom == rowTeacher && colRoom == colTeacher) {

									// Get vacant time slot pairs for the class
									list<int> allBatchesVacant = lbxl.createDummyVacantList();;

									for (auto itrBatchCode = (*itrData).batch_codes.begin(); itrBatchCode != (*itrData).batch_codes.end(); itrBatchCode++) {
										list<int> batchVacant = lbxl.findVacantForClass(2, (*itrBatchCode));
										allBatchesVacant = lbxl.findMututalVacant(allBatchesVacant, batchVacant);
									}
									
									//cout << "Currently looking in " << (*itrData).getDepartment() + (*itrData).getBatch() << "\'s Time Table" << endl;
									for (auto itrBatch = allBatchesVacant.begin(); itrBatch != allBatchesVacant.end(); itrBatch++) {
										int rowBatch = *itrBatch;
										int colBatch = *(++itrBatch);

										// If the free time slot has a match in batch's time slot list too:
										if (rowRoom == rowBatch && colRoom == colBatch) {

											// Write data on the match time slots in teacher, lab and batch excel sheet
											Sheet* temp = lbxl.getSheetByName(lbxl.books[1], rooms[i]);
											lbxl.mergeWrite(rowRoom, colRoom, 1, temp, *itrData, rooms[i]);

											for (auto itrTeach = (*itrData).instructor_names.begin(); itrTeach != (*itrData).instructor_names.end(); itrTeach++) {
												temp = lbxl.getSheetByName(lbxl.books[0], (*itrTeach));
												lbxl.mergeWrite(rowRoom, colRoom, 0, temp, *itrData, rooms[i]);
											}

											for (auto itrBatchCode = (*itrData).batch_codes.begin(); itrBatchCode != (*itrData).batch_codes.end(); itrBatchCode++) {
												temp = lbxl.getSheetByName(lbxl.books[2], (*itrBatchCode));
												lbxl.mergeWrite(rowRoom, colRoom, 2, temp, *itrData, rooms[i]);
											}

											itrData = --dtp.erase(itrData);
											shouldISkip = true;
											//cout << "Slot Match Found!!! YATTA!!!" << endl;
											cout << "Slot Assignment # " << ++assignedSlotCount << endl;
											threeCredCount++;
										}

										if (shouldISkip == true) break;

									}
								}
								if (shouldISkip == true) break;
							}
							if (shouldISkip == true) break;
						}
						if (shouldISkip == true) break;
					}
				}

			}
		}
	}
	cout << "3 cred lecs assigned = " << threeCredCount << endl;
	return assignedSlotCount;
}



int calclateAssignments(list<DataTuple> dtp) {
	int oneCreditHourCourses = 0;
	int twoCreditHourCourses = 0;
	int threeCreditHourCourses = 0;

	// Count courses of each type
	for (auto itr = dtp.begin(); itr != dtp.end(); itr++) {
		if ((*itr).credit_hours == 1) {
			oneCreditHourCourses++;
		}
		else if ((*itr).credit_hours == 2) {
			twoCreditHourCourses++;
		}
		else if ((*itr).credit_hours == 3) {
			threeCreditHourCourses++;
		}
	}

	// 2 *  threeCreditHourCourses because labs and 2 credit hour courses take place once evey week while 3 credit hour courses have two lessons every week
	return oneCreditHourCourses + twoCreditHourCourses + 2 * threeCreditHourCourses;
}



int main() {

	// connect to the schema names 'timetable' in MySql workbench using the password 'password123'
	/*
	string password;
	cout << "Enter password to the MySQL server: " << endl;
	cin >> password;
	*/

	SQLFunctions sql("password123", "timetable");
	list<DataTuple> data = DataTuple::loadData(sql);

	
	// A few debugging steps to calculate the number of assignments required
	cout << "SQL Data Entries = " << data.size() << endl;
	int expectedTimeSlotAssignments = calclateAssignments(data);
	cout << "Expected Assignments = " << expectedTimeSlotAssignments << endl;
	
	// This loop will keep retrying to make time tables until all of the data has been used
	while (true) {
		LIBXLFunctions lbxl;
		lbxl.initialize(sql);
		if (algorithm(data, lbxl) >= expectedTimeSlotAssignments) {
			break;
		}
		cout << "Time Table Creation failed! Retrying!" << endl;
		for (auto itr = data.begin(); itr != data.end(); itr++) {
			(*itr).display();
		}
	}
	cout << "Timetables Created" << endl;
	
	return 0;
}
