//============================================================
//
//  Copyright (c) 1996-2009, Nicola Salmoria and the MAME Team.
//  Visit http://mamedev.org for licensing and usage restrictions.
//
//  MAME4DROID by David Valdeita (Seleuco)
//
//============================================================

#ifdef ANDROID
#include <android/log.h>
#endif

#include "osdepend.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "droidinput.h"
#include "droidsound.h"
#include "droidvideo.h"
#include "render.h"
#include "clifront.h"
#include "mame.h"
#include "emuopts.h"
#include "options.h"
#include "ui.h"
#include "driver.h"

#include "myosd.h"



//============================================================
//  GLOBALS
//============================================================

// a single rendering target
static render_target *our_target;


static const options_entry droid_mame_options[] =
{
	{ "initpath", ".;/mame", 0, "path to ini files" },
	{ NULL, NULL, OPTION_HEADER, "DROID OPTIONS" },
	{ "safearea(0.01-1)", "1.0", 0, "Adjust video for safe areas on older TVs (.9 or .85 are usually good values)" },
	{ NULL }
};

//============================================================
//  FUNCTION PROTOTYPES
//============================================================

void osd_exit(running_machine *machine);

//============================================================
//  main
//============================================================

#ifdef ANDROID
//int SDL_main(int argc, char **argv)
int android_main  (int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	static char *args[255];	int n=0;
	int ret;
	FILE *f;

	printf("Iniciando\n");

	myosd_init();

	droid_setup_video();

	while(1)
	{
	  // cli_execute does the heavy lifting; if we have osd-specific options, we
	  // would pass them as the third parameter here
		n=0;
		args[n]= (char *)"mame4droid";n++;
		//args[n]= (char *)"-skip_gameinfo"; n++;
		//args[n]= (char *)"starforc"; n++;
		//args[n]= (char *)"1944"; n++;
		//args[n]= (char *)"mslug3"; n++;
		//args[n]= (char *)"outrun"; n++;
		//args[n]= (char *)"-autoframeskip"; n++;
		//args[n]= (char *)"-noautoframeskip"; n++;
		//args[n]= (char *)"-nosound"; n++;
		//args[n]= (char *)"-novideo"; n++;
		//args[n]= (char *)"-nosleep"; n++;
		//args[n]= (char *)"-sleep"; n++;
		args[n]= (char *)"-nocoinlock"; n++;

	  f=fopen("mame4droid.cfg","r");
	  if (f) {
			fscanf(f,"%d",&myosd_last_game_selected);
			fclose(f);
	  }

	  ret = cli_execute(n, args, droid_mame_options);

	  f=fopen("mame4droid.cfg","w");
	  if (f) {
		  fprintf(f,"%d",myosd_last_game_selected);
		  fclose(f);
		  sync();

	  }
	}

	myosd_deinit();

	return ret;
}


//============================================================
//  osd_init
//============================================================

void osd_init(running_machine *machine)
{

	add_exit_callback(machine, osd_exit);

	our_target = render_target_alloc(machine, NULL, 0);
	if (our_target == NULL)
		fatalerror("Error creating render target");

	myosd_inGame = !(machine->gamedrv == &GAME_NAME(empty));

	options_set_bool(mame_options(), OPTION_CHEAT,myosd_cheat,OPTION_PRIORITY_CMDLINE);
    options_set_bool(mame_options(), OPTION_AUTOSAVE,myosd_autosave,OPTION_PRIORITY_CMDLINE);
    options_set_bool(mame_options(), OPTION_SOUND,myosd_sound_value != -1,OPTION_PRIORITY_CMDLINE);
    if(myosd_sound_value!=-1)
       options_set_int(mame_options(), OPTION_SOUND,myosd_sound_value,OPTION_PRIORITY_CMDLINE);

	droid_init_input(machine);
	droid_init_sound(machine);
	droid_init_video(machine);


}

void osd_exit(running_machine *machine)
{

	if (our_target != NULL)
		render_target_free(our_target);
	our_target = NULL;
}


void osd_update(running_machine *machine, int skip_redraw)
{
	if (!skip_redraw && our_target!=NULL)
	{
		droid_video_render(our_target);
	}

	droid_poll_input(machine);

	myosd_check_pause();
}

//============================================================
//  osd_wait_for_debugger
//============================================================

void osd_wait_for_debugger(const device_config *device, int firststop)
{
	// we don't have a debugger, so we just return here
}

