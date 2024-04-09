/*
Analysis: Write a C++ program to prompt user for a file name, read the data containing information for a course, store/process the data, 
          display a menu and allow user to pick an operation to perform. Continue till user decides to quit, and print a final report.
Design:
    Input --> Receive input from an Input File with at most 50 students in a class. Each student's information will include first and last name, student ID,
              six test scores, one quiz score, average grade and letter grade (both of which will be calculated later).
    Process --> Create a struct to represent each student's data listed in input; last names can be multiple words.
                Will need to initiate an array of type struct to store all of the classes' information in the main program.
                Function to open/check that file opened properly.
                Utility functions to read, print (to monitor and output file), and process the struct array (process letter grades).
                    - Use the highest five test scores to calculate the average score, and the quiz score
                    - Tests are worth 90 % of the total, and the quiz is 10 %
                    - Verify that all scores are between 0 and 100; if not, set average to - 1 and grade to I (invalid)
                    - Use the following grading scale to calculate the letter grade:
                        90 - 100 (A)
                        80 - 89.9 (B)
                        70 - 79.9 (C)
                        60 - 69.9 (D)
                        0 - 59.9 (F)
                        Invalid grades (-1) is assigned I
                Process and Menu Function giving the user the abilities listed in the output section of this algorithm; pause for the user to continue (press enter)
                Additional necessary functions:
                    - Sort students by last name (A-Z)
                    - Search for a student by last name (assume that all last names are unique)
                    - Print one student's report
                    - Add new student (prompting for last name; do not add duplicates). Collect information in the following order:
                        1) last name
                        2) first name
                        3) ID
                        4) quiz score
                        5) 6 test scores separated by white space
                    - Delete existing student (again, prompt for last name)
                    - Edit student's test scores or quiz score (validate input and do not update unless correct); then use pre-existing function to print updated
                      student (Names and Student IDs are unique and cannot be edited) Format must be as follows:
                        Enter last name : Jordan

                        To change quiz score enter 1 :
                        To change a test score enter 2 :
                        To exit enter 3 :
                        Test #    Score
                        1         80.00
                        2         70.00
                        3         50.00
                        4         100.00
                        5         90.00
                        6         100.00
                        Enter test number to change : 3
                        Enter new score : 100
    Output --> Menu must provide the following options (look exactly like below):
                1)  Print course content to the monitor
                2)  Print course content to a file
                3)  Sort the list of students alphabetically
                4)  Search for a student
                5)  Add a new student to the list
                6)  Remove a student from the list
                7)  Edit student's scores
                9)  Exit the program
               
               Sample final report output file format:
                        Course Name : Introduction to Computer Programming Fundamentals
                        Course ID : CSCI 1
                        Class Location : MBA 315

                        Name               ID                   Average      Grade
                        Adams, John        111 - 22 - 3333      91.5         A
                        Smith Jr., Willy   222 - 11 - 4444      87.0         B
                        Jordan, Phil       777 - 88 - 6666      -1           I

               Sample list output for monitor:
                        Name               Average    Grade
                        Adams, John        91.5        A
                        Smith Jr., Will    87.0        B
                        Jordan, Phil       -1          I

Additions:
- now search is not case sensitive
- user can search for names using sub strings
- program updates/saves the input file before ending program; it prompt the user for a new file name
- now uses a struct to respresent the whole course:
        Course Name
        Course ID
        Location
        Array of students type struct
        Class Average
        Average Letter Grade
*/

#include<iostream>
#include<string>
#include<fstream>
#include<iomanip>
#include<cctype>
#define CAP 50

using namespace std;

struct studentInfo {
    string firstName;
    string lastName;
    string studentID;
    float quizScore;
    float testScores[6];
    double averageGrade;
    char letterGrade;
};

struct wholeCourse {
    string className;
    string classID;
    string classLocation;
    studentInfo studentData[CAP];
    double classAverage;
    int numValidGrades;
};

bool OpenFile(ifstream& inFS);
int ReadCourse(wholeCourse& classData, int size);
int ReadStudentList(studentInfo classList[], int size, ifstream& inFS);
void ProcessAverage(studentInfo classList[], int size);
void ProcessClassAverage(wholeCourse& classData, int size);
bool TestValues(studentInfo& studentUser); 
char ProcessLetterGrade(double averageGrade);
void ProcessMenu(wholeCourse& classData, int& size);
void DisplayMenu(void);
void RunMenu(char menuSelection, wholeCourse& classData, int& size);
void PrintStudentTable(studentInfo classList[], int size);
void PrintCourseReport(wholeCourse& classData, int size);
string LastCommaFirst(string lastName, string firstName);
void SortStudentList(studentInfo classList[], int size);
int SearchList(studentInfo classList[], int size, string& nameLast);
void FindStudent(studentInfo classList[], int size);
void PrintStudent(studentInfo classList[], int size, int index);
void AddStudent(studentInfo classList[], int& size);
void RemoveStudent(studentInfo classList[], int& size);
void EditStudent(studentInfo classList[], int size);
void UpdateInputFile(wholeCourse& classData, int size);

int main() {
    wholeCourse classInfo;
    int size;

    size = ReadCourse(classInfo, CAP);

    if (size > 0) {
        ProcessAverage(classInfo.studentData, size);
        ProcessMenu(classInfo, size);
    }
//    else {
//        cout << "Error: there is no data in the file. The program will now terminate.";
//    }
    
    exit(0);
}

bool OpenFile(ifstream& inFS) {
    string fileName;

    cout << "Please enter the input file name." << endl;
    cin >> fileName;

    cout << endl;

    inFS.open(fileName.c_str());
    if (inFS.fail()) {
        cout << "Failed to open file." << endl;
        return false;
    }
    else {
        return true;
    }
}

int ReadCourse(wholeCourse& classData, int size) {
    ifstream inFS;
    int dataSize;
    string temp;        //to hold course name and course ID, before splitting

    if (OpenFile(inFS)) {
        getline(inFS, temp);
        classData.className = temp.substr(0, (temp.size() - 5));
        classData.classID = temp.substr((temp.size() - 5), 5);

        getline(inFS, classData.classLocation);
        
        dataSize = ReadStudentList(classData.studentData, CAP, inFS);
    }
    else {
        dataSize = -1;
    }

    inFS.close();

    return dataSize;
}

int ReadStudentList(studentInfo classList[], int size, ifstream& inFS) {
    int i = 0, j;
    string temp;
    
    inFS >> classList[i].firstName;
    getline(inFS, temp);
    classList[i].lastName = temp.substr(1, (temp.size() - 1));
    inFS >> classList[i].studentID;
    inFS >> classList[i].quizScore;
    for (j = 0; j < 6; ++j) {       //six test scores
        inFS >> classList[i].testScores[j];
    }
        
    while (!inFS.eof()) {
        ++i;
        inFS >> classList[i].firstName;
        getline(inFS, temp);
        classList[i].lastName = temp.substr(1, (temp.size() - 1));
        inFS >> classList[i].studentID;
        inFS >> classList[i].quizScore;
        for (j = 0; j < 6; ++j) {       //six test scores
            inFS >> classList[i].testScores[j];
        }
    } 
    inFS.ignore(10, '\n');

    return i;
}

void ProcessAverage(studentInfo classList[], int size) {
    int i, j;
    float minVal, testTotal = 0.0;
    double testAverage;

    for (i = 0; i < size; ++i) {
        if (TestValues(classList[i])) {
            continue;
        }

        minVal = classList[i].testScores[0];
        for (j = 0; j < 6; ++j) {
            if (classList[i].testScores[j] < minVal) {
                minVal = classList[i].testScores[j];
            }
            testTotal = testTotal + classList[i].testScores[j];
        }
        testAverage = ((double)testTotal - (double)minVal) / 5.0;
        classList[i].averageGrade = (testAverage * 0.9) + (classList[i].quizScore * 0.1);

        classList[i].letterGrade = ProcessLetterGrade(classList[i].averageGrade);
        testTotal = 0;
    }
}

void ProcessClassAverage(wholeCourse& classData, int size) {
    int i;
    double totalScores = 0;

    classData.numValidGrades = 0;

    for (i = 0; i < size; ++i) {
        if (classData.studentData[i].averageGrade == -1) {
            continue;
        }
        totalScores = totalScores + classData.studentData[i].averageGrade;
        ++classData.numValidGrades;
    }
    classData.classAverage = totalScores / classData.numValidGrades;
}

bool TestValues(studentInfo& studentUser) {
    int i;
    
    if (studentUser.quizScore < 0 || studentUser.quizScore > 100) {
        studentUser.averageGrade = -1;
        studentUser.letterGrade = 'I';
        return true;
    }
    else {
        for (i = 0; i < 6; ++i) {
            if (studentUser.testScores[i] < 0 || studentUser.testScores[i] > 100) {
                studentUser.averageGrade = -1;
                studentUser.letterGrade = 'I';
                return true;
            }
        }
        return false;
    }
}

char ProcessLetterGrade(double averageGrade) {
    if (averageGrade >= 90 && averageGrade <= 100) {
        return 'A';
    }
    else if (averageGrade >= 80 && averageGrade < 90) {
        return 'B';
    }
    else if (averageGrade >= 70 && averageGrade < 80) {
        return 'C';
    }
    else if (averageGrade >= 60 && averageGrade < 70) {
        return 'D';
    }
    else if (averageGrade >= 0 && averageGrade < 60) {
        return 'F';
    }
    else {
        return 'I';
    }
}

void ProcessMenu(wholeCourse& classData, int& size) {
    char menuOption;
    
    cout << left << fixed << showpoint << setprecision(2);
    
    do {
        DisplayMenu();
        cin >> menuOption;

        cin.ignore(10, '\n');

        RunMenu(menuOption, classData, size);

        cout << "Please type enter to continue." << endl;
        getchar();      //forces user to hit enter (or another character) to continue

    } while (menuOption != '9');

}

void DisplayMenu(void) {
    cout << "Menu:" << endl;
    cout << "1)  Print course content to the monitor" << endl;
    cout << "2)  Print course content to a file" << endl;
    cout << "3)  Sort the list of students alphabetically" << endl;
    cout << "4)  Search for a student" << endl;
    cout << "5)  Add a new student to the list" << endl;
    cout << "6)  Remove a student from the list" << endl;
    cout << "7)  Edit student's scores" << endl;
    cout << "9)  Exit the program" << endl;
    cout << "Please type your choice using the numbers above, then press enter." << endl << endl;
}

void RunMenu(char menuSelection, wholeCourse& classData, int& size) {
    switch (menuSelection) {
        case '1':
            PrintStudentTable(classData.studentData, size);
            break;
        case '2':
            PrintCourseReport(classData, size);
            break;
        case '3':
            SortStudentList(classData.studentData, size);
            PrintStudentTable(classData.studentData, size);
            break;
        case '4':
            FindStudent(classData.studentData, size);
            break;
        case '5':
            AddStudent(classData.studentData, size);
            break;
        case '6':
            RemoveStudent(classData.studentData, size);
            PrintStudentTable(classData.studentData, size);
            break;
        case '7':
            EditStudent(classData.studentData, size);
            ProcessAverage(classData.studentData, size);
            PrintStudentTable(classData.studentData, size);
            break;
        case '9':
            UpdateInputFile(classData, size);
            break;
        default:
            cout << "Error: the choice entered is not recognized. Please try again." << endl;
            break;
    }
}

void PrintStudentTable(studentInfo classList[], int size) {
    int i;
    string lastFirst;
        
    cout << setw(20) << "Name" << setw(10) << "Average" << "Grade" << endl;
    
    for (i = 0; i < size; ++i) {
        lastFirst = LastCommaFirst(classList[i].lastName, classList[i].firstName);
        cout << setw(20) << lastFirst << setw(10) << classList[i].averageGrade << classList[i].letterGrade << endl;
    }
}

void PrintCourseReport(wholeCourse& classData, int size) {
    ofstream outFS;
    string outputFile, lastFirst, splitID;
    int i;

    cout << "Please enter the output file name." << endl;
    cin >> outputFile;

    outFS.open(outputFile);

    outFS << left << fixed << showpoint << setprecision(2) << setw(20) << "Course Name:" << classData.className << endl;
    outFS << setw(20) << "ID:" << classData.classID << endl;
    outFS << setw(20) << "Class Location:" << classData.classLocation << endl << endl;

    outFS << setw(20) << "Name" << setw(14) << "ID" << setw(13) << "Average" << "Letter Grade" << endl;
    for (i = 0; i < size; ++i) {
        lastFirst = LastCommaFirst(classData.studentData[i].lastName, classData.studentData[i].firstName);
        splitID = classData.studentData[i].studentID.substr(0, 3) + "-" + classData.studentData[i].studentID.substr(3, 2) + "-" + classData.studentData[i].studentID.substr(5, 4);
        outFS << setw(20) << lastFirst << setw(14) << splitID;
        outFS << setw(13) << classData.studentData[i].averageGrade << classData.studentData[i].letterGrade << endl;
    }

    ProcessClassAverage(classData, size);
    outFS << endl << "Class Average for " << classData.numValidGrades << " students: " << classData.classAverage << endl;

    getchar();      //forces user to hit enter (or another character) to continue
}

string LastCommaFirst(string lastName, string firstName) {
    string lastFirst;

    lastFirst = lastName + ", " + firstName;

    return lastFirst;
}

void SortStudentList(studentInfo classList[], int size) {
    bool flag;
    int i;
    studentInfo temp;       //hold for a swap

    do {
        --size;
        flag = false;
        for (i = 0; i < size; ++i) {
            if (classList[i].lastName > classList[i + 1].lastName) {
                temp = classList[i];
                classList[i] = classList[i + 1];
                classList[i + 1] = temp;
                flag = true;        //setting the flag to true, will run the loop again till all information is sorted properly
            }
        }
    } while (flag);
    
}

void FindStudent(studentInfo classList[], int size) {
    string findStudent;
    int index;

    cout << "Enter last name: ";
    cin >> findStudent;
    cout << endl;

    index = SearchList(classList, size, findStudent);

    if (index == -1) {
        cout << findStudent << " is not on the list." << endl;
    }
    else {
        PrintStudent(classList, size, index);
    }

    getchar();      //forces user to hit enter (or another character) to continue
}

int SearchList(studentInfo classList[], int size, string& nameLast) {
    int i;
    string holdVal;     //to put substring into, if first letter is lowercase and string is not found, to search for substring

    for (i = 0; i < size; ++i) {
        if (classList[i].lastName.find(nameLast, 0) != string::npos) {
            return i;
        }
        else {
            if (islower(nameLast.at(0))) {
                holdVal = nameLast.substr(1, nameLast.size() - 1);
                if (classList[i].lastName.find(holdVal, 0) != string::npos) {
                    return i;
                }
            }
        }
    }
    return -1;
}

void PrintStudent(studentInfo classList[], int size, int index) {
    string lastFirst;
    
    lastFirst = LastCommaFirst(classList[index].lastName, classList[index].firstName);
    
    cout << setw(15) << "Name:" << lastFirst << endl;
    cout << setw(15) << "ID:" << classList[index].studentID << endl;
    cout << setw(15) << "Quiz Score:" << classList[index].quizScore << endl;
    cout << setw(15) << "Test Scores:" << classList[index].testScores[0] << " " << classList[index].testScores[1] << " " << classList[index].testScores[2] << " ";
    cout << classList[index].testScores[3] << " " << classList[index].testScores[4] << " " << classList[index].testScores[5] << endl;
    cout << setw(15) << "Average:" << classList[index].averageGrade << endl;
    cout << setw(15) << "Letter Grade:" << classList[index].letterGrade << endl;
}

void AddStudent(studentInfo classList[], int& size) {
    studentInfo temp;
    int index, i;

    cout << "Please enter student's last name." << endl;
    getline(cin, temp.lastName);

    index = SearchList(classList, size, temp.lastName);

    if (index != -1) {
        cout << temp.lastName << " is on the list, duplicated names are not allowed." << endl;
        return;
    }

    cout << "Please enter student's first name." << endl;
    cin >> temp.firstName;

    cout << "Please enter student's ID." << endl;
    cin >> temp.studentID;

    cout << "Please enter quiz score." << endl;
    cin >> temp.quizScore;

    cout << "Please enter the six test scores each separated by a space." << endl;
    for (i = 0; i < 6; ++i) {
        cin >> temp.testScores[i];
    }

    cin.ignore(10, '\n');

    
    if (index == -1) {
        classList[size] = temp;
        ++size;
    }

    ProcessAverage(classList, size);
}

void RemoveStudent(studentInfo classList[], int& size) {
    string temp;
    int index;

    cout << "Enter last name:" << endl;
    getline(cin, temp);

    index = SearchList(classList, size, temp);

    if (index != -1) {
        --size;
        classList[index] = classList[size];
    }
    else {
        cout << temp << " is not on the list." << endl;
    }
}

void EditStudent(studentInfo classList[], int size) {
    string temp;
    int index, changeTest;
    float holdVal;
    char userOption;

    cout << "Enter last name:" << endl;
    getline(cin, temp);

    index = SearchList(classList, size, temp);
    
    if (index == -1) {
        cout << temp << " is not on the list, try again." << endl;
        exit(0);
    }

    cout << "To change quiz score enter 1:" << endl;
    cout << "To change a test score enter 2:" << endl;
    cout << "To exit enter 3:" << endl;
    cin >> userOption;

    switch (userOption) {
        case '1':
            cout << "Enter new quiz score:" << endl;
            cin >> holdVal;
            if (holdVal < 0 || holdVal > 100 || !cin) {
                cout << "Error: the score entered is not valid. The student's data will not be updated." << endl;
            }
            else {
                classList[index].quizScore = holdVal;
            }
            break;
        case '2':
            cout << setw(10) << "Test #" << "Score" << endl;
            cout << setw(10) << "1" << classList[index].testScores[0] << endl;
            cout << setw(10) << "2" << classList[index].testScores[1] << endl;
            cout << setw(10) << "3" << classList[index].testScores[2] << endl;
            cout << setw(10) << "4" << classList[index].testScores[3] << endl;
            cout << setw(10) << "5" << classList[index].testScores[4] << endl;
            cout << setw(10) << "6" << classList[index].testScores[5] << endl;
            cout << "Enter test number to change:" << endl;
            cin >> changeTest;
            if (changeTest < 1 || changeTest > 6 || !cin) {
                 cout << "Error: the test number entered is not valid. The student's data will not be updated." << endl;
                 break;
            }
            cout << "Enter new score:" << endl;
            cin >> holdVal;
            if (holdVal < 0 || holdVal > 100 || !cin) {
                cout << "Error: the score entered is not valid. The student's data will not be updated." << endl;
            }
            else {
                classList[index].testScores[changeTest - 1] = holdVal;
            }
            break;
        case '3':
            break;
        default:
            cout << "Error: the choice entered is not recognized. The student's data will not be updated." << endl;
            break; 
    }

    getchar();      //forces user to hit enter (or another character) to continue
}

void UpdateInputFile(wholeCourse& classData, int size) {
    ofstream outFS;
    string fileName;
    int i, j;

    cout << "Before closing the program, the updated data will be saved to a new input file for later use." << endl;
    cout << "Please enter the input file name." << endl;
    cin >> fileName;

    outFS.open(fileName);

    outFS << classData.className << " " << classData.classID << endl;
    outFS << classData.classLocation << endl;
    for (i = 0; i < size; ++i) {
        outFS << classData.studentData[i].firstName << " " << classData.studentData[i].lastName << endl;
        outFS << classData.studentData[i].studentID << " " << classData.studentData[i].quizScore << " ";
        for (j = 0; j < 6; ++j) {
            outFS << classData.studentData[i].testScores[j] << " ";
        }
        outFS << endl;
    }

    outFS.close();
}