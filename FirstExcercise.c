#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
	// Structure to represent a scheduled activity
typedef struct
{
	char title[100];
	int state;	// 0: Undone, 1: Done
	int advised;	//0: not, 1: Yes
} Activity;

typedef struct
{
	int hour;
	int minute;
	int second;
} AppTime;

AppTime * appTime;
// Function to print the activity message with a 3-second delay
void printWithDelay(const char *message)
{
	printf("%s\n", message);
	usleep(3000000);	// Delay for 3 seconds
}

int timeFactor = 1;
int responseAsked = 0;	//0 asked, 1: not asked
char userInput[10];
Activity activities[8] = {
		{
		"Breakfast time", 0, 0
	},
	{
		"Morning walk", 0, 0
	},
	{
		"Work/study time", 0, 0
	},
	{
		"Lunch break", 0, 0
	},
	{
		"Rest and relaxation", 0, 0
	},
	{
		"Exercise time", 0, 0
	},
	{
		"Dinner time", 0, 0
	},
	{
		"Evening leisure activities", 0, 0
	}
};

Activity* findForHour(int hour)
{
	return &activities[hour / 3];
}

void adviseActivity(Activity *activity)
{
	printWithDelay(activity->title);

	if (activity->state == 0)
	{
		// Ask if the activity is being done
		printWithDelay("Are you doing this activity? (yes/no)");
		responseAsked = 1;
		while (1)
		{
			scanf("%s", userInput);

			if (strcmp(userInput, "yes") != 0 && strcmp(userInput, "no") != 0)
			{
				printWithDelay("Please answer yes or no");
				continue;
			}

			if (strcmp(userInput, "yes") == 0)
			{
				activity->state = 1;
				printWithDelay("Activity marked as done.");
			}

			responseAsked = 0;
			break;
		}
	}
	else
	{
		// Print a message if the activity is already done
		printWithDelay("Chill, you have already done this activity.");
	}
}

void reset()
{
	for (int i = 0; i < 8; i++)
	{
		(&activities[i])->state = 0;
	}
}

//tick is second
void tick()
{
	appTime->second += 1;
	if (appTime->second == 60)
	{
		appTime->second = 0;
		appTime->minute++;
	}

	if (appTime->minute == 60)
	{
		appTime->minute = 0;
		appTime->hour++;
	}

	if (appTime->hour == 24)
	{
		appTime->hour = 0;
	}
}

void *backgroundRunner()
{
	int currentHour, currentMinute;
	while (1)
	{
		currentHour = appTime->hour;
		currentMinute = appTime->minute;
		Activity * activity;
		if (currentMinute == 0 && currentHour % 3 == 0)
		{
			printf("%d", currentHour);
			activity = findForHour(currentHour);
			if (activity->advised == 0) adviseActivity(activity);
			activity->advised = 1;
		}

		if (currentMinute == 50 && currentHour % 3 == 2)
		{
			activity = findForHour(currentHour);
			if (activity->state == 0) adviseActivity(activity);
		}

		if (currentHour == 00 && currentMinute == 00)
		{
			reset();
		}

		tick();
		usleep(1000000 / timeFactor);
	}

	return NULL;
}

void *promptRunner()
{
	printf("Enter the current time (HH:MM or 'now'): ");
	while (scanf("%s", userInput) != EOF)
	{
		if (responseAsked == 1) continue;
		int currentHour, currentMinute;
		if (strcmp(userInput, "now") == 0)
		{
			// Get the current system time if 'now' is entered
			time_t now = time(NULL);
			struct tm *timeinfo = localtime(&now);
			currentHour = timeinfo->tm_hour;
			currentMinute = timeinfo->tm_min;
		}
		else
		{
			// Parse the entered time
			sscanf(userInput, "%d:%d", &currentHour, &currentMinute);
		}

		Activity *activity = findForHour(currentHour);

		adviseActivity(activity);
	}

	return NULL;
}

void startBackgroundScheduler()
{
	pthread_t background_thread;
	pthread_create(&background_thread, NULL, backgroundRunner, NULL);
}

void startUserPrompter()
{
	pthread_t user_prompter_thread;
	pthread_create(&user_prompter_thread, NULL, promptRunner, NULL);
}

void parseTimeSpeedFactor(int argc, char *argv[])
{
	if (argc < 2) return;
	sscanf(argv[1], "%d", &timeFactor);
}

void initTime()
{
	time_t now = time(NULL);
	struct tm *timeinfo = localtime(&now);
	appTime = (AppTime*) malloc(sizeof(AppTime));
	appTime->hour = timeinfo->tm_hour;
	appTime->minute = timeinfo->tm_min;
	appTime->second = timeinfo->tm_sec;
}

int main(int argc, char *argv[])
{
	parseTimeSpeedFactor(argc, argv);
	initTime();

	startBackgroundScheduler();
	startUserPrompter();

	while (1);
	return 0;
}
