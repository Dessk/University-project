#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>;
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_video.h>
#include "WavReader.h"
#include "StandardBeatDetection.h"

#define program_version 0.5
#define version_type "BETA"
#define author "Bartosz Staniek"


typedef struct {
	int x;
	int y;
	int w;
	double h;
	ALLEGRO_COLOR color;

}Bar;



int main(int args, char** argv) {

	printf("=====BEAT-DETECTOR=====\n\
Version: %s %.2f\n\
Author:%s\n\
Working file formats: .wav, .ogv\n\
Partially working formats: .mp4\n\
=======================\n", version_type, program_version, author);

	printf("Enter file path to analyze : ");

	char user_input[20];
	char user_input_copy[20];
	scanf("%s", user_input);

	char* check_input = strstr(user_input, ".wav");
	char* check_mp4_input = strstr(user_input, ".mp4");
	char* check_ogv_input = strstr(user_input, ".ogv");

	strcpy(user_input_copy, user_input);
	char ogv_final_command[200];
	char file_path[200] = __FILE__;
	char* last = strrchr(file_path, '\\');
	*++last = 0;
	strcat(file_path, user_input);

	char ffmpeg_path[200] = __FILE__;
	char* last_ffm = strrchr(ffmpeg_path, '\\');
	*last_ffm = 0;
	strcat(ffmpeg_path, "\\ffmpeg\\bin\\ffmpeg.exe");
	
	if (check_mp4_input != NULL) {
		printf("MP4 extension. Preparing file (It may take a while )...\n =====FFMPEG-INFO=====\n");

		char* edit = strrchr(user_input, '.');
		*++edit = 0;
		strcat(user_input, "wav");

		char* edit2 = strrchr(user_input_copy, '.');
		*++edit2 = 0;
		strcat(user_input_copy, "ogv");

		char final_command[200];
		sprintf(final_command, "%s -i %s -vn -acodec pcm_s16le -ac 2 -ar 44100 %s", ffmpeg_path, file_path, user_input);
		sprintf(ogv_final_command, "%s -i %s -q:v 10 -c:v libtheora -c:a libvorbis %s",ffmpeg_path,file_path,user_input_copy);
		system(final_command);
	}
	else if (check_ogv_input != NULL) {
		printf("OGV extension. Preparing file (It may take a while )...\n =====FFMPEG-INFO=====\n");
		char* edit = strrchr(user_input, '.');
		*++edit = 0;
		strcat(user_input, "wav");

		char final_command[200];
		sprintf(final_command, "%s -i %s -ac 2 -f wav %s", ffmpeg_path, file_path, user_input);
		system(final_command);
	}
	else if (check_input != NULL) {
		printf("\nCorrect extension : WAV. \n");
	}
	else {
		printf("\nWrong extension ! Exiting program... \n");
		exit(-1);
	}

	printf("%s", user_input);
	FILE* file_read;
	if((file_read = ReadWAV(user_input)) == NULL){exit(-1);}
	double user_prec;

	printf("Enter detection accuracy:");
	scanf("%lf", &user_prec);
	printf("\n");

	StandardBeatDetect(file_read,user_prec);
	fclose(file_read);


	Bar bars[43];
	Bar bars2[43];

	int w = 860 / 43;

	FILE* file;
	if ((file = fopen("output_values_c_avg.txt", "r")) == NULL) {
		exit(-1);
	}

	FILE* file2;
	if ((file2 = fopen("output_values.txt", "r")) == NULL) {
		exit(-1);
	}


	for (int i = 0; i < 43; i++) {
		bars[i].x = i * w;
		bars[i].y = 600;
		bars[i].w = w-2;
		bars[i].h = 600-10;
		bars[i].color = al_map_rgb(0, 127, 255);

		bars2[i].x = i * w;
		bars2[i].y = 600;
		bars2[i].w = w - 2;
		bars2[i].h = 600 - 10;
		bars2[i].color = al_map_rgb(0, 127, 255);
	}


	bool visualizer_loop = true;
	
	ALLEGRO_DISPLAY* display;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_EVENT evt;
	ALLEGRO_TIMEOUT timeout;
	ALLEGRO_TIMER* timer;
	ALLEGRO_SAMPLE* sample;
	ALLEGRO_VIDEO* video = NULL;
	ALLEGRO_BITMAP* bitmap = NULL;
	int draw_frame = 0;


	al_init();
	al_init_primitives_addon();
	al_init_video_addon();
	al_init_timeout(&timeout, 0.06);
	al_init_acodec_addon();
	al_install_audio();
	al_reserve_samples(1);
	display = al_create_display(860, 600);
	queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 43.0);


	al_register_event_source(queue,al_get_display_event_source(display));
	al_register_event_source(queue,al_get_timer_event_source(timer));

	if (check_mp4_input != NULL) {
		system(ogv_final_command);
		video = al_open_video(user_input_copy);
		al_register_event_source(queue, al_get_video_event_source(video));
		al_start_video(video, al_get_default_mixer());
	}
	else if (check_ogv_input != NULL) {
		video = al_open_video(user_input_copy);
		al_register_event_source(queue, al_get_video_event_source(video));
		al_start_video(video, al_get_default_mixer());
	}
	else {
		sample = al_load_sample(user_input);
		al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}

	printf("Playing : %s ", user_input);
	al_start_timer(timer);

	while (visualizer_loop) {

		bool event_rec = al_wait_for_event_until(queue, &evt, &timeout);
		if (event_rec && evt.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			visualizer_loop = false;
		}
		else if (event_rec && evt.type == ALLEGRO_EVENT_TIMER) {
			double rec;
			if (fscanf(file, "%lf", &rec) != EOF) {
				for (int i = 42; i > 0; i--) {
					bars[i].h = bars[i - 1].h;
				}
				bars[0].h = 600 - rec*0.7;
			}
			if (fscanf(file2, "%lf", &rec) != EOF) {
				for (int i = 42; i > 0; i--) {
					bars2[i].h = bars2[i - 1].h;
				}
				bars2[0].h = 600 - rec*0.7;
			}
			al_clear_to_color(al_map_rgb(0, 0, 0));
			if (video != NULL) {
				bitmap = al_get_video_frame(video);
				if (bitmap) {
					float scale = 1.0; 
					float sw = al_get_bitmap_width(bitmap);
					float sh = al_get_bitmap_height(bitmap);
					float dw = scale * al_get_video_scaled_width(video);
					float dh = scale * al_get_video_scaled_height(video);
					al_draw_scaled_bitmap(bitmap, 0, 0, sw, sh, 0, 0, 860, 600, 0);
				}
				draw_frame = 0;
			}
			for (int i = 0; i < 43; i++) {
				al_draw_filled_rectangle(bars[i].x + 2, bars[i].h, bars[i].x + bars[i].w, bars[i].y,al_map_rgb(165,27,11));
				al_draw_filled_rectangle(bars2[i].x + 2, bars2[i].h, bars2[i].x + bars2[i].w, bars2[i].y, al_map_rgb(20, 100, 220));
			}
			al_flip_display();
		}
	}
	fclose(file);
	fclose(file2);
	al_destroy_event_queue(queue);
	al_destroy_timer(timer);
	al_destroy_display(display);
	if (video != NULL) {
		al_close_video(video);
	}

	return EXIT_SUCCESS;
}