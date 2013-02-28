#ifndef MMA7361_H
#define MMA7361_H

enum mma7361_range
{
	MMA7361_RANGE_1_5G,
	MMA7361_RANGE_6_0G
};

void mma7361_init(void);
void mma7361_enable(void);
void mma7361_disable(void);
void mma7361_set_range(enum mma7361_range range);

#endif // MMA7361_H
