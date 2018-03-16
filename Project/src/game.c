#include <minix/syslib.h>
#include <minix/driver.h>
#include <minix/drivers.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include "game.h"
#include "stbi_png.h"
#include "video_gr.h"
#include "kbd.h"
#include "timer.h"
#include "mouse.h"
#include "rtc.h"

/* irq lines for IO/devices */
int g_hookid_timer = 0;
int g_hookid_kbd = 1;
int g_hookid_mouse = 12;

/* variables storing and counting bytes */
unsigned long g_scancode;
unsigned long g_packet[3];
unsigned short g_count_bytes = 0;
unsigned long g_byte;

unsigned long status, outbuff_trash;

/* Snake singly-linked list */
struct Snake* first_node = NULL;
struct Snake* last_node = NULL;

void handle_event(Game* game, game_event_t game_event) {
	switch (game->current_state) {
	case INIT:
		/* IO devices hookid's */
		game->hookid_timer = 0;
		game->hookid_kbd = 1;
		game->hookid_mouse = 12;
		/* initializing menu */
		game->menu = initialize_menu();
		/* initializing cursor */
		game->cursor = initialize_cursor();
		/* subscribe timer 0 interrupts */
		timer_subscribe_int(&g_hookid_timer);
		/* subscribe mouse interrupts */
		mouse_subscribe_int(&g_hookid_mouse);
		/* enable mouse */
		mouse_write_cmd(ENABLE_MOUSE);
		/* start vg 800x600 resolution */
		vg_init(0x115);
		/* go to menu */
		game->current_state = MENU;
		main_menu(game);
		break;
	case MENU:
		if (game_event == PLAY_BUTTON) {
			/* initialize snake */
			game->snake = initialize_snake();
			/* initializes game font */
			game->font = initialize_font();
			/* read words from file */
			game->words = read_words(&game->n_words);
			/* subscribe keyboard interrupts */
			kbd_subscribe_int(&g_hookid_kbd);
			/* play game */
			game->current_state = PLAY;
			play_game(game);
		} else if (game_event == EXIT_BUTTON) {
			/* unsubscribe timer 0 interrupts */
			timer_unsubscribe_int(&g_hookid_timer);
			/* unsubscribe mouse interrupts */
			mouse_unsubscribe_int(&g_hookid_mouse);
			/* destroy cursor */
			destroy_cursor(game->cursor);
			/* destroy menu */
			destroy_menu(game->menu);
			/* free game */
			free(game);
			/* exit vg mode */
			vg_exit();
			vg_free();
			/* leave the game */
			game->current_state = LEAVE;
		}
		break;
	case PLAY:
		if (game_event == END_GAME) {
			/* destroys words */
			destroy_words(game->words);
			/* destroys font */
			destroy_font(game->font);
			/* destroys snake */
			destroy_snake(game->snake);
			/* unsubscribe keyboard interrupts */
			kbd_unsubscribe_int(&g_hookid_kbd);
			g_hookid_kbd = 1;
			/* return to menu */
			game->current_state = MENU;
		}
		break;
	default:
		break;
	}
}

Menu* initialize_menu() {

	Menu* menu = (Menu *) malloc(sizeof(Menu));
	if (menu == NULL)
		return NULL;

	/* loading menu png images */
	menu->menu = stbi_png_load(&menu->width, &menu->height, MENU_IMGPATH);
	if (menu->menu == NULL) {
		printf("Menu's \"menu\" png image not found!\n");
		return NULL;
	}
	menu->menu_play = stbi_png_load(&menu->width, &menu->height,
			MENUPLAY_IMGPATH);
	if (menu->menu_play == NULL) {
		printf("Menu's \"menu_play\" png image not found!\n");
		return NULL;
	}
	menu->menu_exit = stbi_png_load(&menu->width, &menu->height,
			MENUEXIT_IMGPATH);
	if (menu->menu_exit == NULL) {
		printf("Menu's \"menu_exit\" png image not found!\n");
		return NULL;
	}
	menu->snake_victory = stbi_png_load(&menu->width, &menu->height,
			SNAKE_VICT_IMGPATH);
	if (menu->snake_victory == NULL) {
		printf("Snake's victory png image not found!\n");
		return NULL;
	}
	menu->cursor_victory = stbi_png_load(&menu->width, &menu->height,
			CURSOR_VICT_IMGPATH);
	if (menu->cursor_victory == NULL) {
		printf("Cursor's victory png image not found!\n");
		return NULL;
	}

	menu->current_background = menu->menu;

	menu->play_button = (coord_t *) malloc(sizeof(coord_t));
	menu->exit_button = (coord_t *) malloc(sizeof(coord_t));

	/* -- PLAY button -- */
	/* left corner coordinates */
	(menu->play_button[0]).x = 340;
	(menu->play_button[0]).y = 190;
	/* right corner coordinates */
	(menu->play_button[1]).x = 450;
	(menu->play_button[1]).y = 250;

	/* -- EXIT button -- */
	/* left corner coordinates */
	(menu->exit_button[0]).x = 340;
	(menu->exit_button[0]).y = 290;
	/* right corner coordinates */
	(menu->exit_button[1]).x = 450;
	(menu->exit_button[1]).y = 350;

	return menu;
}

void destroy_menu(Menu* menu) {

	stbi_free(menu->menu);
	stbi_free(menu->menu_play);
	stbi_free(menu->menu_exit);
	stbi_free(menu->snake_victory);
	stbi_free(menu->cursor_victory);
	free(menu);
}

void menu_handling(Game* game) {

	int playBoxX = (game->menu->play_button[0]).x;
	int playBoxY = (game->menu->play_button[0]).y;
	int exitBoxY = (game->menu->exit_button[0]).y;
	int playBoxFinalX = (game->menu->play_button[1]).x;
	int playBoxFinalY = (game->menu->play_button[1]).y;
	int exitBoxFinalY = (game->menu->exit_button[1]).y;

	if ((game->cursor->coord).x >= playBoxX
			&& (game->cursor->coord).x <= playBoxFinalX) {
		if ((game->cursor->coord).y >= playBoxY
				&& (game->cursor->coord).y <= playBoxFinalY) {

			game->menu->current_background = game->menu->menu_play;

			/* if option was clicked */
			if (g_packet[0] & LB) {
				/* play button selected */
				handle_event(game, PLAY_BUTTON);
			}

		} else if ((game->cursor->coord).y >= exitBoxY
				&& (game->cursor->coord).y <= exitBoxFinalY) {

			game->menu->current_background = game->menu->menu_exit;

			/* if option was clicked */
			if (g_packet[0] & LB) {
				/* exit button selected */
				handle_event(game, EXIT_BUTTON);
			}
		} else
			game->menu->current_background = game->menu->menu;
	} else
		game->menu->current_background = game->menu->menu;
}

Cursor* initialize_cursor() {

	Cursor* cursor = (Cursor *) malloc(sizeof(Cursor));
	if (cursor == NULL)
		return NULL;

	/* initial coordinates -> center of the screen */
	(cursor->coord).x = 395;
	(cursor->coord).y = 270;

	/* loading png's cursor image */
	cursor->image = stbi_png_load(&cursor->width, &cursor->height,
			CURSOR_IMGPATH);
	if (cursor->image == NULL) {
		printf("Cursor's png image not found!\n");
		return NULL;
	}

	return cursor;
}

void destroy_cursor(Cursor* cursor) {

	stbi_free(cursor->image);
	free(cursor);
}

void clean_cursor(Game* game, Word word, int letter_index) {

	vg_cursor_clear();

	size_t x, y;
	for (y = 0; y < game->cursor->width; y++) {
		for (x = 0; x < game->cursor->height; x++) {
			draw_pixel((game->cursor->coord).x + x, (game->cursor->coord).y + y,
					MOUSE_BG_COLOR);
		}
	}

	spawn_letters(game, word, letter_index, 0);

	vg_copy();
}

void print_cursor(Cursor* cursor, Menu* menu) {

	/* prints cursor over initial menu png to the screen */
	if (menu != NULL)
		vg_png(menu->current_background, menu->width, menu->height, 0, 0);

	size_t x, y;
	for (y = 0; y < cursor->height; y++) {
		for (x = 0; x < cursor->width; x++) {
			uint32_t rgb[3];
			size_t c;
			for (c = 0; c < 3; c++)
				rgb[c] = cursor->image[(x + y * cursor->width) * 3 + c];
			int32_t color = ((rgb[0] << 16) & RED) | ((rgb[1] << 8) & GREEN)
							| (rgb[2] & BLUE);
			draw_pixel((cursor->coord).x + x, (cursor->coord).y + y, color);
		}
	}

	vg_copy();
}

void update_cursor(Cursor* cursor, int in_menu) {

	signed char delta_x = g_packet[1];
	signed char delta_y = g_packet[2];

	if (delta_x > 10)
		delta_x = 10;
	else if (delta_x < -10)
		delta_x = -10;

	if (delta_y > 10)
		delta_y = 10;
	else if (delta_y < -10)
		delta_y = -10;

	/* update mouse coordinates */
	(cursor->coord).x += delta_x;
	(cursor->coord).y -= delta_y;

	/* test for collision with the screen */
	if (!in_menu) {
		if ((cursor->coord).x >= H_RES - BORDER_SIZE - 10)
			(cursor->coord).x = H_RES - BORDER_SIZE - 10;
		else if ((cursor->coord).x < MIDDLE_BORDER + BORDER_SIZE)
			(cursor->coord).x = MIDDLE_BORDER + BORDER_SIZE;

		if ((cursor->coord).y >= V_RES - BORDER_SIZE - 10)
			(cursor->coord).y = V_RES - BORDER_SIZE - 10;
		else if ((cursor->coord).y < BORDER_SIZE)
			(cursor->coord).y = BORDER_SIZE;
	} else {
		if ((cursor->coord).x >= H_RES)
			(cursor->coord).x = H_RES;
		else if ((cursor->coord).x < 0)
			(cursor->coord).x = 0;

		if ((cursor->coord).y >= V_RES)
			((cursor->coord).y = V_RES);
		else if ((cursor->coord).y < 0)
			(cursor->coord).y = 0;
	}
}

int test_collision_cursor(Cursor* cursor, Word word, int letter_index) {

	/* test collision with letters on the screen */
	size_t i;
	for (i = letter_index; i < word.n_letters_mouse; i++) {

		int distance_x = word.coord_mouse[i].x - cursor->coord.x;
		int distance_y = word.coord_mouse[i].y - cursor->coord.y;

		/* if blocks collided horizontally or vertically */
		if (abs(distance_y) < 16 && abs(distance_x) < 16) {

			/* if letter eaten is in the correct order */
			if (i != letter_index) {

				/* if eaten letter has more than 2 appearances on the word */
				if (word.letters[i] == word.letters[letter_index]) {
					coord_t temp = word.coord_mouse[i];
					word.coord_mouse[i] = word.coord_mouse[letter_index];
					word.coord_mouse[letter_index] = temp;

					return 2;
				}

				return 1;
			}

			return 2;
		}
	}

	return 0;
}

Font* initialize_font() {

	Font* font = (Font *) malloc(sizeof(Font));
	if (font == NULL)
		return NULL;

	font->font_img = stbi_png_load(&font->width, &font->height, FONT_IMGPATH);
	if (font->font_img == NULL) {
		printf("Font's png image not found!\n");
		return NULL;
	}

	return font;
}

void destroy_font(Font* font) {

	stbi_free(font->font_img);
	free(font);
}

Word* read_words(size_t* n_words) {

	/* maximum number of words is 20 */
	struct Word* words = (struct Word *) malloc(20 * sizeof(struct Word));
	if (words == NULL)
		return NULL;

	*n_words = 0;
	char line[100];

	FILE* words_file = NULL;
	words_file = fopen(WORDS_FILEPATH, "r");

	if (words_file != NULL) {
		printf("File successfully read!\n");

		/* read each line of the file */
		while (fgets(line, 100, words_file) != NULL) {

			/* ignoring \n chars */
			if (line[strlen(line) - 1] == '\n')
				line[strlen(line) - 1] = '\0';

			/* allocating mem space to store letters and its coordinates */
			words[*n_words].letters = (char *) malloc(sizeof(line));
			words[*n_words].coord_kbd = (coord_t *) malloc(
					strlen(line) * sizeof(coord_t));
			words[*n_words].coord_mouse = (coord_t *) malloc(
					(strlen(line) + 20) * sizeof(coord_t));

			int x, y, kbdUsedCoord, mouseUsedCoord;
			size_t i;
			for (i = 0; i < strlen(line); i++) {
				words[*n_words].letters[i] = toupper(line[i]);

				/* generating coordinates for the new letter,
				 * considering not to obtain occupied positions */
				do {
					kbdUsedCoord = 0;

					do {
						x = (rand() % 469) + 6;
					} while (x % 20 != 0);

					do {
						y = (rand() % 574) + 6;
					} while (y % 20 != 0);

					size_t j;
					for (j = 0; j < i; j++) {

						int equal_x, equal_y, distance_x, distance_y;

						/* if coordinates coincide with those of other letter */
						equal_x = words[*n_words].coord_kbd[j].x == x;
						equal_y = words[*n_words].coord_kbd[j].y == y;

						/* verifying the distance of the spawned letter
						 * to the snake when in between levels */
						if (*n_words > 0) {
							distance_x =
									abs(
											(words[*n_words - 1].coord_kbd[words[*n_words
																				 - 1].n_letters_kbd - 1].x)
																				 - x);
							distance_y =
									abs(
											(words[*n_words - 1].coord_kbd[words[*n_words
																				 - 1].n_letters_kbd - 1].y)
																				 - y);
						} else {
							distance_x = abs((last_node->coord).x - x);
							distance_y = abs((last_node->coord).y - y);
						}

						/* if coordinate is used or distance between the snake
						 * and the spawned letter is inferior to 50, being too close
						 * and immediately colliding with the snake */
						if ((equal_x && equal_y) || (distance_x < 50)
								|| (distance_y < 50)) {
							kbdUsedCoord = 1;
						}
					}
				} while (kbdUsedCoord);

				(words[*n_words].coord_kbd[i]).x = x;
				(words[*n_words].coord_kbd[i]).y = y;
				words[*n_words].n_letters_kbd = strlen(line);

				do {
					mouseUsedCoord = 0;

					do {
						x = (rand() % 295) + 500;
					} while (x % 20 != 0);

					do {
						y = (rand() % 574) + 6;
					} while (y % 20 != 0);

					size_t j;
					for (j = 0; j < i; j++) {
						if (words[*n_words].coord_mouse[j].x == x
								&& words[*n_words].coord_mouse[j].y == y) {
							mouseUsedCoord = 1;
						}
					}
				} while (mouseUsedCoord);

				(words[*n_words].coord_mouse[i]).x = x;
				(words[*n_words].coord_mouse[i]).y = y;
			}

			/* generating coordinates for the deceiving letters,
			 * considering not to obtain occupied positions */
			size_t k;
			for (k = strlen(line); k < strlen(line) + 20; k++) {
				words[*n_words].letters[k] = ('A' + (rand() % 26));

				do {
					mouseUsedCoord = 0;

					do {
						x = (rand() % 295) + 500;
					} while (x % 20 != 0);

					do {
						y = (rand() % 574) + 6;
					} while (y % 20 != 0);

					size_t p;
					for (p = 0; p < k; p++) {
						if (words[*n_words].coord_mouse[p].x == x
								&& words[*n_words].coord_mouse[p].y == y) {
							mouseUsedCoord = 1;
						}
					}
				} while (mouseUsedCoord);

				(words[*n_words].coord_mouse[k]).x = x;
				(words[*n_words].coord_mouse[k]).y = y;
			}

			/* the mouse has 20 more letters than the keyboard */
			words[*n_words].n_letters_mouse = strlen(line) + 20;

			(*n_words)++;
		}

		fclose(words_file);
		return words;
	}

	printf("File could not be read!\n");
	return NULL;
}

void destroy_words(Word* words) {

	free(words);
}

void spawn_letters(Game* game, Word word, unsigned int letter_index, int kbd) {

	size_t j;
	if (kbd) {
		for (j = letter_index; j < word.n_letters_kbd; j++) {
			vg_tile(game->font->font_img, word.letters[j], game->font->width,
					game->font->height, word.coord_kbd[j].x,
					word.coord_kbd[j].y);
		}
	} else {
		for (j = letter_index; j < word.n_letters_mouse; j++) {
			vg_tile(game->font->font_img, word.letters[j], game->font->width,
					game->font->height, word.coord_mouse[j].x,
					word.coord_mouse[j].y);
		}
	}
}

Snake* new_node(Snake info) {

	Snake* new_node = (Snake *) malloc(sizeof(Snake));
	if (new_node == NULL)
		return NULL;

	*new_node = info;
	new_node->next = NULL;
	return new_node;
}

void push_front(Snake* node) {

	if (first_node == last_node && first_node == NULL) {
		first_node = node;
		last_node = node;
		first_node->next = NULL;
		last_node->next = NULL;
	} else {
		Snake* temp = first_node;
		first_node = node;
		first_node->next = temp;
	}
}

void push_back(Snake* node) {

	if (first_node == last_node && last_node == NULL) {
		first_node = node;
		last_node = node;
		first_node->next = NULL;
		last_node->next = NULL;
	} else {
		Snake* tmp = first_node;
		while (tmp->next != NULL) {
			tmp = tmp->next;
		}

		tmp->next = node;
		tmp->next->next = NULL;
		last_node = node;
	}
}

Snake* pop_front() {

	Snake* info = NULL;
	if (first_node != NULL) {
		info = first_node;
		first_node = first_node->next;
	}

	/* to never dereference a NULL pointer */
	if (first_node == NULL) {
		first_node = info;
	}

	return info;
}

Snake* pop_back() {

	Snake* info = NULL;
	if (first_node->next == NULL) {
		info = first_node;
		first_node = NULL;
		last_node = NULL;
	} else {
		Snake* tmp = first_node;
		while (tmp->next->next != NULL) {
			tmp = tmp->next;
		}
		info = tmp->next;
		tmp->next = NULL;
		last_node = tmp;
	}

	/* to never dereference a NULL pointer */
	if (last_node == NULL) {
		last_node = info;
	}

	return info;
}

void spawn_block(Snake* snake) {

	int sucess = 0;
	Snake* new_block = new_node(*first_node);

	switch (first_node->key) {
	case W_KEY:
		(new_block->coord).y += snake->side;
		sucess = 1;
		break;
	case A_KEY:
		(new_block->coord).x += snake->side;
		sucess = 1;
		break;
	case S_KEY:
		(new_block->coord).y -= snake->side;
		sucess = 1;
		break;
	case D_KEY:
		(new_block->coord).x -= snake->side;
		sucess = 1;
		break;
	default:
		printf("Wrong direction error!\n");
	}

	if (sucess)
		push_front(new_block);
	else
		free(new_block);
}

Snake* initialize_snake() {

	Snake head;
	head.side = 20;
	(head.coord).x = 0;
	(head.coord).y = 300;
	head.key = D_KEY;
	head.color = SNAKE_COLOR;
	head.next = NULL;

	Snake* node = new_node(head);
	push_front(node);
	return node;
}

void destroy_snake(Snake* snake) {

	struct Snake *tmp = first_node;
	struct Snake *next = NULL;
	while (tmp != NULL) {
		next = tmp->next;
		free(tmp);
		tmp = next;
	}
	first_node = NULL;
	last_node = NULL;
}

void print_snake(Game* game, Word word, unsigned int letter_index) {

	vg_snake_clear();

	spawn_letters(game, word, letter_index, 1);

	Snake* tmp = NULL;
	for (tmp = first_node; tmp != NULL; tmp = tmp->next) {
		size_t i, j;
		for (i = 0; i < game->snake->side; i++)
			for (j = 0; j < game->snake->side; j++)
				draw_pixel((tmp->coord).x + i, (tmp->coord).y + j, tmp->color);
	}

	vg_copy();
}

void update_snake(unsigned long scancode) {

	int success = 1;
	Snake* block = pop_front();

	switch (scancode) {
	case W_KEY:
		printf("W_KEY PRESSED\n");
		if (last_node->key != S_KEY) {
			block->key = W_KEY;
			(block->coord).x = (last_node->coord).x;
			(block->coord).y = (last_node->coord).y - block->side;
		} else {
			block->key = S_KEY;
			(block->coord).x = (last_node->coord).x;
			(block->coord).y = (last_node->coord).y + block->side;
		}
		break;
	case A_KEY:
		printf("A_KEY PRESSED\n");
		if (last_node->key != D_KEY) {
			block->key = A_KEY;
			(block->coord).x = (last_node->coord).x - block->side;
			(block->coord).y = (last_node->coord).y;
		} else {
			block->key = D_KEY;
			(block->coord).x = (last_node->coord).x + block->side;
			(block->coord).y = (last_node->coord).y;
		}
		break;
	case S_KEY:
		printf("S_KEY PRESSED\n");
		if (last_node->key != W_KEY) {
			block->key = S_KEY;
			(block->coord).x = (last_node->coord).x;
			(block->coord).y = (last_node->coord).y + block->side;
		} else {
			block->key = W_KEY;
			(block->coord).x = (last_node->coord).x;
			(block->coord).y = (last_node->coord).y - block->side;
		}
		break;
	case D_KEY:
		printf("D_KEY PRESSED\n");
		if (last_node->key != A_KEY) {
			block->key = D_KEY;
			(block->coord).x = (last_node->coord).x + block->side;
			(block->coord).y = (last_node->coord).y;
		} else {
			block->key = A_KEY;
			(block->coord).x = (last_node->coord).x - block->side;
			(block->coord).y = (last_node->coord).y;
		}
		break;
	default:
		success = 0;
		break;
	}

	if (success)
		push_back(block);
}

int test_collision_snake(Snake* snake, Word word, int letter_index) {

	/* test collision with the middle and screen borders */
	if ((last_node->coord).x >= MIDDLE_BORDER
			|| ((last_node->coord).x + snake->side / 2) <= 0
			|| (last_node->coord).y >= V_RES
			|| ((last_node->coord).y + snake->side / 2) <= 0)
		return 1;

	else {
		/* test collision in snake's body itself */
		Snake* tmp = NULL;
		for (tmp = first_node; tmp != last_node; tmp = tmp->next) {
			if ((last_node->coord).x == (tmp->coord).x
					&& (last_node->coord).y == (tmp->coord).y) {
				return 1;
			}
		}

		/* test collision with letters on the screen */
		size_t i;
		for (i = letter_index; i < word.n_letters_kbd; i++) {

			int equal_x = word.coord_kbd[i].x == last_node->coord.x;
			int distance_y = abs(word.coord_kbd[i].y - last_node->coord.y);
			int equal_y = (word.coord_kbd[i].y == last_node->coord.y);
			int distance_x = abs(word.coord_kbd[i].x - last_node->coord.x);

			/* if blocks collided horizontally or vertically */
			if ((equal_x && distance_y < 20) || (equal_y && distance_x < 20)) {

				/* if letter eaten is in the correct order */
				if (i != letter_index) {

					/* if eaten letter has more than 2 appearances on the word */
					if (word.letters[i] == word.letters[letter_index]) {
						coord_t temp = word.coord_kbd[i];
						word.coord_kbd[i] = word.coord_kbd[letter_index];
						word.coord_kbd[letter_index] = temp;
						return 2;
					}

					return 1;
				}

				return 2;
			}
		}

		return 0;
	}
}

int main_menu(Game* game) {

	unsigned long status, outbuff_trash;
	int irq_mouse = BIT(game->hookid_mouse);
	int ipc_status;
	int r;
	message msg;

	/* clear OUT_BUF */
	readStatusRegister(&status);
	if (status & OBF) {
		readOutBuffer(&outbuff_trash);
	}

	while (game->current_state != LEAVE) {
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_mouse) { /* subscribed mouse interrupt */
					/* read status register */
					readOutBuffer(&g_byte);
					/* if g_byte's 3rd bit is set, it might be the first byte from the packet,
					 * so we only begin to store packet bytes when we receive one byte complying
					 * with this condition */
					if (((g_byte & BIT(3)) && g_count_bytes == 0)
							|| g_count_bytes > 0) {
						g_packet[g_count_bytes] = g_byte;
						g_count_bytes++;
					}

					/* if the first packet has been stored */
					if (g_count_bytes == 3) {
						/* reset packet array index */
						g_count_bytes = 0;
						/* update cursor's position */
						update_cursor(game->cursor, 1);
						/* print mouse's new position */
						print_cursor(game->cursor, game->menu);
						/* menu options handling */
						menu_handling(game);
					}
				}
				break;
			default:
				printf("Not a mouse notification!\n");
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	/* clear OUT_BUF */
	readStatusRegister(&status);
	if (status & OBF) {
		readOutBuffer(&outbuff_trash);
	}

	return 0;
}

int play_game(Game* game) {

	unsigned long status, outbuff_trash;
	int irq_timer = BIT(game->hookid_timer);
	int irq_kbd = BIT(game->hookid_kbd);
	int irq_mouse = BIT(game->hookid_mouse);
	int ipc_status;
	int r;
	message msg;

	/* clear OUT_BUF */
	readStatusRegister(&status);
	if (status & OBF) {
		readOutBuffer(&outbuff_trash);
	}

	/* game state indicators */
	int lvl_kbd = 0, lvl_mouse = 0, kbd_hit = 0;
	int letter_index_kbd = 0, letter_index_mouse = 0;
	unsigned long count = 0;

	vg_snake_clear();
	vg_cursor_clear();

	int snakeWon = 0, cursorWon = 0;
	while (!(snakeWon || cursorWon)) {
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_timer) {
					count++;
					/* 15fps per second */
					if (count % 4 == 0) {
						/* if some key was pressed */
						if (kbd_hit) {
							update_snake(g_scancode);
							kbd_hit = 0;
						} else {
							update_snake(last_node->key);
						}

						/* prints snake on the screen */
						print_snake(game, game->words[lvl_kbd],
								letter_index_kbd);

						/* test for collision */
						switch (test_collision_snake(game->snake,
								game->words[lvl_kbd], letter_index_kbd)) {
						case 0:
							/* no collisions */
							break;
						case 1:
							/* collision with screen
							 * or the snake itself */
							cursorWon = 1;
							break;
						case 2:
							letter_index_kbd++;
							spawn_block(game->snake);
							if (letter_index_kbd
									== game->words[lvl_kbd].n_letters_kbd) {
								letter_index_kbd = 0;
								lvl_kbd++;
								if (lvl_kbd == game->n_words) {
									snakeWon = 1;
								}
							}
							break;
						default:
							printf("Snake's collision error!\n");
							break;
						}
					}
				}

				if (msg.NOTIFY_ARG & irq_kbd) {
					if (kbd_asm_handler() == 0) {
						printf("Error executing kbd_asm_handler()!\n");
						return 1;
					}

					/* if scancode represents a valid makecode */
					if (g_scancode == W_KEY || g_scancode == A_KEY
							|| g_scancode == S_KEY || g_scancode == D_KEY) {
						kbd_hit = 1;
					}
				}

				if (msg.NOTIFY_ARG & irq_mouse) { /* subscribed mouse interrupt */
					/* read status register */
					readOutBuffer(&g_byte);
					/* if g_byte's 3rd bit is set, it might be the first byte from the packet,
					 * so we only begin to store packet bytes when we receive one byte complying
					 * with this condition */
					if (((g_byte & BIT(3)) && g_count_bytes == 0)
							|| g_count_bytes > 0) {
						g_packet[g_count_bytes] = g_byte;
						g_count_bytes++;
					}
					/* if the first packet has been stored */
					if (g_count_bytes == 3) {
						/* reset packet array index */
						g_count_bytes = 0;
						/* clean cursor last position */
						clean_cursor(game, game->words[lvl_mouse],
								letter_index_mouse);
						/* update cursor's position */
						update_cursor(game->cursor, 0);
						/* print mouse's new position */
						print_cursor(game->cursor, NULL);

						if (g_packet[0] & LB) {
							/* test for collision */
							switch (test_collision_cursor(game->cursor,
									game->words[lvl_mouse], letter_index_mouse)) {
							case 0:
								/* no collisions */
								break;
							case 1:
								/* collision with wrong letter */
								snakeWon = 1;
								break;
							case 2:
								letter_index_mouse++;
								if (letter_index_mouse
										== game->words[lvl_mouse].n_letters_kbd) {
									letter_index_mouse = 0;
									lvl_mouse++;
									if (lvl_mouse == game->n_words) {
										cursorWon = 1;
									}
								}
								break;
							default:
								printf("Cursor's collision error!\n");
								break;
							}
						}
					}
				}
				break;
			default:
				printf("Not a keyboard notification!\n");
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	handle_event(game, END_GAME);

	/* disable mouse */
	mouse_write_cmd(DISABLE_MOUSE);

	/* prints victory screen */
	if (snakeWon) {
		write_winner(1);
		victory_screen(game->menu, "snake");
	}
	else if (cursorWon) {
		write_winner(0);
		victory_screen(game->menu, "cursor");
	}

	/* enable mouse */
	mouse_write_cmd(ENABLE_MOUSE);

	/* clear OUT_BUF */
	readStatusRegister(&status);
	if (status & OBF) {
		readOutBuffer(&outbuff_trash);
	}

	return 0;
}

int write_winner(int winner) {

	/* Date and Time variables */
	unsigned long sec = 0;
	unsigned long min = 0;
	unsigned long hour = 0;
	unsigned long day = 0;
	unsigned long month = 0;
	unsigned long year = 0;

	FILE* winners_file = NULL;
	winners_file = fopen(WINNERS_FILEPATH, "a");
	if (winners_file == NULL) {
		printf("Error opening \"winners.txt\" file!\n");
		return 1;
	}

	/* obtain the time and date */
	rtc_time(&sec, &min, &hour);
	rtc_date(&day, &month, &year);
	fprintf(winners_file, "%s won at %x:%x:%x in the date %x/%x/20%x\n",
			WHO_WON(winner), hour, min, sec, day, month, year);

	fclose(winners_file);
	return 0;
}

void victory_screen(Menu* menu, char* winner) {

	if (strncmp(winner, "snake", strlen("snake")) == 0)
		vg_png(menu->snake_victory, menu->width, menu->height, 0, 0);
	else if (strncmp(winner, "cursor", strlen("cursor")) == 0)
		vg_png(menu->cursor_victory, menu->width, menu->height, 0, 0);

	vg_copy();
	sleep(5);
}
