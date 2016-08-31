struct Volume
{
    int ***array;
    int xdim;
    int ydim;
    int zdim;
};

void allocvolume(struct Volume * v);
void freevolume(struct Volume * v);