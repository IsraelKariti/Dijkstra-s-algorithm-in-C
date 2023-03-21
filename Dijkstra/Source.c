#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BMP "fishpool.bmp"
#define BMPCPY "fishpool-copy.bmp"
#define TXT "pools.txt"
#define BEST_TXT "best-route.txt"

typedef struct { //pixel color
	int r;
	int g;
	int b;
} color_t;

typedef struct { //pixel's coordinates
	int x;
	int y;
}co_t;

typedef struct pixel { //list of pixels
	co_t p;
	struct pixel* next;
}pix_t;

typedef struct pool { //pools' list extructed of bmp
	int size; //the number of pixels that combine the pool
	co_t poolCenter;
	pix_t* pArr; //list of pool pixels
	struct pool* next;
}poolList_t;

typedef struct logger {
	double time;
	double oil;
	co_t center;
	int size;
	struct logger* next;
} log_t;



// prototype /
void option1();															//find pools in bmp and write finding to a text-file
unsigned char** readBMP(unsigned int* width, unsigned int* hight);		//read bmp file
poolList_t* createNewPool(int x, int y, int size, poolList_t** head);
void remove_From_List(poolList_t** justAnotherPoolList, poolList_t* maxPool);
void show_display_to_screen_pools_after_order(poolList_t* justAnotherPoolList);
void option2();
void option3();
int numbers_only(char* rnum, int check);
poolList_t* createReaderList(FILE* fReading2);
double distance(int x, int x1, int y, int y1);
poolList_t* addhead(poolList_t* readerList, int x, int y);
void createLog(poolList_t* readerList, double oil, double time, log_t** pH);
double getBestTime(poolList_t** headBestRoute);
void clearHeadBestRoute(log_t** headBestRoute);
void clearReaderList(poolList_t* h);
log_t* copyLOG(log_t* pH2);
void copyHeadBestRoute(log_t** pH, log_t** headBestRoute);
void printRoute(double oil, double neededOil, double totalTime, int width, int hight, log_t** pH, log_t** headBestRoute);
void deleteCurrLogFromList(log_t** pH);
poolList_t* getClose(poolList_t* completeList, poolList_t* currLog, log_t** pH);
poolList_t* getSecondClose(poolList_t* completeList, poolList_t* currLog, log_t** pH);
void opt3REC(poolList_t* completeList, poolList_t* currLog, double oil, double time, int width, int hight, log_t** pH, log_t** headBestRoute);
log_t* opt3Listing(FILE* fReading2, int x, int y, int oil, int width, int hight);
void opt3c(log_t* br);

poolList_t* createNewPool(int x, int y, int size, poolList_t** head) {
	poolList_t* pl = (poolList_t*)calloc(1, sizeof(poolList_t));
	pl->pArr = NULL;
	pl->poolCenter.x = x;
	pl->poolCenter.y = y;
	pl->size = size;
	pl->next = *head;

	// make the top of the list point to the new pool
	*head = pl;
}

void remove_From_List(poolList_t** justAnotherPoolList, poolList_t* maxPool) {
	poolList_t* list = *justAnotherPoolList;

	// corner case: the masx is first
	if (list == maxPool) {
		(*justAnotherPoolList) = list->next;
		free(list);
		return;
	}

	// find the pool before max
	while (list->next != maxPool) {
		list = list->next;
	}
	// short the list
	list->next = maxPool->next;

	//
	free(maxPool);
}

void show_display_to_screen_pools_after_order(poolList_t* justAnotherPoolList) {
	printf("Sorted pools by size:\nCoordinate\tSize\n==========\t====\n");

	while (justAnotherPoolList != NULL) {
		poolList_t* maxPool = justAnotherPoolList;
		poolList_t* raner = justAnotherPoolList;

		while (raner) {
			if (maxPool->size < raner->size) {
				maxPool = raner;
			}
			raner = raner->next;
		}

		printf("(%3d,%3d)\t%d\n", maxPool->poolCenter.x, maxPool->poolCenter.y, maxPool->size);

		remove_From_List(&justAnotherPoolList, maxPool);
	}
}

void option2() {
	FILE* f = NULL;
	poolList_t* justAnotherPoolList = NULL;

	fopen_s(&f, TXT, "rt");
	if (f == NULL)
		return;

	char str[100] = { 0 };
	// skip header
	fread(str, 1, 55, f);

	//char c = fgetc(f);
	for (;;) {
		char arr[100] = { 0 };
		// '('
		fread(arr, 1, 1, f);

		int x = 0;
		int canREAD = fscanf_s(f, "%d", &x);
		if (canREAD == EOF)
			break;
		// ','
		fread(arr, 1, 1, f);

		// get to the y
		int y = 0;
		fscanf_s(f, "%d", &y);

		// ','
		fread(arr, 1, 1, f);
		fread(arr, 1, 1, f);
		fread(arr, 1, 1, f);
		fread(arr, 1, 1, f);


		// get to the size
		int size = 0;
		fscanf_s(f, "%d%*c", &size);

		// add new pool to list
		createNewPool(x, y, size, &justAnotherPoolList);


	}

	show_display_to_screen_pools_after_order(justAnotherPoolList);
}




void option3()
{
	int oil = 0, x = 0, y = 0;
	int  checkFW, checkFR1, checkFR2, width, hight, i = 0, check = 0, gap = 0;
	char psic = 0, stone, rnum[30], rnum2[30];
	char* num = { NULL };
	char* pointer = { NULL };

	FILE* fReading1;
	checkFR1 = fopen_s(&fReading1, BMP, "r");
	if (checkFR1 != 0) {
		printf_s("Error open the flake.bmp\n");
	}
	else {
		fseek(fReading1, 18L, SEEK_SET);
		width = _getw(fReading1);
		hight = _getw(fReading1);

		printf_s("Please Enter valid x,y start coordinate, bmp width is %d and height is %d\n", width, hight);

		gets_s(rnum, sizeof(rnum));
		strcpy_s(rnum2, sizeof(rnum2), rnum);
		num = strtok_s(rnum, "\n", &pointer);
		while (num == NULL)//When no command was given
		{
			printf_s("\n");
			gets_s(rnum, sizeof(rnum));//Waiting for command again
			num = strtok_s(rnum, "\n", &pointer);
		}
		num = strtok_s(rnum2, " \t", &pointer);
		while (num == NULL)//When no command was given
		{
			num = strtok_s(NULL, " \t\n", &pointer);
		}

		if (rnum != NULL) {
			while (rnum[i] != '\0')
			{
				if (strchr(rnum, ','))
					psic = 1;
				else psic = 0;
				i++;
			}
			if (psic == 1) {
				i = 0;
				while (rnum2[i] != '\0')
				{
					if (strchr(rnum2, ','))
						gap = 0;
					else gap = 1;
					i++;
				}
				num = strtok_s(rnum, ",", &pointer);
				x = atoi(num);
				num = strtok_s(NULL, "\t \n", &pointer);
				while (num == NULL)//When no command was given
				{
					num = strtok_s(NULL, "\t \n", &pointer);
				}
				y = atoi(num);
			}check = numbers_only(rnum, check);

		}

		while ((x < 0 || x > width || y < 0 || y > hight || psic != 1 || check == 1 || gap == 1))
		{
			printf_s("Please Enter valid x,y start coordinate, bmp width is %d and height is %d\n", width, hight);

			gets_s(rnum, sizeof(rnum));
			strcpy_s(rnum2, sizeof(rnum2), rnum);
			num = strtok_s(rnum, "\n", &pointer);
			while (num == NULL)//When no command was given
			{
				printf_s("\n");
				gets_s(rnum, sizeof(rnum));//Waiting for command again
				num = strtok_s(rnum, "\n", &pointer);
			}
			num = strtok_s(rnum2, " \t", &pointer);
			while (num == NULL)//When no command was given
			{
				num = strtok_s(NULL, " \t", &pointer);
			}
			if (rnum != NULL) {
				i = 0;
				while (rnum[i] != '\0')
				{
					if (strchr(rnum, ','))
						psic = 1;
					else psic = 0;
					i++;
				}
				if (psic == 1) {
					gap = 0;
					i = 0;
					while (rnum2[i] != '\0')
					{
						if (strchr(rnum2, ','))
							gap = 0;
						else gap = 1;
						i++;
					}
					num = strtok_s(rnum, ",", &pointer);
					x = atoi(num);
					num = strtok_s(NULL, "\n", &pointer);
					while (num == NULL)//When no command was given
					{
						num = strtok_s(NULL, "\t \n", &pointer);
					}
					y = atoi(num);
				}check = numbers_only(rnum, check);

			}
		}
		printf_s("Please enter valid oil supply in range 1-1000\n");
		gets_s(rnum, sizeof(rnum));
		num = strtok_s(rnum, "\n", &pointer);
		while (num == NULL)//When no command was given
		{
			printf_s("\n");
			gets_s(rnum, sizeof(rnum));//Waiting for command again
			num = strtok_s(rnum, "\n", &pointer);
		}
		num = strtok_s(rnum, " \t", &pointer);
		while (num == NULL)//When no command was given
		{
			num = strtok_s(NULL, " \t", &pointer);
		}
		if (rnum != NULL) {
			check = numbers_only(rnum, check);
			if (check != 1) {
				num = strtok_s(rnum, "\n", &pointer);
				oil = atoi(num);
			}
		}

		while (oil <= 0 || oil > 1000 || check == 1)
		{
			printf_s("Please enter valid oil supply in range 1-1000\n");
			gets_s(rnum, sizeof(rnum));
			num = strtok_s(rnum, "\n", &pointer);
			while (num == NULL)//When no command was given
			{
				printf_s("\n");
				gets_s(rnum, sizeof(rnum));//Waiting for command again
				num = strtok_s(rnum, "\n", &pointer);
			}
			num = strtok_s(rnum, " \t", &pointer);
			while (num == NULL)//When no command was given
			{
				num = strtok_s(NULL, " \t", &pointer);
			}
			if (rnum != NULL) {
				check = numbers_only(rnum, check);
				if (check != 1) {
					num = strtok_s(rnum, "\n", &pointer);
					oil = atoi(num);
				}

			}
		}

		FILE* fReading2;
		FILE* fWriting;
		checkFR2 = fopen_s(&fReading2, TXT, "r");
		checkFW = 0;
		if ((checkFR2 != 0) || (checkFW != 0))
		{
			printf_s("Problem reading pools.txt file\n");
			stone = getchar();
		}
		else
		{
			int successPrint = 0;
			log_t* br = opt3Listing(fReading2, x, y, oil, width, hight, &successPrint);
			if (successPrint == 1) {
				printf("New best-route.txt file was created\n");

				opt3c(br, width, hight);

				clearHeadBestRoute(br);

			}
			else {
				printf("Sorry, could not reach destination with these inputs\n");
			}


		}
	}
}

FILE* getDUP() {

	FILE* ORG = NULL;
	FILE* DUP = NULL;
	fopen_s(&ORG, BMP, "rb");
	if (ORG == NULL) {
		return;
	}

	fopen_s(&DUP, BMPCPY, "wb");
	if (DUP == NULL) {
		return;
	}

	unsigned int* g = fgetc(ORG);
	while (g != EOF) {

		fputc(g, DUP);

		g = fgetc(ORG);
	}
	fclose(ORG);
	return (DUP);
}


double getSX(log_t* br) {

	double s = (double)(br->next->center.y - br->center.y) / (br->next->center.x - br->center.x);
	return s;

}
double getSY(log_t* br) {

	double s = (double)(br->next->center.x - br->center.x) / (double)(br->next->center.y - br->center.y);
	return s;

}
double getCutX(log_t* br, double s) {
	return (double)br->center.y - s * br->center.x;
}
double getCutY(log_t* br, double s) {
	return (double)br->center.x - s * br->center.y;
}

int getPadding(int width) {

	int bytes = width * 3;
	int remainder = bytes % 4;

	if (remainder == 0)
		return 0;
	else {
		return 4 - remainder;
	}
}
void pixelB(int i, int currY, FILE* DUP, int width) {
	// CHANGE coordinate to 0 - based
	int x = i - 1;
	int y = currY - 1;

	// header
	fseek(DUP, 54, SEEK_SET);

	int pding = getPadding(width);

	int lineSize = width * 3 + pding;

	///MOVE on all the lines
	for (int a = 0; a < y; a++) {
		fseek(DUP, lineSize, SEEK_CUR);
	}

	fseek(DUP, x * 3, SEEK_CUR);

	fputc(255, DUP);
	fputc(0, DUP);
	fputc(0, DUP);
}
void pixelG(int i, int currY, FILE* DUP, int width) {
	// CHANGE coordinate to 0 - based
	int x = i - 1;
	int y = currY - 1;

	// header
	fseek(DUP, 54, SEEK_SET);

	int pding = getPadding(width);

	int lineSize = width * 3 + pding;

	///MOVE on all the lines
	for (int a = 0; a < y; a++) {
		fseek(DUP, lineSize, SEEK_CUR);
	}

	fseek(DUP, x * 3, SEEK_CUR);

	fputc(0, DUP);
	fputc(255, DUP);
	fputc(0, DUP);
}

void paintToNext(log_t* br, FILE* DUP, int width, int hight) {
	int i;
	if (br->next == NULL)
		return;

	// get the slope
	double s = getSX(br);

	// get the hituch with the 
	double cut = getCutX(br, s);

	// calculate the direction of advancement
	int addition = br->next->center.x > br->center.x ? 1 : -1;

	// move from the first log to the second log
	int currY;
	for (i = br->center.x; i != br->next->center.x; i += addition) {
		currY = s * i + cut;
		pixelG(i, currY, DUP, width);
	}
	currY = s * i + cut;
	pixelG(i, currY, DUP, width);

	////////////////////////YYYYYYYYYYYYYYYY
	// get the slope
	double sy = getSY(br);

	// get the hituch with the 
	double cuty = getCutY(br, sy);

	// calculate the direction of advancement
	int additiony = br->next->center.y > br->center.y ? 1 : -1;

	// move from the first log to the second log
	int currX;
	for (i = br->center.y; i != br->next->center.y; i += additiony) {
		currX = sy * i + cuty;
		pixelB(currX, i, DUP, width);
	}
	currX = sy * i + cuty;
	pixelB(currX, i, DUP, width);

	paintToNext(br->next, DUP, width, hight);
}

void opt3c(log_t* br, int width, int hight) {
	FILE* DUP = getDUP();

	paintToNext(br, DUP, width, hight);

	fclose(DUP);
}

poolList_t* addhead(poolList_t* readerList, int x, int y) {
	// create the head
	poolList_t* h = (poolList_t*)calloc(1, sizeof(poolList_t));
	h->pArr = NULL;
	h->poolCenter.x = x;
	h->poolCenter.y = y;
	h->size = 0;
	h->next = readerList;
	return h;
}

void createLog(poolList_t* readerList, double oil, double time, log_t** pH) {
	// create log for the current location
	log_t* l = (log_t*)calloc(1, sizeof(log_t));
	l->size = readerList->size;
	l->center.x = readerList->poolCenter.x;
	l->center.y = readerList->poolCenter.y;
	l->next = NULL;
	l->oil = oil;
	l->time = time;

	// add the new log to the list of logs
	log_t* head = *pH;
	if (head == NULL) {// if the list is empty
		*pH = l;
	}
	else {
		// find the last element in the list of logs
		while (head->next != NULL) {
			head = head->next;
		}
		head->next = l;
	}
}
// get the best time from the last best log
double getBestTime(log_t** headBestRoute) {
	log_t* bestTimeH = *headBestRoute;
	if (bestTimeH == NULL)
		return 10000000;

	// get the last elmeent in the list
	while (bestTimeH->next) {
		bestTimeH = bestTimeH->next;
	}

	return bestTimeH->time;
}
// clean list of logs
void clearHeadBestRoute(log_t** headBestRoute) {
	if (headBestRoute == NULL)
		return;
	log_t* hbr = *headBestRoute;
	log_t* hbr2 = *headBestRoute;

	while (hbr) {

		hbr2 = hbr;
		hbr = hbr->next;
		free(hbr2);
	}
	*headBestRoute = NULL;
}
// clean list of logs
void clearReaderList(poolList_t* h) {
	if (h == NULL)
		return;
	poolList_t* th = h;

	while (h) {

		th = h;
		h = h->next;
		free(th);
	}
}

//crete a copy for a speceific log
log_t* copyLOG(log_t* pH2) {
	log_t* copy = (log_t*)calloc(1, sizeof(log_t));
	*copy = *pH2;// copy all values
	copy->next = NULL;
}

// copy the best route from the log to the HBR
void copyHeadBestRoute(log_t** pH, log_t** headBestRoute) {
	log_t* pH2 = *pH;


	log_t* hbr = NULL;
	log_t* lastHBR = NULL;

	while (pH2) {

		// create copy for the current element
		log_t* curr = copyLOG(pH2);
		// chain to the list of best logs
		if (hbr == NULL) {
			hbr = lastHBR = curr;
		}
		else {
			lastHBR->next = curr;
			lastHBR = curr;
		}

		pH2 = pH2->next;
	}

	*headBestRoute = hbr;
}
void addLastLog(log_t** headBestRoute, double neededOil, double totalTime, int width, int hight) {
	log_t* f = *headBestRoute;
	while (f->next) {
		f = f->next;
	}
	f->next = (log_t*)calloc(1, sizeof(log_t));
	f->next->center.x = width;
	f->next->center.y = hight;
	f->next->size = 0;
	f->next->oil = neededOil;
	f->next->time = totalTime;
	f->next->next = NULL;

}

// print the best route to screen and file
void printRoute(double oil, double neededOil, double totalTime, int width, int hight, log_t** pH, log_t** headBestRoute) {

	// 1) print to screen

	log_t* pH2 = *pH;
	while (pH2) {
		printf("Time=%.2lf (%d,%d) oil=%.2lf -> ", pH2->time, pH2->center.x, pH2->center.y, pH2->oil);
		pH2 = pH2->next;
	}
	printf("Time=%.2lf (%d,%d) oil=%.2lf\n", totalTime, width, hight, oil - neededOil);

	//2) print to file
	FILE* fWriting = NULL;

	fopen_s(&fWriting, BEST_TXT, "w");
	if (!fWriting)
		return;

	fprintf_s(fWriting, "Best Route	Size\n");
	pH2 = *pH;
	while (pH2) {
		fprintf_s(fWriting, "(%3d,%3d)	%d\n", pH2->center.x, pH2->center.y, pH2->size);
		pH2 = pH2->next;
	}
	fprintf_s(fWriting, "(%3d,%3d)	0\n", width, hight);
	fclose(fWriting);

	// 3) update the best route log
	clearHeadBestRoute(headBestRoute);
	copyHeadBestRoute(pH, headBestRoute);
	// create last point
	addLastLog(headBestRoute, neededOil, totalTime, width, hight);
}

// delete the last element in list of logs
void deleteCurrLogFromList(log_t** pH) {
	log_t* l = *pH;

	// corner case: list is empty
	if (l == NULL)
		return;

	// if there is a single element
	if (l->next == NULL)
	{
		free(*pH);
		*pH = NULL;
		return;
	}

	while (l->next->next != NULL) {

		l = l->next;
	}

	free(l->next);
	l->next = NULL;
}
// FIND CLOSEST 1) not myself + 2) not starting point 
poolList_t* getClose(poolList_t* completeList, poolList_t* currLog, log_t** pH) {
	double min = 1000000;
	poolList_t* minP = NULL;

	while (completeList)
	{
		if (completeList == currLog || completeList->size == 0) {
			completeList = completeList->next;

		}
		else {
			double d = distance(completeList->poolCenter.x, currLog->poolCenter.x, completeList->poolCenter.y, currLog->poolCenter.y);
			if (d < min) {
				min = d;
				minP = completeList;
			}

			completeList = completeList->next;

		}
	}

	log_t* p = *pH;
	while (p) {
		if (minP->poolCenter.x == p->center.x && minP->poolCenter.y == p->center.y)
			return NULL;
		p = p->next;
	}

	return minP;
}

// find the second close pool from the list
// FIND CLOSEST 1) not myself + 2) not starting point 3) not closest
poolList_t* getSecondClose(poolList_t* completeList, poolList_t* currLog, log_t** pH) {
	double min1 = 1000000;
	poolList_t* min1P = NULL;
	double min2 = 1000000;
	poolList_t* min2P = NULL;

	while (completeList)
	{
		// check if the same pool OR starting point
		if (completeList == currLog || completeList->size == 0) {
			completeList = completeList->next;

		}
		else {
			double d = distance(completeList->poolCenter.x, currLog->poolCenter.x, completeList->poolCenter.y, currLog->poolCenter.y);
			if (d < min1) {
				min2 = min1;
				min2P = min1P;

				min1 = d;
				min1P = completeList;
			}
			else if (d < min2) {
				min2 = d;
				min2P = completeList;
			}

			completeList = completeList->next;

		}
	}
	// min2 is verified to be the second closest 
	log_t* p = *pH;
	while (p) {
		// check if already in the list
		if (min2P->poolCenter.x == p->center.x && min2P->poolCenter.y == p->center.y)
			return NULL;
		p = p->next;
	}

	return min2P;
}

void opt3REC(poolList_t* completeList, poolList_t* currLog, double oil, double time, int width, int hight, log_t** pH, log_t** headBestRoute, int* sup) {

	// add this pool to the lis of logs
	createLog(currLog, oil, time, pH);

	// get the time to compare with
	double bestTime = getBestTime(headBestRoute);

	// calculate if the distance is possible 
	double neededDistance = distance(currLog->poolCenter.x, width, currLog->poolCenter.y, hight);
	double neededOil = neededDistance * 0.2;
	double totalTime = time + neededDistance / 0.2;

	//check if the oil is enough for the end point
	if (neededOil < oil && totalTime < bestTime) {
		*sup = 1;
		printRoute(oil, neededOil, totalTime, width, hight, pH, headBestRoute);

		deleteCurrLogFromList(pH);
		return;
	}

	// find the closest neighbour
	poolList_t* close = getClose(completeList, currLog, pH);
	if (close != NULL) {
		double closeDist = distance(currLog->poolCenter.x, close->poolCenter.x, currLog->poolCenter.y, close->poolCenter.y);
		double closeOil = closeDist * 0.2;
		// check if the oil to the close pool is sufficeint
		if (oil >= closeOil) {
			closeOil = oil - closeOil + close->size * 0.2;
			double closeTime = time + closeDist / 0.2 + close->size;

			// call the rec
			opt3REC(completeList, close, closeOil, closeTime, width, hight, pH, headBestRoute, sup);

		}
	}

	// find the closest neighbour
	poolList_t* secondClose = getSecondClose(completeList, currLog, pH);
	if (secondClose != NULL) {
		double secondCloseDist = distance(currLog->poolCenter.x, secondClose->poolCenter.x, currLog->poolCenter.y, secondClose->poolCenter.y);
		double secondCloseOil = secondCloseDist * 0.2;
		// check if the oil to the close pool is sufficeint
		if (oil >= secondCloseOil) {
			secondCloseOil = oil - secondCloseOil + secondClose->size * 0.2;
			double secondCloseTime = time + secondCloseDist / 0.2 + secondClose->size;

			// call the rec
			opt3REC(completeList, secondClose, secondCloseOil, secondCloseTime, width, hight, pH, headBestRoute, sup);

		}
	}
	// delete the current pool from the log list
	deleteCurrLogFromList(pH);

}

log_t* opt3Listing(FILE* fReading2, int x, int y, int oil, int width, int hight, int* sup) {
	//convert file to list
	poolList_t* readerList = createReaderList(fReading2);
	fclose(fReading2);


	// create head for the list
	readerList = addhead(readerList, x, y);

	log_t* head = NULL;
	log_t* headBestRoute = NULL;
	opt3REC(readerList, readerList, oil, 0, width, hight, &head, &headBestRoute, sup);


	// clean the memory
	clearHeadBestRoute(&head);
	clearReaderList(readerList);
	return headBestRoute;
}

int main(int argc, char* argv[]) {
	char d = argc > 1 ? argv[1][0] : 'f';
	int choice = 0;
	do {
		printf("--------------------------\nME LAB services\n--------------------------\nMenu:\n");
		printf("1. Scan pools\n2. Print sorted pool list\n3. Select route\n9. Exit.\nEnter choice: ");
		choice = fgetc(stdin) - '0';
		if (choice + '0' != '\n' && fgetc(stdin) != '\n')
			while ((choice = fgetc(stdin)) != '\n');
		printf("\n");
		switch (choice) {
		case 1:
			option1(); break;
		case 2:
			option2(); break;
		case 3:
			option3(); break;
		case 9:
			printf("Good bye!\n"); break;
		default:
			printf("Bad input, try again\n\n");
		}
	} while (choice != 9);
	return 0;
}
// check if the coordinate is inside the matrix bounds
int checkBound(int w, int h, int j, int i) {
	if (j < 0 || i < 0 || j == w || i == h)
		return 0;
	else
		return 1;
}
// connect 'toConnect' to end of the head
void connect_to_end_of_pixels_list(pix_t* head, pix_t* toConnect) {
	// find the last node in the list
	while (head->next != NULL) {
		head = head->next;
	}
	head->next = toConnect;
}

pix_t* get_pixel_recursion(unsigned char** mat, int w, int h, int j, int i) {
	int cb = checkBound(w, h, j, i);
	if (cb == 0)
		return NULL;

	if (mat[i][j] != 1)
		return NULL;

	mat[i][j] = 0;

	pix_t* localPixel = (pix_t*)malloc(sizeof(pix_t));
	localPixel->next = NULL;
	localPixel->p.x = j;
	localPixel->p.y = i;

	// create variable for all 4 directions
	pix_t* adjacent[4] = { NULL, NULL, NULL, NULL };
	int directionX[4] = { 0, 0, 1, -1 };
	int directionY[4] = { 1, -1, 0, 0 };

	// get pixel list from all 4 directions
	for (int a = 0; a < 4; a++) {
		pix_t* connect = get_pixel_recursion(mat, w, h, j + directionX[a], i + directionY[a]);
		// connect the list to this pixel
		connect_to_end_of_pixels_list(localPixel, connect);
	}

	return localPixel;
}
int size_of_list(pix_t* listOfPixel) {
	int size = 0;
	do {
		size++;
		listOfPixel = listOfPixel->next;
	} while (listOfPixel);

	return size;
}

// delete a list with two pixels
void clear_pixel_list_with_2_pixels(pix_t* listOfPixel) {
	if (listOfPixel->next->next == NULL) {
		free(listOfPixel->next);
		free(listOfPixel);
		return;
	}
}
// free list of pixels
void clear_pixel_list(pix_t* listOfPixel) {

	if (listOfPixel == NULL)
		return;
	if (listOfPixel->next == NULL) {
		return;
	}
	// 2 elements
	clear_pixel_list_with_2_pixels(listOfPixel);

	// delete the list
	for (;;) {

		// check if there are only 2 pixels left in the list
		if (listOfPixel->next->next == NULL) {
			clear_pixel_list_with_2_pixels(listOfPixel);
			break;
		}

		// if there are more elements in the list
		pix_t* prev = listOfPixel;
		pix_t* curr = listOfPixel->next;

		// find the last element in the list
		while (curr->next != NULL) {
			curr = curr->next;
			prev = prev->next;

		}
		// free the last element in the list
		free(curr);
		// delete the last pointer
		prev->next = NULL;
	}
}

int get_right_pixel(pix_t* list) {
	// find the right pixel
	int right = -1;
	pix_t* temp = list;
	while (temp) {

		if (temp->p.x > right)
			right = temp->p.x;
		temp = temp->next;
	}
	return right;
}


int get_left_pixel(pix_t* list) {
	// find the right pixel
	int left = 100000;
	pix_t* temp = list;
	while (temp) {

		if (temp->p.x < left)
			left = temp->p.x;
		temp = temp->next;
	}
	return left;
}


int get_up_pixel(pix_t* list) {
	// find the right pixel
	int up = -1;
	pix_t* temp = list;
	while (temp) {

		if (temp->p.y > up)
			up = temp->p.y;
		temp = temp->next;
	}
	return up;
}

int get_down_pixel(pix_t* list) {
	// find the right pixel
	int down = 100000;
	pix_t* temp = list;
	while (temp) {
		if (temp->p.y < down)
			down = temp->p.y;
		temp = temp->next;
	}
	return down;
}


// calculate the center of the pixels of the pool
co_t center_of_list_of_pixels(pix_t* listOfPixel) {

	int right = get_right_pixel(listOfPixel);
	int left = get_left_pixel(listOfPixel);
	int up = get_up_pixel(listOfPixel);
	int down = get_down_pixel(listOfPixel);

	co_t avg;
	avg.x = right / 2 + left / 2 + 1;
	avg.y = up / 2 + down / 2 + 1;
	return avg;
}

void yes_pool(poolList_t** pPoolList, poolList_t* listOfPixel) {

	poolList_t* current;

	// attack new pool to the pool list
	if (*pPoolList == NULL) {
		*pPoolList = current = (poolList_t*)calloc(1, sizeof(poolList_t));
	}
	else {
		// make temp hold first pool
		poolList_t* temp = *pPoolList;

		// make temp hold LAST pool
		while (temp->next)
			temp = temp->next;

		temp->next = current = (poolList_t*)calloc(1, sizeof(poolList_t));
	}

	// put the list into the pool
	current->pArr = listOfPixel;

	// get the size of the pool
	current->size = size_of_list(listOfPixel);

	// get the center of the pool
	current->poolCenter = center_of_list_of_pixels(listOfPixel);

	current->next = NULL;
}
// print to file
void print_to_file(poolList_t* pl, int width, int height) {
	FILE* f = NULL;
	fopen_s(&f, TXT, "wt");
	if (f == NULL)
		return;

	fprintf(f, "Image size (%dx%d)\nPool Center	Size\n===========\t====\n", width, height);
	while (pl) {
		fprintf(f, "(%d,%d)   	%d\n", pl->poolCenter.x, pl->poolCenter.y, (int)pl->size);
		pl = pl->next;
	}

	fclose(f);
}
void clear_pool_list(poolList_t* poolList) {
	poolList_t* z;
	while (poolList) {
		z = poolList;
		poolList = poolList->next;

		clear_pixel_list(z->pArr);

		free(z);
	}
}

void option1() {
	int allPools = 0;
	unsigned char** mat = NULL;
	co_t bmpSize = { 0,0 };
	poolList_t* poolList = NULL;
	mat = readBMP(&bmpSize.x, &bmpSize.y); //read bmp file and create matrix
	if (!mat) return;
	int w = bmpSize.x;
	int h = bmpSize.y;

	//TODO			
	// lines
	for (int i = 0; i < h; i++) {
		// cols
		for (int j = 0; j < w; j++)
		{
			if (mat[i][j] == 1) {
				// call a recursion for all the pool pixel
				pix_t* listOfPixel = get_pixel_recursion(mat, w, h, j, i);
				int countpx = size_of_list(listOfPixel);
				if (countpx > 9) {
					allPools++;
					yes_pool(&poolList, listOfPixel);
				}
				else {
					clear_pixel_list(listOfPixel);
				}
			}
		}
	}

	printf("Coordinate x1,y1 of the first discoverd pool (%d,%d)\nSize %d\nTotal of %d pools.\n", poolList->poolCenter.x, poolList->poolCenter.y, (int)poolList->size, allPools);

	// print to file
	print_to_file(poolList, w, h);

	clear_pool_list(poolList);
}

/*read bmp image to a matrix
@file - bmp file
@width,hight - bmp dimension
@return the blue matrix of the image
*/
unsigned char** readBMP(unsigned int* width, unsigned int* hight) {
	FILE* bmp = NULL;
	unsigned char** blue = NULL; //blue matrix of bmp image
	//TODO reading data from a bmp file
	fopen_s(&bmp, BMP, "rb");
	if (bmp == NULL)
		return NULL;
	fseek(bmp, 18, SEEK_SET);//the Bitmap file header is 14 bytes and the size of the dbi size is 4
	//int w=0;
	//int h = 0;
	int w = 0;
	int h = 0;
	fread(&w, 4, 1, bmp);//read the width(int is 4 bytes in size,size of number of elements is 1)
	fread(&h, 4, 1, bmp);//the cruser moved another 8 bytes after read so we have 28 byts more to reach pixels
	fseek(bmp, 28, SEEK_CUR);
	*width = w;
	*hight = h;
	// m=now i will adrees the padding
	int padding = 4 - (((*width) * 3) % 4);
	if (padding == 4)
		padding == 0;

	int lineSize = w * 3 + padding;
	blue = (int*)calloc(h, sizeof(int));
	if (blue == NULL)
		return NULL;
	for (int i = 0; i < h; i++) {
		blue[i] = (int*)calloc(w, sizeof(int));
		if (blue[i] == NULL)
			return NULL;
	}

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++)
		{
			color_t RGB = { 0,0,0 };
			RGB.r = 0;
			RGB.g = 0;
			RGB.b = 0;
			fread(&RGB.r, 1, 1, bmp);
			fread(&RGB.g, 1, 1, bmp);
			fread(&RGB.b, 1, 1, bmp);
			if (RGB.b == 155 && RGB.g == 190 && RGB.r == 245)
				blue[i][j] = 1;
		}
		char p = 0;
		for (int k = 0; k < padding; k++)
			fread(&p, 1, 1, bmp);
	}



	return blue;
}

poolList_t* buildFlakesListLOOP(unsigned char** blue, int w, int h) {

}


poolList_t* buildPoolsList(unsigned char** blue, unsigned int w, unsigned int h) {
	poolList_t* res = buildFlakesListLOOP(blue, w, h);
	return res;
}

poolList_t* buildPoolsListLOOP(unsigned char** blue, unsigned int w, unsigned int h) {
	poolList_t* head = NULL;
	for (int i = 0; i < (int)h; i++)
		for (int j = 0; j < (int)w; j++)
			if (blue[i][j] == 1) {
				// step 1: create new pool node for list
				poolList_t* temp = (poolList_t*)calloc(1, sizeof(poolList_t));

				// set the new pool to the end of the pool list

				// in case this is the first pool
				if (head == NULL)
					head = temp;
				else {// in case this is just another pool
					poolList_t* endy = head;
					while (endy->next) endy = endy->next;
					endy->next = temp;
				}
				/*
								temp->pArr = findPixAdj(blue, w, h, i, j);
								temp->p1.x = j;
								temp->p1.y = i;
								temp->next = NULL;
								temp->size = findSizeOfGroup(temp->pArr);
								temp->p2.y = findP2y(temp->pArr);
								temp->p2.x = findP2x(temp->pArr, temp->p2.y);
								*/
			}

	return head;
}

poolList_t* createReaderList(FILE* fReading2)
{
	char str[100] = { 0 };
	char pointer[100] = { 0 };
	char delims[] = "\t \n,()";
	poolList_t* completed = NULL;
	poolList_t* completedEND = NULL;
	// skip 2 lines
	for (int i = 0; i < 8; i++) {
		fscanf_s(fReading2, "%s", str, 100);
	}
	fscanf_s(fReading2, "%c", &str[0], 1);
	int run = 1;
	while (run) {
		// read line
		char* after = fgets(str, 100, fReading2);
		// stop condition if the end of file have been reached
		if (after == NULL) {
			break;
		}
		poolList_t* ptr = (poolList_t*)calloc(1, sizeof(poolList_t));

		pix_t* pArr = NULL;

		char* s;
		s = strtok_s(str, delims, &pointer);
		ptr->poolCenter.x = atoi(s);

		s = strtok_s(NULL, "\t \n,()", &pointer);
		ptr->poolCenter.y = atoi(s);


		s = strtok_s(NULL, "\t \n,()", &pointer);
		ptr->size = atoi(s);
		ptr->next = NULL;

		// insert the struct to the completed list
		// this is the head of the list

		if (completed == NULL) {
			completed = ptr;
			completedEND = ptr;
		}
		else {
			completedEND->next = ptr;
			completedEND = completedEND->next;
		}
	}
	return completed;
}

int numbers_only(char* rnum, int check)
//Function for checking numbers only
{
	int i = 0;
	check = 0;
	while ((rnum[i] != '\0') && (check != 1))//Checks that the word consists of numbers only
	{
		if (rnum[i] >= '0' && rnum[i] <= '9' || rnum[i] == ' ' || rnum[i] == '\t')
			check = 0;
		else check = 1;
		i++;
	}
	return (check);
}


double distance(int x, int x1, int y, int y1)
{
	double D = 0, a = 0, b = 0;
	a = ((double)x1 - x);
	b = ((double)y1 - y);
	D = ((double)sqrt(pow((a), 2) + pow((b), 2)));
	return(D);
}


//route_t* FRoute(int x, int y, double oil, visit_t* Fvisited, double time, poolList_t* readerList, int width, int hight) {
//	double dis = distance(x, width, y, hight);
//	double neededOil = dis * 0.2;
//	double time = dis / 0.2;
//	if (neededOil <= oil) {
//
//	}
//}