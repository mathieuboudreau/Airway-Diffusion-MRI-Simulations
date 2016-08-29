float f_calc_epsilon()
{
    /*Calculate machine epsilon for float data type*/
    float eps = (float)1.0;

    do {
        eps /= (float)2.0;
    }
    while ((float)(1.0 + eps) != (float)1.0);
    return eps;
}

double d_calc_epsilon()
{
    /*Calculate machine epsilon for double data type*/
    double eps = (double)1.0;

    do {
        eps /= (double)2.0;
    }
    while ((double)(1.0 + eps) != (double)1.0);
    return eps;
}
