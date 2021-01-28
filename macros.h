
#define set_P0(w_) 		FIO0SET = (unsigned int)((1)<<((w_)));
#define clr_P0(w_) 		FIO0CLR = (unsigned int)((1)<<((w_)));

#define set_P1(w_) 		FIO1SET = (unsigned int)((1)<<((w_)));
#define clr_P1(w_) 		FIO1CLR = (unsigned int)((1)<<((w_)));

#define lobyte(w) ((unsigned char)(w))
#define hibyte(w) ((unsigned char)(w >> 8))
