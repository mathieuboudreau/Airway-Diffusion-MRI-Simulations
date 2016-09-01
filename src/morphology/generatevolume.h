#include "../params/default_buddedcylinders.h"

struct Volume
{
    int ***array;
    int xdim;
    int ydim;
    int zdim;
};

void generatevolume(struct Volume * v, int (*shape)(), struct BuddedCylinderParams * params);
void allocvolume   (struct Volume * v);
void freevolume    (struct Volume * v);