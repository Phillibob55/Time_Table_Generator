#pragma once

#include <libxl.h>
#include "DataTuple.h"

using namespace libxl;

class LIBXLFunctions {
public:
	Book* books[3]; //0 = teachers, 1 = rooms, 2 = classes
	
	Format* columnNameFormat[3];
	Format* emptySlotFormat[3];
	Format* timeSlotFormat[3];

public:
	LIBXLFunctions() {
		for (int i = 0; i < 3; i++) {
			books[i] = nullptr;
			columnNameFormat[i] = nullptr;
			emptySlotFormat[i] = nullptr;
			timeSlotFormat[i] = nullptr;
		}
	}

	static std::string listToString(std::list<std::string> ls) {
		std::string superString;
		for (auto itr = ls.begin(); itr != ls.end(); itr++) {
			superString += *itr + " ";
		}
		return superString;
	}

	void mergeWrite(int startRow, int startCol, int bookIndex, Sheet* sheet, DataTuple dtp, std::string rooma) {
		int additionalCellsToMerge = 0;
		int creditHours = dtp.credit_hours;

		// temp is the text that will be written in a cell. It's value changes depending on the book
		// Because... A teacher time table doesn't need to have teacher's name written in EVERY SINGLE CELL.. it's already on the sheet!
		std::string temp;
		if (bookIndex == 0)
			temp = dtp.course_name + "\n" + " " + " (" + rooma + ")" + " -- " + listToString(dtp.batch_codes);
		else if (bookIndex == 1)
			temp = dtp.course_name + "\n" + listToString(dtp.instructor_names) + " " + " -- " + listToString(dtp.batch_codes);
		else
			temp = dtp.course_name + "\n" + listToString(dtp.instructor_names) + " " + " (" + rooma + ")";
		
		// Converting the std::string to const wchar_T*
		std::wstring widestr = std::wstring((temp).begin(), (temp).end());
		const wchar_t* line = widestr.c_str();

		// Merge cells depending+ on credit hours of the course i.e. 1 cell = 30 minutes
		if (creditHours == 1) {
			additionalCellsToMerge = 5;
			sheet->writeStr(startRow, startCol+3, L"");
		}
		else if (creditHours == 2) {
			additionalCellsToMerge = 3;
		}
		else if (creditHours == 3) {
			additionalCellsToMerge = 2;
		}
		else {
			std::cout << "Invalid Credit Hours for the course!" << std::endl;
		}

		// Writing the text in the specified cell and the merging cells according number of credit hours
		sheet->writeStr(startRow, startCol, line, timeSlotFormat[bookIndex]);
		sheet->setMerge(startRow, startRow, startCol, startCol + additionalCellsToMerge);

		// Applying the format to the merged cells to make em look sexier ;)
		for (int i = 1; i <= additionalCellsToMerge; i++) {
			sheet->writeStr(startRow, startCol + i, L"", timeSlotFormat[bookIndex]);
		}
	}
	static Sheet* getSheetByName(Book* book, std::string sheetName) {
		// Return pointer to a sheet with a certain name from the specified book

		std::wstring widestr = std::wstring((sheetName).begin(), (sheetName).end());
		const wchar_t* name = widestr.c_str();

		for (int i = 0; i < book->sheetCount(); ++i) {
			if (wcscmp(book->getSheet(i)->name(), name) == 0) {
				return book->getSheet(i);
			}
		}
		return 0;
	}

	std::list<int> createDummyVacantList() {

		std::array<int,5> roomSlotIndices = { 1,4,7,11,14 };
		std::list<int> slotRowCol;

		for (int row = 2; row < 7; row++) {
			for (int col = 0; col < roomSlotIndices.size(); col++) {
				//Check if the cells at (row,roomSlotIndices[col]) is not already assigned a class
				slotRowCol.push_back(row);
				slotRowCol.push_back(roomSlotIndices[col]);
				
			}
		}
		return slotRowCol;
	}

	std::list<int> findMututalVacant(std::list<int> list1, std::list<int> list2) {
		std::list<int> commonTime;
		for (auto itr1 = list1.begin(); itr1 != list1.end(); itr1++ ) {
			int vac11 = *itr1;
			int vac12 = *(++itr1);

			for (auto itr2 = list2.begin(); itr2 != list2.end(); itr2++) {
				int vac21 = *itr2;
				int vac22 = *(++itr2);

				if (vac11 == vac21 && vac12 == vac22) {
					commonTime.push_back(vac11);
					commonTime.push_back(vac22);
				}

			}
		}
		return commonTime;
	}

	std::list<int> findVacantForClass(int bookIndex, std::string sheetName) {
		//Return a std list of integers representing the row and coloumns of vacant time slot indices
		//e.g. if a class is free at monday 8:00-9:30, it'll add 2,1 to the list. (2 and 1 represent row and column indices in the excel sheet) 
		
		// Time slots correspond to times where classes are viable {8:00-9:30, 9:30-11:00, 11:30-1:00, 1:00-2:30, 2:30-4:00};
		std::array<int, 5> roomSlotIndices = { 1,4,7,11,14 };
		Sheet* targetSheet = getSheetByName(books[bookIndex], sheetName);
		std::list<int> slotRowCol;

		for (int row = 2; row < 7; row++) {
			for (int col = 0; col < 5; col++) {
				//Check if the cells at (row,roomSlotIndices[col]) is not already assigned a class
				if (row == 6 && col == 3) continue;
				if (targetSheet->cellType(row, roomSlotIndices[col]) == CELLTYPE_BLANK) {
					slotRowCol.push_back(row);
					slotRowCol.push_back(roomSlotIndices[col]);
				}
			}
		}
		return slotRowCol;
	}

	std::list<int> findVacantForLab(int bookIndex, std::string sheetName) {
		//Return a std list of integers representing the row and coloumns of vacant time slot indices
		// NOTE: WE need 2 vacant time slots for a lab
		//e.g. if a class is free at monday 8:00-9:30 AND 9:30-11:00, it'll add 2,1 to the list. (2 and 1 represent row and column indices in the excel sheet) 

		// Time slots correspond to times where classes are viable {8:00-9:30, 9:30-11:00, 11:30-1:00, 1:00-2:30, 2:30-4:00};
		std::array<int, 4> labSlotIndices = { 1,4,11,14 };
		Sheet* targetSheet = getSheetByName(books[bookIndex], sheetName);
		std::list<int> slotRowCol;

		for (int row = 2; row < 7; row++) {
			for (int col = 0; col < labSlotIndices.size(); col++) {
				if (row == 6 && col == 3) continue;
				//Check if the cells at (row,roomSlotIndices[col]) as well as at (row,roomSlotIndices[col + 1]) is not already assigned a class
				if (targetSheet->cellType(row, labSlotIndices[col]) == CELLTYPE_BLANK && targetSheet->cellType(row, labSlotIndices[col + 1]) == CELLTYPE_BLANK) {
					slotRowCol.push_back(row);
					slotRowCol.push_back(labSlotIndices[col]);
				}
			}
		}
		return slotRowCol;
	}

	std::list<int> findVacantForTwoCreditHour(int bookIndex, std::string sheetName) {
		//Return a std list of integers representing the row and coloumns of vacant time slot indices
		//e.g. if a class is free at monday 9:30-11:30, it'll add 2,4 to the list. (2 and 4 represent row and column indices in the excel sheet) 

		// Time slots correspond to times where classes are viable {9:30-11:30};
		int roomSlotIndices = 7;
		Sheet* targetSheet = getSheetByName(books[bookIndex], sheetName);
		std::list<int> slotRowCol;

		for (int row = 2; row < 7; row++) {
			if (row == 6) continue;
			if (targetSheet->cellType(row, roomSlotIndices) == CELLTYPE_BLANK) {
				slotRowCol.push_back(row);
				slotRowCol.push_back(roomSlotIndices);
			}
		}
		return slotRowCol;
	}

	Sheet* newSheet(const wchar_t &sheetName, int bookIndex) {

		//Creating the proper Sheet with the given name
		Sheet* tempSheet = books[bookIndex]->addSheet(&sheetName);

		// Making the empty time slot cells grey with no borders
		// Also double their height and increase their width by a factor of 0.33. Trust me.. it looks awesome
		for (int i = 2; i <= 6; i++) {
			for (int j = 1; j <= 16; j++) {
				tempSheet->writeStr(i,j,L"", emptySlotFormat[bookIndex]);
				tempSheet->writeBlank(i, j, emptySlotFormat[bookIndex]);
			}
		}

		//Writing the Columns Names
		const wchar_t* columnNames[] = {L"Days", L"08:00-08:30", L"08:30-09:00", L"09:00-09:30", L"09:30-10:00", L"10:00-10:30", L"10:30-11:00", L"11:00-11:30", L"11:30-12:00", L"12:00-12:30", L"12:30-13:00", L"13:00-13:30", L"13:30-14:00", L"14:00-14:30", L"14:30-15:00", L"15:00-15:30", L"15:30-16:00"};
		int sizeOfColumnNames = 17;

		for (int i = 0; i < sizeOfColumnNames; i++) {
			tempSheet->writeStr(1, i, columnNames[i], columnNameFormat[bookIndex]);
		}

		// Writing the Days in first column
		const wchar_t* Days[] = { L"Monday", L"Tuesday", L"Wednesday" , L"Thursday" , L"Friday" };
		int noOfDays = 5;

		for (int i = 0; i < noOfDays; i++) {
			tempSheet->writeStr(i + 2, 0, Days[i], timeSlotFormat[bookIndex]);
		}

		//Resizing the rows and columns so that it looks pweetttyy UwU
		tempSheet->setCol(0,0,11);
		tempSheet->setCol(1,16,9.5);
		for (int i = 2; i < 7; i++)
			tempSheet->setRow(i,42);

		return tempSheet;
	}

	void initializeBooks() {
		//Create 3 workbooks for Teachers, Rooms and Classes
		for (int i = 0; i < 3; i++) {
			books[i] = xlCreateXMLBook();
		}
	}
	void inititlizeFormats() {
		for (int i = 0; i < 3; i++) {
			//Column Name Formats are used for the time slot columns... i.e. "Days",  "8:00-8:30", ""8:30-9:00", etc
			columnNameFormat[i] = books[i]->addFormat();
				columnNameFormat[i]->setAlignV(ALIGNV_TOP);
				columnNameFormat[i]->setAlignH(ALIGNH_CENTER);
				columnNameFormat[i]->setPatternForegroundColor(COLOR_DARKBLUE_CL);
				columnNameFormat[i]->setFillPattern(FILLPATTERN_SOLID);
		
			//Font for column headers
			Font* columnNameFont = books[i]->addFont();
				columnNameFont->setColor(COLOR_WHITE);
				columnNameFont->setName(L"Times New Roman");
				columnNameFormat[i]->setFont(columnNameFont);

				//Thick black borders for column headers
				columnNameFormat[i]->setBorder(BORDERSTYLE_THICK);
				columnNameFormat[i]->setBorderColor(COLOR_BLACK);

			// The empty slots format is the default format for all cells within the time table. It is used to represent available times
			emptySlotFormat[i] = books[i]->addFormat();
				emptySlotFormat[i]->setPatternForegroundColor(COLOR_GRAY25);
				emptySlotFormat[i]->setFillPattern(FILLPATTERN_SOLID);
				emptySlotFormat[i]->setBorder(BORDERSTYLE_NONE);

			// Time slot format is used for the first column and for any time slot assigned for a lecture or lab
			timeSlotFormat[i] = books[i]->addFormat();
				timeSlotFormat[i]->setAlignV(ALIGNV_CENTER);
				timeSlotFormat[i]->setAlignH(ALIGNH_CENTER);
				timeSlotFormat[i]->setPatternForegroundColor(COLOR_WHITE);
				timeSlotFormat[i]->setFillPattern(FILLPATTERN_SOLID);
			
			//Font for time slots
			Font* timeSlotFont = books[i]->addFont();
				timeSlotFont->setColor(COLOR_BLACK);
				timeSlotFont->setName(L"Times New Roman");
				timeSlotFormat[i]->setFont(timeSlotFont);
				timeSlotFormat[i]->setWrap(true);
				//Thick black borders for time slots
				timeSlotFormat[i]->setBorder(BORDERSTYLE_THICK);
				timeSlotFormat[i]->setBorderColor(COLOR_BLACK);
		}
	}

	void initializeSheetsInEachBook(SQLFunctions sql) {

		// Make a separate sheer for every room/lab
		// Rooms = G1, G2, G3, G4, G5, CS LAB, Computing LAB, Systems Lab
		std::array<std::string, 7> rooms = { "G01", "G03", "G04", "G05", "F02", "CS Lab", "Computing Lab"};
		for (int i = 0; i < rooms.size(); i++) {
			std::wstring widestr = std::wstring(rooms[i].begin(), rooms[i].end());
			const wchar_t* widecstr = widestr.c_str();
			newSheet(*widecstr, 1);
		}

		// Make a separate sheet for every teacher
		std::list<std::string> teachers = DataTuple::getUniqueInstructers(sql);
		for (auto itr = teachers.begin(); itr != teachers.end(); itr++) {
			std::wstring widestr = std::wstring((*itr).begin(), (*itr).end());
			const wchar_t* widecstr = widestr.c_str();
			newSheet(*widecstr, 0);
		}

		// Make a separate sheet for every class
		std::list<std::string> classes = DataTuple::getUniqueBatches(sql);
		for (auto itr = classes.begin(); itr != classes.end(); itr++) {
			std::wstring widestr = std::wstring((*itr).begin(), (*itr).end());
			const wchar_t* widecstr = widestr.c_str();
			newSheet(*widecstr, 2);
		}
	}

	void initialize(SQLFunctions sql) {
		initializeBooks();
		inititlizeFormats();
		initializeSheetsInEachBook(sql);
		std::cout << "LibXL Initialized Successfully...." << std::endl;
	}

	~LIBXLFunctions() {
		//Saving and releasing all excel files
		books[0]->save(L"Teachers.xlsx");
		books[1]->save(L"Rooms.xlsx");
		books[2]->save(L"Classes.xlsx");

		for (int i = 0; i < 3; i++) {
			books[i]->release();
		}
		std::cout << "Time tables successfully generated!" << std::endl;
	}
};
