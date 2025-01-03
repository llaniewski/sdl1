
void setrgbcolor(double rd, double gd, double bd);
void setcolor(double c);
int is_pressed(char ch);
inline int pressed_space() { return is_pressed(32); }
void graphics(int sx, int sy);
int animate(double fps);
void clear();
void line(double x1, double y1, double x2, double y2);
void circle(double x, double y, int r);
void point(double x, double y);
