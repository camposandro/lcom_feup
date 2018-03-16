#ifndef __GAME_H
#define __GAME_H

/**
 * @file game.h
 */

/**
 *	@defgroup Game
 *	@{
 *
 *	Functions implementing the game's logic
 */

/* PNG image paths */
#define WORDS_FILEPATH		"/home/snaktionary/res/words.txt"
#define WINNERS_FILEPATH	"/home/snaktionary/res/winners.txt"
#define FONT_IMGPATH		"/home/snaktionary/res/font.png"
#define CURSOR_IMGPATH		"/home/snaktionary/res/cursor.png"
#define MENU_IMGPATH		"/home/snaktionary/res/menu.png"
#define MENUPLAY_IMGPATH	"/home/snaktionary/res/menu_play.png"
#define MENUEXIT_IMGPATH	"/home/snaktionary/res/menu_exit.png"
#define SNAKE_VICT_IMGPATH	"/home/snaktionary/res/snake_won.png"
#define CURSOR_VICT_IMGPATH	"/home/snaktionary/res/cursor_won.png"

/* Keyboard's game keys */
#define W_KEY	0x11
#define A_KEY	0x1e
#define S_KEY	0x1f
#define D_KEY	0x20

/* Game's borders size */
#define BORDER_SIZE		5
#define MIDDLE_BORDER	495

/**
 *	@brief Outputs the name of the winner
 */
#define WHO_WON(x) (x ? "Snake": "Mouse")

/**
 * @brief Game states
*/
typedef enum { INIT, MENU, PLAY, LEAVE } game_state_t;

/**
 * @brief Game events
*/
typedef enum { NO_EVENT, PLAY_BUTTON, EXIT_BUTTON, END_GAME } game_event_t;

/**
 * @brief Game's object coordinates
*/
typedef struct coord_t {
	int x; /**< Object's x coordinate on the screen */
	int y; /**< Object's y coordinate on the screen */
} coord_t;

/**
 * @brief Game's cursor
*/
typedef struct Cursor {
	coord_t coord;			/**< Cursor's coordinates */
	int width;				/**< Cursor's image width */
	int height;				/**< Cursor's image height */
	unsigned char* image;	/**< Cursor's png image */
} Cursor;

/**
 * @brief Game's menu
*/
typedef struct Menu {
	int width;							/**< Menu's image width */
	int height;							/**< Menu's image height */
	coord_t* play_button;					/**< Menu's play button coordinates on the screen */
	coord_t* exit_button;					/**< Menu's exit button coordinates on the screen */
	unsigned char* menu;				/**< Menu's main png image */
	unsigned char* menu_play;			/**< Menu's png image with play button selected */
	unsigned char* menu_exit;			/**< Menu's png image with exit button selected */
	unsigned char* snake_victory;		/**< Snake's victory png image */
	unsigned char* cursor_victory;		/**< Cursor's victory png image */
	unsigned char* current_background;	/**< Menu's current png image */
} Menu;

/**
 * @brief Game's in-game font
*/
typedef struct Font {
	int width;					/**< Font's image width */
	int height;					/**< Font's image height */
	unsigned char* font_img;	/**< Font's png image */
} Font;

/**
 * @brief Game's word
*/
typedef struct Word {
	char* letters;					/**< Word's set of letters */
	coord_t* coord_kbd;				/**< Word's letters coordinates for the snake */
	coord_t* coord_mouse;				/**< Word's letters coordinates for the cursor */
	unsigned int n_letters_kbd;		/**< Word's number of letters for the keyboard */
	unsigned int n_letters_mouse;	/**< Word's number of letters plus deceiving mouse letters */
} Word;

/**
 * @brief Game's snake block
*/
typedef struct Snake {
	coord_t coord;			/**< Snake's block coordinates on the screen */
	int side;				/**< Snake's block side size */
	unsigned long key;		/**< Snake's block moving direction */
	unsigned long color;	/**< Snake's block color */
	struct Snake* next;		/**< Snake's next block */
} Snake;

/**
 * @brief Game's struct
*/
typedef struct Game {
	int hookid_timer;				/**< Timer's irq line (0) */
	int hookid_kbd;					/**< Keyboard's irq line (1) */
	int hookid_mouse;				/**< Mouse's irq line (12) */
	Menu* menu;						/**< Game's menu */
	Cursor* cursor;					/**< Game's cursor */
	Font* font;
	Word* words;					/**< Game's words */
	Snake* snake;					/**< Game's snake */
	unsigned int n_words;			/**< Game's number of words */
	game_state_t current_state;		/**< Game's current state */
} Game;

/**
 *  @brief Game's event handler
 *
 *  Handles an event generated throughout the game, changing the
 *  game's state based on its current state and event passed as
 *  an argument.
 *
 *  @param game Pointer to the game's struct
 *  @param game_event Game's event to be handled
 */
void handle_event(Game* game, game_event_t game_event);

/**
 *  @brief Game's menu initializer
 *
 * 	Initializes the game's menu along with its backgrounds and
 * 	returns a pointer to it.
 *
 *  @return Returns pointer to the game's menu
 */
Menu* initialize_menu();

/**
 *  @brief Game's menu destroyer
 *
 * 	Destroys the game's menu, freeing all memory allocated to it.
 *
 *	@param menu Game's menu to be destroyed
 */
void destroy_menu(Menu* menu);

/**
 *  @brief Game's menu handler
 *
 * 	Handles cursor's interaction with the menu. The user may select
 * 	2 types of buttons clicking on them with the mouse's left button-
 * 	-the "Play Button" or the "Exit Button", choosing whether to start
 * 	a game or to leave.
 *
 *	@param game Pointer to game's struct
 */
void menu_handling(Game* game);

/**
 *  @brief Game's cursor initializer
 *
 * 	Initializes the game's cursor along with its initial coordinates
 * 	and png image.
 *
 *	@return Returns pointer to the game's cursor
 */
Cursor* initialize_cursor();

/**
 *  @brief Game's cursor destroyer
 *
 * 	Destroys the game's cursor, freeing all memory allocated to store it.
 *
 *	@param cursor Game's cursor to be destroyed
 */
void destroy_cursor(Cursor* cursor);

/**
 *  @brief Cursor's clean function
 *
 * 	Cleans cursor's position at some moment.
 *
 *	@param Game Game's struct
 *	@param word Word currently being printed on the screen
 *	@param letter_index Letter's index to start printing the word letters
 */
void clean_cursor(Game* game, Word word, int letter_index);

/**
 *  @brief Prints game's cursor on the screen
 *
 *  Function to print the cursor on the screen based on its current background,
 *  defined in the game's menu. If parameter "menu" is passed as being NULL, the
 *  cursor is printed over no background.
 *
 *  @param cursor Game's cursor
 *  @param menu	Game's menu
 */
void print_cursor(Cursor* cursor, Menu* menu);

/**
 *  @brief Updates game's cursor position on the screen
 *
 *	Updates cursor's position on the screen, after the received mouse packet bytes.
 *	If the cursor is in the game's menu then it is allowed to move on the entire screen,
 *	however if it is playing, it is only allowed to move inside its right part of the screen.
 *
 *  @param cursor Game's cursor
 *  @param in_menu Variable indicating whether cursor is in menu or not
 */
void update_cursor(Cursor* cursor, int in_menu);

/**
 *  @brief Tests cursor's collision
 *
 *	Tests any collision with the cursor, right after updating its position
 *	on the screen. Collision is detected when the player clicks the mouse's
 *	left-button on any letter on the screen.
 *
 *  @param cursor Game's cursor
 *  @param word Word to test collision with
 *  @param letter_index Letter's index the snake is supposed to collide with
 *  @return Returns 0 on non-collision, 1 on collision with the wrong letter,
 *  2 if collision with the right letter occurs.
 */
int test_collision_cursor(Cursor* cursor, Word word, int letter_index);

/**
 *  @brief Game's font initializer
 *
 * 	Initializes the game's font.
 *
 *	@return Returns pointer to the game's font
 */
Font* initialize_font();

/**
 *  @brief Game's font destroyer
 *
 * 	Destroys the game's font, freeing all memory allocated to store it.
 *
 *	@param font Game's font to be destroyed
 */
void destroy_font(Font* font);

/**
 *  @brief Words reading function
 *
 * 	Reads all words from the "words.txt" file, allocating memory
 * 	for them to be added into the game. Returns a pointer to the
 * 	struct containing these word's information.
 *
 *	@return Returns a pointer to the game's word struct
 */
Word* read_words();

/**
 *  @brief Game's word destroyer
 *
 * 	Destroys the game's word database, freeing all memory allocated
 * 	to store them.
 *
 *	@param words Game's word struct to be destroyed
 */
void destroy_words(Word* words);

/**
 *  @brief Game's spawn letters function
 *
 * 	Function for spawning word letters on the screen, based on the
 * 	word and letter_index passed as arguments. The letters printed
 * 	from the word's letters array start from letter_index and end
 * 	when there's no letter left.
 *
 *	@param game Game's struct
 *	@param word Word to be printed on the screen
 *	@param letter_index Letter's index from where to start spawning
 *	@param kbd Variable indicating whether to spawn on snake or in
 *	cursor's side of the screen
 */
void spawn_letters(Game* game, Word word, unsigned int letter_index, int kbd);

/**
 *  @brief Snake's node creator
 *
 * 	Creates a new block for the snake with the information
 * 	passed as a parameter.
 *
 *	@param info Information for the new snake block
 *	@return Returns pointer to the created snake node
 */
Snake* new_node(Snake info);

/**
 *  @brief Snake's push_front() function
 *
 * 	Inserts a new block into the front of the snake's linked-list,
 * 	passed as a parameter.
 *
 *	@param node Snake's node to be inserted
 */
void push_front(Snake* node);

/**
 *  @brief Snake's push_back() function
 *
 * 	Inserts a new block into the back of the snake's linked-list,
 * 	passed as a parameter.
 *
 *	@param node Snake's node to be inserted
 */
void push_back(Snake* node);

/**
 *  @brief Snake's pop_front() function
 *
 * 	Erases the first element of the snake linked-list, returning it.
 *
 *	@return Returns pointer to the snake's block removed
 */
Snake* pop_front();

/**
 *  @brief Snake's pop_back() function
 *
 * 	Erases the last element of the snake linked-list, returning it.
 *
 *	@return Returns pointer to the snake's block removed
 */
Snake* pop_back();

/**
 *  @brief Spawns a new block on the snake
 *
 * 	Creates a new block and using push_front() function inserts it
 * 	on the snake. The snake grows by 1 block.
 *
 *	@param snake Snake struct
 */
void spawn_block(Snake* snake);

/**
 *  @brief Game's snake initializer
 *
 * 	Initializes the game's snake with a block and returns
 * 	its created struct.
 *
 *	@return Returns pointer to the game's snake
 */
Snake* initialize_snake();

/**
 *  @brief Game's snake destroyer
 *
 * 	Destroys the game's snake, freeing all memory allocated to it.
 *
 *	@param snake Game's snake to be destroyed
 */
void destroy_snake(Snake* snake);

/**
 *  @brief Prints game's snake on the screen
 *
 *  Function to print the snake on the screen. The current word is important
 *  while printing the snake, so is the index of the next letter to be eaten.
 *  Hence, after printing the snake, the letters left to be eaten of the
 *  current word are re-printed to the screen.
 *
 *  @param game Game's struct
 *  @param word Snake's current playing word
 *  @param letter_index Word's letter index to start printing the word letters
 */
void print_snake(Game* game, Word word, unsigned int letter_index);

/**
 *  @brief Updates game's snake position on the screen
 *
 *	Updates snake's position on the screen, after the received keyboard scancode.
 *	As it only appears on the screen while playing, the snake is only able to move
 *	inside its left part of the screen.
 *
 *  @param scancode Scancode to update snake's moving direction
 */
void update_snake(unsigned long scancode);

/**
 *  @brief Tests snake's collision
 *
 *	Tests any collision with the snake, right after updating its position on
 *	the screen. Collision may be detected against the game predefined borders,
 *	the snake body itself or against any letter printed on the screen.
 *
 *  @param snake Game's snake
 *  @param word Word to test collision with
 *  @param letter_index Letter's index the snake is supposed to collide with
 *  @return Returns 0 on non-collision, 1 on collision with the wrong letter or with
 *  the snake's body itself, 2 if collision with the right letter occurs.
 */
int test_collision_snake(Snake* snake, Word word, int letter_index);

/**
 *  @brief Game's main menu
 *
 * 	Game's entry point.
 *
 *	@param game Pointer to game's struct
 *	@return Returns 0 on success and non 0 otherwise
 */
int main_menu(Game* game);

/**
 *  @brief Game's logic function
 *
 * 	Game's main function allowing the players to interact with the snake
 * 	and mouse, simultaneously. The snake's player must orderly collide the
 * 	snake with the word's letters shown on the screen. At the same time,
 * 	the cursor's player objective is similar, but using the mouse's left button
 * 	to click on them. The first player to end the game, or the last one to make
 * 	a mistake, wins.
 *
 *	@param game Pointer to game's struct
 *	@return Returns 0 on success and non 0 otherwise
 */
int play_game(Game* game);

/**
 *  @brief Function to write the winner to file
 *
 * 	Writes game's winner to the file "winner.txt" along with the date and hour
 * 	of the game played.
 *
 *	@param winner Parameter representing the winner
 *	@return Returns 0 on success and non 0 otherwise
 */
int write_winner(int winner);

/**
 * 	@brief Prints victory screen
 *
 * 	After the game ends, this function is responsible for printing the respective
 * 	winner screen, being the winner passed a char* parameter (it may only be
 * 	"snake" or "cursor").
 *
 * 	@param Menu Game's menu
 * 	@param winner Game's winner ("snake" or "cursor")
 */
void victory_screen(Menu* menu, char* winner);

/**@}*/

#endif /* __GAME_H */
