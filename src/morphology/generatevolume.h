struct Volume
{
    int ***array;
    int xdim;
    int ydim;
    int zdim;
};

void generatevolume(struct Volume * v, int (*shape)() );
void allocvolume   (struct Volume * v);
void freevolume    (struct Volume * v);