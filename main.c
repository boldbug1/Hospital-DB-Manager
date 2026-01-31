#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

typedef struct
{
    char *name;
    char gender;
    int age;
    char *pos;
} patient;

patient *patients = NULL;

int len = 0;

void loadPatientBinary();
void savePatientText(patient p);
void savePatientBinary(patient p);
void addPatient(const char *name, int age, char gender, const char *positon);
void showAll();
void removePatient(int index);
void loadPatientExcel(const char *filename);

void print_truncated(const char *str, int width)
{
    int len = strlen(str);
    if (len > width)
    {
        for (int i = 0; i < width; i++)
            putchar(str[i]);
        printf("..."); // truncated indicator
        // pad to keep column aligned
        for (int i = 0; i < (width - 3); i++)
            putchar(' ');
    }
    else
    {
        printf("%-*s", width, str); // left-align with padding
    }
}

void freeBuffer()
{
    while (getchar() != '\n')
        ;
}

int main()
{
    loadPatientBinary();

    int running = 1;
    char inputName[1024], inputPos[1024], inputGender;
    int inputAge;
    int inputIndex;
    int chs;
    while (running)
    {
        printf("\n\n\n");
        printf("------------Hospitl Database Management-------------");
        printf("\n1.Add Patient\n2.Remove Patient\n3.Show all patients\n4.Show reports\n5.LOAD CSV IN EXCEL\n6.Exit\n");
        scanf(" %d", &chs);
        switch (chs)
        {
        case 1:
            freeBuffer();
            printf("Enter Patient's name :");
            fgets(inputName, sizeof(inputName), stdin);
            inputName[strcspn(inputName, "\n")] = 0;

            printf("Enter Age(1-110) :");
            scanf(" %d", &inputAge);
            if (inputAge < 1 || inputAge > 110)
                printf("Invalid age Input !!");
            freeBuffer();
            printf("Enter Gender(M/F/O) : ");
            scanf(" %c", &inputGender);
            if (inputGender != 'M' && inputGender != 'F' && inputGender != 'O')
                printf("Invalid choice ");
            freeBuffer();
            printf("Enter the problem/disease : ");
            fgets(inputPos, sizeof(inputPos), stdin);
            inputPos[strcspn(inputPos, "\n")] = 0;
            addPatient(inputName, inputAge, inputGender, inputPos);
            break;
        case 2:
            printf("Enter's patient index number :");
            scanf(" %d", &inputIndex);
            removePatient(inputIndex);
            printf("Removed patient succesfully!");
            freeBuffer();
            break;

        case 3:
            showAll();
            break;
        case 5:
            loadPatientExcel("patients.csv");
            break;
        case 6:
            printf("EXITING.....\n");
            running = 0;
            break;
        default:
            printf("Error - wrong input\n");
            break;
        }
    }
}

void loadPatientBinary()
{
    FILE *fp = fopen("patients.dat", "rb");
    if (!fp)
    {
        printf("File loading error");
        patients = NULL;
        len = 0;
        return;
    }

    len = 0;

    while (1)
    {
        int nameLen, posLen;
        patient p;

        if (fread(&nameLen, sizeof(int), 1, fp) != 1)
            break;
        p.name = malloc(nameLen + 1);
        fread(p.name, sizeof(char), nameLen, fp);
        p.name[nameLen] = '\0';

        fread(&p.age, sizeof(int), 1, fp);
        fread(&p.gender, sizeof(char), 1, fp);

        fread(&posLen, sizeof(int), 1, fp);
        p.pos = malloc(posLen + 1);
        fread(p.pos, sizeof(char), posLen, fp);
        p.pos[posLen] = '\0';

        patient *temp = realloc(patients, (len + 1) * sizeof(patient));
        if (!temp)
        {
            printf("Memory allocation failed!\n");
            break;
        }
        patients = temp;
        patients[len++] = p;
    }
    fclose(fp);
}

void savePatientBinary(patient p)
{
    FILE *fp = fopen("patients.dat", "ab");
    if (!fp)
    {
        printf("File opening failed!");
        perror("File open failed!");
        return;
    }

    int nameLen = strlen(p.name);
    int posLen = strlen(p.pos);

    fwrite(&nameLen, sizeof(int), 1, fp);
    fwrite(p.name, sizeof(char), nameLen, fp);

    fwrite(&p.age, sizeof(int), 1, fp);
    fwrite(&p.gender, sizeof(char), 1, fp);

    fwrite(&posLen, sizeof(int), 1, fp);
    fwrite(p.pos, sizeof(char), posLen, fp);

    fclose(fp);
}

void showAll()
{
    {

        printf("| %-4s | %-20s | %-4s | %-10s | %-30s |\n\n",
               "Sln", "Name", "Age", "Gender", "Disease/Problem");

        for (int i = 0; i < len; i++)
        {
            printf(" %-4d  ", i + 1);
            print_truncated(patients[i].name, 20);
            printf("  %-4d  %-10c  ", patients[i].age, patients[i].gender);
            print_truncated(patients[i].pos, 30);
            printf("\n");
        }

        printf("printed succesfully all patients!");
    }
}

void removePatient(int index)
{
    index = index - 1;
    if (index < 0 || index >= len || patients == NULL)
    {
        printf(RED "Error - incorrect index\n" RESET);
        return;
    }

    // 1. Remove from RAM
    free(patients[index].name);
    free(patients[index].pos);

    for (int i = index; i < len - 1; i++)
    {
        patients[i] = patients[i + 1];
    }
    len--;

    if (len > 0)
    {
        patients = (patient *)realloc(patients, (len * sizeof(patient)));
    }
    else
    {
        free(patients);
        patients = NULL;
    }

    // 2. Rewrite the file correctly
    FILE *fp = fopen("patients.dat", "wb"); // "wb" clears the file to start fresh

    if (!fp)
    {
        printf("File opening failed\n");
        return;
    }

    // CRITICAL: This loop must match savePatientBinary EXACTLY
    for (int i = 0; i < len; i++)
    {
        int nameLen = strlen(patients[i].name);
        int posLen = strlen(patients[i].pos);

        // 1. Name Length & Name
        fwrite(&nameLen, sizeof(int), 1, fp);
        fwrite(patients[i].name, sizeof(char), nameLen, fp); // No '&' for strings!

        // 2. Age & Gender (Must come BEFORE position to match your loader)
        fwrite(&patients[i].age, sizeof(int), 1, fp);
        fwrite(&patients[i].gender, sizeof(char), 1, fp);

        // 3. Position Length & Position
        fwrite(&posLen, sizeof(int), 1, fp);
        fwrite(patients[i].pos, sizeof(char), posLen, fp); // No '&' for strings!
    }
    fclose(fp);
}

void addPatient(const char *name, int age, char gender, const char *positon)
{
    patient *temp = (patient *)realloc(patients, (len + 1) * sizeof(patient));

    if (temp == NULL)
    {
        printf("Memory allocation failed !");
    }
    else
    {
        patients = temp;
    }

    patients[len].name = (char *)malloc((strlen(name)) + 1);
    strcpy(patients[len].name, name);

    patients[len].age = age;

    patients[len].gender = gender;

    patients[len].pos = (char *)malloc(strlen(positon) + 1);
    strcpy(patients[len].pos, positon);

    savePatientBinary(patients[len]);
    savePatientText(patients[len]);

    len++;
}

void savePatientText(patient p)
{
    FILE *fp = fopen("patients.csv", "a");
    if (!fp)
    {
        perror("File open failed");
        return;
    }

    fprintf(fp, "%s,%d,%c,%s\n", p.name, p.age, p.gender, p.pos);

    fclose(fp);
}

void loadPatientExcel(const char *filename)
{
    char command[512];

    snprintf(command, sizeof(command), "start \"\" \"%s\"", filename);

    printf("Opening %s...\n", filename);
    int result = system(command);
}