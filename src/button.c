#include "files.h"
#include "audio.h"
#include "button.h"
#include "SDL_ttf.h"
#include "simple_logger.h"
#include "camera.h"


/* button resource manager */
static Button		*buttonList = NULL;
static int			numButtons = 0;
static int			maxButtons = 0;

/* button font data */
static TTF_Font		*buttonFont = NULL;
static SDL_Color	color = {255, 255, 255};


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

	TTF_CloseFont(buttonFont);
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

	buttonFont = TTF_OpenFont(PEP_FONT, 28);

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
	if(self->hoverNoise != NULL)
	{
		audio_sound_free(&self->hoverNoise);
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
		buttonList[i].hoverNoise = audio_load_sound(BUTTON_HOVER_NOISE , 0, FX_IMPACTS);

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
	Mouse *mouse = mouse_get();

	if(mouse_button_intersect(button, mouse))
	{
		if(button->state != HOVERED)
		{
			button->state = HOVERED;
			audio_play_sound(button->hoverNoise);
		}
		
		if(mouse->clicked)
		{
			button->click();
		}
	}
	else
	{
		button->state = NORMAL;
	}
}

int mouse_button_intersect(Button *button, Mouse *mouse)
{
	SDL_Rect aB, bB;

	//this assumes that the mouse is only one frame wide and tall, also assumes the button's collision is its entire spriteSize

	if(!button)
	{
		slog("button error");
		return 0;
	}
	aB = rect(button->position.x + button->bounds.x,
				button->position.y + button->bounds.y,
				button->bounds.w,
				button->bounds.h
		);
	bB = rect(mouse->position.x + mouse->bounds.x,
				mouse->position.y + mouse->bounds.y,
				mouse->bounds.w,
				mouse->bounds.h
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

Button *button_load_big(Vect2d position, char *labelText)
{
	Button *button = NULL;
	
	button = button_new(position); 
	button->buttonSprite = sprite_load("images/button.png", vect2d_new(500, 100), 1, 2);
	button->bounds = rect(0, 0, button->buttonSprite->frameSize.x, button->buttonSprite->frameSize.y);
	button->label = sprite_load_text(buttonFont, labelText, color);
	return button;
}

Button *button_load_small(Vect2d position, char *labelText)
{
	Button *button = NULL;
	
	button = button_new(position); 
	button->buttonSprite = sprite_load("images/tiny_button.png", vect2d_new(250, 50), 1, 2);
	button->bounds = rect(0, 0, button->buttonSprite->frameSize.x, button->buttonSprite->frameSize.y);
	button->label = sprite_load_text(buttonFont, labelText, color);
	return button;
}

