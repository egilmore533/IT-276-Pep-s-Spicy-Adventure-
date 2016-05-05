#include "button.h"
#include "SDL_ttf.h"
#include "simple_logger.h"
#include "camera.h"

static Button *buttonList = NULL;
static int numButtons = 0;
static int maxButtons = 0;
static TTF_Font *button_font = NULL;
static SDL_Color color = {255, 255, 255};


void button_empty_list()
{
	int i;
	Button *button = NULL;
	if(!buttonList)
	{
		slog("buttonList not initialized");
		return;
	}
	for(i = 0; i < maxButtons; ++i)
	{
		button = &buttonList[i];
		button_free(&button);
	}
	memset(buttonList, 0, sizeof(Button) * maxButtons);
	for(i = 0; i < maxButtons; i++)
	{
		buttonList[i].buttonSprite = NULL;
		buttonList[i].label = NULL;
	}
	numButtons = 0;
}

void button_close_system()
{
	int i;
	Button *button;
	if(!buttonList)
	{
		slog("buttonList not initialized");
		return;
	}
	for(i = 0; i < maxButtons; i++)
	{
		button = &buttonList[i];
		button_free(&button);
	}
	numButtons = 0;
	free(buttonList);
	buttonList = NULL;

	TTF_CloseFont(button_font);
}

void button_initialize_system(int buttonsMax)
{
	int i;
	if(buttonsMax == 0)
	{
		slog("button max == 0");
		return;
	}
	buttonList = (Button *) malloc (sizeof (Button) * buttonsMax);
	if(!buttonList)
	{
		slog("failed to initialize buttonList");
		return;
	}
	memset(buttonList, 0, sizeof (Button) * buttonsMax);
	numButtons = 0;
	for(i = 0; i < buttonsMax; i++)
	{
		buttonList[i].buttonSprite = NULL;
		buttonList[i].label = NULL;
	}
	maxButtons = buttonsMax;

	button_font = TTF_OpenFont("fonts/HussarPrintASpicyAdventure.ttf", 28);

	atexit(button_close_system);
}

void button_free(Button **button)
{
	Button *self;
	if(!button)
	{
		return;
	}
	else if(!*button) 
	{
		return;
	}
	self = *button;
	self->alive--;

	if(self->buttonSprite != NULL)
	{
		sprite_free(&self->buttonSprite);
	}
	if(self->label != NULL)
	{
		sprite_free(&self->label);
	}
	
	numButtons--;
	*button = NULL;
}

Button *button_new(Vect2d position)
{
	int i;
	/*makesure we have the room for a new sprite*/
	if(numButtons + 1 > maxButtons)
	{
		slog("Maximum Buttons Reached.");
		exit(1);
	}
	for(i = 0; i < maxButtons; i++)
	{
		if(buttonList[i].alive)
		{
			continue;
		}
		memset(&buttonList[i],0,sizeof(Button));
		buttonList[i].alive = 1;
		buttonList[i].draw = &button_draw;
		buttonList[i].update = &button_update;
		buttonList[i].position = position;
		numButtons++;
		return &buttonList[i];
	}
	return NULL;
}

void button_draw(Button *button)
{
	Vect2d offset = vect2d_new(button->buttonSprite->frameSize.x / 2 - button->label->frameSize.x / 2, button->buttonSprite->frameSize.y / 2 - button->label->frameSize.y / 2);
	
	sprite_draw(button->buttonSprite, button->state, button->position); //state is either 0 or 1 so that should be goods
	
	offset = vect2d_new(offset.x + button->position.x, offset.y + button->position.y);
	sprite_text_draw(button->label, offset);
}

void button_update(Button *button)
{
	int mouseX, mouseY;
	Vect2d mousePosition;
	SDL_Event click_event;
	Entity *cam = camera_get();
	
	SDL_GetMouseState(&mouseX, &mouseY);
	mousePosition = vect2d_new(mouseX, mouseY);

	if(cam)
	{
		mousePosition.x += cam->position.x;
		mousePosition.y += cam->position.y;

	}

	if(mouse_button_intersect(button, mousePosition))
	{
		button->state = Hovered;
		SDL_PollEvent(&click_event);
		if(click_event.type == SDL_MOUSEBUTTONDOWN)
		{
			button->click();
		}
	}
	else
	{
		button->state = Normal;
	}
}

int mouse_button_intersect(Button *button, Vect2d mousePosition)
{
	SDL_Rect aB, bB;
	SDL_Rect mouseBounds, buttonBounds;

	//this assumes that the mouse is only one frame wide and tall, also assumes the button's collision is its entire spriteSize
	mouseBounds = rect(0, 0, 1, 1);
	buttonBounds = rect(0, 0, button->buttonSprite->frameSize.x, button->buttonSprite->frameSize.y);

	if(!button)
	{
		slog("button error");
		return 0;
	}
	aB = rect(button->position.x + buttonBounds.x,
				button->position.y + buttonBounds.y,
				buttonBounds.w,
				buttonBounds.h
		);
	bB = rect(mousePosition.x + mouseBounds.x,
				mousePosition.y + mouseBounds.y,
				mouseBounds.w,
				mouseBounds.h
		);
	return rect_intersect(aB, bB);
}

void button_update_all()
{
	int i;
	for(i = 0; i < maxButtons; i++)
	{
		if(!buttonList[i].alive)
		{
			continue;
		}
		if(!buttonList[i].update)
		{
			continue;
		}
		buttonList[i].update(&buttonList[i]);
	}
}

void button_draw_all()
{
	int i;
	for(i = 0; i < maxButtons; i++)
	{
		if(!buttonList[i].alive)
		{
			continue;
		}
		if(!buttonList[i].draw)
		{
			continue;
		}
		buttonList[i].draw(&buttonList[i]);
	}
}

////////////////////////////////////////////////////////////////////

Button *button_load_arcade_mode(Vect2d position)
{
	Button *button = NULL;
	
	button = button_new(position); 
	button->buttonSprite = sprite_load("images/button.png", vect2d_new(500, 100), 1, 2);
	button->label = sprite_load_text(button_font, "Arcade Mode", color);
	return button;
	
	//set the click event in whatever is creating the button, so main menu creates a button then says it will perfrom the arcade mode func or will go to the controls
}

Button *button_load_editor_mode(Vect2d position)
{
	Button *button = NULL;
	
	button = button_new(position); 
	button->buttonSprite = sprite_load("images/button.png", vect2d_new(500, 100), 1, 2);
	button->label = sprite_load_text(button_font, "Editor Mode", color);
	return button;
}

Button *button_load_yes_back(Vect2d position)
{
	Button *button = NULL;
	
	button = button_new(position); 
	button->buttonSprite = sprite_load("images/tiny_button.png", vect2d_new(250, 50), 1, 2);
	button->label = sprite_load_text(button_font, "Yes", color);
	return button;
}

Button *button_load_no_back(Vect2d position)
{
	Button *button = NULL;
	
	button = button_new(position); 
	button->buttonSprite = sprite_load("images/tiny_button.png", vect2d_new(250, 50), 1, 2);
	button->label = sprite_load_text(button_font, "No", color);
	return button;
}

Button *button_load_controls(Vect2d position)
{
	Button *button = NULL;
	
	button = button_new(position); 
	button->buttonSprite = sprite_load("images/button.png", vect2d_new(500, 100), 1, 2);
	button->label = sprite_load_text(button_font, "Controls", color);
	return button;
}

Button *button_load_next(Vect2d position)
{
	Button *button = NULL;
	
	button = button_new(position); 
	button->buttonSprite = sprite_load("images/tiny_button.png", vect2d_new(250, 50), 1, 2);
	button->label = sprite_load_text(button_font, "Next", color);
	return button;
}

Button *button_load_previous(Vect2d position)
{
	Button *button = NULL;
	
	button = button_new(position); 
	button->buttonSprite = sprite_load("images/tiny_button.png", vect2d_new(250, 50), 1, 2);
	button->label = sprite_load_text(button_font, "Prev", color);
	return button;
}

