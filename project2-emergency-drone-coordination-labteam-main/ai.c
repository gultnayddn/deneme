#include "headers/ai.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void assign_mission(Drone *drone, Coord target) {
    pthread_mutex_lock(&drone->lock);
    drone->target = target;
    drone->status = ON_MISSION;
    pthread_mutex_unlock(&drone->lock);
}

Drone *find_closest_idle_drone(Coord target) {
    Drone *closest = NULL;
    int min_distance = INT_MAX;
    pthread_mutex_lock(&drones->lock);  // List mutex
    Node *node = drones->head;
    while (node != NULL) {
        Drone *d = (Drone *)node->data;
        if (d->status == IDLE) {
            int dist = abs(d->coord.x - target.x) +
                       abs(d->coord.y - target.y);
            if (dist < min_distance) {
                min_distance = dist;
                closest = d;
            }
        }
        node = node->next;
    }
    pthread_mutex_unlock(&drones->lock);  // List mutex
    return closest;
}

void *ai_controller(void *arg) {
    while (1) {
        pthread_mutex_lock(&survivors->lock);

        Survivor *s = NULL;
        if (survivors->number_of_elements > 0) {
            // Survivor'u alın ve listeden çıkarın atomik şekilde
            s = survivors->head->data;
            survivors->removedata(survivors, s);
        }
        pthread_mutex_unlock(&survivors->lock);

        if (s != NULL) {
            Drone *closest = find_closest_idle_drone(s->coord);
            if (closest) {
                assign_mission(closest, s->coord);

                s->status = 1;
                s->helped_time = s->discovery_time;

                pthread_mutex_lock(&helpedsurvivors->lock);
                helpedsurvivors->add(helpedsurvivors, s);
                pthread_mutex_unlock(&helpedsurvivors->lock);

                printf("Drone %d assigned to survivor at (%d, %d)\n", closest->id, s->coord.x, s->coord.y);
                printf("Survivor %s being helped by Drone %d\n", s->info, closest->id);
            } else {
                // Drone yoksa survivor tekrar survivors listesine eklenebilir
                pthread_mutex_lock(&survivors->lock);
                survivors->add(survivors, s);
                pthread_mutex_unlock(&survivors->lock);
            }
        }

        sleep(1);
    }
    return NULL;
}


