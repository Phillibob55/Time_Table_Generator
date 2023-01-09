DROP SCHEMA IF EXISTS timetable;
CREATE SCHEMA timetable;
USE timetable;

CREATE TABLE lectures(
	lecture_id INT PRIMARY KEY,
    course_name VARCHAR(255),
    credit_hours VARCHAR(255)
);

CREATE TABLE details(
	lecture_id INT,
    instructor_name VARCHAR(255),
	batch VARCHAR(255),
    
    FOREIGN KEY (lecture_id) REFERENCES lectures(lecture_id)
);

CREATE VIEW viewtemp AS
SELECT * FROM details NATURAL JOIN lectures;
