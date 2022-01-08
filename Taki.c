#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> // for the srand and rand functions
#include <time.h> // to use time(NULL)

#define MAX_NAME_LEN 21
#define MIN_CARDDECK_SIZE 4
#define MAX_CARD_TYPE_LEN 6
#define MAX_CARD_COLOR_LEN 2
#define UNFINISHED 0
#define FINISHED 1
#define STATS_ARRAY_LEN 14

#define NUMBER1 1
#define NUMBER2 2
#define NUMBER3 3
#define NUMBER4 4
#define NUMBER5 5
#define NUMBER6 6
#define NUMBER7 7
#define NUMBER8 8
#define NUMBER9 9
#define PLUS 10
#define STOP 11
#define CHANGE_DIRECTION 12
#define TAKI 13
#define CHANGE_COLOR 14

#define RED 15
#define BLUE 16
#define GREEN 17
#define YELLOW 18


typedef struct Card
{
	int cardColorNum;
	int cardTypeNum;
	char cardColor[MAX_CARD_COLOR_LEN];
	char cardType[MAX_CARD_TYPE_LEN];

}CARD;

typedef struct Player
{
	char name[MAX_NAME_LEN];
	CARD* cardDeck;
	int deckSize; // logsize of the card deck
	int deckCapacity; // physical size of the card deck
	bool inTaki; //checks if player is in TAKI card effect
	int lastCardType; //stores the type of the last card to use when the TAKI-card effect ends

}PLAYER;

typedef struct cardStats
{
	int frequency;
	int typeNum;

}CARD_STATS;

typedef struct gameInfo
{
	CARD_STATS stats[STATS_ARRAY_LEN]; // the statistics array that stores stats values and indexes

	int currPlayer; // stores the current player index
	int numberOfPlayers; // number of players who are playing
	int winner; // stores the player index who is the winner
	int rotation; // clockwise - (1) , counterclockwise - (-1)
	bool ended; // changes when the game has ended to 1-FINISHED

	CARD upperCard; // the upper card at the moment

}GAME_INFO;

void red();
void green();
void blue();
void yellow();
void reset();
void consoleColor(int color);

int canPlaceCard(GAME_INFO* gameInfo, PLAYER* player, CARD* card);
void drawCard(GAME_INFO* gameInfo, PLAYER* player);
CARD* deckRealloc(PLAYER* player, int newSize);
void checkMemoryAlloc(void* array);
void getNumOfPlayers(int* numOfPlayers);
PLAYER* getPlayerNames(int numOfPlayers);
void generateCard(CARD* card);
char* getCardType(int typeNum);
void setCardType(CARD* card);
void setCardColor(CARD* card);
void createFirstUpperCard(GAME_INFO* gameInfo);
void gameStart(GAME_INFO* gameInfo, int numOfPlayers, PLAYER* playerArray);
void printUpperCard(GAME_INFO* gameInfo);
void printCard(PLAYER playerArray, int cardInd);
void printPlayerCards(PLAYER player);
void removeCard(PLAYER* player, int cardInd);
void turnText(PLAYER* player);
int doCardEffect(GAME_INFO* gameInfo, PLAYER* player, int cardType);
int gameTurn(GAME_INFO* gameInfo, PLAYER* player);
CARD_STATS* merge(CARD_STATS arr1[], int size1, CARD_STATS arr2[], int size2);
void copyArr(CARD_STATS cards1[], CARD_STATS cards2[], int size);
void mergeSort(CARD_STATS arr[], int size);
void gameLoop(GAME_INFO* gameInfo, PLAYER* playerArray);
void cleanUp(GAME_INFO* gameInfo, PLAYER* playerArray);

void main()
{
	srand(time(NULL));

	int numOfPlayers;
	GAME_INFO gameInfo;

	getNumOfPlayers(&numOfPlayers);
	PLAYER* playerArray = getPlayerNames(numOfPlayers);

	gameStart(&gameInfo, numOfPlayers, playerArray);
	gameLoop(&gameInfo, playerArray);
	
	cleanUp(&gameInfo, playerArray);
	
}

// color functions
void red()
{
	printf("\033[1;31m");
}
void green()
{
	printf("\033[0;32m");
}
void blue()
{
	printf("\033[0;34m");
}
void yellow()
{
	printf("\033[1;33m");
}
void reset()
{
	printf("\033[0m");
}
void consoleColor(int color)
{
	if (color == RED)
		red();
	else if (color == GREEN)
		green();
	else if (color == BLUE)
		blue();
	else
		yellow();
}
// creates a new dynamic card array for each player with the wanted size
CARD* deckRealloc(PLAYER* player, int newSize)
{
	CARD* newDeck = NULL;

	newDeck = (CARD*)malloc(newSize * sizeof(CARD)); //creates the new dynamic card deck
	checkMemoryAlloc(newDeck); //checks memory allocation
	player->deckCapacity = newSize; //updates the deck new physical size

	for (int i = 0; i < player->deckSize; i++)
	{
		//copys the card from the old array to the new array
		newDeck[i].cardColorNum = player->cardDeck[i].cardColorNum;
		strcpy(newDeck[i].cardColor, player->cardDeck[i].cardColor);
		newDeck[i].cardTypeNum = player->cardDeck[i].cardTypeNum;
		strcpy(newDeck[i].cardType, player->cardDeck[i].cardType);
	}
	free(player->cardDeck); //frees the old card deck

	return newDeck;
}
// checks if memory allocation was successful, if not prints out an error message
void checkMemoryAlloc(void* array)
{
	if (array == NULL)
	{
		printf("Memory allocation failed!\n");
		exit(1);
	}
}
// prints the welcome message and gets the number of players
void getNumOfPlayers(int* numOfPlayers)
{
	printf("************  Welcome to TAKI game !!! ***********\n");
	printf("Please enter the number of players: \n");

	scanf("%d", numOfPlayers);
}
// creates a dynamic players array and puts the players names into the array
PLAYER* getPlayerNames(int numOfPlayers)
{
	PLAYER* playerArray = (PLAYER*)malloc(numOfPlayers * sizeof(PLAYER));
	checkMemoryAlloc(playerArray);

	for (int i = 0; i < numOfPlayers; i++)
	{
		printf("Please enter the first name of player #%d: \n", i + 1);
		scanf("%s", &playerArray[i].name);
	}
	return playerArray;
}
// generates a random card type and color
void generateCard(CARD* card)
{
	card->cardTypeNum = rand() % 13 + 1;
	card->cardColorNum = rand() % 3 + 15;
}
// returns the string that corresponds to the card type number we got
char* getCardType(int typeNum)
{
	switch (typeNum)
	{
	case NUMBER1:
		return "1";
	case NUMBER2:
		return "2";
	case NUMBER3:
		return "3";
	case NUMBER4:
		return "4";
	case NUMBER5:
		return "5";
	case NUMBER6:
		return "6";
	case NUMBER7:
		return "7";
	case NUMBER8:
		return "8";
	case NUMBER9:
		return "9";
	case PLUS:
		return "+";
	case STOP:
		return "STOP";
	case CHANGE_DIRECTION:
		return "<->";
	case TAKI:
		return "TAKI";
	case CHANGE_COLOR:
		return "COLOR";
	default:
		return "";
	}
}
// copies the chosen string from getCardType into the card's card type
void setCardType(CARD* card)
{
	strcpy(card->cardType, getCardType(card->cardTypeNum));
}
// sets the card color according to the corresponding switch case
void setCardColor(CARD* card)
{
	switch (card->cardColorNum)
	{
	case RED:
		strcpy(card->cardColor, "R");
		break;
	case BLUE:
		strcpy(card->cardColor, "B");
		break;
	case GREEN:
		strcpy(card->cardColor, "G");
		break;
	case YELLOW:
		strcpy(card->cardColor, "Y");
		break;
	}
}
// function that draws a card from the global card deck
void drawCard(GAME_INFO* gameInfo, PLAYER* player)
{
	if (player->deckSize >= player->deckCapacity) // makes the player card deck bigger if theres no space left for a new card
	{
		player->cardDeck = deckRealloc(player, player->deckCapacity * 2);
	}
	generateCard(&player->cardDeck[player->deckSize]);
	setCardType(&player->cardDeck[player->deckSize]);
	setCardColor(&player->cardDeck[player->deckSize]);
	gameInfo->stats[player->cardDeck[player->deckSize].cardTypeNum - 1].frequency++; //adds +1 to the frequency of that specific card type in the stats array
	player->deckSize++; //updates the deck log size to +1 after each draw
}
// creates the first upper card at the beginning of the game
void createFirstUpperCard(GAME_INFO* gameInfo)
{
	generateCard(&gameInfo->upperCard);
	setCardType(&gameInfo->upperCard);
	setCardColor(&gameInfo->upperCard);
	gameInfo->stats[gameInfo->upperCard.cardTypeNum - 1].frequency++;
}
// initializes the game
void gameStart(GAME_INFO* gameInfo, int numOfPlayers, PLAYER* playerArray)
{
	for (int i = 0; i < STATS_ARRAY_LEN; i++)// resets the stats array
	{
		gameInfo->stats[i].frequency = 0;
		gameInfo->stats[i].typeNum = i + 1;
	}
	gameInfo->rotation = 1; //sets rotation to 1 - clockwise
	gameInfo->currPlayer = 0; //resets the curr player
	gameInfo->numberOfPlayers = numOfPlayers;
	gameInfo->winner = -1;
	gameInfo->ended = UNFINISHED;//sets game ended to 0-UNFINISHED

	createFirstUpperCard(gameInfo); //creates the first upper card

	for (int k = 0; k < numOfPlayers; k++) // creates the card deck array for each player
	{
		playerArray[k].deckSize = 0;
		playerArray[k].deckCapacity = MIN_CARDDECK_SIZE;
		playerArray[k].cardDeck = (CARD*)malloc(MIN_CARDDECK_SIZE * sizeof(CARD));
		playerArray[k].inTaki = false;
		checkMemoryAlloc(playerArray[k].cardDeck);
	}
	for (int i = 0; i < numOfPlayers; i++) // draws 4 cards for each player
	{
		for (int j = 0; j < MIN_CARDDECK_SIZE; j++)
		{
			drawCard(gameInfo, &playerArray[i]);
		}
	}
}
// prints the upper card
void printUpperCard(GAME_INFO* gameInfo)
{
	printf("\nUpper card:\n");

	for (int i = 0; i < 6; i++) //line
	{
		if (i == 0 || i == 5) //line 1 or 6
		{
			printf("*********\n");
		}
		else if (i == 1 || i == 4) //line 2 or 5
		{
			printf("*       *\n");
		}
		else //line 3 or 4
		{
			if (i == 2)
			{
				if (gameInfo->upperCard.cardTypeNum > 0 && gameInfo->upperCard.cardTypeNum < 11)
				{
					printf("*   ");
					consoleColor(gameInfo->upperCard.cardColorNum);
					printf("%s", gameInfo->upperCard.cardType);
					reset();
					printf("   *\n");
				}
				else if (gameInfo->upperCard.cardTypeNum == STOP || gameInfo->upperCard.cardTypeNum == TAKI)
				{
					printf("*  ");
					consoleColor(gameInfo->upperCard.cardColorNum);
					printf("%s", gameInfo->upperCard.cardType);
					reset();
					printf(" *\n");
				}
				else if (gameInfo->upperCard.cardTypeNum == CHANGE_DIRECTION)
				{
					printf("*  ");
					consoleColor(gameInfo->upperCard.cardColorNum);
					printf("%s", gameInfo->upperCard.cardType);
					reset();
					printf("  *\n");
				}
				else if (gameInfo->upperCard.cardTypeNum == CHANGE_COLOR)
				{
					printf("* ");
					printf("%s", gameInfo->upperCard.cardType);
					printf(" *\n");
				}
			}
			else if (i == 3)
			{

				printf("*   ");
				consoleColor(gameInfo->upperCard.cardColorNum);
				printf("%s", gameInfo->upperCard.cardColor);
				reset();
				printf("   *\n");

			}

		}
	}
}
// prints a card
void printCard(PLAYER player, int cardInd)
{
	for (int i = 0; i < 6; i++) //line
	{
		if (i == 0 || i == 5) //line 1 or 6
		{
			printf("*********\n");
		}
		else if (i == 1 || i == 4) //line 2 or 5
		{
			printf("*       *\n");
		}
		else //line 3 or 4
		{
			if (i == 2)
			{
				if (player.cardDeck[cardInd].cardTypeNum > 0 && player.cardDeck[cardInd].cardTypeNum < 11)
				{
					printf("*   ");
					consoleColor(player.cardDeck[cardInd].cardColorNum);
					printf("%s", player.cardDeck[cardInd].cardType);
					reset();
					printf("   *\n");
				}
				else if (player.cardDeck[cardInd].cardTypeNum == STOP || player.cardDeck[cardInd].cardTypeNum == TAKI)
				{
					printf("*  ");
					consoleColor(player.cardDeck[cardInd].cardColorNum);
					printf("%s", player.cardDeck[cardInd].cardType);
					reset();
					printf(" *\n");
				}
				else if (player.cardDeck[cardInd].cardTypeNum == CHANGE_DIRECTION)
				{
					printf("*  ");
					consoleColor(player.cardDeck[cardInd].cardColorNum);
					printf("%s", player.cardDeck[cardInd].cardType);
					reset();
					printf("  *\n");
				}
				else if (player.cardDeck[cardInd].cardTypeNum == CHANGE_COLOR)
				{
					printf("* ");
					printf("%s", player.cardDeck[cardInd].cardType);
					printf(" *\n");
				}
			}
			else if (i == 3)
			{
				if (player.cardDeck[cardInd].cardTypeNum == CHANGE_COLOR)
					printf("*       *\n");
				else
				{
					printf("*   ");
					consoleColor(player.cardDeck[cardInd].cardColorNum);
					printf("%s", player.cardDeck[cardInd].cardColor);
					reset();
					printf("   *\n");
				}
			}

		}
	}
}
// prints the deck of a player
void printPlayerCards(PLAYER player)
{
	for (int i = 0; i < player.deckSize; i++)
	{
		printf("\nCard #%d:\n", i + 1);
		printCard(player, i);
	}
}
// removes the card from the player's hand after using it
void removeCard(PLAYER* player, int cardInd)
{
	// moves each card one to the left to the position of the card that is chosen to be put, to override it and remove it from the deck
	for (int i = cardInd; i < player->deckSize - 1; i++)
	{
		player->cardDeck[i].cardColorNum = player->cardDeck[i + 1].cardColorNum;
		strcpy(player->cardDeck[i].cardColor, player->cardDeck[i + 1].cardColor);
		player->cardDeck[i].cardTypeNum = player->cardDeck[i + 1].cardTypeNum;
		strcpy(player->cardDeck[i].cardType, player->cardDeck[i + 1].cardType);
	}
	// resets the last cell in the card deck array after moving each card position, to make sure no card duplications are left
	player->cardDeck[player->deckSize - 1].cardColorNum = 0;
	strcpy(player->cardDeck[player->deckSize - 1].cardColor, "");
	player->cardDeck[player->deckSize - 1].cardTypeNum = 0;
	strcpy(player->cardDeck[player->deckSize - 1].cardType, "");
	// updates the deck log size to -1 after removing 1 card
	player->deckSize--;
}
// changes the upper card to the card the player has chosen to put
void placeCard(GAME_INFO* gameInfo, CARD* card)
{
	gameInfo->upperCard.cardColorNum = card->cardColorNum;
	strcpy(gameInfo->upperCard.cardColor, card->cardColor);
	gameInfo->upperCard.cardTypeNum = card->cardTypeNum;
	strcpy(gameInfo->upperCard.cardType, card->cardType);
}
// prints the options the player has each turn (depends on if hes in TAKI-card effect or not)
void turnText(PLAYER* player)
{
	if (player->inTaki == true)
	{
		printf("\nPlease enter 0 if you want to finish your turn\n");
		printf("or 1 - %d if you want to put one of your cards in the middle: \n", player->deckSize);
	}
	else
	{
		printf("\nPlease enter 0 if you want to take a card from the deck\n");
		printf("or 1 - %d if you want to put one of your cards in the middle: \n", player->deckSize);
	}
}
// a function that activates the corresponding card effect and returns the rotation in how the turns should go depends on the card activated
int doCardEffect(GAME_INFO* gameInfo, PLAYER* player, int cardType)
{
	switch (cardType)
	{
	case PLUS:// keeps the turn with the current player, if its the last card the player has, makes him draw another card
	{
		if (player->deckSize == 0)
			drawCard(gameInfo, player);
		return 0;
	}
	case STOP:// skips the turn of the next player
	{
		if (gameInfo->numberOfPlayers == 2)
		{
			if (player->deckSize == 0)
				drawCard(gameInfo, player);
			return 0;
		}
		else
		{
			return gameInfo->rotation * 2;
		}
	}
	case TAKI:// the player is now in TAKI-card effect and can put as many cards he'd like with the same color as the TAKI card
	{
		player->inTaki = true;
		return 0;
	}
	case CHANGE_DIRECTION:// changes the rotation in how the game turns go
	{
		gameInfo->rotation *= -1;
		return gameInfo->rotation;
	}
	default: // all the normal cards with no special effect
	{
		return gameInfo->rotation;
	}
	}
}
// has everything that happens in each game turn and return a number
// if 0 - turn stays with curr player
// if 1/2 - turn goes to the next player clockwise
// if -1/-2 - turn goes to the next player counterclockwise
int gameTurn(GAME_INFO* gameInfo, PLAYER* player)
{
	int input;

	printUpperCard(gameInfo); // prints the upper card
	printf("\n%s's turn:\n", player->name);
	printPlayerCards(*player); // prints the player cards
	turnText(player); // prints the turn text
	scanf("%d", &input); // takes the player input

	// while loop to make sure the input is valid, prints message if not
	while ((input > player->deckSize && input < 0) || (input != 0 && canPlaceCard(gameInfo, player, &player->cardDeck[input - 1]) == 0))
	{
		if (input > player->deckSize && input < 0)
		{
			printf("Invalid choice! Try again.\n");
		}
		else
		{
			printf("Invalid card! Try again.\n");
		}
		turnText(player);
		scanf("%d", &input);
	}
	if (input == 0) // if the player chose 0 and hes in TAKI-card effect means he finished his turn, else hes not inTaki and draws a card
	{
		if (player->inTaki == false)
		{
			drawCard(gameInfo, player);
			return gameInfo->rotation;
		}
		else // if inTaki == true
		{
			player->inTaki = false; // finishes the turn
			int nextTurn = doCardEffect(gameInfo, player, player->lastCardType); // saves the effect of the last card in the "TAKI fall"
			player->lastCardType = 0; // resets the last card type from the "TAKI fall"
			return nextTurn; // returns the rotation from the card effect in which the turn should go
		}
	}
	else
	{
		CARD* card = &player->cardDeck[input - 1];
		int cardType = card->cardTypeNum;

		switch (cardType)
		{

		case CHANGE_COLOR:// lets the player choose the color he wants to change the COLOR card to
		{
			printf("Please enter your color choice:\n1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
			int color;
			scanf("%d", &color);
			while (color < 1 && color > 4)// makes sure the input is valid
			{
				printf("Invalid color! Try again.\n");
				printf("Please enter your color choice:\n1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
				scanf("%d", &color);
			}

			card->cardColorNum = 15 + ((-2 + color + 4) % 4); // changes to the corresponding color to the color num entered
			setCardColor(card);
			placeCard(gameInfo, card);
			removeCard(player, input - 1);
			if (player->inTaki == true) // if player was in TAKI-card effect, it now exists the TAKI-card effect
			{
				player->inTaki = false;
				player->lastCardType = 0; // resets the last card in the "TAKI fall" that was saved
			}
			return gameInfo->rotation;
		}
		default: // the case for all the cards that are not CHANGE COLOR
		{
			placeCard(gameInfo, card);
			removeCard(player, input - 1);
			if (player->inTaki == false)// if the player is not in "TAKI fall", do the card effect as usual
			{
				return doCardEffect(gameInfo, player, cardType);
			}
			else// if the player is in "TAKI fall" then save the card type of the chosen card
			{
				player->lastCardType = cardType;
				return 0;
			}
		}
		}
	}
}
// checks if the chosen card can be placed returns 1/0
int canPlaceCard(GAME_INFO* gameInfo, PLAYER* player, CARD* card)
{
	if (card->cardTypeNum == CHANGE_COLOR) // the change color card can be placed anytime
		return 1;
	if (gameInfo->upperCard.cardColorNum == card->cardColorNum)
		return 1;// checks if the color of the chosen card fits the color of the upper card
	if (gameInfo->upperCard.cardTypeNum == card->cardTypeNum && player->inTaki == false)
		return 1;// checks if the type of the chosen card fits the type of the upper card and the player is not in the TAKI-card effect
	return 0;
}
// prints at the end of the game the statistics of how many times each card was drawn in the game
void printStats(GAME_INFO* gameInfo)
{
	mergeSort(gameInfo->stats, STATS_ARRAY_LEN);
	printf("************ Game Statistics ************\n");
	printf("Card # | Frequency\n");
	printf("__________________\n");
	for (int i = 0; i < STATS_ARRAY_LEN; i++)
	{
		CARD_STATS* stat = &gameInfo->stats[i];
		if (stat->typeNum == CHANGE_COLOR)
		{
			printf(" COLOR |    %d\n", stat->frequency);
		}
		else if (stat->typeNum == TAKI || stat->typeNum == STOP)
		{
			printf(" %s  |    %d\n", getCardType(stat->typeNum), stat->frequency);
		}
		else if (stat->typeNum == CHANGE_DIRECTION)
		{
			printf("  <->  |    %d\n", stat->frequency);
		}
		else
		{
			printf("   %s   |    %d\n", getCardType(stat->typeNum), stat->frequency);
		}
	}

}
// the game loop thats responsible for the whole game and ends when there is a winner
void gameLoop(GAME_INFO* gameInfo, PLAYER* playerArray)
{
	while (gameInfo->ended == UNFINISHED)
	{
		PLAYER* currPlayer = &playerArray[gameInfo->currPlayer];
		int turn = gameTurn(gameInfo, currPlayer);

		if (currPlayer->deckSize == 0) // if a player reaches 0 cards, he wins
		{
			gameInfo->winner = gameInfo->currPlayer; // updates the winner to the current player
			gameInfo->ended = FINISHED; // updates the bool that the game has finished
			printf("The winner is... %s! Congratulations!\n\n", currPlayer->name);
			printStats(gameInfo); // prints the statistics of the game
		}
		else // if no one won, keep the game going
		{
			gameInfo->currPlayer += turn;

			// if the curr player is a negative number it adds the number of players to get back to the right rotation
			if (gameInfo->currPlayer < 0)
			{
				gameInfo->currPlayer += gameInfo->numberOfPlayers;
			}
			// if the curr player is a bigger than/equal to the number of players, it subtracts the number of players to get back to the right rotation
			else if (gameInfo->currPlayer >= gameInfo->numberOfPlayers)
			{
				gameInfo->currPlayer -= gameInfo->numberOfPlayers;
			}
		}
	}
}
// a merge function
CARD_STATS* merge(CARD_STATS arr1[], int size1, CARD_STATS arr2[], int size2)
{
	CARD_STATS* arrFinal;
	arrFinal = (CARD_STATS*)malloc((size1 + size2) * sizeof(CARD_STATS));

	checkMemoryAlloc(arrFinal);

	int read1, read2;
	int write;

	read1 = read2 = write = 0;

	while ((read1 < size1) && (read2 < size2))
	{
		if (arr1[read1].frequency > arr2[read2].frequency)
		{
			arrFinal[write].frequency = arr1[read1].frequency;
			arrFinal[write].typeNum = arr1[read1].typeNum;
			write++;
			read1++;
		}
		else
		{
			arrFinal[write].frequency = arr2[read2].frequency;
			arrFinal[write].typeNum = arr2[read2].typeNum;
			write++;
			read2++;
		}
	}
	while (read1 < size1)
	{
		arrFinal[write].frequency = arr1[read1].frequency;
		arrFinal[write].typeNum = arr1[read1].typeNum;
		write++;
		read1++;
	}
	while (read2 < size2)
	{
		arrFinal[write].frequency = arr2[read2].frequency;
		arrFinal[write].typeNum = arr2[read2].typeNum;
		write++;
		read2++;
	}
	return arrFinal;
}
// a function that copies one array onto another
void copyArr(CARD_STATS cards1[], CARD_STATS cards2[], int size)
{
	for (int i = 0; i < size; i++)
	{
		cards1[i].frequency = cards2[i].frequency;
		cards1[i].typeNum = cards2[i].typeNum;
	}
}
// a marge sort function
void mergeSort(CARD_STATS arr[], int size)
{
	CARD_STATS* tmpArr = NULL;
	if (size <= 1)
		return;

	mergeSort(arr, size / 2);
	mergeSort(arr + size / 2, size - size / 2);
	tmpArr = merge(arr, size / 2, arr + size / 2, size - size / 2);
	if (tmpArr)
	{
		copyArr(arr, tmpArr, size);
		free(tmpArr);
	}
	else
	{
		printf("Memory allocation failure!!!\n");
		exit(1);
	}
}
// frees the dynamic arrays of each player's card deck and the players array
void cleanUp(GAME_INFO* gameInfo, PLAYER* playerArray)
{
	for (int i = 0; i < gameInfo->numberOfPlayers; i++)
	{
		free(playerArray[i].cardDeck);
	}
	free(playerArray);
}
