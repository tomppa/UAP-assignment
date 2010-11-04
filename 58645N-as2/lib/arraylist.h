struct arli
{
  int size;
  int *values;
};

extern int create(struct arli*);
extern int add(struct arli*, int);
extern int eradicate(struct arli*, int);
extern int get(struct arli, int);
extern void destroy(struct arli*);

