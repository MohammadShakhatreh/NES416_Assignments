/*
* Team names:
* Mohammad Shakhatreh
* Khaled Al-rejjal
*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define COURSES_LEN 4

struct Course {
    char name[200];
    char code[20];
    int credit;
    float grade;

} courses[COURSES_LEN]; // Declare an array of courses

/*
* Display the commands the program understands.
*/
void help() {
    puts("1 - Diplay all courses information.");
    puts("2 - Compute GPA of all courses.");
    puts("3 - Display highest grade and the corresponding course name.");
    puts("4 - Display lowest grade and the corresponding course name");
    puts("5 - Display course(s) code(s) whose credit hours are based on your input.");
    puts("6 - Display course information for given course code.");
    puts("7 - Display the grade as letter format for given course code(ex: 85 is B).");
    puts("8 - Help.");
    puts("9 - Exit.");
}

/*
* Loads courses from a file and puts them in the global array courses.
* file format is name, code, credit, grade
* each course is on separate line.
*/
int load_courses(char *filename) {
    // Open file in read mode
    FILE *fp = fopen(filename, "r");

    // File failed to open
    if (fp == NULL)
        return -1;

    for (int i = 0; i < COURSES_LEN; ++i){
        // [^,\n] means read a string until a comma or newline
        fscanf(fp, "%[^,\n], %[^,\n], %d, %f\n",
            courses[i].name,
            courses[i].code,
            &courses[i].credit,
            &courses[i].grade
        );
    }

    fclose(fp);
    return 0;
}
/*
* convert a grade to a symbol
*/
char *gtos(float grade) {
    if (grade >= 95)
        return "A+";
    else if(grade >= 85)
        return "A";
    else if(grade >= 80)
        return "A-";
    else if(grade >= 77)
        return "B+";
    else if(grade >= 73)
        return "B";
    else if(grade >= 70)
        return "B-";
    else if(grade >= 67)
        return "C+";
    else if(grade >= 63)
        return "C";
    else if(grade >= 60)
        return "C-";
    else if(grade >= 57)
        return "D+";
    else if(grade >= 53)
        return "D";
    else if(grade >= 50)
        return "D-";
    else
        return "F";
}

// ******************************
// commands
// ******************************
void display_courses_info() {
    for(int i = 0; i < COURSES_LEN; ++i) {
        printf("%s %s %.2f %d\n", courses[i].name, courses[i].code, courses[i].grade, courses[i].credit);
    }
}

void compute_gpa() {
    float gpa = 0.0;

    for(int i = 0; i < COURSES_LEN; ++i){
        gpa += courses[i].grade;
    }

    printf("GPA: %.2f\n", gpa / COURSES_LEN);
}

void display_highest_grade() {
    struct Course max = courses[0];

    for(int i = 0; i < COURSES_LEN; ++i) {
        if (courses[i].grade > max.grade)
            max = courses[i];
    }

    printf("%s\n", max.name);
}

void display_lowest_grade() {
    struct Course min = courses[0];
    for(int i = 0; i < COURSES_LEN; ++i) {
        if (courses[i].grade < min.grade)
            min = courses[i];
    }

    printf("%s\n", min.name);
}

void display_courses_by_credit() {
    printf("Enter courses credit: ");

    int credit, cnt = 0;
    scanf("%d", &credit);

    for(int i = 0; i < COURSES_LEN; ++i) {
        if(courses[i].credit == credit) {
            printf("%s ", courses[i].code); ++cnt;
        }
    }

    if(cnt > 0)
        puts("");
    else 
        puts("No courses with this credit.");
}

void display_course_by_code() {
    printf("Enter course code: ");

    char code[20];
    bool found = false;
    scanf("%s", code);

    for(int i = 0; i < COURSES_LEN; ++i) {
        if(strcmp(code, courses[i].code) == 0) {
            printf("%s %s %.2f %d\n", courses[i].name, courses[i].code, courses[i].grade, courses[i].credit);
            found = true;
            break;
        }
    }

    if(!found)
        puts("No course with this code.");
}

void display_grade_for_course() {
    printf("Enter course code: ");

    char code[20];
    bool found = false;
    scanf("%s", code);

    for(int i = 0; i < COURSES_LEN; ++i) {
        if(strcmp(code, courses[i].code) == 0) {
            printf("grade: %s\n", gtos(courses[i].grade));
            found = true;
            break;
        }
    }

    if(!found)
        puts("No course with this code.");
}

int main(int argc, char *argv[]){
    // The program accepts a filename argument to load the courses from.
    if (argc != 2) {
        printf("usage: %s filename\n", argv[0]);
        return -1;
    }

    // Trying to load the courses.
    char *filename = argv[1];
    if (load_courses(filename) != 0){
        printf("failed to open the file.\n");
        return -1;
    }

    help(); // Print help for the first time.

    int choice = 0;
    bool exit = false;
    while(!exit) {
        printf("> ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                display_courses_info();
                break;
            case 2:
                compute_gpa();
                break;
            case 3:
                display_highest_grade();
                break;
            case 4:
                display_lowest_grade();
                break;
            case 5:
                display_courses_by_credit();
                break;
            case 6:
                display_course_by_code();
                break;
            case 7:
                display_grade_for_course();
                break;
            case 8:
                help();
                break;
            case 9:
                exit = true;
                break;
        }
    }

    return 0;
}