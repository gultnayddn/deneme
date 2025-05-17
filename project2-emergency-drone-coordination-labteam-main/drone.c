#include "headers/drone.h"
#include "headers/globals.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Global drone fleet
Drone *drone_fleet = NULL;
int num_drones = 10; // Default fleet size

void initialize_drones() {
    drone_fleet = malloc(sizeof(Drone) * num_drones);
    srand(time(NULL));

    for(int i = 0; i < num_drones; i++) {
        drone_fleet[i].id = i;
        drone_fleet[i].status = IDLE;
        drone_fleet[i].coord = (Coord){rand() % map.width, rand() % map.height};
        drone_fleet[i].target = drone_fleet[i].coord; // Initial target=current position
        pthread_mutex_init(&drone_fleet[i].lock, NULL);
        
        //TODO in Phase-2 you should use this for client drones,
        // Add to global drone list
        pthread_mutex_lock(&drones->lock);
        drones->add(drones, &drone_fleet[i]); 
        pthread_mutex_unlock(&drones->lock);
        
        // Create thread
        pthread_create(&drone_fleet[i].thread_id, NULL, drone_behavior, &drone_fleet[i]);
    }
}

void* drone_behavior(void *arg) {
    Drone *d = (Drone*)arg;
    
    while(1) {
        pthread_mutex_lock(&d->lock);
        
        if(d->status == ON_MISSION) {
            // Move toward target (1 cell per iteration)
            if(d->coord.x < d->target.x) d->coord.x++;
            else if(d->coord.x > d->target.x) d->coord.x--;
            
            if(d->coord.y < d->target.y) d->coord.y++;
            else if(d->coord.y > d->target.y) d->coord.y--;

            // Check mission completion
          if (d->coord.x == d->target.x && d->coord.y == d->target.y) {
    d->status = IDLE;
    printf("Drone %d: Mission completed!\n", d->id);

    // Survivor'ı bul ve işle
    pthread_mutex_lock(&survivors->lock);
    Node *curr = survivors->head;
    Survivor *target_survivor = NULL;
    while (curr) {
        Survivor *s = (Survivor *)curr->data;
        if (s->coord.x == d->target.x && s->coord.y == d->target.y) {
            target_survivor = s;
            break;
        }
        curr = curr->next;
    }

    if (target_survivor) {
        // Yardım edildi olarak işaretle
        target_survivor->status = 1;
        target_survivor->helped_time = time(NULL);

        // Survivor'ı helped listesine ekle
        pthread_mutex_lock(&helpedsurvivors->lock);
        helpedsurvivors->add(helpedsurvivors, target_survivor);
        pthread_mutex_unlock(&helpedsurvivors->lock);

        // Survivor'ı aktif survivor listesinden çıkar
        survivors->removedata(survivors, target_survivor);

        // Harita hücresinden de çıkar
        pthread_mutex_lock(&map.cells[target_survivor->coord.x][target_survivor->coord.y].survivors->lock);
        map.cells[target_survivor->coord.x][target_survivor->coord.y].survivors->removedata(
            map.cells[target_survivor->coord.x][target_survivor->coord.y].survivors,
            target_survivor
        );
        pthread_mutex_unlock(&map.cells[target_survivor->coord.x][target_survivor->coord.y].survivors->lock);
    }
    pthread_mutex_unlock(&survivors->lock);
}


        }
        
        pthread_mutex_unlock(&d->lock);
        sleep(1); // Update every second
    }
    return NULL;
}




void cleanup_drones() {
    for(int i = 0; i < num_drones; i++) {
        pthread_cancel(drone_fleet[i].thread_id);
        pthread_mutex_destroy(&drone_fleet[i].lock);
    }
    free(drone_fleet);
}