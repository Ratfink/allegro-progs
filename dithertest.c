/* Copyright (c) 2012 Clayton G. Hobbs
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#define FPS 60
#define HIGH 255
#define LOW 0

/* dithertest.c: draw randomly dithered images in 8 colors at a maximum of 60
 * FPS (but probably slower).
 *
 * compiling: $ gcc dithertest.c -o dithertest -std=c99 `pkg-config --cflags \
 --libs allegro-5.0 allegro_image-5.0`
 *
 * usage: dithertest <image>
 *
 * bugs: Close button works intermittently for some reason.  Use ^C as a
 * workaround.
 */

int main(int argc, char **argv)
{
    int doexit = 0;
    int random = 0;
    int redraw = 0;
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_BITMAP *image = NULL;
    ALLEGRO_BITMAP *image2 = NULL;
    ALLEGRO_COLOR pixel;
    unsigned char pr = 0;
    unsigned char pg = 0;
    unsigned char pb = 0;

    if (argc != 2) {
        return 1;
    }

    srand(time(NULL));

    al_init();
    al_init_image_addon();

    image = al_load_bitmap(argv[1]);
    image2 = al_load_bitmap(argv[1]);
    timer = al_create_timer(1.0 / FPS);
    display = al_create_display(al_get_bitmap_width(image),
            al_get_bitmap_height(image));
    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    while (!doexit) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            al_lock_bitmap(image, al_get_bitmap_format(image),
                    ALLEGRO_LOCK_READONLY);
            al_lock_bitmap(image2, al_get_bitmap_format(image2),
                    ALLEGRO_LOCK_WRITEONLY);
            al_set_target_bitmap(image2);
            for (int x = al_get_bitmap_width(image) - 1; x >= 0; x--) {
                for (int y = al_get_bitmap_height(image) - 1; y >= 0; y--) {
                    pixel = al_get_pixel(image, x, y);
                    al_unmap_rgb(pixel, &pr, &pg, &pb);
                    random = rand() % 255;
                    if (random < pr) {
                        pr = HIGH;
                    } else {
                        pr = LOW;
                    }
                    if (random < pg) {
                        pg = HIGH;
                    } else {
                        pg = LOW;
                    }
                    if (random < pb) {
                        pb = HIGH;
                    } else {
                        pb = LOW;
                    }
                    pixel = al_map_rgb(pr, pg, pb);
                    al_put_pixel(x, y, pixel);
                }
            }
            al_unlock_bitmap(image);
            al_unlock_bitmap(image2);
            redraw = true;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if (redraw) {
            redraw = false;
            al_set_target_backbuffer(display);
            al_draw_bitmap(image2, 0, 0, 0);
            al_flip_display();
        }
    }

    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_bitmap(image);
    al_destroy_bitmap(image2);
    al_destroy_event_queue(event_queue);
}
